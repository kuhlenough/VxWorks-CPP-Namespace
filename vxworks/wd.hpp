/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 *
 */

#ifndef __INCwdhpp
#define __INCwdhpp

#ifndef __RTP__ 
#include <wdLib.h>
#include <functional>
#include "object.hpp"

#ifdef __cplusplus

namespace vxworks 
{
	
/*!

\brief  A VxWorks Watchdog Class
  
 This library provides a general watchdog timer facility. Any task may create
 a watchdog timer and use it to run a specified callback in the context of the
 system-clock ISR, after a specified delay.
  
 The watchdog class wraps the 
  [wdLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/wdLib.html) 
 library. It is only available in the kernel.
  
 Special care must be taken with the callback() as it executes as pseudo interrupt.  
 It has no context so must not block, call system functions, use RTTI or C++ exceptions.

 The kernel watchdog class should not be confused with the /watchdog I/O device that
 is available with some boards.    
 
*/
class wd : public object< WDOG_ID >
    {

private:
    
    std::function<void() noexcept> func;
    
    void callback() noexcept
	{
	func();
	}
    
    static void _callback( wd * me ) noexcept
	{
	    me->callback();
	}
    
public:

    /*!
    *   Create a VxWorks watchdog timer.
    *    
    */
    wd()
	{
	id = ::wdCreate();
	if (id == NULL)
	    throw;
	}
    
    /*! Delete a watchdog */
    ~wd()
	{
	::wdDelete(id);
	}
	
	
    /*!
    *  Start a watchdog with std:function as a callback.
    *
    * This method adds a watchdog timer to the system tick queue. The 
    * specified watchdog routine will be called from interrupt level 
    * after the specified number of ticks has elapsed. 
    * 
    * To replace either the timeout delay or the routine to be 
    * executed, invoke this method again; only the most recent
    * use of  wd::start( ) on a given watchdog has any effect. 
    *(If your application requires multiple watchdogs create additional
    * instances of watchdog class.) To cancel a watchdog timer before the
    * specified tick count is reached, call wd::cancel( ).
    *
    * Calling this method with a delay value of 0 is not allowed, and will 
    * result in a return value of ERROR.
    *
    * Watchdog timers execute only once, but some applications require
    * periodically executing timers. To achieve this effect, the timer callback
    * itself must call wdStart( ) to restart the timer on each invocation.
    *
    */
    _Vx_STATUS start( _Vx_ticks_t  delay, std::function<void() noexcept> routine)
	{
	func = routine;  
	return ::wdStart(id, delay, reinterpret_cast<FUNCPTR>( wd::_callback), 
			 reinterpret_cast<_Vx_usr_arg_t>(this));
	}
    /*!
    * Start a watchdog with VxWorks FUNCPTR type as a callback. 
    * This form mimics the C function wdStart().
    *
    */
    _Vx_STATUS start( _Vx_ticks_t  delay, FUNCPTR pRoutine,
			 _Vx_usr_arg_t parameter)
	{
	return ::wdStart(id, delay, pRoutine, parameter);
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-compat-mangling"
    
    /*!
    * Start a watchdog timer with a void function pointer as the callback.
    * This form reflects the actual function used for C callback.
    * (This has not been adopted by wdStart() so as to maintain backward compatibility)     
    */
    _Vx_STATUS start( _Vx_ticks_t  delay, void ( * pFunc)(_Vx_usr_arg_t) noexcept,
			 _Vx_usr_arg_t parameter)
	{
	return ::wdStart(id, delay, reinterpret_cast<FUNCPTR>(pFunc), parameter);
	}
#pragma clang diagnostic pop   
    
    /*!
    * Cancel a watchdog timer before it fires.
    *
    * This routine cancels a currently running watchdog timer by zeroing its
    * delay count. Watchdog timers may be cancelled from interrupt level.
    */
    _Vx_STATUS cancel()
	{
	return ::wdCancel(id);
	}
    }; // wd 
}      // vxworks
#endif // __cplusplus 
#endif // !__RTP__
#endif // __INCwdhpp
