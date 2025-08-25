/**
**************************************************
* @file queue.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 20, 2025
* @brief Module for creating and using queues
* @verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'queue_create' to create a queue.
  (#) Call 'queue_delete' to delete a queue.
  (#) Call 'queue_write_back' to store a new element at the head
  	  and to use a queue as a fifo.
  (#) Call 'queue_write_front' to store a new element at the tail
  	  and to use a queue as a lifo.
  (#) Call 'queue_read' to read an element.
  (#) Call 'queue_peek' to peek an element.
==================================================
* @endverbatim
**************************************************
*/

#ifndef UTILS_QUEUE_QUEUE_H_
#define UTILS_QUEUE_QUEUE_H_

/* Includes */
#include <stddef.h>

/* Public Preprocessor defines */
#define QUEUE_SUCCESS               0
#define QUEUE_NO_MEMORY             1
#define QUEUE_IS_NULL               2
#define QUEUE_DATA_NO_MEMORY        3
#define QUEUE_PREVENTED_OVERRITE    4
#define QUEUE_NO_ELEMENT            5
#define QUEUE_COPY_ERROR            6
#define QUEUE_LENGTH                3
#define QUEUE_INFO_REGISTER         0x111

/* Public Preprocessor macros */
/* Public type definitions */

/// Control information for a queue
typedef struct {
	size_t size;            ///< queues max size
	size_t length;          ///< currently stored elements
	size_t element_size;    ///< max element size entry
	size_t head;            ///< queues head
	size_t tail;            ///< queues tail
	void **data;            ///< queues data pointer
} queue_t;

/* Public functions (prototypes) */
size_t queue_create(queue_t **queue, size_t queue_size, size_t element_size);
size_t queue_delete(queue_t **queue);
size_t queue_push_front(queue_t **queue, void *element, size_t element_size);
size_t queue_push_back(queue_t **queue, void *element, size_t element_size);
size_t queue_read(queue_t **queue, void **element);
size_t queue_peek(queue_t **queue, void *element);
size_t queue_checking(queue_t **queue);
#endif /* UTILS_QUEUE_QUEUE_H_ */
