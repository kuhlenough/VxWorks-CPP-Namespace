/* chrono2tic.hpp - C++ mutex class header */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 */

#include <private/clockLibP.h>
#include <chrono>

#ifndef __INCchrono2tichpp
#define __INCchrono2tichpp

#ifdef __cplusplus


namespace vxworks 
{
//  helper functions to convert <chrono> to VxWorks system tics

using namespace std::chrono;

template<class Rep, class Period>
static inline _Vx_ticks_t chrono2tic( const duration<Rep, Period>& _Rel_time)
	{
	milliseconds msec = duration_cast<milliseconds>(_Rel_time);
	return static_cast<_Vx_ticks_t>((static_cast<unsigned long long>(CLOCKS_PER_SEC) * msec.count() )/1000ull); 
	}

template<class clock, class duration>
static inline _Vx_ticks_t time_point2tic( const time_point<clock,duration> tp)
	{
	struct timespec ts;
	_Vx_ticks_t sysTicks;
	clockid_t clock_id;
	auto secs = time_point_cast<seconds>(tp);
	auto ns = time_point_cast<nanoseconds>(tp) -
             time_point_cast<nanoseconds>(secs);

	ts.tv_sec = secs.time_since_epoch().count();
	ts.tv_nsec = ns.count();

	::clock_absTimeoutCalc (CLOCK_REALTIME, &ts, &sysTicks );
	return sysTicks ;
	}

}	// vxworks
#endif  // __cplusplus 
#endif  // __INCchrono2tichpp
