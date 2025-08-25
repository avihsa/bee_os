#include <string.h>
#include <criterion/criterion.h>
#include "utils/queue.h"
#include "utils/dictionary.h"
#include "utils/linked_list.h"
#include "kernel/task.h"
#include "kernel/kernel.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#define GET_FUNCTION_NAME(x) (#x)

#define SKIP_TEST_QUEUE         0
#define SKIP_TEST_DICTIONARY    0
#define SKIP_TEST_LINKED_LIST   0
#define SKIP_TEST_QUEUE         0
#define SKIP_TEST_KERNEL        0

Test(queue, null_operations, .disabled = SKIP_TEST_QUEUE) {
    int i = 42;
    int *k = &i;
    queue_t *queue = NULL;
    int status = queue_delete(&queue);
    cr_expect_eq(status, QUEUE_IS_NULL, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_delete), status);
    status = queue_push_front(&queue, &i, sizeof(i));
    cr_expect_eq(status, QUEUE_IS_NULL, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_push_front), status);
    status = queue_push_back(&queue, &i, sizeof(int));
    cr_expect_eq(status, QUEUE_IS_NULL, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_push_back), status);
    status = queue_read(&queue, (void **) &k);
    cr_expect_eq(status, QUEUE_IS_NULL, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_read), status);
    status = queue_peek(&queue, (void **) &k);
    cr_expect_eq(status, QUEUE_IS_NULL, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_peek), status);

    status = queue_create(&queue, 2, sizeof(int));
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_create), status);
    free(queue->data);
    queue->data = NULL;

    status = queue_delete(&queue);
    cr_expect_eq(status, QUEUE_DATA_NO_MEMORY, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_delete), status);
    status = queue_push_front(&queue, &i, sizeof(i));
    cr_expect_eq(status, QUEUE_DATA_NO_MEMORY, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_push_front), status);
    status = queue_push_back(&queue, &i, sizeof(int));
    cr_expect_eq(status, QUEUE_DATA_NO_MEMORY, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_push_back), status);
    status = queue_read(&queue, (void **) &k);
    cr_expect_eq(status, QUEUE_DATA_NO_MEMORY, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_read), status);
    status = queue_peek(&queue, (void **) &k);
    cr_expect_eq(status, QUEUE_DATA_NO_MEMORY, "queue shall be uninitialized on %s: %i", GET_FUNCTION_NAME(queue_peek), status);


}

Test(queue, iteration, .disabled = SKIP_TEST_QUEUE) {

    queue_t *queue = NULL;
    int n = 42;
    int status = queue_create(&queue, n, sizeof(int));
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_create), status);


    for (int i=0; i<n; i++) {
        status = queue_push_front(&queue, &i, sizeof(i));
        cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_push_front), status);
    }

    int k = 0;
    int *k_pointer = &k;
    for (int i=0; i<n; i++) {
        
        status = queue_read(&queue, (void **) &k_pointer);
        cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_read), status);
        cr_expect_eq(k, i, "expected read value the same as inserted value: %i==%i", k, i);
    }

    status = queue_delete(&queue);
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_delete), status);
}


Test(queue, queue_operations, .disabled = SKIP_TEST_QUEUE) {
    queue_t *queue = NULL;
    int status = queue_create(&queue, 2, sizeof(int));
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_create), status);

    int a = 42;
    status = queue_push_front(&queue, &a, sizeof(a));
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_push_front), status);

    int b = a/2;
    status = queue_push_back(&queue, &b, sizeof(int));
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_push_back), status);

    int peeked = 0;
    status = queue_peek(&queue, &peeked);
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_peek), status);
    cr_expect_eq(peeked, b, "expected the same value: %i, status:%i", peeked, status);

    

    int read_b = 0;
    int *read_b_pointer = &read_b;
    status = queue_read(&queue, (void **) &read_b_pointer);
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_read), status);
    cr_expect_eq(read_b, b, "Expected the same value: %i, status:%i", read_b, status);

    int read_a = 0;
    int *read_a_pointer = &read_a;
    status = queue_read(&queue, (void **) &read_a_pointer);
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_read), status);
    cr_expect_eq(read_a, a, "Expected the same value: %i, status:%i", peeked, status);

    status = queue_delete(&queue);
    cr_expect_eq(status, QUEUE_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(queue_delete), status);
}

Test(dictionary, null_operations, .disabled = SKIP_TEST_DICTIONARY) {
    dictionary_t *dictionary = NULL;
    size_t size = 4;

    int a = 42;
    int *b = &a;

    

    int status = dictionary_add(&dictionary, 0, (void **) &b);
    cr_expect_eq(status, DICTIONARY_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_add), status);

    status = dictionary_get(&dictionary, 0, (void **) &b);
    cr_expect_eq(status, DICTIONARY_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_get), status);

    status = dictionary_delete(&dictionary);
    cr_expect_eq(status, DICTIONARY_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_delete), status);



    status = dictionary_create(&dictionary, size);
    cr_expect_eq(status, DICTIONARY_SUCCESS, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_add), status);
    free(dictionary->data);
    dictionary->data = NULL;


    status = dictionary_add(&dictionary, 0, (void **) &b);
    cr_expect_eq(status, DICTIONARY_DATA_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_add), status);

    status = dictionary_get(&dictionary, 0, (void *) b);
    cr_expect_eq(status, DICTIONARY_DATA_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_get), status);

    status = dictionary_delete(&dictionary);
    cr_expect_eq(status, DICTIONARY_DATA_NO_MEMORY, "dictionary shall be uninitialized on %s: %i", GET_FUNCTION_NAME(dictionary_delete), status);

    free(dictionary);
}



Test(dictionary, iteration, .disabled = SKIP_TEST_DICTIONARY) {
    dictionary_t *dictionary = NULL;
    size_t size = 4;

    int a = 42;
    int* b = &a;

    int status = dictionary_create(&dictionary, size);
    cr_expect_eq(status, DICTIONARY_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(dictionary_create), status);

    for (size_t i = 0; i < size; i++) {
        status = dictionary_add(&dictionary, i, (void **) &b);
        cr_expect_eq(status, DICTIONARY_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(dictionary_add), status);
    }
    
    int *k = NULL;
    for (size_t i = 0; i < size; i++) {
        status = dictionary_get(&dictionary, i, (void **) &k);
        cr_expect_eq(status, DICTIONARY_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(dictionary_get), status);
        cr_expect_eq(*k, a, "expected read value the same as inserted value: %i==%i", *k, a);
    }

    
    status = dictionary_delete(&dictionary);
    cr_expect_eq(status, DICTIONARY_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(dictionary_delete), status);


}

Test(linked_list, null_operations, .disabled = SKIP_TEST_LINKED_LIST) {
    linked_list_t *linked_list = NULL;
    linked_list_element_t *linked_list_element = NULL;
    int status = linked_list_push_back(&linked_list, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_pop_front(&linked_list, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_pop_front), status);
    
    status = linked_list_pop_back(&linked_list, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_pop_back), status);

    status = linked_list_push_front(&linked_list, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_push_front), status);

    status = linked_list_delete(&linked_list);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

    status = linked_list_insert_after(&linked_list, &linked_list_element, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_insert_after), status);

    status = linked_list_pop_inner(&linked_list, &linked_list_element, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_pop_inner), status);

    status = linked_list_peek_back(&linked_list, NULL);
    cr_expect_eq(status, LINKED_LIST_NO_MEMORY, "link list shall be uninitialized on %s: %i", GET_FUNCTION_NAME(linked_list_peek_back), status);
}

Test(linked_list, iteration, .disabled = SKIP_TEST_LINKED_LIST) {
    linked_list_t *linked_list = NULL;
    int size = 4;

    int status = linked_list_create(&linked_list);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    int a = 42;
    int *a_pointer = &a;
    for (int i = 0; i<size; i++) {
        status = linked_list_push_back(&linked_list, (void **) &a_pointer);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);
    }

    int *popped = NULL;
    for (int i = 0; i<size; i++) {
        status = linked_list_pop_back(&linked_list, (void **) &popped);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);
        cr_expect_eq(*popped, a, "expected read value the same as inserted value: %i==%i", *popped, a);
    }

    for (int i = 0; i<size; i++) {
        status = linked_list_push_front(&linked_list, (void **) &a_pointer);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);
    }

    for (int i = 0; i<size; i++) {
        status = linked_list_pop_front(&linked_list, (void **) &popped);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);
        cr_expect_eq(*popped, a, "expected read value the same as inserted value: %i==%i", *popped, a);
    }


    status = linked_list_delete(&linked_list);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);
}


Test(linked_list, transfer, .disabled = SKIP_TEST_LINKED_LIST) {

    linked_list_t *linked_list_destination = NULL;
    linked_list_t *linked_list_source = NULL;

    int status = linked_list_create(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    status = linked_list_create(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);


    int a = 0;
    int *a_pointer = &a;

    status = linked_list_push_back(&linked_list_source, (void **) &a_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_transfer(&linked_list_destination, &linked_list_source, &linked_list_source->tail);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_transfer), status);

    int b = 1;
    int *b_pointer = &b;

    status = linked_list_push_back(&linked_list_source, (void **) &b_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_transfer(&linked_list_destination, &linked_list_source, &linked_list_source->head);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_transfer), status);


    int c = 2;
    int *c_pointer = &c;

    status = linked_list_push_back(&linked_list_source, (void **) &c_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);


    int d = 3;
    int *d_pointer = &d;

    status = linked_list_push_back(&linked_list_source, (void **) &d_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);


    int e = 4;
    int *e_pointer = &e;

    status = linked_list_push_back(&linked_list_source, (void **) &e_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_transfer(&linked_list_destination, &linked_list_source, &linked_list_source->head->previous);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_transfer), status);

    linked_list_element_t *iterator = linked_list_destination->tail;

    
    cr_expect_eq(*((int *) iterator->data), a, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), a);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), b, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), b);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), d, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), d);

    iterator = linked_list_source->tail;
    cr_expect_eq(*((int *) iterator->data), e, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), e);

    iterator = iterator->next;
    cr_expect_eq(*((int *) iterator->data), c, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), c);



    status = linked_list_delete(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

    status = linked_list_delete(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

}


Test(linked_list, transfer_after, .disabled = SKIP_TEST_LINKED_LIST) {
    linked_list_t *linked_list_destination = NULL;
    linked_list_t *linked_list_source = NULL;


    int status = linked_list_create(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    status = linked_list_create(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    int a = 1;
    int *a_pointer = &a;

    status = linked_list_push_back(&linked_list_source, (void **) &a_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_push_back(&linked_list_destination, (void **) &a_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);


    int c = 3;
    int *c_pointer = &c;

    status = linked_list_push_back(&linked_list_source, (void **) &c_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_transfer_after(&linked_list_destination, &linked_list_destination->tail, &linked_list_source, &linked_list_source->tail);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_transfer_after), status);

    int b = 2;
    int *b_pointer = &b;

    status = linked_list_push_front(&linked_list_source, (void **) &b_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_transfer_after(&linked_list_destination, &(linked_list_destination)->tail, &linked_list_source, &linked_list_source->head);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_transfer_after), status);

    linked_list_element_t *iterator = linked_list_destination->tail;

    cr_expect_eq(*((int *) iterator->data), a, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), a);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), b, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), b);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), c, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), c);


    status = linked_list_delete(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

    status = linked_list_delete(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

}

Test(linked_list, move_linked_list_after, .disabled = SKIP_TEST_LINKED_LIST) {
    linked_list_t *linked_list_destination = NULL;
    linked_list_t *linked_list_source = NULL;


    int status = linked_list_create(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    status = linked_list_create(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);

    int a = 1;
    int *a_pointer = &a;

    status = linked_list_push_back(&linked_list_source, (void **) &a_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_move_linked_list_after(&linked_list_destination, &linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_move_linked_list_after), status);

    cr_expect_eq(linked_list_source->size, 0, "expected source to be empty, but size : %i", linked_list_source->size);
    cr_expect_eq(linked_list_destination->size, 1, "expected destination to be empty of size 1, but size: %i", linked_list_destination->size);

    int b = 2;
    int *b_pointer = &b;

    status = linked_list_push_front(&linked_list_source, (void **) &b_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);


    int c = 3;
    int *c_pointer = &c;

    status = linked_list_push_front(&linked_list_source, (void **) &c_pointer);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_push_back), status);

    status = linked_list_move_linked_list_after(&linked_list_destination, &linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_move_linked_list_after), status);

    cr_expect_eq(linked_list_source->size, 0, "expected source to be empty, but size : %i", linked_list_source->size);
    cr_expect_eq(linked_list_destination->size, 3, "expected destination to be empty of size 3, but size: %i", linked_list_destination->size);


    linked_list_element_t *iterator = linked_list_destination->tail;

    cr_expect_eq(*((int *) iterator->data), a, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), a);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), b, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), b);
    iterator = iterator->next;

    cr_expect_eq(*((int *) iterator->data), c, "expected read value the same as inserted value: %i==%i", *((int *) iterator->data), c);


    status = linked_list_delete(&linked_list_destination);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

    status = linked_list_delete(&linked_list_source);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);

}


Test(linked_list, specified_element, .disabled = SKIP_TEST_LINKED_LIST) {
    linked_list_t *linked_list = NULL;
    int size = 4;

    int status = linked_list_create(&linked_list);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_create), status);


    int a = 42;
    int* b = &a;

    for (int i = 0; i < size; i++) {
        status = linked_list_insert_after(&linked_list, &linked_list->head, (void **) &b);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_insert_after), status);
    }

    int *popped = NULL;
    int *peeked = NULL;
    for (int i = 0; i < size; i++) {
        status = linked_list_peek_back(&linked_list, (void *) &peeked);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_peek_back), status);
        cr_expect_eq(*peeked, a, "expected read value the same as inserted value: %i==%i", *peeked, a);

        status = linked_list_pop_inner(&linked_list, &linked_list->tail, (void *) &popped);
        cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_pop_inner), status);
        cr_expect_eq(*popped, a, "expected read value the same as inserted value: %i==%i", *popped, a);

        cr_expect_eq(*popped,*peeked, "expected same value for peek and pop of same element: %i==%i", *popped, *peeked);
    }

    status = linked_list_delete(&linked_list);
    cr_expect_eq(status, LINKED_LIST_SUCCESS, "expected no error on %s: %i", GET_FUNCTION_NAME(linked_list_delete), status);
}