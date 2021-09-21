/* shared_mutex.hpp - C++ shared mutex class header */

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
25oct20,brk  created
*/
#include <semLib.h>
#include <private/semLibP.h>
#include "object.hpp"
#include "chrono2tic.hpp"

#ifndef __INCsharedmutexhpp
#define __INCsharedmutexhpp

#ifdef __cplusplus

namespace vxworks 
{
typedef SEM_ID native_handle_type;


class shared_mutex : public object< SEM_ID >
    {
protected:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER  ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE  ;
#endif
    const int defaultMaxReaders = 100;
public:
    shared_mutex
	(
	const char * name  
	)
	{
	named=true;
	id = ::semOpen( name, SEM_TYPE_RW, defaultMaxReaders, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    ~shared_mutex()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}

    
    shared_mutex
	(
	const char * name,
	int maxReaders,
	int options
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_RW, maxReaders, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    shared_mutex
	(
	const char * name,
	int maxReaders,
	int options, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_RW, maxReaders, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    shared_mutex()
	{
	id = ::semRWCreate(saved_options, defaultMaxReaders);
	if (id == SEM_ID_NULL)
	    throw;
	}

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
 
    inline _Vx_STATUS give() noexcept 
	{
	return ::semRWGive(id);
	}
	
    inline void unlock()
	{
	if ( OK != ::semRWGive(id))
	    throw;
	}

    inline void lock()
	{
	if (OK != ::semWTake(id, WAIT_FOREVER))
	    throw;
	}


    inline bool try_lock()
	{
	if (OK == ::semWTake(id, NO_WAIT))
	    return true;
	else
	    return false;
	}

    inline native_handle_type native_handle()
	{
	return id;
	}

    // fill operation 
    inline void operator++()
 	{
	if ( OK != ::semRWGive(id))
	    throw;
	}

    //empty operation
    inline void operator--()
 	{
	if (OK != ::semWTake(id, WAIT_FOREVER))
		    throw;
 	}

    inline void lock_shared()
	{
	if (OK != ::semRTake(id, WAIT_FOREVER))
	    throw;
	}
    
    inline bool try_lock_shared()
	{
	if (OK==::semRTake(id, NO_WAIT))
	    return true;
	else
	    return false;
	}
    
    inline void unlock_shared()
	{
	if ( OK != ::semRWGive(id))
	    throw;
	}
    }; // shared_mutex

class shared_timed_mutex : shared_mutex
    {
public:
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semWTake(id, timeout);
	}
    
    inline _Vx_STATUS take_shared
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semRTake(id, timeout);
	}
    
    
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
