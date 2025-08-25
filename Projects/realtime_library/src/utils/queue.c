/**
**************************************************
* @file queue.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 20, 2025
* @brief Module for creating and using queues.
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'queue_create' to create a queue.
  (#) Call 'queue_delete' to delete a queue.
  (#) Call 'queue_push_front' to store a new element at the head
        and to use a queue as a fifo.
  (#) Call 'queue_push_back' to store a new element at the tail
        and to use a queue as a lifo.
  (#) Call 'queue_read' to read an element.
  (#) Call 'queue_peek' to peek an element.
==================================================
@endverbatim
**************************************************
*/
/* Includes */
#include "utils/queue.h"
#include <string.h>
#include <stdlib.h>
/* Preprocessor defines */

/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */
/* Public functions */
/**
 * @brief Creates a queue by its size and expected element size.
 * @param queue is a queue_t pointer of pointer to be initialized as a queue
 * @param queue_size is the amount of data to be stored
 * @param element_size is the expected data size of an element
 * @return 0 on success or greater 0 on error
 * @info On error check for these errors:
 *     QUEUE_NO_MEMORY: unable to allocate memory for queue
 *    QUEUE_DATA_NO_MEMORY: unable to allocate  memory for data array
 */
size_t queue_create(queue_t **queue, size_t queue_size, size_t element_size) {

    // allocate memory and return on error
    *queue = (queue_t *) malloc(sizeof(queue_t));
    if ((*queue)==NULL) {
        return QUEUE_NO_MEMORY;
    }

    // default initialization
    (*queue)->size = queue_size;
    (*queue)->length = 0;
    (*queue)->head = (*queue)->size-1;
    (*queue)->tail = 0;
    (*queue)->element_size = element_size;

    // create data entries
    (*queue)->data = (void **) malloc((*queue)->size*sizeof(void *));
    if ((*queue)->data==NULL) {
        return QUEUE_DATA_NO_MEMORY;
    }

    // assign continuous memory space
    (*queue)->data[0] = malloc((*queue)->size*(*queue)->element_size);
    for (size_t i = 1; i<(*queue)->size; i++) {
        // allow pointer arithmetic
        (*queue)->data[i] = (char *)(*queue)->data[0] + i * (*queue)->element_size;

        if ((*queue)->data[i]==NULL) {
            return QUEUE_DATA_NO_MEMORY;
        }
    }

    return QUEUE_SUCCESS;
}

/**
 * @brief Deletes a queue.
 * @param queue is a queue_t pointer of pointer, which shall be deleted
 * @return 0 on success or greater 0 on error
 * @info On error check for these errors:
 *     QUEUE_IS_NULL: queue is null
 *    QUEUE_DATA_NO_MEMORY: queue has no data memory
 */
size_t queue_delete(queue_t **queue) {

    // check queue for irregular structure
    size_t status = queue_checking(queue);
    if (status!=QUEUE_SUCCESS) {
        return status;
    }

    // delete queue content
    free((*queue)->data[0]);
    (*queue)->data[0] = NULL;

    // delete queue entries
    free((*queue)->data);
    (*queue)->data = NULL;

    // finish deleting queue
    free((*queue));
    (*queue) = NULL;

    return QUEUE_SUCCESS;
}

/**
 * @brief Store data in a queue and the head advances.
 * @param queue is a queue_t pointer of pointer, where a new element shall be stored
 * @param element is a pointer to data, which should be stored
 * @param element_size is the amount of data to be stored, if it deviates from the initialization size
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     QUEUE_IS_NULL: queue is null
 *     QUEUE_DATA_NO_MEMORY: queue has no data memory
 *     QUEUE_PREVENTED_OVERRITE: prevented overqrite of unread data
 *     QUEUE_COPY_ERROR: unable to copy queue element content
 */
size_t queue_push_front(queue_t **queue, void *element, size_t element_size) {

    // check queue for irregular structure
    size_t status = queue_checking(queue);
    if (status!=QUEUE_SUCCESS) {
        return status;
    }

    // calculate next head position and jump back if necessary
    size_t next_position = ((*queue)->head + 1) % (*queue)->size;
    if (next_position == (*queue)->tail && (*queue)->length!=0) {
        return QUEUE_PREVENTED_OVERRITE;
    }

    // copy data and check for error
    void *destination = memcpy((*queue)->data[next_position], element, element_size);
    // prevent overwrite on non empty queue
    if (destination!=(*queue)->data[next_position]) {
        return QUEUE_COPY_ERROR;
    }

    // update queue
    (*queue)->head = next_position;
    (*queue)->length++;

    return QUEUE_SUCCESS;
}

/**
 * @brief Store data in a queue and the tail decrements.
 * @param queue is a queue_t pointer of pointer, where a new element shall be stored
 * @param element is a pointer to data, which should be stored
 * @param element_size is the amount of data to be stored, if it deviates from the initialization size
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     QUEUE_IS_NULL: queue is null
 *     QUEUE_DATA_NO_MEMORY: queue has no data memory
 *     QUEUE_PREVENTED_OVERRITE: prevented overqrite of unread data
 *     QUEUE_COPY_ERROR: unable to copy queue element content
 */
size_t queue_push_back(queue_t **queue, void *element, size_t element_size) {

    // check queue for irregular structure
    size_t status = queue_checking(queue);
    if (status!=QUEUE_SUCCESS) {
        return status;
    }

    // decrement tail position
    // adding queue size will be equal to tail
    // by subtracting 1 the tail is going to be decremented
    size_t next_position = ((*queue)->tail + ((*queue)->size - 1)) % (*queue)->size;
    // prevent overwrite on non empty queue
    if (next_position == (*queue)->head && (*queue)->length!=0) {
        return QUEUE_PREVENTED_OVERRITE;
    }

    // copy data and check for error
    void *destination = memcpy((*queue)->data[next_position], element, element_size);
    if (destination!=(*queue)->data[next_position]) {
        return QUEUE_COPY_ERROR;
    }

    // update queue
    (*queue)->tail = next_position;
    (*queue)->length++;

    return QUEUE_SUCCESS;
}

/**
 * @brief Read data from a queue and the tail advances.
 * @param queue is a queue_t pointer of pointer, where a new element shall be read
 * @param element is a pointer of pointer to data, which should be assigned
 * @return 0 on success or greater 0 on error
 * @info On error element is NULL and check for this error:
 *     QUEUE_IS_NULL: queue is null
 *     QUEUE_DATA_NO_MEMORY: queue has no data memory
 *     QUEUE_NO_ELEMENT: prevented overqrite of unread data
 *     QUEUE_COPY_ERROR: unable to copy to output content
 */
size_t queue_read(queue_t **queue, void **element) {

    // check queue for irregular structure
    size_t status = queue_checking(queue);
    if (status!=QUEUE_SUCCESS) {
        return status;
    }

    // it is not possible to read on empty queue
    if ((*queue)->length==0) {
        element = NULL;
        return QUEUE_NO_ELEMENT;
    }

    // calculate next tail position and jump back if necessary
    size_t next_position = ((*queue)->tail + 1) % (*queue)->size;
    // copy data and check for error
    void *destination = memcpy(*element, (*queue)->data[(*queue)->tail], (*queue)->element_size);
    if (destination != (*element)) {
        return QUEUE_COPY_ERROR;
    }

    // update queue
    (*queue)->tail = next_position;
    (*queue)->length--;

    return QUEUE_SUCCESS;
}

/**
 * @brief Allows to peek on the next element without advancing the tail.
 * @param queue is a queue pointer of pointer, where a new element shall be peeked
 * @param element is a pointer to data, which should be peeked
 * @return 0 on success or greater 0 on error
 * @info On error element is NULL and check for this error:
 *     QUEUE_IS_NULL: queue is null
 *     QUEUE_DATA_NO_MEMORY: queue has no data memory
 *     unable to copy to output content
 */
size_t queue_peek(queue_t **queue, void *element) {

    // check queue for irregular structure
    size_t status = queue_checking(queue);
    if (status!=QUEUE_SUCCESS) {
        return status;
    }

    // copy data and check for error
    void *destination = memcpy(element, (*queue)->data[(*queue)->tail], (*queue)->element_size);
    if (destination!=element) {
        return QUEUE_COPY_ERROR;
    }

    return QUEUE_SUCCESS;
}




/**
 * @brief Checks whether a queue is valid.
 * @param queue is a queue pointer of pointer, which shall be checked for irregular structure
 * @return 0 on success or greater 0 on error
 * @info check for this error:
 *     QUEUE_IS_NULL: queue is null
 *     QUEUE_DATA_NO_MEMORY: queue has no data memory
 */
size_t queue_checking(queue_t **queue) {

    if ((*queue) == NULL) {
        return QUEUE_IS_NULL;
    }
    else if ((*queue)->data == NULL) {
        return QUEUE_DATA_NO_MEMORY;
    }
    else if ((*queue)->data[0] == NULL) {
        return QUEUE_DATA_NO_MEMORY;
    }

    return QUEUE_SUCCESS;
}

/* Static module functions (implementation) */

