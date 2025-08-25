/**
**************************************************
* @file task.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date May 9, 2025
* @brief Module for creating and using tasks
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'task_create' to create a task
  (#) Call 'task_set_state' to set a tasks state
  (#) Call 'task_set_time_quantum_remaining' to set the
      remaining time quantum of a task
  (#) Call 'task_set_priority' to set a tasks priority
  (#) Call 'task_set_blocked_info' to set a tasks current
      blocked information of which list it belongs to
  (#) All functions call 'task_checking' to validate
      proper task structure. Refer to this function
      for potential error codes not documented in each
      function.
==================================================
@endverbatim
**************************************************
*/
/* Includes */
#include "kernel/task.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
/* Preprocessor defines */
#define DEFAULT_PSR    0x01000000
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */

/* Public functions */
/**
 * @brief Creates a task and fills the its structure with all of the necessary control information.
 * @param task is a task_t pointer of pointer to the created task
 * @param task_main is a function pointer with no arguments and size_t return. It references the tasks main function.
 * @param kernel_task_terminate is a function pointer with no arguments and no return. It references the tasks terminate function.
 * @param u8_task_id is a uint8_t, which is used as a key for fast access.
 * @param task_name is a const char pointer to the tasks name, which is displayed when recording in SEGGER SystemView
 * @param u8_task_priority is a uint8_t, which defines the tasks priority. A lower number specifies a higher priority
 * @param time_quantum is a size_t, which defines a tasks time quantum, its potential maximum consecutive runtime in milliseconds
 * @param wanted_events is a size_t, which defines which events the task expects to receive
 * @param notification_conditions is a function pointer with 2 arguments, a size_t pointer and a size_t, and no return.
 *        It references an optional function which can contain additional control of wanted events
 * @param timeout is a size_t, which defines the tasks timeout in millisecond when waiting for wanted events
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 * @info On error check for these errors:
 *  TASK_NO_MEMORY: unable to allocate memory for task
 *  TASK_DATA_NO_MEMORY: unable to allocate memory for task_data
 */
size_t task_create(task_t **task, size_t (*task_main)(void), void (*kernel_task_terminate)(void), uint8_t u8_task_id, const char *task_name, uint8_t u8_task_priority, size_t time_quantum, size_t wanted_events, void (*notification_conditions)(size_t *, size_t), size_t timeout) {
    (*task) = (task_t *) malloc(sizeof(task_t));

    if ((*task)==NULL) {
        return TASK_NO_MEMORY;
    }

    (*task)->task_main = task_main;
    (*task)->task_data = (TCB_sctTCB_t *) malloc(sizeof(TCB_sctTCB_t));

    if ((*task)->task_data==NULL) {
        return TASK_DATA_NO_MEMORY;
    }

    (*task)->task_data->u8TaskId = u8_task_id;
    (*task)->task_data->u8TaskPrio = u8_task_priority;
    (*task)->task_data->eTaskState = TaskState_Created;
    (*task)->time_quantum = time_quantum;
    (*task)->time_quantum_remaining = 0;
    (*task)->message = NULL;
    (*task)->message_set = false;
    (*task)->delta_time = 0;
    for (size_t task_register = 0; task_register < TCB_TASK_STACK_SIZE; task_register++) {
        (*task)->task_data->au32TaskStack[task_register] = 0;//task_register;
    }
    sprintf((*task)->task_name, "%d: %s", u8_task_id, task_name);

    (*task)->event_register.wanted_events = wanted_events;
    (*task)->event_register.received_events = 0;
    (*task)->event_register.notification_conditions = notification_conditions;
    (*task)->event_register.timeout = timeout;

    //#define __arm__
#ifdef __arm__
    // Stack Frame defaults
    //uint32_t *psr;
    //__asm volatile ("mov %0, xpsr" : "=r" (psr));
    (*task)->task_data->au32TaskStack[TCB_TASK_STACK_SIZE - 1] = DEFAULT_PSR;//(uint32_t) psr;        // xPSR
    (*task)->task_data->au32TaskStack[TCB_TASK_STACK_SIZE - 2] = (uint32_t) task_main;  // PC
    (*task)->task_data->au32TaskStack[TCB_TASK_STACK_SIZE - 3] = (uint32_t) kernel_task_terminate;//(uint32_t) 0xFFFFFFFF; // LR

    (*task)->task_data->u32TaskSP = (uint32_t) &(*task)->task_data->au32TaskStack[TCB_TASK_STACK_SIZE - 16];

    /*(*task)->task_data->au32TaskStack[0] = DEFAULT_PSR;
      (*task)->task_data->au32TaskStack[1] = (uint32_t) task_main;
      (*task)->task_data->au32TaskStack[3] = (uint32_t) 0xFFFFFFFF;
      (*task)->task_data->u32TaskSP = (uint32_t) &(*task)->task_data->au32TaskStack[0];*/
#else
    (*task)->task_data->u32TaskSP = 0;
#endif

#ifdef SEGGER
    (*task)->info.TaskID        = u8_task_id;
    (*task)->info.sName         = (*task)->task_name;
    (*task)->info.Prio            = u8_task_priority;
    (*task)->info.StackBase     = (uint32_t) &(*task)->task_data->au32TaskStack[TCB_TASK_STACK_SIZE];
    (*task)->info.StackSize     = TCB_TASK_STACK_SIZE;
    SEGGER_SET_STACKPOINTER(*task);
#endif

    task_set_state(task, TaskState_Created);
    return TASK_SUCCESS;
}

/**
 * @brief Deletes an existing task.
 * @param task is a task_t pointer of pointer to the task to be deleted
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 */
size_t task_delete(task_t **task) {
    size_t status = task_checking(task);
    if (status!=TASK_SUCCESS) {
        return status;
    }

    free((*task)->task_data);
    (*task)->task_data = NULL;

#ifdef SEGGER
    free((void *)(*task)->info.sName);
    (*task)->info.sName = NULL;
#endif

    free((*task));
    (*task) = NULL;


    return TASK_SUCCESS;
}

/**
 * @brief Sets a tasks state. Most useful for tracking multiple tasks in SEGGER SystemView
 * @param task is a task_t pointer of pointer to the task to set the state of
 * @param task_state is a TCB_eTastStates_t, which defines the state being set
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 * @info On error check for these errors:
 *  TASK_UNDEFINED_STATE: received unexpected task_state
 */
size_t task_set_state(task_t **task, TCB_eTastStates_t task_state) {
    size_t status = task_checking(task);
    if (status!=TASK_SUCCESS) {
        return status;
    }

    (*task)->task_data->eTaskState = task_state;

    switch (task_state) {
        case TaskState_Created:
            SEGGER_SYSVIEW_TASK_CREATE((*task)->info.TaskID);
            SEGGER_SYSVIEW_SEND_TASK_INFO(&((*task)->info));
            break;
        case TaskState_Ready:
            SEGGER_SYSVIEW_TASK_START_READY((*task)->info.TaskID);
            break;
        case TaskState_Running:
            SEGGER_SYSVIEW_TASK_START_EXEC((*task)->info.TaskID);
            break;
        case TaskState_Blocked:
            SEGGER_SYSVIEW_TASK_STOP_READY((*task)->info.TaskID, (*task)->cause);
            break;
        case TaskState_Deleted:
            SEGGER_SYSVIEW_TASK_STOP_EXEC();
            break;
        default:
            return TASK_UNDEFINED_STATE;
    }

    return TASK_SUCCESS;
}

/**
 * @brief Resets a tasks remaining time quantum back to its maximum value.
 * @param task is a task_t pointer of pointer, which references the task of which to reset the time quantum
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 */
size_t task_reset_time_quantum_remaining(task_t **task) {
    size_t status = task_checking(task);
    if (status!=TASK_SUCCESS) {
        return status;
    }

    (*task)->time_quantum_remaining = (*task)->time_quantum;

    return TASK_SUCCESS;
}

/**
 * @brief Sets a tasks priority. Note that a lower value signifies a higher priority.
 * @param task is a task_t pointer of pointer, which references the task of which to set the priority
 * @param u8_task_priority is a uint8_t, which specifies the priority to set the task to.
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 */
size_t task_set_priority(task_t **task, uint8_t u8_task_priority) {
    size_t status = task_checking(task);
    if (status!=TASK_SUCCESS) {
        return status;
    }

    (*task)->task_data->u8TaskPrio = u8_task_priority;

#ifdef SEGGER
    (*task)->info.Prio             = u8_task_priority;
#endif

    return TASK_SUCCESS;
}

/**
 * @brief Sets a tasks current blocked information, showing which list it belongs to.
 * @param task is a task_t pointer of pointer, which references the task of which to set the corresponding blocked list
 * @param blocked_timeout_list is a linked_list_t pointer of pointer, which references the list the task currently belongs to
 * @param blocked_timeout_list_element is a linked_list_element_t pointer of pointer, which references the linked list element containing the task
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 */
size_t task_set_blocked_info(task_t **task, linked_list_t **blocked_timeout_list, linked_list_element_t **blocked_timeout_list_element) {
    size_t status = task_checking(task);
    if (status != TASK_SUCCESS) {
        return status;
    }

    (*task)->blocked_timeout_list = *blocked_timeout_list;
    (*task)->blocked_timeout_list_element = *blocked_timeout_list_element;

    return TASK_SUCCESS;
}

/**
 * @brief Checks whether a task is valid.
 * @param task is a task_t pointer of pointer to the task to be checked
 * @return TASK_SUCCESS on success or unequal TASK_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  TASK_NO_MEMORY: task is null
 *  TASK_DATA_NO_MEMORY: task_data is null
 */
size_t task_checking(task_t **task) {
    if ((*task)==NULL) {
        return TASK_NO_MEMORY;
    }
    else if ((*task)->task_data==NULL) {
        return TASK_DATA_NO_MEMORY;
    }

    return TASK_SUCCESS;
}


/* Static module functions (implementation) */
