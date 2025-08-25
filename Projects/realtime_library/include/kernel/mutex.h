/**
**************************************************
* @file mutex.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Aug 2, 2025
* @brief Module for creating and using mutexes
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'mutex_create' to create a mutex
  (#) Call 'mutex_delete' to delete a mutex
  (#) Call 'mutex_acquire' to acquire a mutex
  (#) Call 'mutex_release' to release an acquired mutex
  (#) Call 'mutex_acquire_non_blocking' to acquire a mutex
  (#) Call 'mutex_release_non_blocking' to release an acquired mutex
==================================================
@endverbatim
**************************************************
*/

#ifndef KERNEL_MUTEX_H_
#define KERNEL_MUTEX_H_
/* Includes */

#include "kernel/semaphore.h"

/* Public Preprocessor defines */

#define MUTEX_SUCCESS       		0
#define MUTEX_NO_MEMORY				1
#define MUTEX_NO_SEMAPHORE			2
#define MUTEX_NO_OWNER_TASK			3
#define MUTEX_UNABLE_TO_DELETE 		4
#define MUTEX_IRREGULAR_STRUCTURE	5
#define MUTEX_UNABLE_TO_ACQUIRE		6
#define MUTEX_UNABLE_TO_RELEASE		7
#define MUTEX_OWNED_BY_OTHER_TASK	8

#define MUTEX_LENGTH				4

#define MUTEX_SEMAPHORE_ERROR_REGISTER 	MUTEX_LENGTH

/* Public Preprocessor macros */
/* Public type definitions */

/// control information for a mutex
typedef struct {
	semaphore_t *binary_semaphore;///< mutexes binary semaphore
	task_t *owner;///< mutexes task owner
	size_t lock_count;///< mutexes lock count
} mutex_t;

/* Public functions (prototypes) */
size_t mutex_create(mutex_t **mutex, size_t id);
size_t mutex_delete(mutex_t **mutex);
size_t mutex_acquire(mutex_t **mutex, linked_list_t **running_task_list, linked_list_element_t **running_task_element, task_t **task);
size_t mutex_release(mutex_t **mutex, linked_list_element_t **element, task_t **task);
size_t mutex_acquire_non_blocking(mutex_t **mutex, task_t **task);
size_t mutex_release_non_blocking(mutex_t **mutex, task_t **task);

#endif /* KERNEL_MUTEX_H_ */
