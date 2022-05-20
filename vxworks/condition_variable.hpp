/* condition_variable.hpp - condition  */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 */

#ifndef __INCconditionhpp
#define __INCconditionhpp

#include <condVarLib.h>
#include "object.hpp"
#include "mutex.hpp"
#include "chrono2tic.hpp"

#ifdef __cplusplus

namespace vxworks 
{

/*! 

\brief  A VxWorks Condition Variable Class

 The condition_variable class wraps the VxWorks condition variable library,
 [condVarLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/condVarLib.html). 
 The behaviour mimics std::condition_variable where possible.

 Condition variables allow tasks to wait on both an event to occur, and to
 acquire a mutex which protects an object associated with that event. Thus,
 condition variables are acquired by taking a mutex, and waiting on the
 condition variable. When the event is signalled, a task will be unblocked
 and the mutex given to that task, which then may access the protected
 resource. It should be noted that signalling a condition variable only 
 unblocks a waiting resource - if a condition variable is signalled before
 a task waits on that condition variable, then the waiting task must wait
 until the condition variable is signalled again.

 A condition variable may only be associated with one mutex at a time; if there
 are waiters, all waiters must have held the same mutex when the condition
 variable was waited on.
 
 The advantage of the VxWorks over the standard class is that a named variable
 may be shared between processes and with the kernel (similar to a POSIX
 pthread_cond_t type). 
 
*/
class condition_variable : public object< CONDVAR_ID >
    {
     
	    
public:
    /*! Delete a condition variable 
     */ 
    ~condition_variable()
	{
	if(named)
	    ::condVarClose(id);
	else
	    ::condVarDelete(id);
	}
    
    /*! 
    Create an unnamed variable
    Legitimate options are CONDVAR_Q_FIFO or CONDVAR_Q_PRIORITY, which
    specifies the queuing type of tasks which wait on this condition
    variable, or CONDVAR_INTERRUPTIBLE or CONDVAR_KERNEL_INTERRUPTIBLE
    that specifies the queue interruptibility.

    The CONDVAR_TASK_DELETION_WAKEUP option allows a delete-safe task
    waiting for a condition variable to be awakened early (before the
    condition is signalled) if some other task attempts to delete the
    delete-safe task.
      */	
    condition_variable
	(
	 int options
	)
	{
	id = ::condVarCreate (options);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}

    /*! 
    Create a named variable.
    Legitimate options are CONDVAR_Q_FIFO or CONDVAR_Q_PRIORITY, which
    specifies the queuing type of tasks which wait on this condition
    variable, or CONDVAR_INTERRUPTIBLE or CONDVAR_KERNEL_INTERRUPTIBLE
    that specifies the queue interruptibility.

    The CONDVAR_TASK_DELETION_WAKEUP option allows a delete-safe task
    waiting for a condition variable to be awakened early (before the
    condition is signalled) if some other task attempts to delete the
    delete-safe task.
    */	
    condition_variable
	 (
	 const string name, 
	 int options, 
	 int mode, 
    	 void *context
	 )
	{
	named=true;
	id = ::condVarOpen(name.c_str(), options,  mode, context);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}

    /*! 
    Create a named variable
    Legitimate options are CONDVAR_Q_FIFO or CONDVAR_Q_PRIORITY, which
    specifies the queuing type of tasks which wait on this condition
    variable, or CONDVAR_INTERRUPTIBLE or CONDVAR_KERNEL_INTERRUPTIBLE
    that specifies the queue interruptibility.

    The CONDVAR_TASK_DELETION_WAKEUP option allows a delete-safe task
    waiting for a condition variable to be awakened early (before the
    condition is signalled) if some other task attempts to delete the
    delete-safe task.
    */	
    condition_variable
	 (
	 const string name, 
	 int options, 
	 int mode 
	 )
	{
	named=true;
	id = ::condVarOpen(name.c_str(), options,  mode, NULL);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}

    /*! Create a named variable    
    */	
    condition_variable
	 (
	 const string name
	 )
	{
	named=true;
	id = ::condVarOpen(name.c_str(), 0,  0, NULL);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}
    /*! Release every task waiting on the condition variable */ 	
    inline void notify_all() noexcept
	{
	::condVarBroadcast (id);
	}

    /*! Signal a condition variable, 
        releasing one waiter if any tasks are waiting. If no task is waiting,
	this routine has no effect. This method also attempts to give the mutex
	associated with the condition variable to the waiting task when it is
	released.
    */ 	
    inline void notify_one() noexcept
	{
	::condVarSignal (id);
	}
    /*! Pends on a condition variable. 
        The mutexId parameter must refer to a mutex which is owned by the
	caller. The mutex will be atomically given until the task is unblocked,
	whereupon the mutex will again be taken.
    */	
    inline void wait( mutex& lock )
	{
	:: condVarWait (canWrite, handle(lock), WAIT_FOREVER);
	}
    
    
    /*! 
    Pends on a condition variable for std::duration. 
    The mutex parameter must refer to a mutex which is owned by the
    caller. The mutex will be atomically given until the task is unblocked,
    whereupon the mutex will again be taken.
	
    If a task times out then this method will return
    ERROR. When this method returns due to a timeout, it sets the **errno** to 
    S_objLib_OBJ_TIMEOUT (defined in objLib.h).
    */	
    template<class Rep, class Period>
    inline void wait_for( timed_mutex& lock,
                    const duration<Rep, Period>& relTime )
	{
	:: condVarWait (canWrite, lock.handle(), chrono2tic(relTime));
	}


    /*! 
    Pends on a condition variable for a period in ticks. 
    The mutexId parameter must refer to a mutex which is owned by the
    caller. The mutex will be atomically given until the task is unblocked,
    whereupon the mutex will again be taken.
	
    A timeout in ticks may be specified. If a task times out then this 
    method will return ERROR. A timeout of WAIT_FOREVER, or NO_WAIT may
    be specified in which case the task will pend forever, or not at all.

    When this method returns due to a timeout, it sets the **errno** to 
    S_objLib_OBJ_TIMEOUT (defined in objLib.h).
    */	
    inline void wait_for( timed_mutex& lock,  _Vx_ticks_t timeout )
	{
	:: condVarWait (canWrite, lock.handle(), timeout);
	}

    };  // condition_variable
}      // vxworks
#endif // __cplusplus 
#endif // __INCqueuehpp  
