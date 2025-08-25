/**
**************************************************
* @file kernel.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 17, 2025
* @brief Module for stm kernel functionality
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'kernel_update' to update certain kernel components
  (#) Call 'kernel_schedule_task' to switch to the next runnable task
  (#) Call 'kernel_start_task' to specify the next runnable task
  (#) Call 'kernel_swap_task' to choose the next runnable task if
      the current running task is blocked

  (#) Call 'kernel_set_status' to set the kernels status
  (#) Call 'kernel_set_stack_pointer' to set the stack pointer
  (#) Call 'kernel_delay_blocking' to delay the running task without
      context switch
  (#) Call 'kernel_get_tick' to get the STM tick count

  (#) Call 'kernel_enter_idle' to enter Idle mode
  (#) Call 'kernel_exit_idle' to exit Idle mode
  (#) Call 'kernel_disable_interrupts' to disable interrupts
  (#) Call 'kernel_enable_interrupts' to enable interrupts

  (#) Call 'kernel_task_terminate' as return function from a task
  (#) Call 'kernel_shutdown' shutdown the system
==================================================
@endverbatim
**************************************************
*/


#include "kernel/kernel.h"
#include "kernel/semaphore.h"
#include "utils/dictionary.h"
#include "utils/support.h"
#include "stm32l4xx_hal.h"
#include "cmsis_gcc.h"
#include "string.h"

#define MSP_PSP

size_t kernel_start_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task);
size_t kernel_swap_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task);
size_t kernel_set_status(Kernel_Status_e status);
size_t kernel_set_stack_pointer(void);

extern void kernel_toggle_critical_section(void);


task_t                  *g_running_task_current             = NULL;
task_t                  *g_running_task_next                = NULL;
task_t                  *g_running_task_previous            = NULL;
Kernel_Status_e         g_kernel_status                     = EN_KERNEL_NOT_INITIALIZED;
uint32_t                g_task_start_time                   = 0;

size_t                  g_dictionary_priority               = 0;
size_t                  g_dictionary_priority_next          = 1;
extern  size_t          g_task_lower_priority;
extern  size_t          g_task_lowest_priority;

linked_list_element_t   *g_linked_list_task_iterator        = NULL;
linked_list_element_t   *g_linked_list_task_iterator_next   = NULL;

dictionary_t            *g_prioritized_tasks                = NULL;

linked_list_t           *g_priority_group_current           = NULL;
linked_list_t           *g_priority_group_next              = NULL;

extern  linked_list_t   *g_terminated_tasks_list;


extern  bool            g_kernel_critical_section_active;
extern  linked_list_t   *g_delayed_tasks;


extern size_t g_available_tasks;
extern size_t kernel_reinsert_task(linked_list_t **source, linked_list_element_t **element, task_t **task);
void kernel_task_terminate(void);

/**
 * @brief Update the kernel and its components.
 * @param None
 * */
void kernel_update(void) {

    // update system components
    HAL_IncTick();
    // exit immediately, if kernel is not running yet
    if (g_running_task_current == NULL
            || g_kernel_status == EN_KERNEL_NOT_INITIALIZED
            || g_kernel_status == EN_KERNEL_ERROR) {
        return;
    }

    // inform segger systick interrupt aka kernel_update was entered
    SEGGER_SYSVIEW_RECORD_ENTER_ISR();

    // Return during critical section, but only after recording for SEGGER
    if (g_kernel_critical_section_active) {
        SEGGER_SYSVIEW_RECORD_EXIT_ISR();
        return;
    }

    // handle delta times in delayed task list
    size_t status = -1;
    task_t *task = NULL;
    if (g_delayed_tasks->size>0) {
        task = (task_t *) g_delayed_tasks->tail->data;
        if (task->delta_time==0) {
            // reinsert task from delay list to its priority group
            status = kernel_reinsert_task(&g_delayed_tasks, &g_delayed_tasks->tail, &task);
        }
        else {
            // decrement delta time
            task->delta_time--;
        }
    }


    // check if time quantum has elapsed and update kernel state
    if (g_running_task_current->time_quantum_remaining == 0
            && !g_kernel_critical_section_active
            && g_kernel_status != EN_KERNEL_IDLE) {
        kernel_start_task(&g_priority_group_next, &g_linked_list_task_iterator_next, &g_running_task_next);
    }
    else if (g_kernel_status == EN_KERNEL_IDLE && status == KERNEL_SUCCESS) {
        // exit idle
        kernel_exit_idle();
    }
    else if (g_running_task_current->time_quantum_remaining > 0) {
        // decrement time quantum
        g_running_task_current->time_quantum_remaining--;
    }

    // inform segger systick interrupt aka kernel_update is about to exit
    SEGGER_SYSVIEW_RECORD_EXIT_ISR();
}

/**
 * @brief Executes the task context switch.
 * @param None
 * @return None
 * */
__attribute__((naked))
void kernel_schedule_task(void) {

    // 1. PendSv pushes stack frame on stack

    // 2. allocate stack memory for following operations and disable interrupts
    __asm volatile ("PUSH {LR}");
    // move stack pointer enough for variables
    __asm volatile ("SUB SP, SP, #8");
    // after moving the stackpointer functions and everything else can be done
    kernel_disable_interrupts();
    size_t status = 0;
    uint32_t psp;

    // 3. push R4-R11 on stack
    __asm volatile ("PUSH {R2}");
    __asm volatile ("MRS R2, PSP");
    __asm volatile ("STMDB R2!, {R4-R11}");
    __asm volatile ("MSR PSP, R2");

    // update system
    __ISB();

    // obtain psp
    psp = __get_PSP();


    // 4. load next task by using its psp and move memory location

    task_reset_time_quantum_remaining(&g_running_task_current);

    status = task_checking(&g_running_task_previous);
    if (status == TASK_SUCCESS) {
        // save previous tasks psp and update segger stack usage
        g_running_task_previous->task_data->u32TaskSP = psp;
        SEGGER_SET_STACKPOINTER(g_running_task_previous);
    }

    // get tasks psp and prepare for reentry
    psp = g_running_task_current->task_data->u32TaskSP;
    task_set_state(&g_running_task_current, TaskState_Running);



    __set_PSP(psp);

    // check if critical section is still set
    if (g_kernel_critical_section_active) {
        kernel_toggle_critical_section();
    }

    // pop R4-R11 from Stack in reverse order

    //__set_CONTROL(__get_CONTROL() | (1 << 1));
    __asm volatile ("MRS R2, PSP");
    __asm volatile ("LDMIA R2!, {R4-R11}");

    __asm volatile ("MSR PSP, R2");
    __asm volatile ("POP {R2}");
    __ISB();


    // 5. enable interrupts
    kernel_enable_interrupts();
    // free up stack and undo changes
    __asm volatile ("ADD SP, SP, #8");
    __asm volatile ("POP {LR}");

    // 6. leave PendSv Routine
    __asm volatile ("BX LR");
}

/**
 * @brief It is called by the current running task, which shall be blocked.
 *        Before it is blocked it might need to determine the next task.
 *        If no task was found, the kernel will be set to idle.
 * @param priority_group is a linked_list_t pointer of pointer to the task which shall be blocked.
 * @param linked_list_element is a linked_list_element_t pointer of pointer to the task which shall be blocked.
 * @param task is a task_t pointer of pointer to the task which shall be blocked.
 */
size_t kernel_swap_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task) {

    // set task to blocked and check for errors
    task_t *task_blocked = (*task);
    size_t status = task_set_state(task, TaskState_Blocked);
    if (status != TASK_SUCCESS) {
        kernel_set_status(EN_KERNEL_ERROR);
        return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
    }

    // check if next task was set
    if ((*priority_group)->size == 0
            || g_linked_list_task_iterator == NULL
            || g_running_task_next == NULL) {

        // determine next task
        linked_list_t *next_priority_group = NULL;
        do {
            status = dictionary_get(&g_prioritized_tasks, g_dictionary_priority_next, (void **) &next_priority_group);
            g_dictionary_priority_next++;
        } while (next_priority_group->size == 0 && g_dictionary_priority_next < KERNEL_MAX_TASK);

        if (next_priority_group->size == 0 && g_dictionary_priority_next == KERNEL_MAX_TASK) {
            // unable to find executable task in any priority, enter idle mode
            kernel_enter_idle();
            // assume next task was set by reinsert
        }
        else {
            // Make sure we are in a critical section
            if (!g_kernel_critical_section_active) {
                kernel_toggle_critical_section();
            }

            // update next task information
            g_dictionary_priority = g_dictionary_priority_next;
            g_priority_group_next = next_priority_group;
            g_linked_list_task_iterator_next = g_priority_group_next->tail;
            g_running_task_next = (task_t *) g_linked_list_task_iterator_next->data;
        }
    }

    // start next task and check for errors
    status = kernel_start_task(&g_priority_group_next, &g_linked_list_task_iterator_next, &g_running_task_next);
    if (status != KERNEL_SUCCESS) {
        return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SWAP);
    }

    // catch blocked task
    while(task_blocked->task_data->eTaskState == TaskState_Blocked);
    return KERNEL_SUCCESS;
}


/**
 * @brief Start the provided task and updates global information to the task by triggering kernel_schedule_task (PendSV_Handler).
 *        A following task will be determined or the same task is selected if its priority group just contains 1 task.
 * @param priority_group is a linked_list_t pointer of pointer to the task which shall be executed next.
 * @param linked_list_element is a linked_list_element_t pointer of pointer to the task which shall be executed next.
 * @param task is a task_t pointer of pointer to the task which shall be executed next.
 * */
size_t kernel_start_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task) {

    // update task state and check for errors
    size_t status = task_set_state(task, TaskState_Ready);
    if (status != TASK_SUCCESS) {
        kernel_set_status(EN_KERNEL_ERROR);
        return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
    }

    // set time quantum
    status = task_reset_time_quantum_remaining(task);
    if (status != TASK_SUCCESS) {
        kernel_set_status(EN_KERNEL_ERROR);
        return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
    }


    // set previous task and set it to ready if not blocked
    g_running_task_previous = g_running_task_current;
    if (g_running_task_previous != NULL && g_running_task_previous->task_data->eTaskState != TaskState_Blocked) {
        status = task_set_state(&g_running_task_previous, TaskState_Ready);
        if (status != TASK_SUCCESS && status != TASK_NO_MEMORY) {
            kernel_set_status(EN_KERNEL_ERROR);
            return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
        }
    }


    // implements priority inheritance
    /*
     * Inheritance starts with lowest priority group
     * and all tasks are moved higher at the same time.
     * To prevent starvation for higher task a cool down is needed,
     * because there can be more lower prioritized tasks.
     */
    static size_t inheritance_cooldown = 0;

    // lower_priority might point to the last used priority group
    // it needs to be prevented that the new priority group is being moved to a higher priority group by inheritance
    if (g_task_lower_priority <= g_dictionary_priority) {
        g_task_lower_priority = g_task_lowest_priority;
    }

    // move priority groups, if the current priority group is lower
    if (inheritance_cooldown == 0 && g_task_lower_priority > g_dictionary_priority) {

        // obtain priority group and check for errors
        linked_list_t *lower_priority_group = NULL;
        status = dictionary_get(&g_prioritized_tasks, g_task_lower_priority, (void **) &lower_priority_group);
        if (status != DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
        }

        // obtain higher priority group and check for errors
        g_task_lower_priority--;
        linked_list_t *higher_priority_group = NULL;
        status = dictionary_get(&g_prioritized_tasks, g_task_lower_priority, (void **) &higher_priority_group);
        if (status != DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_SCHEDULE_TASK);
        }

        // check if current priority group was reached and move priority group
        size_t moved_elements = lower_priority_group->size;

        linked_list_move_linked_list_after(&higher_priority_group, &lower_priority_group);

        if (g_dictionary_priority == g_task_lower_priority) {
            // reset inheritance and set cool down
            g_task_lower_priority = g_task_lowest_priority;
            inheritance_cooldown = moved_elements;
        }
    }

    // set current task information and preset next task
    g_running_task_current = *task;
    g_linked_list_task_iterator = *linked_list_element;
    g_priority_group_current = *priority_group;
    g_priority_group_next = *priority_group;

    if (g_linked_list_task_iterator != NULL) {
        // set next task
        g_linked_list_task_iterator_next = g_linked_list_task_iterator->next;
        if (g_linked_list_task_iterator_next == NULL) {
            // end of priority group and restart with first element
            g_linked_list_task_iterator_next = g_priority_group_current->tail;
        }
        g_running_task_next = (task_t *) g_linked_list_task_iterator_next->data;
        g_dictionary_priority_next = g_dictionary_priority + 1;
    }
    else if (g_priority_group_current->size == 0) {
        // jump to next priority group
        dictionary_get(&g_prioritized_tasks, g_dictionary_priority_next, (void **) &g_priority_group_current);
        g_priority_group_next = g_priority_group_current;
        g_linked_list_task_iterator = g_priority_group_current->tail;
        g_running_task_current = (task_t *) g_linked_list_task_iterator;
        g_dictionary_priority = g_dictionary_priority_next;
    }
    else {
        // end of priority group and restart with first element
        g_linked_list_task_iterator = g_priority_group_current->tail;
        g_running_task_current = (task_t *) g_linked_list_task_iterator->data;
        g_linked_list_task_iterator_next = g_linked_list_task_iterator->next;
        g_running_task_next = (task_t *) g_linked_list_task_iterator_next->data;
        g_dictionary_priority_next = g_dictionary_priority + 1;
    }

    // ------------------- critical section end ----------------------------
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // set PendSV-Flag

    kernel_enable_interrupts();

    return KERNEL_SUCCESS;
}

/**
 * @brief Set the kernel status depending on the kernel state.
 * It blocks when an allowed state should be entered or set to error state.
 * @return 0 on success or greater 0 on error
 * */
size_t kernel_set_status(Kernel_Status_e status) {

    // finite state machine for kernel states
    // it blocks on wrong transition or on an error
    switch (status) {
        case EN_KERNEL_STARTING:
            if (g_kernel_status != EN_KERNEL_NOT_INITIALIZED) {
                while (true);
            }
            break;
        case EN_KERNEL_RUNNING:
            if (!(g_kernel_status == EN_KERNEL_STARTING || g_kernel_status == EN_KERNEL_IDLE)) {
                while (true);
            }
            break;
        case EN_KERNEL_IDLE:
            if (g_kernel_status != EN_KERNEL_RUNNING) {
                while (true);
            }
            break;
        default :
            while(true);
    }

    g_kernel_status = status;

    return KERNEL_SUCCESS;
}

/**
 * @brief Set psp to a valid memory location to preset memory address.
 * @return None
 * */
size_t kernel_set_stack_pointer(void) {
    __set_PSP(__get_MSP());
    // update memory and switch between msp for interrupts and psp for tasks
    __set_CONTROL(__get_CONTROL() | (1 << 1));

    // update and flush system
    __ISB();
    __DSB();

    return KERNEL_SUCCESS;
}


/**
 * @brief Overwrite necessary system function by loading the vector table in memory first
 *           and replacing the vector functions.
 * @return None
 * @info PendSV_Handler is replaced by the kernel_schedule_task function.
 *          SysTick_Handler is replaced by the kernel_update function.
 * */
__attribute__((aligned(256)))
uint32_t vector_table[256];
void kernel_set_system_functions(void) {

    kernel_disable_interrupts();
    // copy vector table to memory and point to the new vector table
    memcpy(vector_table, (void *) SCB->VTOR, 96 * sizeof(uint32_t));
    SCB->VTOR = (uint32_t) vector_table;

    // lower number means higher interrupt priority
    // change priority if necessary
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    // ensures same priority and makes use of interrupt tail chaining
    NVIC_SetPriority(SysTick_IRQn, 0);

    // replace functions and update memory
    __NVIC_SetVector(PendSV_IRQn, (uint32_t) kernel_schedule_task);
    __NVIC_SetVector(SysTick_IRQn, (uint32_t) kernel_update);
    __DSB();
    __ISB();

    kernel_enable_interrupts();
}

/**
 * @brief Delays the task by the amount in milliseconds without context switch.
 * @param delay_millisecods is size_t, which is the amount to delay the current running task.
 * @return None
 * */
void kernel_delay_blocking(size_t delay_millisecods) {

    HAL_Delay((uint32_t) delay_millisecods);
}

/**
 * @brief Returns current Tick amount.
 * @return None
 * */
size_t kernel_get_tick(void) {
    return HAL_GetTick();
}

/**
 * @brief Enters idle state.
 * @return None
 * */
void kernel_enter_idle(void) {

    // set kernel to idle state to active block in debug session
    kernel_set_status(EN_KERNEL_IDLE);
    SEGGER_SYSVIEW_TASK_SYSTEM_IDLE();

    // critical section cannot be active when idle
    if (g_kernel_critical_section_active) {
        kernel_toggle_critical_section();
    }

    // Make sure interrupts are enabled to be able to recover from idle
    kernel_enable_interrupts();

    // when in debug session it can lead to unexpected behavior after sleep
#ifndef DEBUG
    // put system to sleep
    HAL_DBGMCU_EnableDBGSleepMode();
    HAL_PWR_EnableSleepOnExit();
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif

    // catch kernel in idle
    while (g_kernel_status == EN_KERNEL_IDLE);
}

/**
 * @brief Exits idle state.
 * @return None
 * */
void kernel_exit_idle(void) {
    kernel_set_status(EN_KERNEL_RUNNING);

    // when in debug session it can lead to unexpected behavior
#ifndef DEBUG
    HAL_DBGMCU_DisableDBGSleepMode();
    HAL_PWR_DisableSleepOnExit();
#endif
}


/**
 * @brief Disables interrupts.
 * @return None
 * */
void kernel_disable_interrupts(void) {
    __disable_irq();
}

/**
 * @brief Enables interrupts.
 * @return None
 * */
void kernel_enable_interrupts(void) {
    __enable_irq();
}

/**
 * @brief Terminates tasks by moving the task to a terminated task list.
 * @return None
 * */
__attribute__((naked))
void kernel_task_terminate(void) {

    // assign task return value to task
    __asm volatile("MOV %0, R0" : "=r"(g_running_task_current->return_value) : :);

    __asm volatile ("SUB SP, SP, #8");

    size_t status = linked_list_transfer(&g_terminated_tasks_list, &g_priority_group_current, &g_linked_list_task_iterator);
    if (status != LINKED_LIST_SUCCESS) {
        kernel_set_status(EN_KERNEL_ERROR);
    }

    // check if all tasks terminated
    if (g_available_tasks==g_terminated_tasks_list->size) {
        kernel_shutdown();
    }

    // switch to next task
    status = kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
    if (status != KERNEL_SUCCESS) {
        kernel_set_status(EN_KERNEL_ERROR);
    }
    // catch task on any error
    while (true);
}

/**
 * @briefs System is going to be turned off.
 * @return None
 */
void kernel_shutdown(void) {
    HAL_PWREx_EnterSHUTDOWNMode();
}
