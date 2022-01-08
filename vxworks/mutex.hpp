/*! VxWorks C++ mutex class header */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 */

#include <semLib.h>
#include <private/semLibP.h>

#include "object.hpp"
#include "chrono2tic.hpp"

#ifndef __INCmutexhpp
#define __INCmutexhpp

#ifdef __cplusplus

namespace vxworks 
{
typedef SEM_ID native_handle_type;

/*! base mutex class */ 
class mutexCommon : public object< SEM_ID >
{
protected:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER  ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE  ;
#endif

public:
    /*! instantiate a named mutex 
        the mutex has priority queuing of pended tasks and inversion safety */ 
    mutexCommon
	(
	const string name  
	)
	{
	named=true;
	id = ::semOpen( name.c_str(), SEM_TYPE_MUTEX, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! delete a mutex */ 
    ~mutexCommon()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}

    /*! instantiate a named mutex with specific options 
    mutex options are: 
    
    * SEM_Q_PRIORITY (0x1)
          Queue pended tasks on the basis of their priority.
    * SEM_Q_FIFO (0x0)
          Queue pended tasks on a first-in-first-out basis.
    * SEM_DELETE_SAFE (0x4)
          Protect a task that owns the semaphore from unexpected deletion. This option enables an implicit taskSafe( ) for each semTake( ), and an implicit taskUnsafe( ) for each semGive( ).
    * SEM_INVERSION_SAFE (0x8)
          Protect the system from priority inversion. With this option, the task owning the semaphore will execute at the highest priority of the tasks pended on the semaphore, if it is higher than its current priority. This option must be accompanied by the SEM_Q_PRIORITY queuing mode.
    * SEM_EVENTSEND_ERR_NOTIFY (0x10)
          When the semaphore is given, if a task is registered for events and the actual sending of events fails, a value of ERROR is returned and the errno is set accordingly. This option is off by default.
    * SEM_INTERRUPTIBLE (0x20)
          Signals sent to an RTP task blocked on a semaphore created with this option, would make the task ready and return with ERROR and errno set to EINTR. This option has no affect for a kernel task blocked on the same semaphore created with this option. This option is off by default.
    * SEM_TASK_DELETION_WAKEUP (0x2000)
          When this option is set for a semaphore, calls to semTake( ) made by a (typically deletion-safe) task may return ERROR early, with errno set to EDOOM, if some other task is attempting to destroy (delete or restart) the task making the semTake( ) call.
    * SEM_ROBUST (0x20000)
          This option creates a robust mutex. With this option, current or future takers of the mutex are notified if the owning task terminates while holding the mutex lock.
    */ 
    mutexCommon
	(
	const string name,
	int options
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_MUTEX, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! instantiate a named mutex with specific options, mode and context  
        mutex options are: 
    
    * SEM_Q_PRIORITY (0x1)
          Queue pended tasks on the basis of their priority.
    * SEM_Q_FIFO (0x0)
          Queue pended tasks on a first-in-first-out basis.
    * SEM_DELETE_SAFE (0x4)
          Protect a task that owns the semaphore from unexpected deletion. This
	  option enables an implicit taskSafe( ) for each mutex::take(), and an
	  implicit taskUnsafe( ) for each mutex::give( ).
    * SEM_INVERSION_SAFE (0x8)
          Protect the system from priority inversion. With this option, the task
	  owning the semaphore will execute at the highest priority of the tasks
	  pended on the semaphore, if it is higher than its current priority.
	  This option must be accompanied by the SEM_Q_PRIORITY queuing mode.
    * SEM_EVENTSEND_ERR_NOTIFY (0x10)
          When the semaphore is given, if a task is registered for events and the
	  actual sending of events fails, a value of ERROR is returned and
	  the errno is set accordingly. This option is off by default.
    * SEM_INTERRUPTIBLE (0x20)
          Signals sent to an RTP task blocked on a semaphore created with this
	  option, would make the task ready and return with ERROR and errno set
	  to EINTR. This option has no affect for a kernel task blocked on the
	  same semaphore created with this option. This option is off by default.
    * SEM_TASK_DELETION_WAKEUP (0x2000)
          When this option is set for a semaphore, calls to mutex::take( ) made
	  by a (typically deletion-safe) task may return ERROR early, with errno
	  set to EDOOM, if some other task is attempting to destroy
	  (delete or restart) the task making the mutex::take( ) call.
    * SEM_ROBUST (0x20000)
          This option creates a robust mutex. With this option, current or future
	  takers of the mutex are notified if the owning task terminates while
	  holding the mutex lock.
    
    */ 
    mutexCommon
	(
	const string name,
	int options, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_MUTEX, 0, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    
    /*! instantiate an unnamed mutex  */ 
    mutexCommon()
	{
	id = ::semMCreate(saved_options);
	if (id == SEM_ID_NULL)
	    throw;
	}

    /*! instantiate an unnamed mutex with specific options  */ 
    mutexCommon
	(
	 int options
	 )
	{
	saved_options = options;	
	id = ::semMCreate(options);
	if (id == SEM_ID_NULL)
	    throw;
	}

    /*! release ownership of a mutex (fill) */
    inline _Vx_STATUS give() noexcept 
	{
	return ::semMGive(id);
	}
	
    /*! release ownership of a mutex (fill) */	
    inline void unlock()
	{
	if ( OK != ::semMGive(id))
	    throw;
	}

    /*! block until the current task can take ownership of a mutex */
    inline void lock()
	{
	if (OK != ::semMTake(id, WAIT_FOREVER))
	    throw;
	}



    /*! attempt to take ownership of a mutex without pending*/
    inline bool try_lock()
	{
	if (OK == ::semMTake(id, NO_WAIT))
	    return true;
	else
	    return false;
	}

    /*!  return C API object ID */
    native_handle_type native_handle()
	{
	return id;
	}
    
    /*!  fill or give a mutex */ 
    inline void operator++()
 	{
	if ( OK != ::semMGive(id))
	    throw;
	}

    /*! block until the current task can take ownership (or empty) a mutex */
    inline void operator--()
 	{
	if (OK != ::semMTake(id, WAIT_FOREVER))
		    throw;
 	}
    
    /*!  make an inconsistent robust mutex consistent
         
	 A robust mutex becomes inconsistent if its owner terminates while
	 holding the mutex. The next task that takes the mutex will be
	 notified of the state by throwing or returning ERROR with 
	 errno S_semLib_EOWNERDEAD.
     */
    inline _Vx_STATUS consistent() 
	{  
	return ::semMConsistent(id);
        }
	
}; // mutexCommon

/*!

\brief  A VxWorks Mutex Class
  
 This library provides a full featured mutex class for managing 
 mutually exclusive access to resources.
  
 The **mutex** class wraps the VxWorks mutex library,
 [semMLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semMLib.html). 
 The behaviour mimics the C++ std::mutex where possible.
 
 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
 All member functions are inherited, the only difference between a mutex and
 a recursive_mutex is the SEM_NO_RECURSE bit in the constructor options. 
 
*/
class mutex : public mutexCommon
    {
    
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE   ;
#endif
public:
    /*! block until the current task can take ownership of a mutex 
       
	The behaviour of this method is similar to that of ::lock() on a mutex,
	differing only in that instrumentation and sanity checks are omitted 
	to improve performance:

        * Omit semaphore validation with this option. For SMP systems this
	    enables a lock-free algorithm for acquiring or releasing uncontested
	    mutex.

        * No error checking is performed. This includes tests for interrupt
	    context, ownership of the mutex, and validation of tasks unpended
	    by calls to this method.

        * Only system viewer events associated with unpending a waiting task
	    will be sent.

        * Recursion will not be tracked. That is, successive calls to 
	  ::lock_quickly( ) with this option specified will result in deadlock.

       .
       This method is intended for those applications where performance is
       critical. 

     ####CAVEATS
       This method does not support robust mutex semaphores.
    */
    inline _Vx_STATUS take_quickly()
	{
	return ::semMTakeScalable(id, WAIT_FOREVER, saved_options|
	    SEM_NO_ID_VALIDATE|SEM_NO_ERROR_CHECK|SEM_NO_SYSTEM_VIEWER|SEM_NO_RECURSE );
	}


    /*! release ownership of a mutex (fill) */	
    inline _Vx_STATUS give_quickly() noexcept 
	{
	return ::semMGiveScalable(id, WAIT_FOREVER, saved_options|
	    SEM_NO_ID_VALIDATE|SEM_NO_ERROR_CHECK|SEM_NO_SYSTEM_VIEWER|SEM_NO_RECURSE );
	}
	return ::semMGive(id);
	}





    };  // mutex

/*!

\brief  A VxWorks Recursive Mutex Class
  
 This library provides a full featured mutex class for managing 
 mutually exclusive access to resources.
  
 The **mutex** class wraps the VxWorks mutex library,
 [semMLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semMLib.html). 
 The behaviour mimics the C++ std::mutex where possible.
 
 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
 All member functions are inherited, the only difference between a mutex and
 a recursive_mutex is the SEM_NO_RECURSE bit in the constructor options. 
 
*/
class recursive_mutex : public mutexCommon
    {
    
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE   ;
#endif
    /*! block until the current task can take ownership of a mutex 
       
	The behaviour of this method is similar to that of ::lock() on a mutex,
	differing only in that instrumentation and sanity checks are omitted 
	to improve performance:

        * Omit semaphore validation with this option. For SMP systems this
	    enables a lock-free algorithm for acquiring or releasing uncontested
	    mutex.

        * No error checking is performed. This includes tests for interrupt
	    context, ownership of the mutex, and validation of tasks unpended
	    by calls to this method.

        * Only system viewer events associated with unpending a waiting task
	    will be sent.

       .
       This method is intended for those applications where performance is
       critical. 

     ####CAVEATS
       This method does not support robust mutex semaphores.
    */
    inline _Vx_STATUS take_quickly()
	{
	return ::semMTakeScalable(id, WAIT_FOREVER, saved_options|
	    SEM_NO_ID_VALIDATE|SEM_NO_ERROR_CHECK|SEM_NO_SYSTEM_VIEWER );
	}

    };  // recursive_mutex


/*!

\brief  A VxWorks Timed Mutex Class
  
 This library provides a full featured mutex class for managing 
 mutually exclusive access to resources.
  
 The **recursive_mutex** class wraps the VxWorks mutex library,
 [semMLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semMLib.html). 
 The behaviour mimics the C++ std::timed_mutex where possible.
 
 VxWorks mutexes are recursive by default, and the only difference in this 
 class is that the SEM_NO_RECURSE bit is not set in the constructor 
 *option* parameter 

 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
*/
class timed_mutex : public mutexCommon
    {
    
protected:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE   ;
#endif
    static const quick_options =  SEM_NO_ID_VALIDATE|SEM_NO_ERROR_CHECK|SEM_NO_SYSTEM_VIEWER|SEM_NO_RECURSE ;   
public:
   /*! block until the current task can take ownership of a mutex 
       
	The behaviour of this method is similar to that of ::lock() on a mutex,
	differing only in that instrumentation and sanity checks are omitted 
	to improve performance:

        * Omit semaphore validation with this option. For SMP systems this
	    enables a lock-free algorithm for acquiring or releasing uncontested
	    mutex.

        * No error checking is performed. This includes tests for interrupt
	    context, ownership of the mutex, and validation of tasks unpended
	    by calls to this method.

        * Only system viewer events associated with unpending a waiting task
	    will be sent.

        * Recursion will not be tracked. That is, successive calls to 
	  ::lock_quickly( ) with this option specified will result in deadlock.

       .
       This method is intended for those applications where performance is
       critical. 

     ####CAVEATS
       This method does not support robust mutex semaphores.
    */
    inline _Vx_STATUS take_quickly_for(_Vx_ticks_t  timeout)
	{
	return ::semMTakeScalable(id, timeout, saved_option|quick_options);
	}

    /*! optimized give (or fill) of a mutex  
       
	The behaviour of this method is similar to that of ::lock() on a mutex,
	differing only in that instrumentation and sanity checks are omitted 
	to improve performance:

        * Omit semaphore validation with this option. For SMP systems this
	    enables a lock-free algorithm for acquiring or releasing uncontested
	    mutex.

        * No error checking is performed. This includes tests for interrupt
	    context, ownership of the mutex, and validation of tasks unpended
	    by calls to this method.

        * Only system viewer events associated with unpending a waiting task
	    will be sent.

        * Recursion will not be tracked. That is, successive calls to 
	  ::lock_quickly( ) with this option specified will result in deadlock.

       .
       This method is intended for those applications where performance is
       critical. 

     ####CAVEATS
       This method does not support robust mutex semaphores.
    */
 
    inline _Vx_STATUS give_quickly_for(_Vx_ticks_t  timeout) 
	{
	return ::semMTakeScalable(id, timeout, saved_option|quick_options);
	}

    /*! wait to take ownership of mutex for period of time specified in system ticks */ 
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semMTake(id, timeout);
	}

    /*! wait to take ownership of mutex for period of time specified as standard duration */ 
     template<class Rep, class Period>
     inline bool try_lock_for(const duration<Rep, Period>& relTime) 
	{
        if ( OK == ::semMTake(id, chrono2tic(relTime)))
		{
		return true;
		}
	    else 
		{
		return false;
		}		     
	}
    /*! wait to take ownership of mutex until a certain time */ 
     template< class Clock, class Duration >
     inline bool try_lock_until (const time_point<Clock,Duration>& abs_time)
	{
	bool again = true;
	_Vx_ticks_t tics = time_point2tic(abs_time);
	STATUS status;
	
	if (tics == 0)
	    status = ::semMTake(id, NO_WAIT); 
	else
	    status = ::semMTake(id, tics);
	
	if (status == OK)
	    {
	    return true;
	    }
	else 
	    {
	    return false;
	    }
	}
    };  // timed_mutex
    
/*!

\brief  A VxWorks Recursive Timed Mutex Class
  
 This library provides a full featured mutex class for managing 
 mutually exclusive access to resources.
  
 The **recursive_timed_mutex** class wraps the VxWorks mutex library,
 [semMLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semMLib.html). 
 The behaviour mimics the C++ std::recursive_timed_mutex where possible.
 
 VxWorks does not distinguish between timed and un-timed mutexes, the  
 differentiation is in the wrapper classes to follow the C++ convention.

 VxWorks mutexes are recursive by default, and the only difference in this 
 class is that the SEM_NO_RECURSE bit is not set in the constructor 
 *option* parameter 

 All std::duration parameters are converted to system tics, and 
 are rounded accordingly.  
 
 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
*/
class recursive_timed_mutex : public timed_mutex
    {
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE   ;
#endif
    static const quick_options =  SEM_NO_ID_VALIDATE|SEM_NO_ERROR_CHECK|SEM_NO_SYSTEM_VIEWER ;   

    }; // recursive_timed_mutex
} // vxworks
#endif  // __cplusplus 
#endif  // __INCmutexhpp
