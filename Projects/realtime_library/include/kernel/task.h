/**
**************************************************
* @file task.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 20, 2025
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

#ifndef TASK_TASK_H_
#define TASK_TASK_H_
/* Includes */
#include <kernel/tcb.h>
#include <stddef.h>
#include "utils/support.h"
#include <stdbool.h>
#include "utils/linked_list.h"

/* Public Preprocessor defines */
#define TASK_SUCCESS			0
#define TASK_NO_MEMORY			1
#define TASK_DATA_NO_MEMORY		2
#define TASK_LENGTH				3
#define TASK_UNDEFINED_STATE	4
#define TASK_MAX_PRIORITY		(UINT8_MAX / 4)
/* Public Preprocessor macros */
/* Public type definitions */

/// control information for events
typedef struct {
	size_t wanted_events;///< wanted events for a task
	size_t received_events;///< received events from tasks
	void (*notification_conditions)(size_t *, size_t);///< notification condition for complex comunication
	size_t timeout; ///< event receive timeout
} event_register_t;

/// control information for a task
typedef struct {
	TCB_sctTCB_t *task_data;///< tasks data
	size_t (*task_main)(void);///< function pointer to the task
	size_t time_quantum;///< max time quantum
	size_t time_quantum_remaining;///< remaining time quantum
	SEGGER_SYSVIEW_TASKINFO_DECLARE(info);///< segger information, if activated
	size_t cause;///< tasks blocked cause
	char task_name[32];///< tasks name
	void *message;///< stored message from a message queue
	bool message_set;///< indicated, whether a message was set
	size_t delta_time;///< tasks delta time, if blocked and stored in a separate waiting list
	event_register_t event_register; ///< tasks event register
	linked_list_t *blocked_timeout_list;///< shows in which waiting list the task was stored, if receiving events
	linked_list_element_t *blocked_timeout_list_element;///< is the linked list element in a separate waiting list, if receiving events
	size_t return_value;///< tasks exit code
} task_t;
/* Public functions (prototypes) */
size_t task_create(task_t **task, size_t (*task_main)(void), void (*kernel_task_terminate)(void), uint8_t u8_task_id, const char *task_name, uint8_t u8_task_priority, size_t time_quantum, size_t wanted_events, void (*notification_conditions)(size_t *, size_t), size_t timeout);
size_t task_delete(task_t **task);
size_t task_set_state(task_t **task, TCB_eTastStates_t TaskState_Ready);
size_t task_reset_time_quantum_remaining(task_t **task);
size_t task_set_priority(task_t **task, uint8_t u8_task_priority);
size_t task_set_blocked_info(task_t **task, linked_list_t **blocked_timeout_list, linked_list_element_t **blocked_timeout_list_element);
size_t task_checking(task_t **task);
#endif /* TASK_TASK_H_ */
