/* condition_variable.hpp - condition  */

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
#ifndef __INCconditionhpp
#define __INCconditionhpp

#include <condVarLib.h>
#include "object.hpp"

#ifdef __cplusplus

namespace vxworks 
{

class condition_variable : public object< CONDVAR_ID >
    {
public:
    ~condition_variable()
	{
	if(named)
	    ::condVarClose(id);
	else
	    ::condVarDelete(id);
	}
    
    condition_variable
	(
	 int options
	)
	{
	id = ::condVarCreate (options);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}
    
    condition_variable
	 (
	 const char *name, 
	 int options, 
	 int mode, 
    	 void *context
	 )
	{
	named=true;
	id = ::condVarOpen(name, options,  mode, context);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}

    condition_variable
	 (
	 const char *name, 
	 int options, 
	 int mode 
	 )
	{
	named=true;
	id = ::condVarOpen(name, options,  mode, NULL);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}

    condition_variable
	 (
	 const char *name
	 )
	{
	named=true;
	id = ::condVarOpen(name, 0,  0, NULL);
	if (id == CONDVAR_ID_NULL)
		    throw;
	}
	
    void notify_all() noexcept
	{
	::condVarBroadcast (id);
	}

    void notify_one() noexcept
	{
	::condVarSignal (id);
	}
    
    void wait( std::unique_lock<vxworks::mutex>& lock )
	{
	
	}

    };  // condition_variable
}      // vxworks
#endif // __cplusplus 
#endif // __INCqueuehpp  
