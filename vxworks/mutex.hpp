/*! VxWorks C++ mutex class header */

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
    /*! instantiate a named mutex */ 
    mutexCommon
	(
	const char * name  
	)
	{
	named=true;
	id = ::semOpen( name, SEM_TYPE_MUTEX, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    ~mutexCommon()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}

    /*! instantiate a named mutex with specific options */ 
    mutexCommon
	(
	const char * name,
	int options
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_MUTEX, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    /*! instantiate a named mutex with specific options, mode and context  */ 
    mutexCommon
	(
	const char * name,
	int options, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_MUTEX, 0, saved_options, mode, context);
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

    /*! release ownership of a mutex */
    inline _Vx_STATUS give() noexcept 
	{
	return ::semMGive(id);
	}
	
    /*! release ownership of a mutex */	
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
    
}; // mutexCommon

/*! mutex class wrapping VxWorks semMLib */
class mutex : mutexCommon
    {
    
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE   ;
#endif
    };  // mutex

/*! mutex class wrapping VxWorks semMLib */
class recursive_mutex : mutexCommon
    {
    
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE   ;
#endif
    };  // recursive_mutex


/*! mutex class with timeouts wrapping VxWorks semMLib */
class timed_mutex : mutexCommon
    {
    
protected:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_NO_RECURSE   ;
#endif
    
public:
    /* wait to take ownership of mutex for period of time specified in system ticks */ 
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semMTake(id, timeout);
	}

    /* wait to take ownership of mutex for period of time specified as standard duration */ 
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
    /* wait to take ownership of mutex until a certain time */ 
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
    
/*! mutex class with timeouts wrapping VxWorks semMLib */
class recursive_timed_mutex : timed_mutex
    {
private:
#ifdef __RTP__
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE|SEM_USER   ;
#else
    int saved_options = SEM_Q_PRIORITY|SEM_INVERSION_SAFE   ;
#endif

    }; // recursive_timed_mutex
}	// vxworks
#endif  // __cplusplus 
#endif  // __INCmutexhpp
