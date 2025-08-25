/**
**************************************************
* @file test_tasks.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Jun 12, 2025
* @brief Module for creating and test tasks
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'test_tasks_init' to initialize and start
      the kernel, while also starting all activated
      test tasks
==================================================
@endverbatim
**************************************************
*/

#include <stdio.h>
#include <stdarg.h>

#include "test_tasks.h"
#include "kernel/kernel.h"
#include "kernel/semaphore.h"
#include "utils/support.h"
#include "stm32l4xx_hal.h"
#include "main.h"

// Since each task is assigned an id, we specify a certain range of how many
// tasks each testing component can contain. Can be increased if necessary,
// as long as MAX_TASKS_PER_COMPONENT * tested components < TASK_MAX_PRIORITY.
#define MAX_TASKS_PER_COMPONENT     5

#define TASK_ID_BASIC               0
#define TASK_ID_SEMAPHORE           MAX_TASKS_PER_COMPONENT
#define TASK_ID_MESSAGE_QUEUE       MAX_TASKS_PER_COMPONENT * 2
#define TASK_ID_DELAY               MAX_TASKS_PER_COMPONENT * 3
#define TASK_ID_EVENT_REGISTER      MAX_TASKS_PER_COMPONENT * 4
#define TASK_ID_MUTEX               MAX_TASKS_PER_COMPONENT * 5
#define TASK_ID_PRIORITY            MAX_TASKS_PER_COMPONENT * 6
#define TASK_ID_EXIT_TO_SCHEDULER   MAX_TASKS_PER_COMPONENT * 7
#define TASK_ID_TERMINATE           MAX_TASKS_PER_COMPONENT * 8

#define DEFAULT_TASK_RUNTIME        1

int g_count = 0;
size_t semaphore_id = 0;

int g_count_non_blocking = 0;
size_t non_blocking_semaphore_id = 1;

int g_count_mutex = 0;
size_t mutex_id = 0;

int g_count_non_blocking_mutex = 0;
size_t non_blocking_mutex_id = 1;

message_queue_identifier_t *message_queue_identifier;
message_queue_identifier_t *message_queue_identifier_blocked_sender = NULL;

char g_print_buffer[256];

// We decided to deactivate printing because
// 1 - Printing was unsafe in combination with SEGGER System View
// 2 - We had no way of actually retrieving the output
void safe_print(const char *text, ...) {
    kernel_disable_interrupts();

    va_list args;
    va_start(args, text);
//    vprintf(text, args);
    va_end(args);

    kernel_enable_interrupts();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    if (GPIO_Pin == BUTTON_EXTI13_Pin
            && HAL_GPIO_ReadPin(BUTTON_EXTI13_GPIO_Port, BUTTON_EXTI13_Pin) == GPIO_PIN_RESET) {
        kernel_event_send(TASK_ID_TERMINATE+1, 1 << 3);

#if TEST_TASKS_MESSAGE_QUEUE
        // can't send a message if message queues are missing
        uint32_t message = 42;
        kernel_message_queue_send(&message_queue_identifier, &message, sizeof(uint32_t), true);
#endif
    }
}


// -------------- basic test tasks --------------
size_t test_tasks(void) {
    while(1) {
        safe_print("Hello, task!\n");

    }

    return 0;
}

size_t test_tasks_2(void) {
    while(1) {
        safe_print("Hello, task 2!\n");
    }

    return 0;
}

size_t test_tasks_3(void) {
    while(1) {
        safe_print("Hello, task 3!\n");
    }

    return 0;
}

// -------------- semaphore testing --------------
size_t test_tasks_4(void) {
    size_t status = 0;
    while(1) {
        status = kernel_semaphore_acquire(semaphore_id);
        safe_print("%d\n", status);
        g_count++;
        status = kernel_semaphore_release(semaphore_id);
        safe_print("%d\n", status);
    }

    return 0;
}

size_t test_tasks_5(void) {
    size_t status = 0;
    while(1) {
        status = kernel_semaphore_acquire(semaphore_id);
        safe_print("%d\n", status);
        g_count--;
        status = kernel_semaphore_release(semaphore_id);
        safe_print("%d\n", status);
    }

    return 0;
}

size_t test_tasks_5_2(void) {
    size_t status = 0;
    while(1) {
        status = kernel_semaphore_acquire(semaphore_id);
        safe_print("%d\n", status);
        g_count /= 2;
        status = kernel_semaphore_release(semaphore_id);
        safe_print("%d\n", status);
    }

    return 0;
}

size_t test_tasks_non_blocking_semaphore_add(void) {
    size_t status = 0;
    while(1) {
        status = kernel_semaphore_acquire_non_blocking(non_blocking_semaphore_id);
        safe_print("%d\n", status);
        if (status == KERNEL_SUCCESS) {
            g_count_non_blocking++;
            status = kernel_semaphore_release_non_blocking(non_blocking_semaphore_id);
            safe_print("%d\n", status);
        }
        else {
            kernel_exit_to_scheduler();
        }
    }
}

size_t test_tasks_non_blocking_semaphore_sub(void) {
    size_t status = 0;
    while(1) {
        status = kernel_semaphore_acquire_non_blocking(non_blocking_semaphore_id);
        safe_print("%d\n", status);
        if (status == KERNEL_SUCCESS) {
            g_count_non_blocking--;
            status = kernel_semaphore_release_non_blocking(non_blocking_semaphore_id);
            safe_print("%d\n", status);
        }
        else {
            kernel_exit_to_scheduler();
        }
    }
}

// -------------- message queue testing --------------
size_t test_tasks_6(void) {
    uint32_t u32_number[1] = {0};
    u32_number[0] = 42;
    while(1) {
        kernel_message_queue_send(&message_queue_identifier, (void *) u32_number, sizeof(uint32_t), false);
    }

    return 0;
}


size_t test_tasks_7(void) {
    uint32_t u32_number = 0;
    uint32_t *u32_number_pointer = &u32_number;

    while(1) {
        kernel_message_queue_receive(&message_queue_identifier, (void **) &u32_number_pointer);
        safe_print("%ld\n", u32_number);
    }

    return 0;
}

size_t test_tasks_blocked_sender(void) {
    uint32_t message = 0;
    while(1) {
        kernel_message_queue_send_blocking(&message_queue_identifier_blocked_sender, &message, sizeof(uint32_t), false);
        message++;
    }

    return 0;
}

size_t test_task_receiver(void) {
    uint32_t message = 0;
    uint32_t *message_pointer = &message;
    while (1) {
        kernel_message_queue_receive(&message_queue_identifier_blocked_sender, (void *) &message_pointer);
        kernel_message_queue_receive(&message_queue_identifier_blocked_sender, (void *) &message_pointer);
        //kernel_message_queue_receive(&message_queue_identifier_blocked_sender, (void *) &message_pointer);
        kernel_delay(1000);
    }
}

// -------------- delay testing --------------
size_t test_tasks_8(void) {
    while(1) {
        kernel_delay(10);
        safe_print("Hello, task 8!\n");
    }

    return 0;
}

size_t test_tasks_9(void) {
    while(1) {
        kernel_delay(15);
        safe_print("Hello, task 9!\n");
    }

    return 0;
}

size_t test_tasks_10(void) {
    while(1) {
        kernel_delay(12);
        safe_print("Hello, task 10!\n");
    }

    return 0;
}

// -------------- event register testing --------------
size_t test_tasks_11(void) {

    size_t received_events = 0;
    while(1) {
        kernel_event_send(TASK_ID_EVENT_REGISTER+1, 1 << 0);
        kernel_event_receive_blocking(&received_events);
    }

    return 0;
}

size_t test_tasks_12(void) {
    size_t recevied_events = 0;
    while(1) {
        if (kernel_event_receive_timeout(&recevied_events)==KERNEL_SUCCESS) {
            kernel_event_send(TASK_ID_EVENT_REGISTER, 1 << 5);
        }
        kernel_event_send(TASK_ID_EVENT_REGISTER+2, 1 << 3);
    }

    return 0;
}


void notification_condition(size_t *received_events, size_t wanted_events) {
    static size_t count;
    if (count > 10 && (*received_events & (1 << 3)) > 0) {
        count = 0;
    }
    else {
        count++;
        *received_events &= ~wanted_events;
    }
}

size_t test_tasks_13(void) {
    size_t recevied_events = 0;
    while(1) {
        kernel_event_send(TASK_ID_EVENT_REGISTER+1, 1 << 3);
        kernel_event_send(TASK_ID_EVENT_REGISTER, 1 << 3);
        kernel_event_receive_blocking(&recevied_events);
    }

    return 0;
}

// -------------- mutex testing --------------
size_t test_tasks_mutex_1(void) {
    size_t status = 0;
    while(1) {
        status = kernel_mutex_acquire(semaphore_id);
        safe_print("%d\n", status);
        g_count_mutex--;
        status = kernel_mutex_release(semaphore_id);
        safe_print("%d\n", status);
    }
}

size_t test_tasks_mutex_2(void) {
    size_t status = 0;
    while(1) {
        status = kernel_mutex_acquire(mutex_id);
        safe_print("%d\n", status);
        g_count_mutex++;
        status = kernel_mutex_release(mutex_id);
        safe_print("%d\n", status);
    }
}

size_t test_tasks_non_blocking_mutex_add(void) {
    size_t status = 0;
    while(1) {
        status = kernel_mutex_acquire_non_blocking(non_blocking_mutex_id);
        safe_print("%d\n", status);
        if (status == KERNEL_SUCCESS) {
            g_count_non_blocking_mutex++;
            status = kernel_semaphore_release_non_blocking(non_blocking_mutex_id);
            safe_print("%d\n", status);
        }
        else {
            kernel_exit_to_scheduler();
        }
    }
}

size_t test_tasks_non_blocking_mutex_sub(void) {
    size_t status = 0;
    while(1) {
        status = kernel_mutex_acquire_non_blocking(non_blocking_mutex_id);
        safe_print("%d\n", status);
        if (status == KERNEL_SUCCESS) {
            g_count_non_blocking_mutex--;
            status = kernel_semaphore_release_non_blocking(non_blocking_mutex_id);
            safe_print("%d\n", status);
        }
        else {
            kernel_exit_to_scheduler();
        }
    }
}

// -------------- priority testing --------------
size_t test_tasks_middle_priority_1(void) {
    while(1) {
        safe_print("Hello, task!\n");
    }

    return 0;
}

size_t test_tasks_middle_priority_2(void) {
    while(1) {
        safe_print("Hello, task!\n");
    }

    return 0;
}


size_t test_tasks_lowest_priority_1(void) {
    while(1) {
        safe_print("Hello, task!\n");
    }

    return 0;
}

size_t test_tasks_lowest_priority_2(void) {
    while(1) {
        safe_print("Hello, task!\n");
    }

    return 0;
}

// -------------- exit to scheduler testing --------------
size_t test_task_exit_to_scheduler(void) {
    while (1) {
        kernel_delay_blocking(10);
        kernel_exit_to_scheduler();
    }

    return 0;
}

// -------------- terminate testing --------------
size_t test_task_terminate(void) {
    return 42;
}

void notification_terminate_condition(size_t *received_events, size_t wanted_events) {
    static size_t count;
    if (count > 10 && (*received_events & (1 << 3)) > 0) {
        count = 0;
    }
    else {
        count++;
        *received_events &= ~wanted_events;
    }
}

size_t test_task_terminate_rtos(void) {
    size_t event = 0;
    kernel_event_receive_blocking(&event);

    kernel_shutdown();

    return 0;
}

// -------------- monitor task --------------
#ifdef SEGGER
size_t monitor_task(void) {
    dictionary_t* list = kernel_debug_get_list_of_tasks();
    task_t *task = NULL;
    size_t status = 0;
    while(1) {
        for (size_t i = 0; i < list->size; i++) {
            status = dictionary_get(&list, i, (void **) &task);

            if (status != DICTIONARY_VALUE_IS_NULL) {
                SEGGER_SET_STACKPOINTER(task);
                SEGGER_SYSVIEW_SEND_TASK_INFO(&task->info);
            }
        }
        kernel_delay(1000);
    }
}
#endif

size_t test_tasks_init(void) {
    kernel_init();

    // configure and initialize SystemView
    SEGGER_SYSVIEW_CONF();

#ifdef SEGGER
    // monitor task: only to be used with SEGGER System View
    kernel_add_task(monitor_task, TASK_MAX_PRIORITY, STRINGIFY(monitor_task), 0, 1, 0, NULL, 0);
#endif

#if TEST_TASKS_BASIC
    // basic test tasks
    kernel_add_task(test_tasks, TASK_ID_BASIC, "Hello World", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_2, TASK_ID_BASIC+1, STRINGIFY(test_tasks_2), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_3, TASK_ID_BASIC+2, STRINGIFY(test_tasks_3), 1, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_SEMAPHORE
    // semaphore testing
    kernel_semaphore_create(&semaphore_id, SEMAPHORE_BINARY_TOKEN);
    kernel_add_task(test_tasks_5, TASK_ID_SEMAPHORE, "Decrement", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_4, TASK_ID_SEMAPHORE+1, "Increment", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_5_2, TASK_ID_SEMAPHORE+2, "Half", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);

    kernel_semaphore_create(&non_blocking_semaphore_id, SEMAPHORE_BINARY_TOKEN);
    kernel_add_task(test_tasks_non_blocking_semaphore_add, TASK_ID_SEMAPHORE+3, "Non Blocking Semaphore Add", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_non_blocking_semaphore_sub, TASK_ID_SEMAPHORE+4, "Non Blocking Semaphore Subtract", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_MESSAGE_QUEUE
    // message queue testing
    kernel_message_queue_create(&message_queue_identifier, "testing_message_queue", KERNEL_DEFAULT_QUEUE_SIZE, sizeof(uint32_t));
    kernel_add_task(test_tasks_6, TASK_ID_MESSAGE_QUEUE, STRINGIFY(test_tasks_6), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_7, TASK_ID_MESSAGE_QUEUE+1, STRINGIFY(test_tasks_7), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);

    kernel_message_queue_create(&message_queue_identifier_blocked_sender, "testing_message_queue", 2, sizeof(uint32_t));
    kernel_add_task(test_tasks_blocked_sender, TASK_ID_MESSAGE_QUEUE+2, STRINGIFY(test_tasks_blocked_sender), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_task_receiver, TASK_ID_MESSAGE_QUEUE+3, STRINGIFY(test_task_receiver), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_DELAY
    // delay testing
    kernel_add_task(test_tasks_8, TASK_ID_DELAY, STRINGIFY(test_tasks_8), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_9, TASK_ID_DELAY+1, STRINGIFY(test_tasks_9), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_10, TASK_ID_DELAY+2, STRINGIFY(test_tasks_10), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_EVENT_REGISTER
    // event register testing
    kernel_add_task(test_tasks_11, TASK_ID_EVENT_REGISTER, "send_receive", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 10);
    kernel_add_task(test_tasks_12, TASK_ID_EVENT_REGISTER+1, "send_receive", 0, DEFAULT_TASK_RUNTIME, 1 << 0, NULL, 10);
    kernel_add_task(test_tasks_13, TASK_ID_EVENT_REGISTER+2, "send_receive_nonsense", 0, DEFAULT_TASK_RUNTIME, 1 << 3, notification_condition, 10);
#endif

#if TEST_TASKS_MUTEX
    // mutex testing
    kernel_mutex_create(&mutex_id);
    kernel_add_task(test_tasks_mutex_1, TASK_ID_MUTEX, STRINGIFY(test_tasks_mutex_1), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_mutex_2, TASK_ID_MUTEX+1, STRINGIFY(test_tasks_mutex_2), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);

    kernel_mutex_create(&non_blocking_mutex_id);
    kernel_add_task(test_tasks_non_blocking_mutex_add, TASK_ID_MUTEX+2, "Non Blocking Mutex Add", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_non_blocking_mutex_sub, TASK_ID_MUTEX+3, "Non Blocking Mutex Subtract", 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_PRIORITY
    // priority testing
    kernel_add_task(test_tasks_middle_priority_1, TASK_ID_PRIORITY, STRINGIFY(test_tasks_middle_priority_1), TASK_MAX_PRIORITY / 2, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_middle_priority_2, TASK_ID_PRIORITY+1, STRINGIFY(test_tasks_middle_priority_2), TASK_MAX_PRIORITY / 2, DEFAULT_TASK_RUNTIME, 0, NULL, 0);

    kernel_add_task(test_tasks_lowest_priority_1, TASK_ID_PRIORITY+2, STRINGIFY(test_tasks_lowest_priority_1), TASK_MAX_PRIORITY - 1, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_tasks_lowest_priority_2, TASK_ID_PRIORITY+3, STRINGIFY(test_tasks_lowest_priority_2), TASK_MAX_PRIORITY - 1, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_EXIT_TO_SCHEDULER
    // exit to scheduler testing
    kernel_add_task(test_task_exit_to_scheduler, TASK_ID_EXIT_TO_SCHEDULER, STRINGIFY(test_task_exit_to_scheduler), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
#endif

#if TEST_TASKS_TERMINATE
    // terminate task
    kernel_add_task(test_task_terminate, TASK_ID_TERMINATE, STRINGIFY(test_task_terminate), 0, DEFAULT_TASK_RUNTIME, 0, NULL, 0);
    kernel_add_task(test_task_terminate_rtos, TASK_ID_TERMINATE+1, "WALL-E", 0, DEFAULT_TASK_RUNTIME, 1 << 3, notification_terminate_condition, 10);
#endif

    // We attempted using a delay here to wait for SEGGER System View to recognize the start of the recording.
    // For some unknown reason, this causes a hard fault soon during kernel_start.
    // We recommend instead to set a breakpoint here at the call of kernel_start and start the
    // System View recording after launch of debugging, but before arriving at this breakpoint.
    //  HAL_Delay(2000);
    kernel_start();

    return 0;
}
