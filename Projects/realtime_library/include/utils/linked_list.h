/**
**************************************************
* @file linked_list.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 21, 2025
* @brief Module for creating and using a linked lists
@verbatim
==================================================
  ### Resources used ###
  System Memory
==================================================
  ### Usage ###
  (#) Call 'linked_list_create' to create a linked list
  (#) Call 'linked_list_delete' to delete a linked list
  (#) Call 'linked_list_push_back' to push an element to
      the end of a linked list
  (#) Call 'linked_list_pop_front' to pop the first element
      of a linked list
  (#) Call 'linked_list_pop_back' to pop the last element
      of a linked list
  (#) Call 'linked_list_push_front' to push an element to
      the start of a linked list
  (#) Call 'linked_list_insert_after' to insert a new element
      after another specified element
  (#) Call 'linked_list_pop_inner' to pop a specified element
  (#) Call 'linked_list_peek_back' to read the last element
      of a linked list
  (#) Call 'linked_list_transfer' to transfer an element from
      one linked list to another
  (#) Call 'linked_list_transfer_after' to transfer an element from
      one linked list to another after another specified element
  (#) Call 'linked_list_move_linked_list_after' to move all elements
      of a linked list to the end of another linked list
  (#) All functions call 'linked_list_checking' to validate
      proper linked list structure. Refer to this function
      for potential error codes not documented in each
      function.
  (#) All functions call 'linked_list_element_checking' to validate
      proper linked list element structure. Refer to this function
      for potential error codes not documented in each
      function.
==================================================
@endverbatim
**************************************************
*/

#ifndef UTILS_LINKED_LIST_H_
#define UTILS_LINKED_LIST_H_

/* Includes */
#include <stddef.h>
#include <stdint.h>

/* Public Preprocessor defines */
#define LINKED_LIST_SUCCESS	            0
#define LINKED_LIST_ELEMENT_SUCCESS	    0
#define LINKED_LIST_NO_MEMORY           1
#define LINKED_LIST_ELEMENT_NO_MEMORY   2
#define LINKED_LIST_IRREGULAR_STRUCTURE 3
#define LINKED_LIST_NO_ELEMENT 	        4
#define LINKED_LIST_ELEMENT_IS_NULL     5
#define LINKED_LIST_SOURCE_IS_EMPTY		6


#define LINKED_LIST_LENGTH				3

/* Public Preprocessor macros */
/* Public type definitions */

/// Control information for a linked list element
typedef struct element {
	void *data;                 ///< stored data
    struct element *previous;   ///< previous list element
	struct element *next;       ///< next list element
} linked_list_element_t;

/// Control information for a linked list
typedef struct {
	linked_list_element_t *head;    ///< linked lists head
	linked_list_element_t *tail;    ///< linked lists tail
	size_t size;                    ///< linked lists size
} linked_list_t;

/* Public functions (prototypes) */
size_t linked_list_create(linked_list_t **linked_list);
size_t linked_list_delete(linked_list_t **linked_list);
size_t linked_list_push_back(linked_list_t **linked_list, void **data);
size_t linked_list_pop_front(linked_list_t **linked_list, void **data);
size_t linked_list_pop_back(linked_list_t **linked_list, void **data);
size_t linked_list_push_front(linked_list_t **linked_list, void **data);
size_t linked_list_insert_after(linked_list_t **linked_list, linked_list_element_t **linked_list_element, void **data);
size_t linked_list_pop_inner(linked_list_t **linked_list, linked_list_element_t **linked_list_element, void **data);
size_t linked_list_peek_back(linked_list_t **linked_list, void **data);

size_t linked_list_transfer(linked_list_t **linked_list_destination, linked_list_t **linked_list_source, linked_list_element_t **linked_list_element);
size_t linked_list_transfer_after(linked_list_t **linked_list_destination, linked_list_element_t **linked_list_element_destination_after, linked_list_t **linked_list_source, linked_list_element_t **linked_list_element_moving);
size_t linked_list_move_linked_list_after(linked_list_t **linked_list_destination, linked_list_t **linked_list_source);

size_t linked_list_checking(linked_list_t **linked_list);
size_t linked_list_element_checking(linked_list_element_t **linked_list_element);
#endif /* UTILS_LINKED_LIST_H_ */
