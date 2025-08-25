/**
**************************************************
* @file semaphore.c
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
/* Includes */
#include <stdlib.h>
#include "kernel/semaphore.h"
#include "utils/support.h"

/* Preprocessor defines */

/* Preprocessor macros */

/* Module intern type definitions */

/* Static module variables */

/* Static module functions (prototypes) */

/* Public functions */
/**
 * @brief Creates a semaphore with an id and its maximum amount of tokens.
 * @param semaphore is a pointer of pointer to be initialized as a semaphore
 * @param id is the unique id of the semaphore with which it is accessed
 * @param token is maximum amount of tokens, specifying how many times
 *        the semaphore can be acquired
 * @return SEMAPHORE_SUCCESS on success or unequal SEMAPHORE_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_NO_MEMORY: unable to allocate memory for semaphore
 *  SEMAPHORE_NO_WAITING_LIST: unable to initialize waiting list
 */
size_t semaphore_create(semaphore_t **semaphore, size_t id, size_t token) {
    *semaphore = (semaphore_t *) malloc(sizeof(semaphore_t));

    if (semaphore == NULL)
        return SEMAPHORE_NO_MEMORY;

    (*semaphore)->id = id;
    (*semaphore)->token = token;
    (*semaphore)->max_token = token;

    size_t status = linked_list_create(&(*semaphore)->task_waiting_list);
    if (status != LINKED_LIST_SUCCESS)
        return ERROR_INFO(status, SEMAPHORE_LENGTH, SEMAPHORE_NO_WAITING_LIST);

    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Deletes a semaphore
 * @param semaphore is a pointer of pointer to be deleted
 * @return SEMAPHORE_SUCCESS on success or unequal SEMAPHORE_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_NO_WAITING_LIST: unable to delete waiting list
 */
size_t semaphore_delete(semaphore_t **semaphore) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS)
        return status;

    status = linked_list_delete(&(*semaphore)->task_waiting_list);
    if (status != LINKED_LIST_SUCCESS)
        return ERROR_INFO(status, SEMAPHORE_LENGTH, SEMAPHORE_NO_WAITING_LIST);

    free(*semaphore);
    *semaphore = NULL;

    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Attempts to a acquire a semaphore. Moves the running task to the waiting list on failure.
 * @param semaphore is a pointer of pointer to the semaphore to be acquired
 * @param running_task_list is a pointer of pointer to the linked list of ready or running tasks
 * @param running_task_element is a pointer of pointer to the linked list element containing the current running task
 * @param task is a pointer of pointer to the current running task attempting to acquire the semaphore
 * @return SEMAPHORE_SUCCESS on success, SEMAPHORE_NO_TOKENS for unavailable semaphore or unequal for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_UNABLE_TO_ACQUIRE: unable to acquire due to linked list error
 */
size_t semaphore_acquire(semaphore_t **semaphore, linked_list_t **running_task_list, linked_list_element_t **running_task_element, task_t **task) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS) {
        return status;
    }

    if ((*semaphore)->token == 0) {
        status = linked_list_transfer(&((*semaphore)->task_waiting_list), running_task_list, running_task_element);

        if (status != LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, SEMAPHORE_LINKED_LIST_ERROR_REGISTER, SEMAPHORE_UNABLE_TO_ACQUIRE);
        }

        return SEMAPHORE_NO_TOKENS;
    }


    (*semaphore)->token--;
    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Attempts to a release a semaphore. Supplies the first task in the waiting list on success.
 * @param semaphore is a pointer of pointer to the semaphore to be released
 * @param element is a pointer of pointer to a linked list element expecting the first element of the waiting list
 * @param task is a pointer of pointer to a task expecting the data of the first element of the waiting list
 * @return SEMAPHORE_SUCCESS on success, SEMAPHORE_REACHED_MAX_TOKENS for fully available semaphore or unequal for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_UNABLE_TO_RELEASE: unable to release due to linked list error
 */
size_t semaphore_release(semaphore_t **semaphore, linked_list_element_t **element, task_t **task) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS) {
        return status;
    }

    if ((*semaphore)->token >= (*semaphore)->max_token) {
        return SEMAPHORE_REACHED_MAX_TOKENS;
    }

    if ((*semaphore)->task_waiting_list->size != 0) {

        *element = (*semaphore)->task_waiting_list->head;
        status = linked_list_element_checking(element);

        if (status != LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, SEMAPHORE_LINKED_LIST_ERROR_REGISTER, SEMAPHORE_UNABLE_TO_RELEASE);
        }

        *task = (*element)->data;
    }
    else {
        // mutex binary semaphore return task must be null
        *task = NULL;
    }

    (*semaphore)->token++;
    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Attempts to a acquire a semaphore. Does not move the running task to the waiting list on failure.
 * @param semaphore is a pointer of pointer to the semaphore to be acquired
 * @return SEMAPHORE_SUCCESS on success, SEMAPHORE_NO_TOKENS for unavailable semaphore or unequal for an error
 */
size_t semaphore_acquire_non_blocking(semaphore_t **semaphore) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS)
        return status;

    if ((*semaphore)->token == 0){
        return SEMAPHORE_NO_TOKENS;
    }

    (*semaphore)->token--;
    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Attempts to a release a semaphore. Does not check the waiting list on success.
 * @param semaphore is a pointer of pointer to the semaphore to be released
 * @return SEMAPHORE_SUCCESS on success, SEMAPHORE_REACHED_MAX_TOKENS for fully available semaphore or unequal for an error
 */
size_t semaphore_release_non_blocking(semaphore_t **semaphore) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS) {
        return status;
    }

    if ((*semaphore)->token >= (*semaphore)->max_token) {
        return SEMAPHORE_REACHED_MAX_TOKENS;
    }

    (*semaphore)->token++;
    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Checks if a semaphore is avaialable for acquiring.
 * @param semaphore is a pointer of pointer to the semaphore to be checked
 * @return SEMAPHORE_SUCCESS on success, SEMAPHORE_NO_TOKENS for fully acquired semaphore or unequal for an error
 */
size_t semaphore_is_available(semaphore_t **semaphore) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS)
        return ERROR_INFO(status, SEMAPHORE_TOKEN_AVAILABILITY_ERROR_REGISTER, SEMAPHORE_NO_MEMORY);

    if ((*semaphore)->token == 0)
        return SEMAPHORE_NO_TOKENS;

    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Deletes all entries from a semaphores waiting list.
 * @param semaphore is a pointer of pointer to the semaphore to be flushed
 * @return SEMAPHORE_SUCCESS on success or unequal for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_NO_WAITING_LIST: unable to flush waiting list
 */
size_t semaphore_flush(semaphore_t **semaphore) {
    size_t status = semaphore_checking(semaphore);
    if (status != SEMAPHORE_SUCCESS)
        return status;

    while ((*semaphore)->task_waiting_list->size != 0)
    {
        status = linked_list_pop_back(&(*semaphore)->task_waiting_list, NULL);
        if (status != LINKED_LIST_SUCCESS)
            return ERROR_INFO(status, SEMAPHORE_LINKED_LIST_ERROR_REGISTER, SEMAPHORE_NO_WAITING_LIST);
    }

    return SEMAPHORE_SUCCESS;
}

/**
 * @brief Checks whether a semaphore is valid.
 * @param semaphore is a pointer of pointer to the semaphore to be checked
 * @return SEMAPHORE_SUCCESS on success or unequal SEMAPHORE_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  SEMAPHORE_NO_MEMORY: semaphore is null
 *  SEMAPHORE_TOKENS_OVERFLOW: semaphores tokens exceeded max tokens
 *  SEMAPHORE_NO_WAITING_LIST: error in semaphores waiting list
 */
size_t semaphore_checking(semaphore_t **semaphore) {
    if (*semaphore == NULL)
        return SEMAPHORE_NO_MEMORY;

    if ((*semaphore)->token > (*semaphore)->max_token)
        return SEMAPHORE_TOKENS_OVERFLOW;

    size_t status = linked_list_checking(&((*semaphore)->task_waiting_list));
    if (status != LINKED_LIST_SUCCESS)
        return ERROR_INFO(status, SEMAPHORE_LINKED_LIST_ERROR_REGISTER, SEMAPHORE_NO_WAITING_LIST);


    return SEMAPHORE_SUCCESS;
}

/* Static module functions (implementation) */
