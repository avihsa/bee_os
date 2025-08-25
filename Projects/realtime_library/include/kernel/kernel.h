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

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_
/* Includes */
#include "kernel/task.h"
#include "kernel/message_queue.h"
#include "utils/dictionary.h"
#include "kernel/semaphore.h"
#include "kernel/mutex.h"

#include <stddef.h>
/* Public Preprocessor defines */
#define KERNEL_MAX_TASK                     (256 / 4)
#define KERNEL_MAX_MESSAGE_QUEUE            8
#define KERNEL_DEFAULT_QUEUE_SIZE           8
#define KERNEL_MAX_SEMAPHORE                8
#define KERNEL_MAX_MUTEX                    8


#define KERNEL_SUCCESS                              0
#define KERNEL_NO_PRIORITIZED_TASKS                 1
#define KERNEL_NO_DELAYED_TASKS                     2
#define KERNEL_UNABLE_TO_ADD_TASK                   3
#define KERNEL_UNABLE_TO_ADD_MESSAGE_QUEUE          4
#define KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE       5
#define KERNEL_UNABLE_TO_SEND_MESSAGE               6
#define KERNEL_UNABLE_TO_RECEIVE_MESSAGE            7
#define KERNEL_NO_SEMAPHORES                        8
#define KERNEL_UNABLE_TO_ADD_SEMAPHORE              9
#define KERNEL_UNABLE_TO_ACQUIRE_SEMAPHORE          10
#define KERNEL_UNABLE_TO_RELEASE_SEMAPHORE          11
#define KERNEL_UNABLE_TO_CREATE_SEMAPHORE           12
#define KERNEL_UNABLE_TO_DELETE_SEMAPHORE           13
#define KERNEL_UNEXPECTED_SEMAPHORE_AVAILABILTY     14
#define KERNEL_UNABLE_TO_SWAP                       15
#define KERNEL_UNABLE_TO_SCHEDULE_TASK              16
#define KERNEL_UNABLE_TO_UPDATE                     17
#define KERNEL_IS_BLOCKED                           18
#define KERNEL_UNABLE_TO_REINSERT_TASK              19
#define KERNEL_UNABLE_TO_DELAY_TASK                 20
#define KERNEL_NO_LIST_OF_TASKS                     21
#define KERNEL_NO_BLOCKED_TASKS                     22
#define KERNEL_NO_RECEIVED_EVENTS                   23
#define KERNEL_UNABLE_TO_SEND_EVENTS                24
#define KERNEL_UNABLE_TO_RECEIVE_EVENTS             25
#define KERNEL_NO_MUTEXES                           26
#define KERNEL_UNABLE_TO_ADD_MUTEX                  27
#define KERNEL_UNABLE_TO_ACQUIRE_MUTEX              28
#define KERNEL_UNABLE_TO_RELEASE_MUTEX              29
#define KERNEL_UNABLE_TO_CREATE_MUTEX               30
#define KERNEL_UNABLE_TO_DELETE_MUTEX               31
#define KERNEL_UNEXPECTED_MUTEX_AVAILABILTY         32
#define KERNEL_UNABLE_TO_CHANGE_TASK_PRIORITY       33
#define KERNEL_NO_TERMINATED_TASKS_LIST             34
#define KERNEL_NO_MESSAGE_QUEUE                     35
#define KERNEL_UNABLE_TO_DELETE_LIST_OF_TASKS       36
#define KERNEL_UNABLE_TO_DELETE_PRIORITIZED_TASKS   37
#define KERNEL_UNABLE_TO_DELETE_MESSAGE_QUEUE_LIST  38
#define KERNEL_UNABLE_TO_DELETE_SEMAPHORE_LIST      39
#define KERNEL_UNABLE_TO_DELETE_MUTEX_LIST          40
#define KERNEL_UNABLE_TO_DELETE_BLOCKED_LIST        41
#define KERNEL_UNABLE_TO_DELETE_TERMINATED_LIST     42
#define KERNEL_UNABLE_TO_DELETE_PRIORITY_LIST       43


#define KERNEL_LENGTH                            6

// error registers
#define KERNEL_DICTIONARY_ERROR_REGISTER    DICTIONARY_NO_MEMORY
#define KERNEL_LINK_LIST_ERROR_REGISTER     KERNEL_DICTIONARY_ERROR_REGISTER + LINKED_LIST_LENGTH
#define KERNEL_TASK_ERROR_REGISTER          KERNEL_LINK_LIST_ERROR_REGISTER + TASK_LENGTH
#define KERNEL_MESSAGE_QUEUE_ERROR_REGISTER KERNEL_TASK_ERROR_REGISTER + MESSAGE_QUEUE_LENGTH
#define KERNEL_SEMAPHORE_ERROR_REGISTER     KERNEL_MESSAGE_QUEUE_ERROR_REGISTER + SEMAPHORE_LENGTH
#define KERNEL_MUTEX_ERROR_REGISTER         KERNEL_SEMAPHORE_ERROR_REGISTER + MUTEX_LENGTH
/* Public Preprocessor macros */
/* Public type definitions */
typedef enum {
  EN_KERNEL_NOT_INITIALIZED,
  EN_KERNEL_STARTING,
  EN_KERNEL_RUNNING,
  EN_KERNEL_ERROR,
  EN_KERNEL_IDLE,
  EN_KERNEL_MAX_STATE
} Kernel_Status_e;

/* Public functions (prototypes) */
/* Core Kernel */
size_t kernel_init(void);
size_t kernel_deinit(void);
size_t kernel_add_task(size_t (*task_main)(void), uint8_t u8_task_id, const char *task_name, uint8_t u8_task_priority, size_t time_quantum, size_t wanted_events, void (*notification_conditions)(size_t *, size_t), size_t timeout);
size_t kernel_start(void);
size_t kernel_delay(size_t delay_millisecods);

size_t kernel_message_queue_create(message_queue_identifier_t **message_queue_identifier, char *name, size_t queue_size, size_t element_size);
size_t kernel_message_queue_delete(message_queue_identifier_t **message_queue_identifier);
size_t kernel_message_queue_send(message_queue_identifier_t **message_queue_identifier, void *message, size_t element_size, bool urgent);
size_t kernel_message_queue_send_blocking(message_queue_identifier_t **message_queue_identifier, void *message, size_t element_size, bool urgent);
size_t kernel_message_queue_receive(message_queue_identifier_t **message_queue_identifier, void **message);

size_t kernel_semaphore_create(size_t *id, size_t tokens);
size_t kernel_semaphore_delete(size_t *id);
size_t kernel_semaphore_acquire(size_t id);
size_t kernel_semaphore_release(size_t id);
size_t kernel_semaphore_acquire_non_blocking(size_t id);
size_t kernel_semaphore_release_non_blocking(size_t id);
size_t kernel_semaphore_is_available(size_t id);

size_t kernel_mutex_create(size_t *id);
size_t kernel_mutex_delete(size_t *id);
size_t kernel_mutex_acquire(size_t id);
size_t kernel_mutex_release(size_t id);
size_t kernel_mutex_acquire_non_blocking(size_t id);
size_t kernel_mutex_release_non_blocking(size_t id);

size_t kernel_event_receive_timeout(size_t *received_events);
size_t kernel_event_receive_blocking(size_t *received_events);
size_t kernel_event_send(size_t task_id, size_t event);

size_t kernel_exit_to_scheduler(void);

#ifdef DEBUG
dictionary_t* kernel_debug_get_list_of_tasks(void);
#endif

/* STM Kernel */

void kernel_schedule_task(void);
void kernel_update(void);

void kernel_delay_blocking(size_t delay_millisecods);

size_t kernel_get_tick(void);

void kernel_enter_idle(void);
void kernel_exit_idle(void);

void kernel_disable_interrupts(void);
void kernel_enable_interrupts(void);

void kernel_shutdown(void);

#endif /* KERNEL_KERNEL_H_ */
