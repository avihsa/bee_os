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

#ifndef TEST_TEST_STM_TEST_TASKS_H_
#define TEST_TEST_STM_TEST_TASKS_H_

#include <stdlib.h>

// Toggle different component tests here:
#define TEST_TASKS_BASIC                1
#define TEST_TASKS_SEMAPHORE            0
#define TEST_TASKS_MESSAGE_QUEUE        0
#define TEST_TASKS_DELAY                0
#define TEST_TASKS_EVENT_REGISTER       0
#define TEST_TASKS_MUTEX                0
#define TEST_TASKS_PRIORITY             0
#define TEST_TASKS_EXIT_TO_SCHEDULER    0   // cannot run alone!
#define TEST_TASKS_TERMINATE            0


size_t test_tasks_init(void);


#endif /* TEST_TEST_STM_TEST_TASKS_H_ */
