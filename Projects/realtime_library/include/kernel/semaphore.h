/**
**************************************************
* @file semaphore.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 21, 2025
* @brief Module for creating and using semaphores
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'semaphore_create' to create a semaphore
  (#) Call 'semaphore_delete' to delete a semaphore
  (#) Call 'semaphore_acquire' to acquire a semaphore
  (#) Call 'semaphore_release' to release an acquired
      semaphore
  (#) Call 'semaphore_acquire_non_blocking' to acquire
      a semaphore
  (#) Call 'semaphore_release_non_blocking' to release
      an acquired semaphore
  (#) Call 'semaphore_is_available' to check if an existing
      semaphore is available
  (#) Call 'semaphore_flush' to flush all waiting list
      entries of a semaphore
  (#) All functions call 'semaphore_checking' to validate
      proper semaphore structure. Refer to this function
      for potential error codes not documented in each
      function.
==================================================
@endverbatim
**************************************************
*/

#ifndef KERNEL_SEMAPHORE_H_
#define KERNEL_SEMAPHORE_H_
/* Includes */
#include <stddef.h>
#include <stdint.h>

#include "utils/linked_list.h"
#include "kernel/task.h"

/* Public Preprocessor defines */
#define SEMAPHORE_SUCCESS               0
#define SEMAPHORE_NO_MEMORY             1
#define SEMAPHORE_NO_TOKENS             2
#define SEMAPHORE_NO_WAITING_LIST       3
#define SEMAPHORE_UNABLE_TO_ACQUIRE     4
#define SEMAPHORE_REACHED_MAX_TOKENS    5
#define SEMAPHORE_TOKENS_OVERFLOW       6
#define SEMAPHORE_UNABLE_TO_RELEASE     7

#define SEMAPHORE_LENGTH    			3
#define SEMAPHORE_BINARY_TOKEN  		1

#define SEMAPHORE_LINKED_LIST_ERROR_REGISTER SEMAPHORE_LENGTH
#define SEMAPHORE_TOKEN_AVAILABILITY_ERROR_REGISTER 2

/* Public Preprocessor macros */
/* Public type definitions */

/// Control information for a semaphore
typedef struct {
    size_t id;                          ///< semaphores id
    size_t token;                       ///< semaphores available tokens
    size_t max_token;                   ///< semaphores max tokens
    linked_list_t *task_waiting_list;   ///< linked list for storing blocked tasks
} semaphore_t;


/* Public functions (prototypes) */
size_t semaphore_create(semaphore_t **semaphore, size_t id, size_t token);
size_t semaphore_delete(semaphore_t **semaphore);
size_t semaphore_acquire(semaphore_t **semaphore, linked_list_t **running_task_list, linked_list_element_t **running_task_element, task_t **task);
size_t semaphore_release(semaphore_t **semaphore, linked_list_element_t **element, task_t **task);
size_t semaphore_acquire_non_blocking(semaphore_t **semaphore);
size_t semaphore_release_non_blocking(semaphore_t **semaphore);
size_t semaphore_is_available(semaphore_t **semaphore);
size_t semaphore_flush(semaphore_t **semaphore);

size_t semaphore_checking(semaphore_t **semaphore);

#endif /* KERNEL_SEMAPHORE_H_ */
