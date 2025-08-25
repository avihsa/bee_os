/**
**************************************************
* @file mutex.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Aug 3, 2025
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
/* Includes */
#include <stdlib.h>
#include "kernel/mutex.h"
#include "kernel/semaphore.h"
#include "utils/support.h"

/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */

size_t mutex_checking(mutex_t **mutex);

/* Public functions */
/**
 * @brief Creates a mutex with an id.
 * @param mutex is a pointer of pointer to be initialized as a mutex
 * @param id is the unique id of the contained binary semaphore with which it is accessed
 * @return MUTEX_SUCCESS on success or unequal MUTEX_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_NO_MEMORY: unable to allocate memory for mutex
 *  MUTEX_NO_SEMAPHORE: unable to initialize binary semaphore
 */
size_t mutex_create(mutex_t **mutex, size_t id) {
    *mutex = (mutex_t*) malloc(sizeof(mutex_t));

    if (mutex == NULL) {
        return MUTEX_NO_MEMORY;
    }

    // mutex only contains binary semaphore
    size_t status = semaphore_create(&((*mutex)->binary_semaphore), id, SEMAPHORE_BINARY_TOKEN);
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, MUTEX_LENGTH, MUTEX_NO_SEMAPHORE);
    }

    (*mutex)->owner = NULL;
    (*mutex)->lock_count = 0;

    return MUTEX_SUCCESS;
}

/**
 * @brief Deletes a mutex.
 * @param mutex is a pointer of pointer to be deleted
 * @return MUTEX_SUCCESS on success or unequal MUTEX_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_NO_SEMAPHORE: unable to delete binary semaphore
 *  MUTEX_UNABLE_TO_DELETE: unable to delete mutex
 */
size_t mutex_delete(mutex_t **mutex) {
    size_t status = mutex_checking(mutex);
    if (status != MUTEX_SUCCESS) {
        return status;
    }

    status = semaphore_delete(&((*mutex)->binary_semaphore));
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_NO_SEMAPHORE);
    }

    if ((*mutex)->owner != NULL) {
        return MUTEX_UNABLE_TO_DELETE;
    }

    free(*mutex);
    *mutex = NULL;

    return MUTEX_SUCCESS;
}

/**
 * @brief Attempts to a acquire a mutex. Waiting list is managed by semaphore_acquire if needed.
 * @param mutex is a pointer of pointer to the mutex to be acquired
 * @param running_task_list is a pointer of pointer to the linked list of ready or running tasks
 * @param running_task_element is a pointer of pointer to the linked list element containing the current running task
 * @param task is a pointer of pointer to the current running task attempting to acquire the mutex
 * @return MUTEX_SUCCESS on success, MUTEX_OWNED_BY_OTHER_TASK for unavailable mutex or unequal for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_UNABLE_TO_ACQUIRE: unable to acquire due to semaphore error
 */
size_t mutex_acquire(mutex_t **mutex, linked_list_t **running_task_list, linked_list_element_t **running_task_element, task_t **task) {
    size_t status = mutex_checking(mutex);
    if (status != MUTEX_SUCCESS) {
        return status;
    }

    // Mutex can only be acquired if either:
    // - it is not currently acquired (owner == NULL)
    // - the owner tries to acquire it consecutively (recursive locking) (task == owner)
    if (*task == (*mutex)->owner) {
        (*mutex)->lock_count++;
    }
    else {
        // semaphore_acquire catches the owner handling by design:
        // If owner is NULL, we can just normally acquire the semaphore.
        // If calling task is not the owner, semaphore returns NO_TOKENS and exits.
        status = semaphore_acquire(&((*mutex)->binary_semaphore), running_task_list, running_task_element, task);
        if (status == SEMAPHORE_NO_TOKENS) {
            return MUTEX_OWNED_BY_OTHER_TASK;
        }
        else if (status != SEMAPHORE_SUCCESS) {
            return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_UNABLE_TO_ACQUIRE);
        }

        (*mutex)->owner = *task;
        (*mutex)->lock_count++;
    }

    return MUTEX_SUCCESS;
}

/**
 * @brief Attempts to a release a mutex. Task supply is managed by semaphore_release if needed.
 * @param mutex is a pointer of pointer to the mutex to be released
 * @param element is a pointer of pointer to a linked list element expecting the first element of the waiting list
 * @param task is a pointer of pointer to a task expecting the data of the first element of the waiting list
 * @return MUTEX_SUCCESS on success, MUTEX_OWNED_BY_OTHER_TASK for unavailable mutex or unequal for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_UNABLE_TO_RELEASE: unable to release due to semaphore error
 */
size_t mutex_release(mutex_t **mutex, linked_list_element_t **element, task_t **task) {
    size_t status = mutex_checking(mutex);
    if (status != MUTEX_SUCCESS) {
        return status;
    }

    if (*task != (*mutex)->owner) {
        return MUTEX_OWNED_BY_OTHER_TASK;
    }

    if ((*mutex)->lock_count == 1) {
        status = semaphore_release(&((*mutex)->binary_semaphore), element, task);
        if (status != SEMAPHORE_SUCCESS) {
            return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_UNABLE_TO_RELEASE);
        }

        (*mutex)->owner = NULL;
    }

    (*mutex)->lock_count--;

    return MUTEX_SUCCESS;
}

/**
 * @brief Attempts to a acquire a mutex. Does not move the running task to the waiting list on failure.
 * @param mutex is a pointer of pointer to the semaphore to be acquired
 * @param task is a pointer of pointer to the current running task attempting to acquire the mutex
 * @return MUTEX_SUCCESS on success, MUTEX_OWNED_BY_OTHER_TASK for unavailable mutex or unequal for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_UNABLE_TO_ACQUIRE: unable to acquire due to semaphore error
 */
size_t mutex_acquire_non_blocking(mutex_t **mutex, task_t **task) {
    size_t status = mutex_checking(mutex);
    if (status != MUTEX_SUCCESS) {
        return status;
    }

    if ((*mutex)->owner == NULL) {
        status = semaphore_acquire_non_blocking(&((*mutex)->binary_semaphore));
        if (status != SEMAPHORE_SUCCESS) {
            return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_UNABLE_TO_ACQUIRE);
        }

        (*mutex)->owner = *task;
        (*mutex)->lock_count++;
    }
    else if (*task == (*mutex)->owner) {
        (*mutex)->lock_count++;
    }
    else {
        return MUTEX_OWNED_BY_OTHER_TASK;
    }

    return MUTEX_SUCCESS;
}

/**
 * @brief Attempts to a release a mutex. Does not check the waiting list on success.
 * @param mutex is a pointer of pointer to the semaphore to be released
 * @param task is a pointer of pointer to a task attempting to release the mutex
 * @return MUTEX_SUCCESS on success, MUTEX_OWNED_BY_OTHER_TASK for unavailable mutex or unequal for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_UNABLE_TO_RELEASE: unable to acquire due to semaphore error
 */
size_t mutex_release_non_blocking(mutex_t **mutex, task_t **task) {
    size_t status = mutex_checking(mutex);
    if (status != MUTEX_SUCCESS) {
        return status;
    }

    if (*task != (*mutex)->owner) {
        return MUTEX_OWNED_BY_OTHER_TASK;
    }

    if ((*mutex)->lock_count == 1) {
        status = semaphore_release_non_blocking(&((*mutex)->binary_semaphore));
        if (status != SEMAPHORE_SUCCESS) {
            return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_UNABLE_TO_RELEASE);
        }

        (*mutex)->owner = NULL;
    }

    (*mutex)->lock_count--;

    return MUTEX_SUCCESS;
}

/* Static module functions (implementation) */

/**
 * @brief Checks whether a mutex is valid.
 * @param mutex is a pointer of pointer to the mutex to be checked
 * @return MUTEX_SUCCESS on success or unequal MUTEX_SUCCESS for an error
 * @info On error check for these errors and component errors:
 *  MUTEX_NO_MEMORY: mutex is null
 *  MUTEX_NO_SEMAPHORE: error in mutexes binary semaphore
 *  MUTEX_IRREGULAR_STRUCTURE: invalid combination of owner and lock_count
 */
size_t mutex_checking(mutex_t **mutex) {
    if (*mutex == NULL) {
        return MUTEX_NO_MEMORY;
    }

    size_t status = 0;
    status = semaphore_checking(&((*mutex)->binary_semaphore));
    if (status != SEMAPHORE_SUCCESS) {
        return ERROR_INFO(status, MUTEX_SEMAPHORE_ERROR_REGISTER, MUTEX_NO_SEMAPHORE);
    }

    if (( (*mutex)->owner == NULL && (*mutex)->lock_count != 0 )
        || ( (*mutex)->owner != NULL && (*mutex)->lock_count == 0) ) {
        return MUTEX_IRREGULAR_STRUCTURE;
    }

    return MUTEX_SUCCESS;
}
