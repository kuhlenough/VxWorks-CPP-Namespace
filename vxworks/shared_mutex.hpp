/* shared_mutex.hpp - C++ shared mutex class header */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 */

#include <semLib.h>
#include <private/semLibP.h>
#include "object.hpp"
#include "chrono2tic.hpp"
#include <cstring>

#ifndef __INCsharedmutexhpp
#define __INCsharedmutexhpp

#ifdef __cplusplus

namespace vxworks 
{
typedef SEM_ID native_handle_type;


/*!

\brief  A VxWorks Shared Mutex Class
  
 This library provides a full featured mutex semaphore class for managing 
 mutually exclusive access to resources.
  
 The shared_mutex class wraps the Read-Write VxWorks semaphore library,
 [semRWLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semRWLib.html). 
 The behaviour mimics the C++17 std::shared_mutex where possible.
 
 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). The shared_mutex may be taken recursively by the owner without
 error.  

 Though the maximum number of concurrent readers is set per semaphore at
 creation there is also a limit on the maximum concurrent readers for a system
 as defined by SEM_RW_MAX_CONCURRENT_READERS. The value of 
 SEM_RW_MAX_CONCURRENT_READERS will be used as the semaphore's maximum if a
 larger value is specified at creation. This value should be set no larger
 than necessary as a larger maximum concurrent reader value will result in
 longer interrupt and task response. 
 
 This class is initialized with a maximum of 20 readers unless specified
 otherwise.
 
*/
class shared_mutex : public object< SEM_ID >
    {
protected:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER  ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE  ;
#endif
    const int defaultMaxReaders = 20;
public:
    /*! Create a named shared mutex */
    shared_mutex
	(
	const string name  
	)
	{
	named=true;
	id = ::semOpen( name.c_str(), SEM_TYPE_RW, defaultMaxReaders, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! Delete a shared mutex */
    ~shared_mutex()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}

    /*! Create a named shared mutex specifying options and maximum readers */
    shared_mutex
	(
	const string name,
	int maxReaders,
	int options
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_RW, maxReaders, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! Create a named shared mutex specifying options, maximum readers, mode or context */
    shared_mutex
	(
	const string name,
	int maxReaders,
	int options, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_RW, maxReaders, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! Create a unnamed shared mutex */
    shared_mutex()
	{
	id = ::semRWCreate(saved_options, defaultMaxReaders);
	if (id == SEM_ID_NULL)
	    throw;
	}

    /*! Create a unnamed shared mutex specifying the maximum readers */
    shared_mutex
	(
	 int options,
	 int maxReaders
	 )
	{
	saved_options = options;	
	id = ::semRWCreate(options,  maxReaders);
	if (id == SEM_ID_NULL)
	    throw;
	}
 
    /*!  give (empty) a shared mutex */
    inline _Vx_STATUS give() noexcept 
	{
	return ::semRWGive(id);
	}
	
    /*!  unlock (fill) a shared mutex */
    inline void unlock()
	{
	if ( OK != ::semRWGive(id))
	    throw;
	}

    /*!  exclusive lock (empty) a shared mutex */
    inline void lock()
	{
	if (OK != ::semWTake(id, WAIT_FOREVER))
	    throw;
	}


    /*!  exclusively  try to lock (empty) a shared mutex */
    inline bool try_lock()
	{
	if (OK == ::semWTake(id, NO_WAIT))
	    return true;
	else
	    return false;
	}

    /*!  return C API object ID */ 
    inline native_handle_type native_handle()
	{
	return id;
	}

    //! fill operation (unlock)
    inline void operator++()
 	{
	if ( OK != ::semRWGive(id))
	    throw;
	}

    //! exclusive empty operation (lock)
    inline void operator--()
 	{
	if (OK != ::semWTake(id, WAIT_FOREVER))
		    throw;
 	}

    //! shared lock (empty)
    inline void lock_shared()
	{
	if (OK != ::semRTake(id, WAIT_FOREVER))
	    throw;
	}
    
    //! attempt to acquire shared lock
    inline bool try_lock_shared()
	{
	if (OK==::semRTake(id, NO_WAIT))
	    return true;
	else
	    return false;
	}
    
    //! shared unlock (fill)
    inline void unlock_shared()
	{
	if ( OK != ::semRWGive(id))
	    throw;
	}
    }; // shared_mutex



/*!

\brief  A VxWorks Shared Timed Mutex Class
  
 This library provides a full featured mutex semaphore class for managing 
 mutually exclusive access to resources.
  
 The shared_mutex class wraps the Read-Write VxWorks semaphore library,
 [semRWLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semRWLib.html). 
 The behaviour mimics the C++17 std::shared_timed_mutex where possible.
 
 VxWorks does not distinguish between timed and un-timed mutexes, the  
 differentiation is in the wrapper classes to follow the C++ convention. 
 
 The advantage of the VxWorks over the standard class is that a named mutex
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore).  
 
 All std::duration parameters are converted to system tics, and 
 are rounded accordingly.  

 Though the maximum number of concurrent readers is set per semaphore at
 creation there is also a limit on the maximum concurrent readers for a system
 as defined by SEM_RW_MAX_CONCURRENT_READERS. The value of 
 SEM_RW_MAX_CONCURRENT_READERS will be used as the semaphore's maximum if a
 larger value is specified at creation. This value should be set no larger
 than necessary as a larger maximum concurrent reader value will result in
 longer interrupt and task response. 
 
*/


class shared_timed_mutex : public shared_mutex
    {
public:
    
    //! pend and wait to exclusively acquire a lock for a specified period 
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semWTake(id, timeout);
	}
    
    //! pend and wait to acquire a shared lock for a specified period 
    inline _Vx_STATUS take_shared
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semRTake(id, timeout);
	}
    
    
    //! pend and wait to exclusively acquire a lock for a specified period 
    template<class Rep, class Period>
    inline bool try_lock_for(const duration<Rep, Period>& relTime) 
	{
       if ( OK == ::semWTake(id, chrono2tic(relTime)))
		{
		return true;
		}
	    else 
		{
		return false;
		}		     
	}

    //! pend and wait to exclusively acquire a lock until a deadline 
    template< class Clock, class Duration >
    inline bool try_lock_until (const time_point<Clock,Duration>& abs_time)
	{
	bool again = true;
	_Vx_ticks_t tics = time_point2tic(abs_time);
	STATUS status;
	
	if (tics == 0)
	    status = ::semWTake(id, NO_WAIT); 
	else
	    status = ::semWTake(id, tics);
	
	if (status == OK)
	    {
	    return true;
	    }
	else 
	    {
	    return false;
	    }
	}

    //! pend and wait to acquire a shared lock for a specified period 
    template<class Rep, class Period>
    inline bool try_lock_shared_for(const duration<Rep, Period>& relTime) 
	{
       if ( OK == ::semRTake(id, chrono2tic(relTime)))
		{
		return true;
		}
	    else 
		{
		return false;
		}		     
	}

    //! pend and wait to acquire a shared lock until a deadline 
    template< class Clock, class Duration >
    inline bool try_lock_shared_until (const time_point<Clock,Duration>& abs_time)
	{
	bool again = true;
	_Vx_ticks_t tics = time_point2tic(abs_time);
	STATUS status;
	
	if (tics == 0)
	    status = ::semRTake(id, NO_WAIT); 
	else
	    status = ::semRTake(id, tics);
	
	if (status == OK)
	    {
	    return true;
	    }
	else 
	    {
	    return false;
	    }
	}
    
    }; // shared_timed_mutex  
}	// vxworks
#endif  // __cplusplus 
#endif  // __INCsharedmutexhpp    
