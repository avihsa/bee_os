# EZS_SS25

## Project details

This project was an university project at the Technische Hochschule Mittelhessen for the course "Echtzeitsysteme". It taught us how rtos core components actually work by implementing it by ourselves.

We were using an discovery iot node with an stm32l475. By studying the cortex m4 programming manual it was quite simple to create a custom rtos.

It is called bee_os, because when the used stm was shutdown, it always stopped at a funny memory location.

## Import library

- load realtime_library as project to workspace
    - open 'File -> Open Projects from File System'
    - import source choose directory containing the realtime library (not the realtime library directory)
    - select realtime_library
- include and source
    - includes
        - open project properties
        - open 'C/C++ General -> Paths and Symbols -> Includes'
        - add following files as workspace path
            - 'realtime_library/include' => library
            - 'realtime_library/test/test_stm' => for demonstrating the library
    - sources
        - open project properties
        - open 'C/C++ General -> Paths and Symbols -> Source Location'
        - link following folders in the filesystem:
            - 'realtime_library/src' => library
            - 'realtime_library/stm' => library
            - 'realtime_library/test/test_stm' => for demonstrating the library

    - segger
        - define '#define SEGGER' in support.h or open 'project properties -> C/C++ General -> Paths and Symbols -> Symbols' and add SEGGER
        - add following files as workspace path
            - 'realtime_library/segger-sysview/Config
            - 'realtime_library/segger-sysview/SEGGER
        - link following sources in the filesystem:
            - 'realtime_library/segger-sysview'


## Troubleshooting

After automatically importing the library as a project, trying to build it might result in a linker error!

As far as we could resolve it, the only way to satisfy the linkers needs is to reset the source locations 
and link them by hand again with a different name (eg. realtime_library/src -> realtime_library/src2).

In that case, please follow the steps above (-> include and source -> sources) again.


## Architecture and Design Decision

The realtime library uses pointers for all data structure to allow to move them to different lists.

The realtime_library/src/kernel.c implements independent kernel functions from the stm32 while realtime_library/stm/kernel.c implements depends on the stm32.

We are using linked list to allow for an infinite amount for tasks. Tasks can be stored in so called priority groups, which are just linked list.
It means multiple tasks can have same priority and it is easy to implement priority inheritance. A complete priority group is moved up on every task switch. The linked list is capable of moving elements from a list to another list, which allows for O(1) operations.

The task struct acts as a layer to the task tcb struct, which contains contains control inforamtion, which helps to store necessary information for the kernel usage.

Kernel obejct were implemented based of [Real-Time Concepts for Embedded Systems](https://www.taylorfrancis.com/books/mono/10.1201/9781482280821/real-time-concepts-embedded-systems-qing-li-caroline-yao)

Normally all functions return an error code, which can be constructed based of subcomponents. It helps to see what problems occured and can be checked in their corresponding header file.

The documentation can be built with doxygen in realtime_library/docs. This requires the package 'graphviz' to be installed.

## Testing

To test basic subcompoents unit tests were created. To build unit tests you need [cmake](https://cmake.org/) and [criterion](https://criterion.readthedocs.io/en/master/intro.html) installed.

If the support.h contains '#define SEGGER', the build will fail.

Build and run instructions:

    - cd realtime_library
    - mkdir build
    - cmake ..
    - cmake --build .
    - ./test --verbose

Following result is expected:

    [----] Criterion v2.4.1
    [====] Running 2 tests from dictionary:
    [====] Running 6 tests from linked_list:
    [RUN ] dictionary::iteration
    [RUN ] dictionary::null_operations
    [PASS] dictionary::iteration: (0.00s)
    [PASS] dictionary::null_operations: (0.00s)
    [RUN ] linked_list::iteration
    [PASS] linked_list::iteration: (0.01s)
    [RUN ] linked_list::specified_element
    [RUN ] linked_list::move_linked_list_after
    [PASS] linked_list::specified_element: (0.00s)
    [PASS] linked_list::move_linked_list_after: (0.00s)
    [====] Running 3 tests from queue:
    [RUN ] linked_list::null_operations
    [RUN ] linked_list::transfer
    [PASS] linked_list::null_operations: (0.00s)
    [PASS] linked_list::transfer: (0.00s)
    [RUN ] linked_list::transfer_after
    [PASS] linked_list::transfer_after: (0.00s)
    [RUN ] queue::iteration
    [PASS] queue::iteration: (0.00s)
    [RUN ] queue::queue_operations
    [PASS] queue::queue_operations: (0.00s)
    [RUN ] queue::null_operations
    [PASS] queue::null_operations: (0.00s)
    [====] Synthesis: Tested: 11 | Passing: 11 | Failing: 0 | Crashing: 0

Kernel test project:

Import segger-test2 as a project into workspace like with realtime_library.

To test kernel functions enable or disable them in test_task.h 1 for enable and 0 for disable.
Set a breakpoint at kernel_start() in test_tasks.c. If it is not set, segger systemviewer can miss some signals or task names are incorrect.

A monitoring task will update the stack usages roughly every second.

When pressing the user button a message can be send to a task and if pressed often enough an event is sent to a task to turn off the system.

More information can be found in realtime_library/docs/html/index.html.
