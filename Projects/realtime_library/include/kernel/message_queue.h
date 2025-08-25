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

#ifndef MESSAGE_QUEUE_MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_MESSAGE_QUEUE_H_

/* Includes */
#include <utils/queue.h>
#include <kernel/task.h>
#include <utils/linked_list.h>
#include <stddef.h>
#include <stdbool.h>
#include "kernel/task.h"

/* Public Preprocessor defines */
#define MESSAGE_QUEUE_SUCCESS                           0
#define MESSAGE_QUEUE_IDENTIFIER_SUCCESS                0
#define MESSAGE_QUEUE_NO_MEMORY                         1
#define MESSAGE_QUEUE_NO_QUEUE                          2
#define MESSAGE_QUEUE_NO_IDENTIFIER                     3
#define MESSAGE_QUEUE_NO_RECEIVING_LIST                 4
#define MESSAGE_QUEUE_NO_SENDING_LIST                   5
#define MESSAGE_QUEUE_UNABLE_TO_SEND                    6
#define MESSAGE_QUEUE_UNABLE_TO_RECEIVE                 7
#define MESSAGE_QUEUE_NO_TASK_WAITING_LIST_ELEMENT      8
#define MESSAGE_QUEUE_UNABLE_TO_DELETE                  9
#define MESSAGE_QUEUE_LINKED_LIST_ERROR_REGISTER        10
#define MESSAGE_QUEUE_INVALID_TASK_REGISTER             11

#define MESSAGE_QUEUE_LENGTH                            4


#define MESSAGE_QUEUE_LINK_LIST_ERROR_REGISTER                  MESSAGE_QUEUE_LENGTH
#define MESSAGE_QUEUE_QUEUE_ERROR_REGISTER                      MESSAGE_QUEUE_LINK_LIST_ERROR_REGISTER + QUEUE_LENGTH
#define MESSAGE_QUEUE_RECEIVING_LINKED_LIST_ERROR_REGISTER      MESSAGE_QUEUE_QUEUE_ERROR_REGISTER + LINKED_LIST_LENGTH
#define MESSAGE_QUEUE_SENDING_LINKED_LIST_ERROR_REGISTER        MESSAGE_QUEUE_RECEIVING_LINKED_LIST_ERROR_REGISTER + LINKED_LIST_LENGTH
#define MESSAGE_QUEUE_TASK_ERROR_REGISTER                       MESSAGE_QUEUE_LINK_LIST_ERROR_REGISTER + TASK_LENGTH


/* Public Preprocessor macros */
#define GET_QUEUE_ERROR(x) ((x) & 0x111)
#define GET_MESSAGE_QUEUE_ERROR(x) ((x) >> QUEUE_INFO_REGISTER)
#define SET_MESSAGE_QUEUE_ERROR(x) ((x) << QUEUE_INFO_REGISTER)

/* Public type definitions */

/// Control information for the message queue identifier
typedef struct {
    size_t id;      ///< message queues id
    char *name;     ///< message queues name
} message_queue_identifier_t;

/// Control information for the message queue
typedef struct {
    message_queue_identifier_t *message_queue_identifier;   ///< message queues identifier
    queue_t *qcb;                                           ///< message queues control block
    linked_list_t *receiving_task_list;                     ///< linked list for storing blocked receiving tasks
    linked_list_t *sending_task_list;                       ///< linked list for storing blocked sending tasks
} message_queue_t;

/* Public functions (prototypes) */
size_t message_queue_create(message_queue_t **message_queue, size_t message_queue_size, size_t element_size, size_t id, char *name);
size_t message_queue_delete(message_queue_t **message_queue);
size_t message_queue_send(message_queue_t **message_queue, linked_list_element_t **element, task_t **task, void *message, size_t element_size, bool urgent);
size_t message_queue_send_blocking(message_queue_t **message_queue, linked_list_t **running_task_list, linked_list_element_t **sender_element, linked_list_element_t **receiver_element, task_t **receiver_task, void *message, size_t element_size, bool urgent);
size_t message_queue_receive(message_queue_t **message_queue, linked_list_t **running_task_list, linked_list_element_t **running_task_element, task_t **receiver_task, linked_list_element_t **sender_element, task_t **sender_task, void **message);
size_t message_queue_broadcast(message_queue_t **message_queue, void *message);
size_t message_queue_identifier_checking(message_queue_identifier_t **message_queue_identifier);

#endif /* MESSAGE_QUEUE_MESSAGE_QUEUE_H_ */
