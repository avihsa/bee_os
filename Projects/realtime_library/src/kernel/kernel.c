/**
**************************************************
* @file kernel.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 17, 2025
* @brief Module for core kernel functionality
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'kernel_init' to initialize the kernel
  (#) Call 'kernel_deinit' to deinitialize the kernel
  (#) Call 'kernel_add_task' to add a task to be executed
  (#) Call 'kernel_start' to start the kernel
  (#) Call 'kernel_delay' to delay the running task

  (#) Call 'kernel_message_queue_create' to create
      a message queue
  (#) Call 'kernel_message_queue_delete' to delete
      a message queue
  (#) Call 'kernel_message_queue_send' to send a message
      to a task
  (#) Call 'kernel_message_queue_send_blocking' to send a
      blocking message to a task
  (#) Call 'kernel_message_queue_receive' to receive a message

  (#) Call 'kernel_semaphore_create' to create a semaphore
  (#) Call 'kernel_semaphore_delete' to delete a semaphore
  (#) Call 'kernel_semaphore_acquire' to acquire semaphore
  (#) Call 'kernel_semaphore_release' to release an
      acquired semaphore
  (#) Call 'kernel_semaphore_is_available' to check if an
      existing semaphore is available

  (#) Call 'kernel_mutex_create' to create a mutex
  (#) Call 'kernel_mutex_delete' to delete a mutex
  (#) Call 'kernel_mutex_acquire' to acquire a mutex
  (#) Call 'kernel_mutex_release' to release an
      acquired mutex

  (#) Call 'kernel_event_receive_timeout' to receive events
      for a set timeout period
  (#) Call 'kernel_event_receive_blocking' to receive events
      until they arrive
  (#) Call 'kernel_event_send' to send an event to a task

  (#) Call 'kernel_exit_to_scheduler' to force another task
      to run
  (#) Call 'kernel_toggle_critical_section' to toggle the
      current critical section status
  (#) Call 'kernel_reinsert_task' to move a task from blocked
      to running task list

==================================================
@endverbatim
**************************************************
*/
/* Includes */
#include "kernel/kernel.h"
#include "utils/support.h"
#include <stdbool.h>

/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */

// tasks
extern task_t                   *g_running_task_current;
extern task_t                   *g_running_task_next;
extern dictionary_t             *g_prioritized_tasks;
dictionary_t                    *g_list_of_tasks                    = NULL;
linked_list_t                   *g_delayed_tasks                    = NULL;
extern linked_list_element_t    *g_linked_list_task_iterator;
extern linked_list_element_t    *g_linked_list_task_iterator_next;
extern linked_list_t            *g_priority_group_current;
size_t                          g_task_lower_priority               = 0;
size_t                          g_task_lowest_priority              = 0;
extern uint8_t                  g_dictionary_priority;
extern uint8_t                  g_dictionary_priority_next;
extern linked_list_t            *g_priority_group_next;
size_t                          g_available_tasks                   = 0;

// message queues
dictionary_t                    *g_message_queue_list               = NULL;
size_t                          g_message_queue_ids                 = 0;

// semaphores
dictionary_t                    *g_semaphore_list                   = NULL;
size_t                          g_semaphore_ids                     = 0;

// mutexes
dictionary_t                    *g_mutex_list                       = NULL;
size_t                          g_mutex_ids                         = 0;

// kernel
extern Kernel_Status_e          g_kernel_status;
bool                            g_kernel_critical_section_active    = false;
linked_list_t                   *g_blocked_tasks                    = NULL;
linked_list_t                   *g_terminated_tasks_list            = NULL;


/* Static module functions (prototypes) */
extern size_t kernel_start_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task);
extern size_t kernel_swap_task(linked_list_t** priority_group, linked_list_element_t **linked_list_element, task_t **task);
extern size_t kernel_set_status(Kernel_Status_e status);
extern size_t kernel_set_stack_pointer(void);

void kernel_toggle_critical_section(void);
size_t kernel_reinsert_task(linked_list_t **source, linked_list_element_t **element, task_t **task);

extern void kernel_set_system_functions(void);
extern void kernel_task_terminate(void);


/* Public functions */
/**
 * @brief Initializes the kernel and prepares required structures.
 *        Call this function first before any kernel usage.
 * @param None
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_NO_PRIORITIZED_TASKS: unable to initialize prioritized tasks
 *  KERNEL_NO_LIST_OF_TASKS: unable to initialize tasks
 *  KERNEL_NO_DELAYED_TASKS: unable to initialize delayed tasks
 *  KERNEL_NO_MESSAGE_QUEUE: unable to initialize message queues
 *  KERNEL_NO_SEMAPHORES: unable to initialize semaphores
 *  KERNEL_NO_MUTEXES: unable to initialize mutexes
 *  KERNEL_NO_BLOCKED_TASKS: unable to initialize blocked tasks
 */
size_t kernel_init(void) {
    // set relevant system functions, depending on the used platform
    kernel_set_system_functions();
    // create prioritized task list with groups
    size_t status = dictionary_create(&g_prioritized_tasks, KERNEL_MAX_TASK);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_NO_PRIORITIZED_TASKS);
    }

    // add priority groups to priority list
    linked_list_t *priority_group = NULL;
    for (size_t priority = 0; priority<KERNEL_MAX_TASK; priority++) {

        status = linked_list_create(&priority_group);
        if (status!=LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_NO_PRIORITIZED_TASKS);
        }

        status = dictionary_add(&g_prioritized_tasks, priority, (void **) &priority_group);
        if (status!=DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_NO_PRIORITIZED_TASKS);
        }

    }

    // create dictionary which lists all tasks by there linked list element
    status = dictionary_create(&g_list_of_tasks, KERNEL_MAX_TASK);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_NO_LIST_OF_TASKS);
    }


    // create linked list which lists all delayed tasks
    status = linked_list_create(&g_delayed_tasks);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_NO_DELAYED_TASKS);
    }


    // using a dictionary for a fast access of message queues
    status = dictionary_create(&g_message_queue_list, KERNEL_MAX_MESSAGE_QUEUE);
    if (status!=DICTIONARY_SUCCESS) {
        return status;
    }

    // using a dictionary for a fast access of semaphores
    status = dictionary_create(&g_semaphore_list, KERNEL_MAX_SEMAPHORE);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_NO_SEMAPHORES);
    }

    status = dictionary_create(&g_mutex_list, KERNEL_MAX_MUTEX);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_NO_MUTEXES);
    }

    status = linked_list_create(&g_blocked_tasks);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_NO_BLOCKED_TASKS);
    }

    status = linked_list_create(&g_terminated_tasks_list);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_NO_TERMINATED_TASKS_LIST);
    }

    return KERNEL_SUCCESS;
}

size_t kernel_deinit(void) {

    kernel_disable_interrupts();
    size_t status = KERNEL_SUCCESS;

    // delete priority groups
    linked_list_t *priority_group = NULL;
    for (size_t priority = 0; priority<KERNEL_MAX_TASK; priority++) {

        status = dictionary_get(&g_prioritized_tasks, priority, (void **) &priority_group);
        if (status==DICTIONARY_SUCCESS) {
            linked_list_delete(&priority_group);
        }
    }

    status = dictionary_delete(&g_prioritized_tasks);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_PRIORITIZED_TASKS);
    }
    // delete delayed task linked list
    status = linked_list_delete(&g_delayed_tasks);


    // delete message queues
    message_queue_t *message_queue = NULL;
    for (size_t message_queue_id = 0; message_queue_id < g_message_queue_ids; message_queue_id++) {

        status = dictionary_get(&g_message_queue_list, message_queue_id, (void **) &message_queue);
        if (status==DICTIONARY_SUCCESS) {
            message_queue_delete(&message_queue);
        }
    }

    // reset message queue ids
    g_message_queue_ids = 0;

    // delete list of message queue
    status = dictionary_delete(&g_message_queue_list);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE_LIST);
    }

    // delete semaphores
    semaphore_t *semaphore = NULL;
    for (size_t semaphore_id = 0; semaphore_id < g_semaphore_ids; semaphore_id++) {

        status = dictionary_get(&g_semaphore_list, semaphore_id, (void **) &semaphore);
        if (status==DICTIONARY_SUCCESS) {
            semaphore_delete(&semaphore);
        }
    }

    // delete list of semaphores
    status = dictionary_delete(&g_semaphore_list);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_SEMAPHORE_LIST);
    }


    // delete mutexes
    mutex_t *mutex = NULL;
    for (size_t mutex_id = 0; mutex_id < g_mutex_ids; mutex_id++) {

        status = dictionary_get(&g_mutex_list, mutex_id, (void **) &mutex);
        if (status==DICTIONARY_SUCCESS) {
            mutex_delete(&mutex);
        }
    }

    // reset mutex ids
    g_mutex_ids = 0;

    // delete list of semaphores
    status = dictionary_delete(&g_mutex_list);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MUTEX_LIST);
    }


    status = linked_list_delete(&g_blocked_tasks);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_BLOCKED_LIST);
    }

    status = linked_list_delete(&g_terminated_tasks_list);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_TERMINATED_LIST);
    }


    // delete all tasks
    task_t *task = NULL;
    for (size_t tadk_id = 0; tadk_id<KERNEL_MAX_TASK; tadk_id++) {

        status = dictionary_get(&g_list_of_tasks, tadk_id, (void **) &task);
        if (status==DICTIONARY_SUCCESS) {
            task_delete(&task);
        }
    }

    // create dictionary which lists all tasks by there linked list element
    status = dictionary_delete(&g_list_of_tasks);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_LIST_OF_TASKS);
    }


    return KERNEL_SUCCESS;
}

/**
 * @brief Inserts a new task to be executed on kernel start.
 * @param task_main is a function pointer to the task function
 * @param u8_task_id is an uint8_t to set the task id
 * @param u8_task_priority is an uint8_t to indicate in which priority goup it shall be inserted in the prioritized task list
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ADD_TASK: unable to add task due to different subcomponent errors
 */
size_t kernel_add_task(size_t (*task_main)(void), uint8_t u8_task_id, const char *task_name, uint8_t u8_task_priority, size_t time_quantum, size_t wanted_events, void (*notification_conditions)(size_t *, size_t), size_t timeout) {

    // create a mandatory task structure as base for the tcb and relevant kernel attributes
    task_t *task = NULL;
    size_t status = task_create(&task, task_main, kernel_task_terminate, u8_task_id, task_name, u8_task_priority, time_quantum, wanted_events, notification_conditions, timeout);
    if (status!=TASK_SUCCESS) {
        return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
    }

    task_t *already_inserted_task = NULL;
    status = dictionary_get(&g_list_of_tasks, u8_task_id, (void **) &already_inserted_task);
    if (status == DICTIONARY_VALUE_IS_NULL) {
        status = dictionary_add(&g_list_of_tasks, u8_task_id, (void **) &task);
        if (status != DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
        }
    }
    else if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
    }

    // insert the task in the corresponding priority group
    linked_list_t *priority_group = NULL;
    status = dictionary_get(&g_prioritized_tasks, u8_task_priority, (void **) &priority_group);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
    }

    // always push a task at the linked list head, because the kernel uses the tail by default as first
    status = linked_list_push_front(&priority_group, (void **) &task);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
    }

    // preset for priority inheritance
    if (u8_task_priority > g_task_lowest_priority) {
        g_task_lower_priority = u8_task_priority;
        g_task_lowest_priority = u8_task_priority;
    }

    g_available_tasks++;

    return KERNEL_SUCCESS;
}

/**
 * @brief Starts the kernel with the previous added tasks.
 *        Is able to start multiple tasks, if the system supports it.
 *        If the system does not support parallel tasks, starts the firt task
 *        and never returns.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY: error during priority compacting due to subcomponents
 *  KERNEL_UNABLE_TO_ADD_TASK: unable to add task for starting
 */
size_t kernel_start(void){

    kernel_set_status(EN_KERNEL_STARTING);
    task_t *task = NULL;
    linked_list_t *priority_group = NULL;
    linked_list_element_t *task_iterator = NULL;


    size_t status = 0;

    // reallocate tasks by compacting
    for (size_t task_priority = 0; task_priority < TASK_MAX_PRIORITY; task_priority++) {

        status = dictionary_get(&g_prioritized_tasks, task_priority, (void **) &priority_group);
        if (status!=DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY);
        }

        if (priority_group->size > 0) {
            continue;
        }

        // empty priority list discovered
        linked_list_t *lower_priority_group = NULL;
        for (size_t lower_task_priority = task_priority; lower_task_priority <= TASK_MAX_PRIORITY; lower_task_priority++) {
            status = dictionary_get(&g_prioritized_tasks, lower_task_priority, (void **) &lower_priority_group);
            if (status==DICTIONARY_VALUE_IS_NULL) {
                continue;
            }
            else if (status!=DICTIONARY_SUCCESS) {
                return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY);
            }

            if (lower_priority_group->size == 0) {
                continue;
            }

            // update priority according to the priority group, where the tasks are being reinserted
            linked_list_element_t *update_task_priority_iterator = lower_priority_group->tail;
            while (update_task_priority_iterator != NULL) {
                task_t *update_task_priority = (task_t *) update_task_priority_iterator->data;
                status = task_set_priority(&update_task_priority, task_priority);
                if (status != TASK_SUCCESS) {
                    return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY);
                }
                update_task_priority_iterator = update_task_priority_iterator->next;
            }

            g_task_lower_priority = task_priority;
            g_task_lowest_priority = task_priority;

            linked_list_move_linked_list_after(&priority_group, &lower_priority_group);
            break;
        }
    }

    for (int priority = g_task_lowest_priority + 1; priority < KERNEL_MAX_TASK; priority++) {
        // iterate over potentially empty priority groups and delete the lists to save memory
        linked_list_t *priority_group = NULL;
        status = dictionary_get(&g_prioritized_tasks, priority, (void **) &priority_group);
        if (status!=DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY);
        }

        status = linked_list_delete(&priority_group);
        if (status != LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_PRIORITY_LIST);
        }
    }

    // set the alternative stack pointer to a default position
    kernel_set_stack_pointer();


    // iterate over all priority lists and start all containing tasks
    for (size_t task_priority=0; task_priority < TASK_MAX_PRIORITY; task_priority++) {

        // check, if the priority group was actually set or abort on any other error
        status = dictionary_get(&g_prioritized_tasks, task_priority, (void **) &priority_group);
        if (status==DICTIONARY_VALUE_IS_NULL) {
            continue;
        }
        else if (status!=DICTIONARY_SUCCESS) {
            return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_TASK);
        }

        // iterate over the priority group from the prioritized task list and start the task
        task_iterator = priority_group->tail;
        while (task_iterator!=NULL) {
            task = (task_t *) task_iterator->data;

            // check task before any use
            status = task_checking(&task);
            if (status==TASK_SUCCESS) {
                kernel_set_status(EN_KERNEL_RUNNING);
                status = kernel_start_task(&priority_group, &task_iterator, &task);
                if (status!=KERNEL_SUCCESS) {
                    return status;
                }
            }

            task_iterator = task_iterator->next;
        }
    }

    // it will return if the system is capable of running tasks in parallel
    return KERNEL_SUCCESS;
}

/**
 * @brief Create a new message queue for interprocess communication.
 * @param message_queue_identifier is a pointer of pointer to the message queue identifier, which is being used as a key to the message queue
 * @param name is a char pointer to a string, which can be used to identify the message queue
 * @param queue_size is a size_t for the amount of entries in a message queue
 * @param element_size is a size_t, which determines the size of each entry in the message queue
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ADD_MESSAGE_QUEUE: unable to add message queue due to subcomponents
 */
size_t kernel_message_queue_create(message_queue_identifier_t **message_queue_identifier, char *name, size_t queue_size, size_t element_size) {
    // return immediately if the amount of message queue exceeded
    if (g_message_queue_ids>=KERNEL_MAX_MESSAGE_QUEUE) {
        return KERNEL_UNABLE_TO_ADD_MESSAGE_QUEUE;
    }

    // create a new message on the provided parameters
    message_queue_t *message_queue = NULL;
    size_t status = message_queue_create(&message_queue, queue_size, element_size, g_message_queue_ids, name);
    if (status!=MESSAGE_QUEUE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_MESSAGE_QUEUE);
    }

    // add created message queue in a dictionary for fast access
    status = dictionary_add(&g_message_queue_list, g_message_queue_ids, (void **) &message_queue);
    if (status!=MESSAGE_QUEUE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_MESSAGE_QUEUE);
    }

    // increment the amount of message queue to limit the amount
    g_message_queue_ids++;

    // set the key to the message queue
    (*message_queue_identifier) = message_queue->message_queue_identifier;

    return KERNEL_SUCCESS;
}

/**
 * @brief Deletes a message queue.
 * @param message_queue_identifier is a pointer of pointer to the message queue identifier, which is being used as a key to the message queue
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error and the message_queue_identifier will be set to null
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE: unable to delete message queue due to subcomponents
 */
size_t kernel_message_queue_delete(message_queue_identifier_t **message_queue_identifier) {
    // check for valid key
    size_t status = message_queue_identifier_checking(message_queue_identifier);
    if (status!=MESSAGE_QUEUE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE);
    }

    // get the corresponding message queue
    message_queue_t *message_queue = NULL;
    status = dictionary_get(&g_message_queue_list, (*message_queue_identifier)->id, (void **) &message_queue);
    if (status!=DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE);
    }

    // delete the message queue and the key and the message shall become null
    status = message_queue_delete(&message_queue);
    if (status!=MESSAGE_QUEUE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE);
    }

    return KERNEL_SUCCESS;
}

/**
 * @brief Sends a message to a task or stores a message in the message queue.
 * @param message_queue_identifier is a message_queue_identifier pointer of pointer, which is being used as a key to the message queue
 * @param message is data to be send
 * @param element_size is the amount of bytes to stored
 * @param urgent is a bool, if set to true, the message queue becomes a lifo
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_SEND_MESSAGE: unable to send message due to subcomponents
 */
size_t kernel_message_queue_send(message_queue_identifier_t **message_queue_identifier, void *message, size_t element_size, bool urgent) {

    // check message queue identifier for irregular structure
    size_t status = message_queue_identifier_checking(message_queue_identifier);
    if (status!=MESSAGE_QUEUE_IDENTIFIER_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
    }


    // obtain message queue
    message_queue_t *message_queue = NULL;
    status = dictionary_get(&g_message_queue_list, (*message_queue_identifier)->id, (void **)&message_queue);
    if (status) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
    }

    // prevent other tasks of manipulating the message queue by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // send message and be ready for reinserting a task
    linked_list_element_t *element = NULL;
    task_t *task = NULL;
    status = message_queue_send(&message_queue, &element, &task, message, element_size, urgent);
    if (status!=MESSAGE_QUEUE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
    }

    if (task != NULL) {
        // task was returned
        kernel_reinsert_task(&message_queue->receiving_task_list, &element, &task);
    }

    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------

    return KERNEL_SUCCESS;
}


/**
 * @brief Sends a message to a task or stores a message in the message queue. The sender can be blocked
 * @param message_queue_identifier is a message_queue_identifier pointer of pointer, which is being used as a key to the message queue
 * @param message is data to be send
 * @param element_size is the amount of bytes to stored
 * @param urgent is a bool, if set to true, the message queue becomes a lifo
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_SEND_MESSAGE: unable to send message due to subcomponents
 */
size_t kernel_message_queue_send_blocking(message_queue_identifier_t **message_queue_identifier, void *message, size_t element_size, bool urgent) {

    // check message queue identifier for irregular structure
    size_t status = message_queue_identifier_checking(message_queue_identifier);
    if (status!=MESSAGE_QUEUE_IDENTIFIER_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
    }

    // obtain message queue
    message_queue_t *message_queue = NULL;
    status = dictionary_get(&g_message_queue_list, (*message_queue_identifier)->id, (void **)&message_queue);
    if (status) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
    }

    // try to send a message and be ready for reinserting a task
    linked_list_element_t *element = NULL;
    task_t *task = NULL;
    do {
        // prevent other tasks of manipulating the message queue by blocking the context switch
        // ------------------- critical section start -------------------------
        kernel_toggle_critical_section();
        status = message_queue_send_blocking(&message_queue, &g_priority_group_current, &g_linked_list_task_iterator, &element, &task, message, element_size, urgent);
        if (status==MESSAGE_QUEUE_SUCCESS) {
            // task succesfully received a message
            kernel_toggle_critical_section();
            // ------------------- critical section end ----------------------------
            break;
        }
        else if (status==MESSAGE_QUEUE_UNABLE_TO_SEND) {
            // task was inserted in the message queue waiting list
            kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
        }
        else {
            return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_MESSAGE);
        }
    } while(status == MESSAGE_QUEUE_UNABLE_TO_SEND);

    if (task != NULL) {
        // task was returned
        return kernel_reinsert_task(&message_queue->receiving_task_list, &element, &task);
    }

    return KERNEL_SUCCESS;
}

/**
 * @brief Receive a message or will be blocked until a message was received.
 * @param message_queue_identifier is a message_queue_identifier_t pointer of pointer to the message queue identifier, which is being used as a key to the message queue
 * @param message is a void pointer of pointer to the message content, which will be copied to the receiving task
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RECEIVE_MESSAGE: unable to receive message due to subcomponents
 */
size_t kernel_message_queue_receive(message_queue_identifier_t **message_queue_identifier, void **message) {

    // check message queue identifier for irregular structure
    size_t status = message_queue_identifier_checking(message_queue_identifier);
    if (status!=MESSAGE_QUEUE_IDENTIFIER_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_MESSAGE);
    }

    // obtain message queue
    message_queue_t *message_queue = NULL;
    status = dictionary_get(&g_message_queue_list, (*message_queue_identifier)->id, (void **) &message_queue);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_MESSAGE);
    }

    // try to receive a message and be ready for reinserting a task
    linked_list_element_t *sender_element = NULL;
    task_t *sender_task = NULL;
    do {
        // prevent other tasks of manipulating the message queue by blocking the context switch
        // ------------------- critical section start -------------------------
        kernel_toggle_critical_section();
        status = message_queue_receive(&message_queue, &g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current, &sender_element, &sender_task, message);
        if (status==MESSAGE_QUEUE_SUCCESS) {
            // task succesfully received a message
            kernel_toggle_critical_section();
            // ------------------- critical section end ----------------------------
            break;
        }
        else if (status==MESSAGE_QUEUE_UNABLE_TO_RECEIVE) {
            // task was inserted in the message queue waiting list
            kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
        }
        else {
            return ERROR_INFO(status, KERNEL_MESSAGE_QUEUE_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_MESSAGE);
        }

    } while (status == MESSAGE_QUEUE_UNABLE_TO_RECEIVE);

    if (sender_task!=NULL) {
        // task was returned
        return kernel_reinsert_task(&message_queue->sending_task_list, &sender_element, &sender_task);
    }

    return KERNEL_SUCCESS;
}

/**
 * @brief Creates a semaphore to block other tasks from entering a protected section.
 * @param id is a pointer of size_t, which is used as a key for fast access.
 * @param tokens is the maximum amount of tokens that can be acquired from a semaphore
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ADD_SEMAPHORE: unable to add semaphore due to subcomponents
 */
size_t kernel_semaphore_create(size_t *id, size_t tokens) {
    // return immediately if the amount of semaphores exceeded
    if (g_semaphore_ids >= KERNEL_MAX_SEMAPHORE) {
        return KERNEL_UNABLE_TO_ADD_SEMAPHORE;
    }

    // create semaphore and check for errors
    semaphore_t *semaphore = NULL;
    size_t status = semaphore_create(&semaphore, g_semaphore_ids, tokens);
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_SEMAPHORE);
    }

    // insert semaphore in a dictionary for fast access
    status = dictionary_add(&g_semaphore_list, g_semaphore_ids, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ADD_SEMAPHORE);
    }

    // assign key to the semaphore
    *id = g_semaphore_ids;

    // increment amount the amount of semaphores to limit the amount
    g_semaphore_ids++;
    return KERNEL_SUCCESS;
}

/**
 * @brief Deletes an existing semaphore.
 * @param id is a pointer of size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_DELETE_SEMAPHORE: unable to delete semaphore due to subcomponents
 */
size_t kernel_semaphore_delete(size_t *id) {
    semaphore_t *semaphore = NULL;
    size_t status = dictionary_get(&g_semaphore_list, *id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_SEMAPHORE);
    }

    while (semaphore->task_waiting_list->size > 0) {
        status = kernel_reinsert_task(&semaphore->task_waiting_list, &semaphore->task_waiting_list->tail, (task_t **) &semaphore->task_waiting_list->tail->data);
        if (status != KERNEL_SUCCESS) {
            return status;
        }
    }

    status = semaphore_delete(&semaphore);
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_SEMAPHORE);
    }

    *id = 0;

    return KERNEL_SUCCESS;
}

/**
 * @brief Acquires a semaphore to enter a critical section.
 *        The task will try to acquire the semaphore and will be blocked until it was successful.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE: unable to acquire semaphore due to subcomponents
 */
size_t kernel_semaphore_acquire(size_t id) {

    if (id >= KERNEL_MAX_SEMAPHORE) {
        return KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE;
    }

    // get the requested semaphore by the id from the semaphore dictionary
    semaphore_t* semaphore = NULL;
    size_t status = dictionary_get(&g_semaphore_list, id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE);
    }

    // try to obtain a token from the semaphore
    do {
        // prevent other tasks of manipulating the semaphore by blocking the context switch
        // ------------------- critical section start -------------------------
        kernel_toggle_critical_section();
        while (!g_kernel_critical_section_active);

        status = semaphore_acquire(&semaphore, &g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);

        if (status == SEMAPHORE_SUCCESS) {
            // acquired semaphore and semaphore will not be modified
            kernel_toggle_critical_section();
            // ------------------- critical section end ----------------------------
            break;
        }
        else if (status == SEMAPHORE_NO_TOKENS) {
            // task was inserted in the semaphore waiting list
            kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
        }
        else {
            // error happened with the semaphore
            return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE);
        }
    } while (status == SEMAPHORE_NO_TOKENS);

    return KERNEL_SUCCESS;
}


/**
 * @brief Releases a semaphore to exit a critical section.
 *        On every release a task will be moved to its corresponding priority group
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RELEASE_SEMAPHORE: unable to release semaphore due to subcomponents
 */
size_t kernel_semaphore_release(size_t id) {
    if (id >= KERNEL_MAX_SEMAPHORE) {
        return KERNEL_UNABLE_TO_RELEASE_SEMAPHORE;
    }

    // prevent other tasks of manipulating the semaphore by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested semaphore by the id from the semaphore dictionary
    semaphore_t* semaphore = NULL;
    size_t status = dictionary_get(&g_semaphore_list, id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS){
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_SEMAPHORE);
    }

    // release the semaphore and check, if a task can be moved from the waiting list to the corresponding priority group
    task_t* task = NULL;
    linked_list_element_t* element = NULL;
    status = semaphore_release(&semaphore, &element, &task);

    if (task!=NULL) {
        status = kernel_reinsert_task(&semaphore->task_waiting_list, &element, &task);
        if (status != KERNEL_SUCCESS) {
            return status;
        }
    }
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_SEMAPHORE);
    }

    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------


    return KERNEL_SUCCESS;

}

/**
 * @brief Acquires a semaphore to enter a critical section.
 *        The task will try to acquire the semaphore and returns, even if unsuccessful.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE: unable to acquire semaphore due to subcomponents
 */
size_t kernel_semaphore_acquire_non_blocking(size_t id) {

    if (id >= KERNEL_MAX_SEMAPHORE) {
        return KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE;
    }

    // prevent other tasks of manipulating the semaphore by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested semaphore by the id from the semaphore dictionary
    semaphore_t* semaphore = NULL;
    size_t status = dictionary_get(&g_semaphore_list, id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE);
    }

    // try to obtain a token from the semaphore
    status = semaphore_acquire_non_blocking(&semaphore);

    // acquired semaphore and semaphore will not be modified
    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------

    if (status != SEMAPHORE_SUCCESS) {
        // can be error or failed to acquire (SEMAPHORE_NO_TOKENS), handling required by caller
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE);
    }

    return KERNEL_SUCCESS;
}


/**
 * @brief Releases a semaphore to exit a critical section.
 *        Does not access the waiting list.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RELEASE_SEMAPHORE: unable to release semaphore due to subcomponents
 */
size_t kernel_semaphore_release_non_blocking(size_t id) {
    if (id >= KERNEL_MAX_SEMAPHORE) {
        return KERNEL_UNABLE_TO_RELEASE_SEMAPHORE;
    }

    // prevent other tasks of manipulating the semaphore by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested semaphore by the id from the semaphore dictionary
    semaphore_t* semaphore = NULL;
    size_t status = dictionary_get(&g_semaphore_list, id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS){
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_SEMAPHORE);
    }

    // release the semaphore
    status = semaphore_release_non_blocking(&semaphore);

    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------

    if (status != SEMAPHORE_SUCCESS) {
        // can be error or failed to release (SEMAPHORE_REACHED_MAX_TOKENS), handling required by caller
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_SEMAPHORE);
    }

    return KERNEL_SUCCESS;
}

/**
 * @brief Checks if a semaphore is avaialable for acquiring.
 * @param id is size_t, which is used as a key for fast access.
 * @return SEMAPHORE_SUCCESS if available, SEMAPHORE_NO_TOKENS if unavailable or unequal on error
 * @info the return value can be a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RELEASE_SEMAPHORE: unable to release semaphore due to subcomponents
 *  KERNEL_UNEXPECTED_SEMAPHORE_AVAILABILTY: unable to check availability due to subcomponents
 */
size_t kernel_semaphore_is_available(size_t id) {
    if (id >= KERNEL_MAX_SEMAPHORE)
        return KERNEL_UNEXPECTED_SEMAPHORE_AVAILABILTY;

    semaphore_t* semaphore = NULL;

    size_t status = dictionary_get(&g_semaphore_list, id, (void **) &semaphore);
    if (status != DICTIONARY_SUCCESS)
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNEXPECTED_SEMAPHORE_AVAILABILTY);

    status = semaphore_is_available(&semaphore);
    if (status == SEMAPHORE_SUCCESS || status == SEMAPHORE_NO_TOKENS) {
        return status;
    }

    return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNEXPECTED_SEMAPHORE_AVAILABILTY);
}


// MUTEX

/**
 * @brief Creates a mutex to block other tasks from entering a protected section.
 * @param id is a pointer of size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_CREATE_MUTEX: unable to create mutex due to subcomponents
 */
size_t kernel_mutex_create(size_t *id) {
    // return immediately if the amount of mutexes exceeded
    if (g_mutex_ids >= KERNEL_MAX_MUTEX) {
        return KERNEL_UNABLE_TO_CREATE_MUTEX;
    }

    // create mutex and check for errors
    mutex_t *mutex = NULL;
    size_t status = mutex_create(&mutex, g_mutex_ids);
    if (status != MUTEX_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_CREATE_MUTEX);
    }

    // insert mutex in a dictionary for fast access
    status = dictionary_add(&g_mutex_list, g_mutex_ids, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_CREATE_MUTEX);
    }

    // assign key to the mutex
    *id = g_mutex_ids;

    // increment amount the amount of mutexes to limit the amount
    g_mutex_ids++;

    return KERNEL_SUCCESS;
}

/**
 * @brief Deletes an existing mutex
 * @param id is a pointer of size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_DELETE_MUTEX: unable to delete mutex due to subcomponents
 */
size_t kernel_mutex_delete(size_t *id) {
    mutex_t *mutex = NULL;
    size_t status = dictionary_get(&g_mutex_list, *id, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MUTEX);
    }

    while (mutex->binary_semaphore->task_waiting_list->size > 0) {
        status = kernel_reinsert_task(&mutex->binary_semaphore->task_waiting_list, &mutex->binary_semaphore->task_waiting_list->tail, (task_t **) &mutex->binary_semaphore->task_waiting_list->tail->data);
        if (status != KERNEL_SUCCESS) {
            return status;
        }
    }

    status = mutex_delete(&mutex);
    if (status != MUTEX_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_DELETE_MUTEX);
    }

    *id = 0;

    return KERNEL_SUCCESS;
}

/**
 * @brief Acquires a mutex to enter a critical section.
 *        The task will try to acquire the mutex and will be blocked until it was successful.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ACQUIRE_MUTEX: unable to acquire mutex due to subcomponents
 */
size_t kernel_mutex_acquire(size_t id) {

    if (id >= KERNEL_MAX_MUTEX) {
        return KERNEL_UNABLE_TO_ACQUIRE_MUTEX;
    }

    // get the requested mutex by the id from the mutex dictionary
    mutex_t* mutex = NULL;
    size_t status = dictionary_get(&g_mutex_list, id, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
    }

    // try to obtain a token from the mutex
    do {
        // prevent other tasks of manipulating the mutex by blocking the context switch
        // ------------------- critical section start -------------------------
        kernel_toggle_critical_section();
        while (!g_kernel_critical_section_active);

        status = mutex_acquire(&mutex, &g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);

        if (status == MUTEX_SUCCESS) {
            // acquired semaphore and semaphore will not be modified
            kernel_toggle_critical_section();
            // ------------------- critical section end ----------------------------
            break;
        }
        else if (status == MUTEX_OWNED_BY_OTHER_TASK) {
            // task was inserted in the mutexes binary semaphore waiting list
            kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
        }
        else {
            // error happened with the mutex
            return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
        }
    } while (status == MUTEX_OWNED_BY_OTHER_TASK);

    return KERNEL_SUCCESS;
}


/**
 * @brief Releases a mutex to exit a critical section.
 *        On every release a task will be moved to its corresponding priority group
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RELEASE_MUTEX: unable to release mutex due to subcomponents
 */
size_t kernel_mutex_release(size_t id) {
    if (id >= KERNEL_MAX_MUTEX) {
        return KERNEL_UNABLE_TO_RELEASE_MUTEX;
    }

    // prevent other tasks of manipulating the mutex by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested mutex by the id from the mutex dictionary
    mutex_t* mutex = NULL;
    size_t status = dictionary_get(&g_mutex_list, id, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS){
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_MUTEX);
    }

    // release the mutex and check, if a task can be moved from the waiting list to the corresponding priority group
    task_t* task = g_running_task_current;
    linked_list_element_t* element = NULL;
    status = mutex_release(&mutex, &element, &task);

    if (task!=NULL) {
        status = kernel_reinsert_task(&mutex->binary_semaphore->task_waiting_list, &element, &task);
        if (status != KERNEL_SUCCESS) {
            return status;
        }
    }
    if (status != MUTEX_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_RELEASE_MUTEX);
    }

    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------


    return KERNEL_SUCCESS;

}

/**
 * @brief Acquires a mutex to enter a critical section.
 *        The task will try to acquire the mutex and returns, even if unsuccessful.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_ACQUIRE_MUTEX: unable to acquire mutex due to subcomponents
 */
size_t kernel_mutex_acquire_non_blocking(size_t id) {

    if (id >= KERNEL_MAX_MUTEX) {
        return KERNEL_UNABLE_TO_ACQUIRE_MUTEX;
    }

    // prevent other tasks of manipulating the mutex by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested mutex by the id from the mutex dictionary
    mutex_t* mutex = NULL;
    size_t status = dictionary_get(&g_mutex_list, id, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
    }

    // try to obtain a token from the mutex
    status = mutex_acquire_non_blocking(&mutex, &g_running_task_current);

    // acquired mutex and mutex will not be modified
    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------

    if (status != MUTEX_SUCCESS) {
        // error happened with the semaphore
        return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
    }

    return KERNEL_SUCCESS;
}


/**
 * @brief Release a mutex to exit a critical section.
 *        Does not manage the waiting list or blocked tasks.
 * @param id is size_t, which is used as a key for fast access.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RELEASE_MUTEX: unable to release mutex due to subcomponents
 */
size_t kernel_mutex_release_non_blocking(size_t id) {
    if (id >= KERNEL_MAX_MUTEX) {
        return KERNEL_UNABLE_TO_ACQUIRE_MUTEX;
    }

    // prevent other tasks of manipulating the mutex by blocking the context switch
    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // get the requested mutex by the id from the mutex dictionary
    mutex_t* mutex = NULL;
    size_t status = dictionary_get(&g_mutex_list, id, (void **) &mutex);
    if (status != DICTIONARY_SUCCESS){
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
    }

    // release the mutex and check, if a task can be moved from the waiting list to the corresponding priority group
    status = mutex_release_non_blocking(&mutex, &g_running_task_current);

    kernel_toggle_critical_section();
    // ------------------- critical section end ----------------------------

    if (status != MUTEX_SUCCESS) {
        return ERROR_INFO(status, KERNEL_MUTEX_ERROR_REGISTER, KERNEL_UNABLE_TO_ACQUIRE_MUTEX);
    }

    return KERNEL_SUCCESS;
}


/**
 * @brief Delays the task by the amount in milliseconds.
 * @param delay_millisecods is size_t, which is the amount to delay the current running task.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_DELAY_TASK: unable to delay tasks due to subcomponents
 * */
size_t kernel_delay(size_t delay_millisecods) {

    if (delay_millisecods == 0) {
        return KERNEL_SUCCESS;
    }

    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // checking for valid linked list
    size_t status = linked_list_checking(&g_delayed_tasks);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_DELAY_TASK);
    }

    // iterate over the delayed list and insert the task in the list by a delta
    task_t *task = g_running_task_current;
    linked_list_element_t *iterator = g_delayed_tasks->tail;
    size_t delta_sum = 0;
    task_t *delayed_task = NULL;

    // skip iteration if the list is still empty
    while (iterator != NULL) {

        // checking for valid task
        delayed_task = (task_t *) iterator->data;
        status = task_checking(&task);
        if (status != TASK_SUCCESS) {
            return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_DELAY_TASK);
        }

        // sum up the deltas to get the delayed time for each task
        delta_sum += delayed_task->delta_time;

        // check for abort conditions
        if (delta_sum > delay_millisecods) {
            // task is inserted before an already delayed task
            delta_sum -= delayed_task->delta_time;
            // update the delta before insertion to update the total delay for the already delayed task
            delayed_task->delta_time -= delay_millisecods - delta_sum;
            // go a step back, because the linked list supports only insert after
            iterator = iterator->previous;
            break;
        }
        else if (iterator->next == NULL) {
            // end of list reached
            break;
        }


        iterator = iterator->next;
    }

    // determine the delta time to delay the task
    task->delta_time = delay_millisecods - delta_sum;

    // insert task in delayed task list at the corresponding position determined by the iteration
    // a null iterator would mean to insert the element in the first position
    status = linked_list_transfer_after(&g_delayed_tasks, &iterator, &g_priority_group_current, &g_linked_list_task_iterator);
    if (status!=LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_DELAY_TASK);
    }

    kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);

    return KERNEL_SUCCESS;
}

/**
 * @brief Tries to receive events until a previously set timeout was reached.
 * @param received_events is a size_t pointer, which will return the set events.
 * @return KERNEL_SUCCESS on success, KERNEL_UNABLE_TO_RECEIVE_EVENTS on timeout or unequal on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RECEIVE_EVENTS: unable to receive events due to timeout or subcomponents
 * @note Be aware of the difference of returns:
 *       KERNEL_UNABLE_TO_RECEIVE_EVENTS as return signifies a timeout,
 *       the same with concatenated subcomponents signifies an error!
 * */
size_t kernel_event_receive_timeout(size_t *received_events) {
    // shorten the variables
    size_t task_wanted_events = g_running_task_current->event_register.wanted_events;
    size_t task_received_events = g_running_task_current->event_register.received_events;
    size_t status = KERNEL_UNABLE_TO_RECEIVE_EVENTS;

    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();


    // abort loop if wanted events were set
    task_received_events = g_running_task_current->event_register.received_events;
    if ((task_wanted_events & task_received_events) == task_wanted_events) {

        kernel_toggle_critical_section();
        // ------------------- critical section end ----------------------------
        status = KERNEL_SUCCESS;
    }
    else {

        task_t *task = g_running_task_current;
        size_t delay_milliseconds = task->event_register.timeout;
        // insert in delayed task list
        if (delay_milliseconds == 0) {
            return KERNEL_UNABLE_TO_RECEIVE_EVENTS;
        }

        // checking for valid linked list
        size_t status = linked_list_checking(&g_delayed_tasks);
        if (status!=LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
        }

        // iterate over the delayed list and insert the task in the list by a delta
        linked_list_element_t *iterator = g_delayed_tasks->tail;
        size_t delta_sum = 0;
        task_t *delayed_task = NULL;

        // skip iteration if the list is still empty
        while (iterator != NULL) {

            // checking for valid task
            delayed_task = (task_t *) iterator->data;
            status = task_checking(&task);
            if (status != TASK_SUCCESS) {
                return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
            }

            // sum up the deltas to get the delayed time for each task
            delta_sum += delayed_task->delta_time;

            // check for abort conditions
            if (delta_sum > delay_milliseconds) {
                // task is inserted before an already delayed task
                delta_sum -= delayed_task->delta_time;
                // update the delta before insertion to update the total delay for the already delayed task
                delayed_task->delta_time -= delay_milliseconds - delta_sum;
                // go a step back, because the linked list supports only insert after
                iterator = iterator->previous;
                break;
            }
            else if (iterator->next == NULL) {
                // end of list reached
                break;
            }

            iterator = iterator->next;
        }

        // determine the delta time to delay the task
        task->delta_time = delay_milliseconds - delta_sum;

        // insert task in delayed task list at the corresponding position determined by the iteration
        // a null iterator would mean to insert the element in the first position
        status = linked_list_transfer_after(&g_delayed_tasks, &iterator, &g_priority_group_current, &g_linked_list_task_iterator);
        if (status!=LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
        }

        if (iterator == NULL) {
            status = task_set_blocked_info(&g_running_task_current, &g_delayed_tasks, &g_delayed_tasks->tail);
        }
        else {
            status = task_set_blocked_info(&g_running_task_current, &g_delayed_tasks, &iterator->next);
        }
        if (status != TASK_SUCCESS) {
            return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
        }

        kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
        kernel_toggle_critical_section();

        task_received_events = g_running_task_current->event_register.received_events;
        if ((task_wanted_events & task_received_events) == task_wanted_events) {
            status = KERNEL_SUCCESS;
        }

        kernel_toggle_critical_section();
        // ------------------- critical section end ----------------------------
    }

    // provide set events
    *received_events = task_received_events;

    // delete received events
    g_running_task_current->event_register.received_events &= ~g_running_task_current->event_register.wanted_events;
    return status;
}

/**
 * @brief Tries to receive events until the events are set.
 * @param received_events is a size_t pointer, which will return the set events.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_RECEIVE_EVENTS: unable to receive events due to subcomponents
 * */
size_t kernel_event_receive_blocking(size_t *received_events) {

    // shorten the variables
    size_t task_wanted_events = g_running_task_current->event_register.wanted_events;
    size_t task_received_events = g_running_task_current->event_register.received_events;

    // preconfigure error
    size_t status = KERNEL_SUCCESS;
    while (true) {
        // ------------------- critical section start -------------------------
        kernel_toggle_critical_section();

        // abort loop if wanted events were set
        task_received_events = g_running_task_current->event_register.received_events;
        if ((task_wanted_events & task_received_events) == task_wanted_events) {

            kernel_toggle_critical_section();
            // ------------------- critical section end ----------------------------
            status = KERNEL_SUCCESS;
            break;
        }

        // transfer to blocked list
        status = linked_list_transfer(&g_blocked_tasks, &g_priority_group_current, &g_linked_list_task_iterator);
        if (status!=LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, KERNEL_LINK_LIST_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
        }

        // set info, where the task was inserted
        status = task_set_blocked_info(&g_running_task_current, &g_blocked_tasks, &g_blocked_tasks->head);
        if (status != TASK_SUCCESS) {
            return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_RECEIVE_EVENTS);
        }

        kernel_swap_task(&g_priority_group_current, &g_linked_list_task_iterator, &g_running_task_current);
    }

    // provide set events
    *received_events = task_received_events;

    // delete received events
    g_running_task_current->event_register.received_events &= ~g_running_task_current->event_register.wanted_events;
    return status;
}


/**
 * @brief Send events to tasks.
 * @param task_id is size_t to the receiving task
 * @param event is size_t with events to set at the corresponding task
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_SEND_EVENTS: unable to send events due to subcomponents
 * */
size_t kernel_event_send(size_t task_id, size_t event) {

    // ------------------- critical section start -------------------------
    kernel_toggle_critical_section();

    // obtain task were an event should be set
    task_t *task = NULL;
    size_t status = dictionary_get(&g_list_of_tasks, task_id, (void *) &task);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_EVENTS);
    }

    // shorten event variables and notification condition
    size_t wanted_events = task->event_register.wanted_events;
    size_t received_event = task->event_register.received_events;
    void (*notification_conditions)(size_t *, size_t) = task->event_register.notification_conditions;
    // preset received event in order to allow to reset them by the notification condition
    received_event |= event;

    if (notification_conditions!=NULL) {
        // allow to modify the received event
        notification_conditions(&received_event, wanted_events);
    }

    // update received event
    task->event_register.received_events |= received_event;

    // check if the addressed task can be reinserted
    if ((received_event & wanted_events) == wanted_events) {
        // check if addressed task was inserted in a blocked list or in a delayed task list
        if (task->blocked_timeout_list == g_blocked_tasks) {
            kernel_reinsert_task(&task->blocked_timeout_list, &task->blocked_timeout_list_element, &task);
        }
        else if (task->blocked_timeout_list == g_delayed_tasks) {
            // resume addressed task before timeout elapsed
            // check for a delayed task and update its delta time
            linked_list_element_t *next_delayed_task_element = task->blocked_timeout_list_element->next;
            if (next_delayed_task_element != NULL) {
                task_t *next_delayed_task = (task_t *) next_delayed_task_element->data;
                status = task_checking(&task);
                if (status!=TASK_SUCCESS) {
                    return ERROR_INFO(status, KERNEL_TASK_ERROR_REGISTER, KERNEL_UNABLE_TO_SEND_EVENTS);
                }
                // update delta time
                next_delayed_task->delta_time += task->delta_time;
                task->delta_time = 0;
            }

            kernel_reinsert_task(&task->blocked_timeout_list, &task->blocked_timeout_list_element, &task);
        }

        // addressed task is no longer inserted in a delayed task list or blocked list
        task->blocked_timeout_list = NULL;
        task->blocked_timeout_list_element = NULL;
    }


    kernel_toggle_critical_section();
    // ------------------- critical section end -------------------------

    return KERNEL_SUCCESS;
}


/**
 * @brief It will force to start a next task.
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info It inherits error codes from kernel_start_task.
 */
size_t kernel_exit_to_scheduler(void) {

    size_t status = KERNEL_SUCCESS;
    task_t *task = g_running_task_current;

    kernel_toggle_critical_section();
    status = kernel_start_task(&g_priority_group_next, &g_linked_list_task_iterator_next, &g_running_task_next);

    // Catch task
    while(task->task_data->eTaskState != TaskState_Running);

    return status;
}

/* Debug module functions (implementation) */
#ifdef DEBUG
dictionary_t* kernel_debug_get_list_of_tasks(void) {
    return g_list_of_tasks;
}
#endif

/* Static module functions (implementation) */

/**
 * @brief Toggle a critical section to prevent a task switch.
 * @info It allows atomic operations.
 * */
void kernel_toggle_critical_section(void) {
    if (g_kernel_critical_section_active) {
        kernel_disable_interrupts();
    }
    else {
        kernel_enable_interrupts();
    }
    g_kernel_critical_section_active = !g_kernel_critical_section_active;
}


/**
 * @brief Moves a task from blocked to running task list.
 * @param source is a linked_list_t pointer of pointer where an element shall be removed
 * @param element is a linked_list_element_t pointer of pointer which shall be transferred
 * @param task is a task_t pointer of pointer which shall be transferred
 * @return KERNEL_SUCCESS on success or unequal KERNEL_SUCCESS on error
 * @info the return value is a concatenated status error code based of subcomponents:
 *  KERNEL_UNABLE_TO_REINSERT_TASK: unable to reinsert task due to subcomponents
 * */
size_t kernel_reinsert_task(linked_list_t **source, linked_list_element_t **element, task_t **task) {
    // get the priority group for the task
    linked_list_t* priority_group = NULL;
    size_t status = dictionary_get(&g_prioritized_tasks, (*task)->task_data->u8TaskPrio, (void **) &priority_group);
    if (status != DICTIONARY_SUCCESS) {
        return ERROR_INFO(status, KERNEL_DICTIONARY_ERROR_REGISTER, KERNEL_UNABLE_TO_REINSERT_TASK);
    }


    // transfer the task from the source list to its priority group
    status = linked_list_transfer(&priority_group, source, element);
    if (status != LINKED_LIST_ELEMENT_SUCCESS) {
        return ERROR_INFO(status, KERNEL_SEMAPHORE_ERROR_REGISTER, KERNEL_UNABLE_TO_REINSERT_TASK);
    }

    // mark task as ready again
    task_set_state(task, TaskState_Ready);

    // the reinserted tasks priority might be higher than the current priority list
    // or the kernel wakes up from idle
    size_t incoming_priority = (*task)->task_data->u8TaskPrio;
    if (incoming_priority < g_dictionary_priority || g_kernel_status == EN_KERNEL_IDLE) {
        g_priority_group_next = priority_group;
        g_linked_list_task_iterator_next = priority_group->tail;
        g_running_task_next = *task;
        g_dictionary_priority_next = incoming_priority;
        g_dictionary_priority = incoming_priority;
    }
    else {
        // it is important to update the next task logic, if the moved task belongs to the current running priority group
        g_linked_list_task_iterator_next = g_linked_list_task_iterator->next;
        g_running_task_next = g_linked_list_task_iterator_next->data;
    }


    return KERNEL_SUCCESS;
}
