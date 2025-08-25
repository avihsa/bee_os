/**
**************************************************
* @file linked_list.c
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 21, 2025
* @brief Module for creating and using a linked lists
@verbatim
==================================================
  ### Resources used ###
  None
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
/* Includes */
#include <stdlib.h>
#include "utils/linked_list.h"

/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */
size_t linked_list_checking(linked_list_t **linked_list);
size_t linked_list_element_checking(linked_list_element_t **linked_list_element);
size_t linked_list_element_create(linked_list_element_t **linked_list_element, void **data);
/* Public functions */

/**
 * @brief Creates an empty linked list with no elements.
 * @param linked_list is a linked_list_t pointer of pointer to assign the new linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *    LINKED_LIST_NO_MEMORY: unable to allocate memory for linked list
 */
size_t linked_list_create(linked_list_t **linked_list) {

    // allocate memory and check for error
    (*linked_list) = (linked_list_t* ) malloc(sizeof(linked_list_t));
    if ((*linked_list) == NULL) {
        return LINKED_LIST_NO_MEMORY;
    }

    // default assignment
    (*linked_list)->size = 0;
    (*linked_list)->head = NULL;
    (*linked_list)->tail = NULL;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Deletes a linked list including all of its elements
 * @param linked_list is a linked_list_t pointer of pointer, which shall be deleted
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_IRREGULAR_STRUCTURE: unable to fully delete list, no path from head to tail
 */
size_t linked_list_delete(linked_list_t **linked_list) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // delete all linked list elements
    while ((*linked_list)->tail != NULL) {
        linked_list_element_t *tail = (*linked_list)->tail;
        (*linked_list)->tail = (*linked_list)->tail->next;
        tail->previous = NULL;
        free(tail);
        tail = NULL;
        (*linked_list)->size--;
    }

    // check for irregular structure
    if ((*linked_list)->size != 0) {
        return LINKED_LIST_IRREGULAR_STRUCTURE;
    }

    // delete linked list
    free((*linked_list));
    (*linked_list) = NULL;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Pushes a new data element to the end of the linked list.
 * @param linked_list is a linked_list_t pointer of pointer, where a new linked list element is added
 * @param data is a void pointer of pointer to the data, which shall be added in a linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_ELEMENT_NO_MEMORY: unable to allocate memory for new linked list element
 */
size_t linked_list_push_back(linked_list_t **linked_list, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // create a new carrier and add data
    linked_list_element_t *new = NULL;
    status = linked_list_element_create(&new, data);
    if (status != LINKED_LIST_ELEMENT_SUCCESS) {
        return status;
    }

    // insert the linked list element
    if ((*linked_list)->size == 0) {
        // first element
        (*linked_list)->tail = new;
        (*linked_list)->head = new;
    }
    else {
        // next element
        new->next = (*linked_list)->tail;
        (*linked_list)->tail->previous = new;
        (*linked_list)->tail = new;
    }

    // always increase the linked list size
    (*linked_list)->size++;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Pops data from the beginning of the linked list.
 * @param linked_list is a linked_list_t pointer of pointer, where a data element is being removed
 * @param data is a void pointer of pointer to the data, which shall be removed from a linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_NO_ELEMENT: unable to pop a data element
 */
size_t linked_list_pop_front(linked_list_t **linked_list, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // obtain data
    linked_list_element_t *pop = (*linked_list)->head;
    if (pop == NULL) {
        return LINKED_LIST_NO_ELEMENT;
    }
    (*data) = pop->data;

    // remove data element
    (*linked_list)->size--;
    if ((*linked_list)->size == 0 && (*linked_list)->tail == (*linked_list)->head) {
        // linked list is empty
        (*linked_list)->tail = NULL;
        (*linked_list)->head = NULL;
    }
    else if ((*linked_list)->size == 0 && (*linked_list)->tail != (*linked_list)->head) {
        // error
        return LINKED_LIST_IRREGULAR_STRUCTURE;
    }
    else {
        // remove head
        (*linked_list)->head = (*linked_list)->head->previous;
    }

    // delete linked list element
    free(pop);
    pop = NULL;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Pops data from the end of the linked list.
 * @param linked_list is a linked_list_t pointer of pointer, where a data element is being removed
 * @param data is a void pointer of pointer to the data, which shall be removed from a linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_NO_ELEMENT: unable to pop a requested element
 */
size_t linked_list_pop_back(linked_list_t **linked_list, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // obtain data
    linked_list_element_t *pop = (*linked_list)->tail;
    if (pop == NULL) {
        return LINKED_LIST_NO_ELEMENT;
    }
    (*data) = pop->data;

    // remove data element
    (*linked_list)->size--;
    if ((*linked_list)->size == 0 && (*linked_list)->tail == (*linked_list)->head) {
        (*linked_list)->tail = NULL;
        (*linked_list)->head = NULL;
    }
    else if ((*linked_list)->size == 0 && (*linked_list)->tail != (*linked_list)->head) {
        // error
        return LINKED_LIST_IRREGULAR_STRUCTURE;
    }
    else {
        (*linked_list)->tail = (*linked_list)->tail->next;
    }

    // delete linked list element
    free(pop);
    pop = NULL;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Pushes a new data element to the beginning of the linked list.
 * @param linked_list is a linked_list pointer of pointer, where a new linked list element is added
 * @param data is a void pointer of pointer to the data, which shall be added in a linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_ELEMENT_NO_MEMORY: unable to allocate memory for new linked list element
 */
size_t linked_list_push_front(linked_list_t **linked_list, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // create a new carrier and add data
    linked_list_element_t *new = NULL;
    status = linked_list_element_create(&new, data);
    if (status != LINKED_LIST_ELEMENT_SUCCESS) {
        return status;
    }

    // insert the linked list element
    if ((*linked_list)->size == 0) {
        // first element
        (*linked_list)->head = new;
        (*linked_list)->tail = new;
    }
    else {
        // next element
        new->previous = (*linked_list)->head;
        (*linked_list)->head->next = new;
        (*linked_list)->head = new;
    }

    // always increase the linked list size
    (*linked_list)->size++;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Adds a new element after a specified element of the linked list.
 * @param linked_list is a linked_list_t pointer of pointer, where a new linked list element is added
 * @param linked_list_element is a linked_list_element pointer of pointer after which it shall be added
 * @param data is a void pointer of pointer to the data of the newly added element
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_ELEMENT_NO_MEMORY: unable to allocate memory for new linked list element
 */
size_t linked_list_insert_after(linked_list_t **linked_list, linked_list_element_t **linked_list_element, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check if linked list is empty and use push front instead
    if ((*linked_list)->head == NULL || (*linked_list)->head->next == NULL) {
        return linked_list_push_front(linked_list, data);
    }

    // create a new carrier and add data
    linked_list_element_t *new = NULL;
    status = linked_list_element_create(&new, data);
    if (status != LINKED_LIST_ELEMENT_SUCCESS) {
        return status;
    }

    // insert carrier in between
    new->previous = *linked_list_element;
    new->next = (*linked_list_element)->next;
    (*linked_list_element)->next->previous = new;
    (*linked_list_element)->next = new;
    (*linked_list)->size++;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Removes an element from a linked list.
 * @param linked_list is a linked_list_t pointer of pointer where an element is being removed
 * @param linked_list_element is a linked_list_element_t pointer of pointer, which shall be removed
 * @param data is a void pointer of pointer to the data, which shall be removed from a linked list
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_NO_ELEMENT: unable to pop a requested element
 *     LINKED_LIST_IRREGULAR_STRUCTURE: no path from head to tail
 */
size_t linked_list_pop_inner(linked_list_t **linked_list, linked_list_element_t **linked_list_element, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // obtain data
    linked_list_element_t *pop = *linked_list_element;
    if (pop == NULL) {
        return LINKED_LIST_NO_ELEMENT;
    }
    (*data) = pop->data;

    // remove data
    if ((*linked_list)->size == 0 && (*linked_list)->tail == (*linked_list)->head) {
        // linked list is empty
        (*linked_list)->tail = NULL;
        (*linked_list)->head = NULL;
    }
    else if ((*linked_list)->size == 0 && (*linked_list)->tail != (*linked_list)->head) {
        // error
        return LINKED_LIST_IRREGULAR_STRUCTURE;
    }
    else {
        if (*linked_list_element == (*linked_list)->head) {
            // simplify with pop front
            return linked_list_pop_front(linked_list, data);
        }
        else if (*linked_list_element == (*linked_list)->tail) {
            // simplify with pop back
            return linked_list_pop_back(linked_list, data);
        }
        else {
            // remove element in between
            (*linked_list_element)->next->previous = (*linked_list_element)->previous;
            (*linked_list_element)->previous->next = (*linked_list_element)->next;
        }
    }

    // decrease size, when the element was remove in between and not by pop
    (*linked_list)->size--;

    // delete linked list element
    free(pop);
    pop = NULL;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Returns the given lists tails data without destroying the element.
 * @param linked_list is a linked_list_t pointer of pointer where an element is being peeked
 * @param data is a void pointer of pointer to the tails data
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     LINKED_LIST_ELEMENT_IS_NULL: unable to peek back
 */
size_t linked_list_peek_back(linked_list_t **linked_list, void **data) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    if ((*linked_list)->tail == NULL) {
        *data = NULL;
        return LINKED_LIST_ELEMENT_IS_NULL;
    }

    *data = (*linked_list)->tail->data;

    return LINKED_LIST_SUCCESS;
}


/**
 * @brief Transfers a linked list element from source to destinations head.
 * @param linked_list_destination is a linked_list_t pointer of pointer where an element shall be inserted at destinations head
 * @param linked_list_source is a linked_list_t pointer of pointer where an element shall be removed
 * @param linked_list_element is a linked_list_element_t pointer of pointer which shall be transfered
 * @return 0 on success or greater 0 on error
 */
size_t linked_list_transfer(
        linked_list_t **linked_list_destination,
        linked_list_t **linked_list_source,
        linked_list_element_t **linked_list_element) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list_destination);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check linked list for irregular structure
    status = linked_list_checking(linked_list_source);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check linked list element for irregular structure
    status = linked_list_element_checking(linked_list_element);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // abort transfer if empty
    if ((*linked_list_source)->size == 0) {
        return LINKED_LIST_SOURCE_IS_EMPTY;
    }


    // remove linked list element from source
    linked_list_element_t *linked_list_element_tranfer = *linked_list_element;

    if (linked_list_element_tranfer == (*linked_list_source)->head) {
        // linked list element is sources head
        (*linked_list_source)->head = (*linked_list_source)->head->previous;
        if ((*linked_list_source)->head != NULL) {
            // source is not empty
            (*linked_list_source)->head->next = NULL;
        }
    }
    else if (linked_list_element_tranfer->next != NULL) {
        // linked list element is between or tail
        linked_list_element_tranfer->next->previous = linked_list_element_tranfer->previous;
    }

    if (linked_list_element_tranfer == (*linked_list_source)->tail) {
        // linked list element is sources tail
        (*linked_list_source)->tail = (*linked_list_source)->tail->next;
        if ((*linked_list_source)->tail != NULL) {
            // source is not empty
            (*linked_list_source)->tail->previous = NULL;
        }
    }
    else if (linked_list_element_tranfer->previous != NULL) {
        // linked list element is between or head
        linked_list_element_tranfer->previous->next = linked_list_element_tranfer->next;
    }

    // linked list element was removed
    (*linked_list_source)->size--;

    // set linked list element to default before insert in destination
    linked_list_element_tranfer->next = NULL;
    linked_list_element_tranfer->previous = NULL;

    // check if linked list destination is empty
    if ((*linked_list_destination)->size == 0) {
        (*linked_list_destination)->tail = linked_list_element_tranfer;
        (*linked_list_destination)->head = linked_list_element_tranfer;
    }
    else {
        // linked list destination is not empty
        (*linked_list_destination)->head->next = linked_list_element_tranfer;
        linked_list_element_tranfer->previous = (*linked_list_destination)->head;
        (*linked_list_destination)->head = linked_list_element_tranfer;
    }

    (*linked_list_destination)->size++;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Transfers a linked list element from source to destinations head.
 * @param linked_list_destination is a linked_list_t pointer of pointer where an element shall be inserted at destinations head
 * @param linked_list_element_destination_after is a linked_list_element_t pointer of pointer where the moving element shall be transferred after
 * @param linked_list_source is a linked_list_t pointer of pointer where an element shall be removed
 * @param linked_list_element_moving is a linked_list_element_t pointer of pointer which shall be transfered
 * @return 0 on success or greater 0 on error
 */
size_t linked_list_transfer_after(
        linked_list_t **linked_list_destination,
        linked_list_element_t **linked_list_element_destination_after,
        linked_list_t **linked_list_source,
        linked_list_element_t **linked_list_element_moving) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list_destination);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check linked list for irregular structure
    status = linked_list_checking(linked_list_source);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check linked list element for irregular structure
    status = linked_list_element_checking(linked_list_element_moving);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // abort transfer if empty
    if ((*linked_list_source)->size == 0) {
        return LINKED_LIST_SOURCE_IS_EMPTY;
    }


    // remove linked list element from source
    linked_list_element_t *linked_list_element_tranfer = *linked_list_element_moving;

    // remove linked list element from source
    if (linked_list_element_tranfer == (*linked_list_source)->head) {
        // linked list element is sources head
        (*linked_list_source)->head = (*linked_list_source)->head->previous;
        if ((*linked_list_source)->head != NULL) {
            // source is not empty
            (*linked_list_source)->head->next = NULL;
        }
    }
    else if (linked_list_element_tranfer->next != NULL) {
        // linked list element is between or tail
        linked_list_element_tranfer->next->previous = linked_list_element_tranfer->previous;
    }

    if (linked_list_element_tranfer == (*linked_list_source)->tail) {
        // linked list element is sources tail
        (*linked_list_source)->tail = (*linked_list_source)->tail->next;
        if ((*linked_list_source)->tail != NULL) {
            // source is not empty
            (*linked_list_source)->tail->previous = NULL;
        }
    }
    else if (linked_list_element_tranfer->previous != NULL) {
        // linked list element is between or tail
        linked_list_element_tranfer->previous->next = linked_list_element_tranfer->next;
    }

    // linked list element was removed
    (*linked_list_source)->size--;

    // set linked list element to default before insert in destination
    linked_list_element_tranfer->next = NULL;
    linked_list_element_tranfer->previous = NULL;

    // check if linked list destination is empty
    if ((*linked_list_destination)->size == 0) {
        (*linked_list_destination)->tail = linked_list_element_tranfer;
        (*linked_list_destination)->head = linked_list_element_tranfer;
    }
    else {
        // linked list destination is not empty
        if ((*linked_list_destination)->head == (*linked_list_element_destination_after)) {
            // insert at destinations head
            (*linked_list_destination)->head->next = linked_list_element_tranfer;
            linked_list_element_tranfer->previous = (*linked_list_destination)->head;
            (*linked_list_destination)->head = linked_list_element_tranfer;
        }
        else if ((*linked_list_element_destination_after) == NULL) {
            // insert before destinations tail
            linked_list_element_tranfer->next = (*linked_list_destination)->tail;
            (*linked_list_destination)->tail->previous = linked_list_element_tranfer;
            (*linked_list_destination)->tail = linked_list_element_tranfer;
        }
        else {
            // insert at destinations tail
            (*linked_list_element_destination_after)->next->previous = linked_list_element_tranfer;
            linked_list_element_tranfer->next = (*linked_list_element_destination_after)->next;
            (*linked_list_element_destination_after)->next = linked_list_element_tranfer;
        }
    }

    (*linked_list_destination)->size++;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Transfers a linked list element from source to destinations head.
 * @param linked_list_destination is a linked_list_t pointer of pointer where all elements shall be inserted at destinations head
 * @param linked_list_source is a linked_list_t pointer of pointer where all elements shall be removed
 * @return 0 on success or greater 0 on error
 */
size_t linked_list_move_linked_list_after(
        linked_list_t **linked_list_destination,
        linked_list_t **linked_list_source) {

    // check linked list for irregular structure
    size_t status = linked_list_checking(linked_list_destination);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // check linked list for irregular structure
    status = linked_list_checking(linked_list_source);
    if (status != LINKED_LIST_SUCCESS) {
        return status;
    }

    // abort transfer if empty
    if ((*linked_list_source)->size == 0) {
        return LINKED_LIST_SOURCE_IS_EMPTY;
    }

    // check where to move the source linked list to the destination linked list
    if ((*linked_list_destination)->size == 0) {
        // the destination linked list becomes the source linked list, without losing the source linked list content
        (*linked_list_destination)->tail = (*linked_list_source)->tail;
    }
    else {
        // the linked lists are being concatenated in linked list destination
        (*linked_list_destination)->head->next = (*linked_list_source)->tail;
        (*linked_list_source)->tail->previous = (*linked_list_destination)->head;
    }

    // update new head
    (*linked_list_destination)->head = (*linked_list_source)->head;

    // remove content from source and update each sizes
    (*linked_list_source)->tail = NULL;
    (*linked_list_source)->head = NULL;

    (*linked_list_destination)->size += (*linked_list_source)->size;
    (*linked_list_source)->size = 0;

    return LINKED_LIST_SUCCESS;
}

/**
 * @brief Checks whether a linked list is valid.
 * @param linked_list is a linked_list_t pointer of pointer to be checked on
 * @info check for this error:
 *     LINKED_LIST_NO_MEMORY: linked list is invalid
 */
size_t linked_list_checking(linked_list_t **linked_list) {

    if ((*linked_list) == NULL) {
        return LINKED_LIST_NO_MEMORY;
    }

    return LINKED_LIST_SUCCESS;
}

/* Static module functions (implementation) */

/**
 * @brief Checks whether a linked list element is valid.
 * @param linked_list_element is a linked_list_element_t pointer of pointer to be checked on
 * @info check for this error:
 *     LINKED_LIST_ELEMENT_NO_MEMORY: linked list element is invalid
 */
size_t linked_list_element_checking(linked_list_element_t **linked_list_element) {

    if ((*linked_list_element) == NULL) {
        return LINKED_LIST_ELEMENT_NO_MEMORY;
    }

    return LINKED_LIST_ELEMENT_SUCCESS;
}

/**
 * @brief Creates a linked list list element an initializes it with data.
 * @param linked_list_element is a linked_list_element_t pointer of pointer to assign the new linked list element with data
 * @param data is a void pointer of pointer to the data
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *    LINKED_LIST_ELEMENT_NO_MEMORY: unable to allocate memory for linked list element
 */
size_t linked_list_element_create(linked_list_element_t **linked_list_element,
        void **data) {

    // allocate memory
    *linked_list_element = (linked_list_element_t* ) malloc(sizeof(linked_list_element_t));
    if (*linked_list_element == NULL) {
        return LINKED_LIST_ELEMENT_NO_MEMORY;
    }

    // default initialization with data
    (*linked_list_element)->data = *data;
    (*linked_list_element)->previous = NULL;
    (*linked_list_element)->next = NULL;

    return LINKED_LIST_ELEMENT_SUCCESS;
}
