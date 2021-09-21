/* semaphore.hpp - C++ semaphore class header */

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
//#include <inline/semLibInline.h>


#ifndef __INCsemaphorehpp
#define __INCsemaphorehpp

#ifdef __cplusplus

namespace vxworks 
{
class counting_semaphore  : public object< SEM_ID >
    {
    
private:

    int saved_options = SEM_Q_PRIORITY;

public:
    const int max = INT_MAX;
    
    counting_semaphore 
	(
	const char * name  
	)
	{
	named=true;
	id = ::semOpen( name, SEM_TYPE_COUNTING, 0, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    counting_semaphore 
	(
	const char * name,
	int options,
	int initialCount 
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_COUNTING, initialCount, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    counting_semaphore 
	(
	const char * name,
	int options, 
	int initialCount, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_COUNTING, initialCount, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    counting_semaphore ()
	{
	id = ::semCCreate(saved_options,0);
	if (id == SEM_ID_NULL)
	    throw;
	}

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
 
    ~counting_semaphore ()
	{
	if(named)
	    ::semClose(id);
	else
	    ::semDelete(id);
	}
    
    inline _Vx_STATUS give() noexcept 
	{
	return ::semCGive(id);
	}
	
    inline void release()
	{
	if ( OK != ::semCGive(id))
	    throw;
	}
    
    inline void release(std::ptrdiff_t n)
	{
	while(n)
	    {
	    if ( OK != ::semCGive(id))
	    	    throw;
	    --n;
	    }
	
	}
    
    inline void aquire()
	{
	if (OK != ::semCTake(id, WAIT_FOREVER))
	    throw;
	}

    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semCTake(id, timeout);
	}

    inline void try_aquire()
	{
	if (OK != ::semCTake(id, NO_WAIT))
	    throw;
	}
    
    // fill operation 
    inline void operator++()
 	{
	if ( OK != ::semCGive(id))
	    throw;
	}

    //empty operation
    inline void operator--()
 	{
	if (OK != ::semCTake(id, WAIT_FOREVER))
		    throw;
 	}
    
    };  // counting_semaphore 

class binary_semaphore  : public object< SEM_ID >
    {
    
private:

    int saved_options = SEM_Q_PRIORITY;

public:
    const int max = 1;
    
    binary_semaphore 
	(
	const char * name  
	)
	{
	named=true;
	id = ::semOpen( name, SEM_TYPE_BINARY, SEM_EMPTY, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    binary_semaphore 
	(
	const char * name,
	int options,
	SEM_B_STATE initialState 
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_BINARY, initialState, saved_options, 0, NULL);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    binary_semaphore 
	(
	const char * name,
	int options, 
	SEM_B_STATE initialState, 
	int mode,
	void * context
	)
	{
	named=true;
	saved_options = options;
	id = ::semOpen( name, SEM_TYPE_BINARY, initialState, saved_options, mode, context);
	if (id == SEM_ID_NULL)
		throw;
	}
    
    binary_semaphore ()
	{
	id = ::semBCreate(saved_options,SEM_EMPTY);
	if (id == SEM_ID_NULL)
	    throw;
	}

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

    inline void aquire()
	{
	if (OK != ::semBTake(id, WAIT_FOREVER))
	    throw;
	}

    inline _Vx_STATUS take
	(
	_Vx_ticks_t   timeout
	) noexcept
	{
	return ::semBTake(id, timeout);
	}

    inline void try_aquire()
	{
	if (OK != ::semBTake(id, NO_WAIT))
	    throw;
	}
    
    // fill operation 
    inline void operator++()
 	{
	if ( OK != ::semBGive(id))
	    throw;
	}

    //empty operation
    inline void operator--()
 	{
	if (OK != ::semBTake(id, WAIT_FOREVER))
		    throw;
 	}
    
    };  // binary_semaphore 



}	// vxworks
#endif  // __cplusplus 
#endif  // __INCsemaphorehpp
