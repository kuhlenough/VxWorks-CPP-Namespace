/* semaphore.hpp - C++ semaphore class header */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 */

#include <semLib.h>
#include <private/semLibP.h>
#include "object.hpp"
#include "chrono2tic.hpp"
#include <cstring>

#ifndef __INCsemaphorehpp
#define __INCsemaphorehpp

#ifdef __cplusplus

namespace vxworks 
{
/*!

\brief  A VxWorks Counting Semaphore Class
  
 This library provides a counting semaphore class for managing 
 mutually exclusive access to resources and signalling between
 threads.
  
 The counting_semaphore class wraps the counting semaphore library,
 [semCLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semCLib.html). 
 The behaviour mimics the C++ std::counting_semaphore where possible.
 
 The advantage of the VxWorks over the standard class is that a named semaphore
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
*/
	
	
class counting_semaphore  : public object< SEM_ID >
    {
    
private:

    int saved_options = SEM_Q_PRIORITY;

public:
    const int max = INT_MAX;
    
    //! Create a named counting semaphore
    counting_semaphore 
	(
	const string name  
	)
	{
	named=true;
	id = ::semOpen( name.c_str().c_str(), SEM_TYPE_COUNTING, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! Create a named counting semaphore specifying options and initial count. 
    counting_semaphore 
	(
	const string name,
	int options,
	int initialCount 
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str().c_str(), SEM_TYPE_COUNTING, initialCount, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! Create a named counting semaphore specifying options, initial count, mode and context. 
    counting_semaphore 
	(
	const string name,
	int options, 
	int initialCount, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str().c_str(), SEM_TYPE_COUNTING, initialCount, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! Create an unnamed counting semaphore
    counting_semaphore ()
	{
	id = ::semCCreate(saved_options,0);
	if (id == SEM_ID_NULL)
	    throw;
	}
    
    //! Create an unnamed counting semaphore specifying options and initial count. 
    counting_semaphore 
	(
	 int options,
	 int initialCount
	 )
	{
	saved_options = options;	
	id = ::semCCreate(options,initialCount);
	if (id == SEM_ID_NULL)
	    throw;
	}
 
    //! Delete a counting semaphore   
    ~counting_semaphore ()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}
    
    //! give a semaphore (fill)
    inline _Vx_STATUS give() noexcept 
	{
	return ::semCGive(id);
	}
	
    //! give a semaphore (fill)
    inline void release()
	{
	if ( OK != ::semCGive(id))
	    throw;
	}
    
    //! give a semaphore multiple times (fill)
    inline void release(std::ptrdiff_t n)
	{
	while(n)
	    {
	    if ( OK != ::semCGive(id))
	    	    throw;
	    --n;
	    }
	
	}
    //! pend and wait to acquire a semaphore 
    inline void acquire()
	{
	if (OK != ::semCTake(id, WAIT_FOREVER))
	    throw;
	}
	
    //! pend and wait to acquire a semaphore for std::duration
    template<class Rep, class Period>
    inline _Vx_STATUS take
	(
	const duration<Rep, Period>& relTime,
	) noexcept
	{
	return ::semCTake(id, chrono2tic(relTime));
	}

   
    //! pend and wait to acquire a semaphore for *timeout* tics 
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semCTake(id, timeout);
	}

    //! try to acquire a semaphore without pending  
    inline void try_aquire()
	{
	if (OK != ::semCTake(id, NO_WAIT))
	    throw;
	}
    
    //! fill operation 
    inline void operator++()
 	{
	if ( OK != ::semCGive(id))
	    throw;
	}

    //! empty operation
    inline void operator--()
 	{
	if (OK != ::semCTake(id, WAIT_FOREVER))
		    throw;
 	}
    
    };  // counting_semaphore 
/*!

\brief  A VxWorks Binary Semaphore Class
  
 This library provides a binary semaphore class for managing 
 mutually exclusive access to resources and signalling between
 threads.
  
 The binary_semaphore class wraps the binary semaphore library,
 [semBLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/semBLib.html). 
 The behaviour mimics the C++ std::binary_semaphore where possible.
 
 The advantage of the VxWorks over the standard class is that a named semaphore
 may be shared between processes and with the kernel (similar to a POSIX
 semaphore). 
 
*/
class binary_semaphore  : public object< SEM_ID >
    {
    
private:

    int saved_options = SEM_Q_PRIORITY;

public:
    const int max = 1;
    
    //! create a named binary semaphore 
    binary_semaphore 
	(
	const string name  
	)
	{
	named=true;
	id = ::semOpen( name.c_str(), SEM_TYPE_BINARY, SEM_EMPTY, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! create a named binary semaphore 
    binary_semaphore 
	(
	const string name,
	int options,
	SEM_B_STATE initialState 
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_BINARY, initialState, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! create a named binary semaphore 
    binary_semaphore 
	(
	const string name,
	int options, 
	SEM_B_STATE initialState, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name.c_str(), SEM_TYPE_BINARY, initialState, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    //! create a unnamed binary semaphore 
    binary_semaphore ()
	{
	id = ::semBCreate(saved_options,SEM_EMPTY);
	if (id == SEM_ID_NULL)
	    throw;
	}

    //! create a unnamed binary semaphore 
    binary_semaphore 
	(
	 int options,
	 SEM_B_STATE initialState
	 )
	{
	saved_options = options;	
	id = ::semBCreate(options,initialState);
	if (id == SEM_ID_NULL)
	    throw;
	}
 
    //! delete a binary semaphore 
    ~binary_semaphore ()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}
    
    inline _Vx_STATUS give() noexcept 
	{
	return ::semBGive(id);
	}
	
    inline void release()
	{
	if ( OK != ::semBGive(id))
	    throw;
	}
    
    inline void release(std::ptrdiff_t n)
	{
	while(n)
	    {
	    if ( OK != ::semBGive(id))
	    	    throw;
	    --n;
	    }
	}

    //! pend and wait to acquire a semaphore for std::duration
    inline void aquire()
	{
	if (OK != ::semBTake(id, WAIT_FOREVER))
	    throw;
	}

    //! pend and wait to acquire a semaphore 
    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semBTake(id, timeout);
	}

    //! pend and wait to acquire a semaphore for std::duration
    template<class Rep, class Period>
    inline _Vx_STATUS take
	(
	const duration<Rep, Period>& relTime,
	) noexcept
	{
	return ::semBTake(id, chrono2tic(relTime));
	}

     //! try to acquire a semaphore without pending  
     inline void try_aquire()
	{
	if (OK != ::semBTake(id, NO_WAIT))
	    throw;
	}
    
    //!  fill operation 
    inline void operator++()
 	{
	if ( OK != ::semBGive(id))
	    throw;
	}

    //! empty operation
    inline void operator--()
 	{
	if (OK != ::semBTake(id, WAIT_FOREVER))
		    throw;
 	}
    
    };  // binary_semaphore 



}	// vxworks
#endif  // __cplusplus 
#endif  // __INCsemaphorehpp
