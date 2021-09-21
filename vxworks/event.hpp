/*! VxWorks event class */

/*
 * Copyright (c) 2020 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
07oct20,brk  created
*/
#ifndef __INCeventhpp
#define __INCeventhpp

#include <eventLib.h>
#include <thread>
#include "chrono2tic.hpp"

#ifdef __cplusplus

namespace vxworks 
{
/*! VxWorks C++ wrapper for eventLib 

\brief  A VxWorks Event Class
  
 This library provides a general light weight signalling. Any task (or thread)
 may send an event to another. 
  
 The event class wraps the 
  [eventLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/eventLib.html) 
 library. 
  
 Events are means of communication and synchronization between tasks and 
 interrupt service routines. Only tasks can wait for events, but events can
 be sent from task and ISR context. Events are an attractive lighter weight
 alternative to binary semaphores to perform task-to-task or ISR-to-task
 synchronization.
 
 Each VxWorks task has its own event register where events sent to it are
 stored. An event is identified as one bit in the event register. Currently 32
 different events are supported. Of those, eight are reserved for VxWorks
 internal use, and 24 are available for application development: VXEV01 through
 VXEV24. Application developers must have agreement on the assignment of
 individual events to avoid conflicting uses.

 The receiving task must explicitly check its event register to determine if it
 has received events, using receive(). A task can wait for multiple events by
 passing the bitwise OR of those events, and it can specify whether it is
 waiting for any one of those events, or for all of them. If the event or
 events for which a task is waiting for has not yet been received, the task
 pends (unless NO_WAIT timeout is specified).

 Events are delivered asynchronously to a task with send( ) and can be sent
 regardless of the state of the receiving task. Events are not accumulated;
 if the same event is received by a task several times before it calls 
 receive( ), it is treated as if it were received only once - similarly to
 a binary semaphore. It is not possible to track how many times each event
 has been received by a task.
 
*/
*/
class event
    {
    
public:
     /*!  send an event to a VxWorks task ID */
    inline _Vx_STATUS send (
		     TASK_ID taskId,
		     _Vx_event_t events)
	    {
	    return ::eventSend (taskId, events);
	    }

    /*! send an event to a std::thread()  */
    inline _Vx_STATUS send (
		     std::thread thread,
		     _Vx_event_t events)
	    {
	    // assert: the first member of the native handle 
	    // is the task ID
	    void * t = thread.native_handle();
	    TASK_ID * pTid = static_cast<TASK_ID *>(t);
	    return ::eventSend ( * pTid , events);
	    }

     /*!  
     Pend and wait to receive any events sent to the current task or thread. 
	 
     The *options* parameter is used to control various aspects of this
     method's behaviour. One of which is to specify if the caller wishes
     to wait for all events to be received or only one. One of the
     following must be specified:

     - **EVENTS_WAIT_ANY** (0x1)
       Wait for any one of the wanted events.

     - **EVENTS_WAIT_ALL** (0x0)
       Wait for all wanted events.

     Another *option* is to specify if the events written to 
     *pEventsReceived* are only those received and wanted or all events
     received. Note that an event can be received at any time, including
     before ::receive( ) is called. By default this method returns only
     wanted events unless the following option is specified:

     - **EVENTS_RETURN_ALL** (0x2)
       Causes the routine to return all received events whether they are
       wanted (as specified in events) or not. It also causes all events
       to be cleared from the task's events register.

     The third *option* available allows the caller to specify if the
     received unwanted events are to be cleared from the calling task's
     events register. They are cleared by default unless the following
     option is specified:

     - **EVENTS_KEEP_UNWANTED** (0x4)
       Tells this routine not to clear unwanted events. In cases where
       the **EVENTS_RETURN_ALL** option is used, all events are cleared even
       if this option is selected. Wanted events are always cleared hence
       this option has not effect on them.

     . 
     Lastly, it is possible to retrieve events that have already been
     received without affecting the events register by selecting the
     following option:

     - **EVENTS_FETCH** (0x80)
       If this option is specified, the contents of the calling task's
       events register are copied to the location pointed to by 
       *pEventsReceived* and the routine returns immediately. The events
       are not cleared from the register. The events and timeout 
       arguments are ignored and so are all other options specified.

     - **EVENTS_Q_INTERRUPTIBLE** (0x10000)
       If this option is specified, the receive( ) call will return **ERROR**
       with *errno* set to **EINTR** if the call was interrupted by a signal
       handler (and the calling task belongs to a non-kernel RTP). Note that
       the same behavior occurs if eventLibInit( ) was called with the 
       **EVENTS_INTERRUPTIBLE** option.

     - **EVENTS_TASK_DELETION_WAKEUP** (0x20000)
       If a deletion-safe task calls receive( ) specifying this option, and
       the receive( ) call pends (or is about to pend), the call may return
       **ERROR** early with *errno* set to **EDOOM**, if some other task 
       attempts to delete the task calling receive( ) while it is deletion
       safe, before or while it is pended. Note that this effect is
       single-shot; a subsequent call to receive( ) after an **EDOOM** 
       error return will not result in another such **EDOOM** return, 
       unless another attempt to delete the task has occurred.

     .
     The *timeout* parameter specifies the number of ticks to wait for wanted
     events. It can also have the following special values:

     - **NO_WAIT** (0)
	Return immediately, even if no events have arrived.

     - **WAIT_FOREVER** (-1U)
	Never time out.

     The received events are copied to the location pointed to by
     *pEventsReceived* unless a NULL pointer is passed. This is true even
     when the routine returns **ERROR**. If **EVENTS_WAIT_ALL** is
     specified this includes those events received before timeout has
     expired. In addition, events received after timeout has expired,
     but before the caller has returned, may be copied to *pEventsReceived*
     regardless of whether *EVENTS_WAIT_ALL* or *EVENTS_WAIT_ANY* is
     specified.
     */
    inline _Vx_STATUS receive (
			_Vx_event_t events,
			_Vx_UINT32 options,
			_Vx_ticks_t timeout,
			_Vx_event_t *eventsReceived)
	    {
	    return ::eventReceiveEx(events, options, timeout, eventsReceived);
	    }

    /*! Pend and  wait to receive any events sent to the current task for a period of time.
        Specify a timeout in std::duration()     */
    template<class Rep, class Period>
    inline _Vx_STATUS receive_for (
			_Vx_event_t events,
			_Vx_UINT32 options,
			const duration<Rep, Period>& relTime,
			_Vx_event_t *eventsReceived)
	    {
	    return ::eventReceiveEx(events, options, chrono2tic(relTime), eventsReceived);
	    }    

    /*! Pend and  wait to receive any events sent to the current task until a specific
        time.  
        Specify a timeout of a std::time_point()     */
    template< class Clock, class Duration >
    inline _Vx_STATUS receive_until (
			_Vx_event_t events,
			_Vx_UINT32 options,
			const time_point<Clock,Duration>& absTime,
			_Vx_event_t *eventsReceived)
	    {
	    return ::eventReceiveEx(events, options, time_point2tic(absTime), eventsReceived);
	    }    
    
    
    /*! Pend and  wait to receive any events sent the current task for a specific number of system ticks. 
       The *timeout* parameter specifies the number of ticks to wait for wanted
       events. It can also have the following special values:

       *  **NO_WAIT** (0)
          Return immediately, even if no events have arrived.

       *  **WAIT_FOREVER** (-1U)
          Never time out.
    */
    inline _Vx_STATUS receive (
			_Vx_event_t events,
			_Vx_UINT32 options,
			_Vx_ticks_t timeout)
	    {
	    return ::eventReceiveEx(events, options, timeout, NULL);
	    }
    

    /*!  check for specific events sent the current task without pending  */	
    inline _Vx_STATUS poll(
		    _Vx_event_t events,
		    _Vx_UINT32 options,
		    _Vx_event_t *eventsReceived)
	    {
	    return ::eventReceiveEx(events, options, NO_WAIT, eventsReceived);
	    }
    
    /*!  check for any event sent the current task without pending  */
    inline _Vx_STATUS fetch( 
		     _Vx_event_t *eventsReceived)
	    {
	    return ::eventReceiveEx(0, EVENTS_FETCH, NO_WAIT, eventsReceived);
	    }
	
    /*!  clear any events for the current task. Since events can be received at
    any time, the caller cannot assume its events register is actually cleared
    by the time this method returns unless interrupts are locked when this
    method is called.  */
    inline _Vx_STATUS clear(void)
	    {
	    return ::eventClear();
	    }
    };  // event
}	// vxworks
#endif  // __cplusplus 
#endif  // __INCeventhpp
