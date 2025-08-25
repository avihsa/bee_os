/**
 **************************************************
 * @file message_queue.c
 * @author Christopher-Marcel Klein, Ameline Seba
 * @version v1.0
 * @date Apr 20, 2025
 * @brief Module for creating using a message queue
 @verbatim
 ==================================================
 ### Resources used ###
 None
 ==================================================
 ### Usage ###
 (#) Call 'message_queue_create' to create a message queue
 (#) Call 'message_queue_delete' to delete a message queue
 (#) Call 'message_queue_send' to send info to a task
 (#) Call 'message_queue_send_blocking' to send info to a task,
     but the sending task can be blocked
 (#) Call 'message_queue_receive' to receive a message
 (#) Call 'message_queue_identifier_checking' to check the message queues identifier
 (#) Call 'message_queue_checking' to check the message queues
 ==================================================
 @endverbatim
 **************************************************
 */
/* Includes */
#include <kernel/message_queue.h>
#include <string.h>
#include <stdlib.h>
#include "utils/linked_list.h"
#include "utils/support.h"

/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */
size_t message_queue_checking(message_queue_t **message_queue);
/* Public functions */

/**
 * @brief Creates a message queue by its size, expected element size, id and name.
 * @param message_queue is a message_queue_t pointer of pointer to be initialized as a message queue
 * @param queue_size is the amount of data to be stored
 * @param element_size is the expected data size of an element
 * @param id is a numeric identifier
 * @param name is a string identifier
 * @return 0 on success or greater 0 on error
 * @info On error check for these errors and component errors:
 *     MESSAGE_QUEUE_NO_MEMORY: unable to allocate memory for message queue
 *    MESSAGE_QUEUE_NO_IDENTIFIER: unable to allocate memory for message queue identifier
 *  MESSAGE_QUEUE_NO_RECEIVING_LIST: unable to initialize receiving list
 *  MESSAGE_QUEUE_NO_SENDING_LIST: unable to initialize sending list
 */
size_t message_queue_create(message_queue_t **message_queue, size_t message_queue_size, size_t element_size, size_t id, char *name) {

    // allocate memory and return to error
    (*message_queue) = (message_queue_t*) malloc(sizeof(message_queue_t));
    if ((*message_queue) == NULL) {
        return MESSAGE_QUEUE_NO_MEMORY;
    }

    // assign identifier for id and name
    (*message_queue)->message_queue_identifier = (message_queue_identifier_t*) malloc(sizeof(message_queue_identifier_t));
    if ((*message_queue)->message_queue_identifier == NULL) {
        return MESSAGE_QUEUE_NO_IDENTIFIER;
    }

    (*message_queue)->message_queue_identifier->id = id;
    (*message_queue)->message_queue_identifier->name = name;

    // create queue for message queue
    size_t status = queue_create(&(*message_queue)->qcb, message_queue_size, element_size);
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_QUEUE_ERROR_REGISTER, MESSAGE_QUEUE_NO_QUEUE);
    }

    // create receiving task linked list
    status = linked_list_create(&(*message_queue)->receiving_task_list);
    if (status != LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_RECEIVING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_NO_RECEIVING_LIST);
    }

    // create sending task linked list
    status = linked_list_create(&(*message_queue)->sending_task_list);
    if (status != LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_SENDING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_NO_SENDING_LIST);
    }

    return MESSAGE_QUEUE_SUCCESS;
}

/**
 * @brief Deletes a message queue
 * @param message_queue is a message_queue_t pointer of pointer to be deleted
 * @return 0 on success or greater 0 on error
 * @info The return value is a concatenated status error code based of subcomponents.
 */
size_t message_queue_delete(message_queue_t **message_queue) {

    // check message queue for irregular structure
    size_t status = message_queue_checking(message_queue);
    if (status != MESSAGE_QUEUE_SUCCESS) {
        return status;
    }

    // delete queue
    status = queue_delete(&(*message_queue)->qcb);
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_QUEUE_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_DELETE);
    }

    // delete receiving task list
    status = linked_list_delete(&(*message_queue)->receiving_task_list);
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_RECEIVING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_DELETE);
    }

    // delete sending task list
    status = linked_list_delete(&(*message_queue)->sending_task_list);
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_SENDING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_DELETE);
    }

    // delete identifier
    free(&(*message_queue)->message_queue_identifier);
    (*message_queue)->message_queue_identifier = NULL;

    // delete message queue
    free(*message_queue);
    *message_queue = NULL;

    return MESSAGE_QUEUE_SUCCESS;
}

/**
 * @brief Sends a message to a task without blocking
 * @param message_queue is a message_queue_t pointer of pointer to use for communication
 * @param element is a linked_list_element_t pointer of pointer to the receiver
 * @param task is a task_t pointer of pointer to the receiver
 * @param message is data to be send
 * @param element_size is the amount of bytes to stored
 * @param urgent is a bool, if set to true, the message queue becomes a lifo
 * @return 0 on success or greater 0 on error
 * @info The return value is a concatenated status error code based of subcomponents.
 */
size_t message_queue_send(
        message_queue_t **message_queue,
        linked_list_element_t **element,
        task_t **task,
        void *message,
        size_t element_size,
        bool urgent) {

    // check message queue for irregular structure
    size_t status = message_queue_checking(message_queue);
    if (status != MESSAGE_QUEUE_SUCCESS) {
        return status;
    }

    // before a message is queued, check for available tasks
    if ((*message_queue)->receiving_task_list->size > 0) {

        // assign return values
        (*element) = (*message_queue)->receiving_task_list->tail;
        (*task) = (task_t*) (*element)->data;

        // check task for irregular structure
        status = task_checking(task);
        if (status != TASK_SUCCESS) {
            return ERROR_INFO(status, MESSAGE_QUEUE_INVALID_TASK_REGISTER, MESSAGE_QUEUE_UNABLE_TO_SEND);
        }

        // copy message
        void *destination = memcpy((*task)->message, message, element_size);
        if (destination != (*task)->message) {
            return MESSAGE_QUEUE_UNABLE_TO_SEND;
        }

        // signal task on next receive a message was stored
        (*task)->message_set = true;

        return MESSAGE_QUEUE_SUCCESS;
    }

    // determine where a message shall be inserted based on urgency
    if (urgent) {
        status = queue_push_back(&(*message_queue)->qcb, message, element_size);
    }
    else {
        status = queue_push_front(&(*message_queue)->qcb, message, element_size);
    }

    // check queue for errors
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_LENGTH, MESSAGE_QUEUE_UNABLE_TO_SEND);
    }

    return MESSAGE_QUEUE_SUCCESS;
}

/**
 * @brief Sends a message to a task. The task can be blocked by insertion to the sending waiting list.
 *         Sending a message can fail, which will need a retry.
 * @param message_queue is a message_queue_t pointer of pointer to use for communication
 * @param running_task_list is a linked_list_t pointer of pointer to the current running source list, where the sender task belongs
 * @param sender_element is a linked_list_element_t to the sender
 * @param receiver_element is a linked_list_element_t pointer of pointer to the receiver
 * @param receiver_task is a task_t pointer of pointer to the receiver
 * @param message is data to be send
 * @param element_size is the amount of bytes to stored
 * @param urgent is a bool, if set to true, the message queue becomes a lifo
 * @return 0 on success or greater 0 on error
 * @info The return value is a concatenated status error code based of subcomponents.
 */
size_t message_queue_send_blocking(
        message_queue_t **message_queue,
        linked_list_t **running_task_list,
        linked_list_element_t **sender_element,
        linked_list_element_t **receiver_element,
        task_t **receiver_task,
        void *message,
        size_t element_size,
        bool urgent) {

    // solve blocked sending by checking on failure for send messages
    size_t status = message_queue_send(message_queue, receiver_element, receiver_task, message, element_size, urgent);

    if (status != MESSAGE_QUEUE_SUCCESS) {
        // sending a message has failed and the sender is being inserted
        status = linked_list_transfer(&(*message_queue)->sending_task_list, running_task_list, sender_element);
        return ERROR_INFO(status, MESSAGE_QUEUE_SENDING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_SEND);
    }

    return MESSAGE_QUEUE_SUCCESS;
}


/**
 * @brief Receive a message.
 * @param message_queue is a message_queue_t pointer of pointer to use for communication
 * @param running_task_list is a linked_list_t pointer of pointer to the current running source list, where the receiver task belongs
 * @param running_task_element is a linked_list_element_t pointer of pointer, which points to the receiver task
 * @param receiver_task is a task_t pointer of pointer to the receiver
 * @param sender_element is a linked_list_element_t pointer of pointer to a blocked sender
 * @param sender_task is a task_t pointer of pointer to a blocked task
 * @param message is data to be received, it can be empty, which will need a retry
 * @return 0 on success or greater 0 on error
 * @info The return value is a concatenated status error code based of subcomponents.
 */
size_t message_queue_receive(
        message_queue_t **message_queue,
        linked_list_t **running_task_list,
        linked_list_element_t **running_task_element,
        task_t **receiver_task,
        linked_list_element_t **sender_element,
        task_t **sender_task,
        void **message) {

    // check message queue for irregular structure
    size_t status = message_queue_checking(message_queue);
    if (status != MESSAGE_QUEUE_SUCCESS) {
        return status;
    }

    // check task for irregular structure
    status = task_checking(receiver_task);
    if (status != TASK_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_TASK_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_RECEIVE);
    }

    // check if a message was stored at a task
    if ((*receiver_task)->message_set) {
        // parked message can be used
        // reset signal mechanic
        (*receiver_task)->message_set = false;
        (*message) = (*receiver_task)->message;
        (*receiver_task)->message = NULL;
        // try to obtain blocked sending task
        if ((*message_queue)->sending_task_list->size > 0) {
            (*sender_element) = (*message_queue)->sending_task_list->tail;
            (*sender_task) = (task_t*) (*sender_element)->data;
        }

        return MESSAGE_QUEUE_SUCCESS;
    }

    // a message was not parked to a task
    status = queue_read(&(*message_queue)->qcb, message);
    if (status == QUEUE_NO_ELEMENT) {
        // no message found and the task about to block
        (*receiver_task)->message = (*message);
        status = linked_list_transfer(&(*message_queue)->receiving_task_list, running_task_list, running_task_element);
        if (status != LINKED_LIST_SUCCESS) {
            return ERROR_INFO(status, MESSAGE_QUEUE_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_UNABLE_TO_RECEIVE);
        }

        return MESSAGE_QUEUE_UNABLE_TO_RECEIVE;
    }

    // a message was assigned
    // try to obtain blocked sending task
    if ((*message_queue)->sending_task_list->size > 0) {
        (*sender_element) = (*message_queue)->sending_task_list->tail;
        (*sender_task) = (task_t*) (*sender_element)->data;
    }

    return MESSAGE_QUEUE_SUCCESS;
}

/**
 * @brief Checks whether a queue identifier is valid.
 * @param message_queue_identifier is a message_queue_identifier_t pointer of pointer, which shall be checked for irregular structure
 * @return 0 on success or greater 0 on error
 * @info check for this error:
 *     MESSAGE_QUEUE_NO_IDENTIFIER: message queue identifier is null
 */
size_t message_queue_identifier_checking(message_queue_identifier_t **message_queue_identifier) {

    // check for invalid identifier
    if (message_queue_identifier == NULL) {
        return MESSAGE_QUEUE_NO_IDENTIFIER;
    }

    return MESSAGE_QUEUE_IDENTIFIER_SUCCESS;
}

/* Static module functions (implementation) */

/**
 * @brief Checks whether a message queue is valid.
 * @param message_queue is a message_queue_t pointer of pointer, which shall be checked for irregular structure
 * @return 0 on success or greater 0 on error
 * @info The return value is a concatenated status error code based of subcomponents.
 */
size_t message_queue_checking(message_queue_t **message_queue) {

    // check main component
    if (*message_queue == NULL) {
        return MESSAGE_QUEUE_NO_MEMORY;
    }

    // check each subcomponent for irregular data structure
    size_t status = message_queue_identifier_checking(&(*message_queue)->message_queue_identifier);
    if (status != MESSAGE_QUEUE_IDENTIFIER_SUCCESS) {
        return status;
    }

    status = queue_checking(&(*message_queue)->qcb);
    if (status != QUEUE_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_QUEUE_ERROR_REGISTER, MESSAGE_QUEUE_QUEUE_ERROR_REGISTER);
    }

    status = linked_list_checking(&(*message_queue)->receiving_task_list);
    if (status != LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_RECEIVING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_NO_RECEIVING_LIST);
    }

    status = linked_list_checking(&(*message_queue)->receiving_task_list);
    if (status != LINKED_LIST_SUCCESS) {
        return ERROR_INFO(status, MESSAGE_QUEUE_SENDING_LINKED_LIST_ERROR_REGISTER, MESSAGE_QUEUE_NO_SENDING_LIST);
    }

    return MESSAGE_QUEUE_SUCCESS;
}
