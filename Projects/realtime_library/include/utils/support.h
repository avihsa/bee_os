/**
**************************************************
* @file support.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date May 9, 2025
* @brief Module for useful macros
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Does not contain any functions, only defines!
  (#) Use STRINGIFY to generate a string from any
      given type.
  (#) Use ERROR_INFO to generate a combined error code
      from given status and subcomponent status
  (#) Use the different SEGGER contained defines to
      be able to record the control flow in SEGGER
      System View. Further info on the different
      functions called through these defines can be
      found in the SEGGER User Manual,
      with the exception of:
      SEGGER_SET_STACKPOINTER:
          calculates and sets the current stack
          usage to record it for System View.
      SEGGER_SYSVIEW_TASKINFO_DECLARE:
          adds the SEGGER_SYSVIEW_TASKINFO component
          to the task_t structure.
==================================================
@endverbatim
**************************************************
*/

#define STRINGIFY(x)	#x

// sub_component_status = subcomponent, register = where to store status, component_status = component itself
#define ERROR_INFO(sub_component_status, component_register, component_status) (((sub_component_status) << (component_register)) | (component_status))

#define SEGGER

#ifdef SEGGER
// Segger SysView
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_Conf.h"
// Functions
#define SEGGER_SYSVIEW_RECORD_ENTER_ISR 				SEGGER_SYSVIEW_RecordEnterISR
#define SEGGER_SYSVIEW_RECORD_EXIT_ISR  				SEGGER_SYSVIEW_RecordExitISR
#define SEGGER_SYSVIEW_CONF 			   				SEGGER_SYSVIEW_Conf
#define SEGGER_SYSVIEW_START							SEGGER_SYSVIEW_Start
#define SEGGER_SYSVIEW_SEND_TASK_INFO(Info) 			SEGGER_SYSVIEW_SendTaskInfo(Info)
#define SEGGER_SYSVIEW_TASK_CREATE(pTask)				SEGGER_SYSVIEW_OnTaskCreate((unsigned)(pTask))
#define SEGGER_SYSVIEW_TASK_START_READY(pTask)			SEGGER_SYSVIEW_OnTaskStartReady((unsigned)(pTask))
#define SEGGER_SYSVIEW_TASK_START_EXEC(pTask)			SEGGER_SYSVIEW_OnTaskStartExec((unsigned)(pTask))
#define SEGGER_SYSVIEW_TASK_STOP_READY(pTask, cause)	SEGGER_SYSVIEW_OnTaskStopReady((unsigned)(pTask), (cause))
#define SEGGER_SYSVIEW_TASK_STOP_EXEC					SEGGER_SYSVIEW_OnTaskStopExec
#define SEGGER_SYSVIEW_TASK_SYSTEM_IDLE					SEGGER_SYSVIEW_OnIdle

#define SEGGER_SET_STACKPOINTER(task)					(task)->info.StackUsage = ( (task)->info.StackBase - (task)->task_data->u32TaskSP ) / 4

// Types
#define SEGGER_SYSVIEW_TASKINFO_DECLARE(info) 			SEGGER_SYSVIEW_TASKINFO info


#else	// Segger inactive
// Functions
#define SEGGER_SYSVIEW_RECORD_ENTER_ISR()
#define SEGGER_SYSVIEW_RECORD_EXIT_ISR()
#define SEGGER_SYSVIEW_CONF()
#define SEGGER_SYSVIEW_START()
#define SEGGER_SYSVIEW_SEND_TASK_INFO(Info)
#define SEGGER_SYSVIEW_TASK_CREATE(pTask)
#define SEGGER_SYSVIEW_TASK_START_READY(pTask)
#define SEGGER_SYSVIEW_TASK_START_EXEC(pTask)
#define SEGGER_SYSVIEW_TASK_STOP_READY(pTask, cause)
#define SEGGER_SYSVIEW_TASK_STOP_EXEC(pTask)
#define SEGGER_SYSVIEW_TASK_SYSTEM_IDLE()

#define SEGGER_SET_STACKPOINTER(task)

// Types
#define SEGGER_SYSVIEW_TASKINFO_DECLARE(info)
#endif
