/* queue.hpp - message queue header */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 */

#ifndef __INCqueuehpp
#define __INCqueuehpp

#include <msgQLib.h>
#include <msgQEvLib.h>
#include "object.hpp"
#include "chrono2tic.hpp"
#include <cstring>

#ifdef __cplusplus

namespace vxworks 
{
//! unlink a named message queue	
void unlink( string name )
	{
	::msgQUnlink( name.c_str().c_str());
	}


// (VxWorks message queue )
class msgQcommon : public object< MSG_Q_ID >
    {
public:
    ~msgQcommon()
	{
	if(named)
	    ::msgQClose(id);
	else
	    ::msgQDelete(id);
	}
  
   
    /*! The number of messages currently in the queue.
     */    
    ssize_t numMsgs()
	{
	return  ::msgQNumMsgs(id);
	}
    
    /*! start the event notification process from the queue
        for the calling thread 
    */
    _Vx_STATUS eventStart
        (
        _Vx_event_t events,   /* user-defined events                */
        UINT8       options   /* event-related msg Q options        */
        )
	{
	return ::msgQEvStart(id, events, options);
	}
    
    /*! stop the event notification process from the queue
        for the calling thread 
    */
   _Vx_STATUS eventStop ()
       {
       return ::msgQEvStop(id);
       }
    
    /*! The number of messages currently queued. 
        ( just like numMsgs )       
    */
    ssize_t size() const
	{
	return ::msgQNumMsgs(id);
	}
    /*!
    Returns true if the queue is empty.
    */
    bool empty() const
	{
	return ( 0== ::msgQNumMsgs(id) );
	}
    };


/*!

\brief  A VxWorks Message Queue Class

This is wrapper to the 
[msgQLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/msgQLib.html) 
library.  Like the underlying C library a message is an abstract 
buffer with variable length. If named messages can be passed between contexts
similar to a POSIX queue. For a class that offers named queue more similar to std::queue
use vxworks::queue.       

*/
class msgQ: public msgQcommon
    {
private:
     const int default_mode = OM_DESTROY_ON_LAST_CALL | OM_CREATE ; 	    
     const int default_options = MSG_Q_FIFO ; 	    
public:
    //! Create a VxWorks named message queue specifying all parameters 
    msgQ( const string name, size_t maxMsgs, 
			     size_t maxMsgLength, int options, int mode,
			     void * context)
	{
	named = true;
	id = ::msgQOpen( name.c_str(), maxMsgs, maxMsgLength,  options, mode, context);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}

    //! Create a VxWorks named message queue 
    msgQ( const string name, size_t maxMsgs, 
			     size_t maxMsgLength)
	{
	named = true;
	id = ::msgQOpen(  name.c_str(), maxMsgs, maxMsgLength,  default_options, default_mode, NULL);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    //! Create an unnamed queue
    msgQ(size_t maxMsgs, 
	     size_t maxMsgLength, int options)
	{
	id = ::msgQCreate (maxMsgs, maxMsgLength, options);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    //! open an existing named queue, created in another context 
    msgQ(const char& name)
	{
	named = true;
	id = ::msgQOpen( &name, 0, 0, 0, 0, NULL);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}

    //! put a message on the front of the queue, pending for a timeout and specifying a message priority 
    _Vx_STATUS send 
    	(
	 char &    buffer,         /* message to send */
	 size_t    nBytes,         /* length of message */
	 _Vx_ticks_t timeout,      /* ticks to wait */
	 int       priority        /* MSG_PRI_NORMAL or MSG_PRI_URGENT */
    	)
	{
	return ::msgQSend(id, &buffer, nBytes, timeout, priority);
	}

    //! put a message on the front of the queue, pend if the queue is full
    inline _Vx_STATUS send 
    	(
	 char&     buffer,         /* message to send */
	 size_t    nBytes         /* length of message */
    	)
	{
	return ::msgQSend(id, &buffer, nBytes, WAIT_FOREVER, MSG_PRI_NORMAL);
	}
    
    //! remove a message from the end of the queue, wait timeout tics for a message if queue is empty
    inline ssize_t recieve(char &buffer,       /* buffer to receive message */
		    size_t      maxNBytes,    /* length of buffer */
		    _Vx_ticks_t timeout       /* ticks to wait */
		    )
	{
	 return ::msgQReceive( id, &buffer, maxNBytes, timeout);
	}
 
    //! remove a message from the end of the queue, wait a std:duration for a message if queue is empty
    template<class Rep, class Period>
    inline ssize_t recieve(
		    char &      buffer,       /* buffer to receive message */
		    size_t      maxNBytes,    /* length of buffer */
		    const duration<Rep, Period>& relTime   )
	{
	 return ::msgQReceive( id, &buffer, maxNBytes, chrono2tic(relTime));
	}

 
    //! remove a message from the end of the queue, pend indefinitely till a message is available
    inline ssize_t recieve(char &      buffer,       /* buffer to receive message */
		    size_t      maxNBytes    /* length of buffer */
		    )
	{
	 return ::msgQReceive( id, &buffer, maxNBytes, WAIT_FOREVER);
	}
    
    //! remove a message from the end of the queue, return error immediately if no message is available
    inline ssize_t poll(char &      buffer,       /* buffer to receive message */
		 size_t      maxNBytes    /* length of buffer */
		 )
	{
	 return ::msgQReceive( id, &buffer, maxNBytes, NO_WAIT);
	}
    }; // msgQ 

/*!
\brief  An Inter-context Named Queue Class
        
Like vxworks::msgQ this is wrapper to the 
[msgQLib](https://docs.windriver.com/bundle/vxworks_kernel_coreos_21_07/page/CORE/msgQLib.html) 
library. However this class takes a template to describe it's message. In
this way it is similar to std::queue, however a vxworks::queue is **not** 
a wrapper to an underlying container class. And thus manipulation of the underlying
queue contents is quite limited compared to std::queue.

*/
template <typename M> class queue : public msgQcommon
    {
private:
    const size_t sizeM = sizeof(M);
    const int default_mode = OM_DESTROY_ON_LAST_CALL | OM_CREATE ; 	    
    const int default_options = MSG_Q_FIFO ; 	    

public:    
    
    /*! Instantiate a named queue with an optional *context* token.   
    */
    queue(const string name, size_t maxMsgs, 
    			     int options, int mode,
    			     void * context)
    	{
	named = true;
    	id = ::msgQOpen( name.c_str(), maxMsgs, sizeM,  options, mode, context);
    	if (id == MSG_Q_ID_NULL)
    	    throw;
    	}

    /*! Instantiate a named queue that holds up to *maxMsgs* in FIFO order.   
    */
    queue(const string name, size_t maxMsgs )
	{
	named = true;
	id = ::msgQOpen( name.c_str(), maxMsgs, sizeM,  default_options, default_mode, NULL);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    /*! Instantiate a unnamed queue that holds up to *maxMsgs* in FIFO order.   
    */
    queue(size_t maxMsgs )
	{
	id = ::msgQCreate (maxMsgs, sizeM, default_options);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

     /*! Open an existing named queue from a second context
         */
    queue(const string name)
	{
	named = true;
	id = ::msgQOpen( name.c_str(), 0, 0, 0, 0, NULL);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}
    
    //! put a message of type M at the front of the queue, pending if the queue is full for *timeout* tics 
    inline _Vx_STATUS send 
    	(
	M& message,
	 _Vx_ticks_t timeout,      /* ticks to wait */
	 int       priority        /* MSG_PRI_NORMAL or MSG_PRI_URGENT */
    	)
	{
	return ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, timeout, priority);
	}
	
    //! put a message of type M at the front of the queue, pending if the queue is full for std::duration
     template<class Rep, class Period>
     inline _Vx_STATUS send( M& message, const duration<Rep, Period>& relTime) 
	{
	return ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, chrono2tic(relTime), MSG_PRI_NORMAL);
	}

    //! put a message of type M at the front of the queue, pending indefinitely if the queue is full 
    inline STATUS send 
    	(
	M& message
	)
	{
	return ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL);
	}
    
    //! put a message of type M at the front of the queue, pending indefinitely if the queue is full
    inline void push( 
	     const M& message 
	     )
	{
	if ( OK != ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL))
	    throw;
	}
    
    //! remove a message from the end of the queue, wait timeout tics for a message if queue is empty
    inline ssize_t recieve(
		    M& message,    /* pointer to message */
		    _Vx_ticks_t timeout       /* ticks to wait */
		    )
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, timeout);
	}
    
    //! remove a message from the end of the queue, wait a std:duration for a message if queue is empty
    template<class Rep, class Period>
    inline ssize_t recieve(
		    M& message,    /* pointer to message */
		    const duration<Rep, Period>& relTime     		    )
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, chrono2tic(relTime));
	}

   //! remove a message from the end of the queue, pend indefinitely till a message is available
   inline ssize_t recieve(
		    M& message
		    )
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER);
	}

    //! remove a message from the end of the queue, return error immediately if no message is available
   inline ssize_t poll(
		M& message 
		)
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, NO_WAIT);
	}

    //! operator to send a message 
    void operator<< ( M& message)
 	{
	if (OK != ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL))
	    throw;
	}
	 
    //! operator to receive a message 
    void operator>> (  M& message)
 	{
	if (OK != ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER) )
	    throw;
	}
    };  // queue
}      // vxworks
#endif // __cplusplus 
#endif // __INCqueuehpp    
