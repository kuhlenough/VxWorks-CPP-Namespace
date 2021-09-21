/* queue.hpp - message queue header */

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
#ifndef __INCqueuehpp
#define __INCqueuehpp

#include <msgQLib.h>
#include <msgQEvLib.h>
#include "object.hpp"

#ifdef __cplusplus

namespace vxworks 
{
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
    
    //void unlink()
	///{
	//::msgQUnlink(d);
	//}
    
    /*! The number of messages currently in the queue.
     */    
    ssize_t numMsgs()
	{
	return  ::msgQNumMsgs(id);
	}
    
    _Vx_STATUS eventStart
        (
        _Vx_event_t events,   /* user-defined events                */
        UINT8       options   /* event-related msg Q options        */
        )
	{
	return ::msgQEvStart(id, events, options);
	}
    
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
use vxworks:queue.       

*/
class msgQ: public msgQcommon
    {
public:
    msgQ(const char * name, size_t maxMsgs, 
			     size_t maxMsgLength, int options, int mode,
			     void * context)
	{
	named = true;
	id = ::msgQOpen( name, maxMsgs, maxMsgLength,  options, mode, context);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}

    msgQ(const char * name, size_t maxMsgs, 
			     size_t maxMsgLength, int options, int mode)
	{
	named = true;
	id = ::msgQOpen( name, maxMsgs, maxMsgLength,  options, mode, NULL);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    // unnamed queue
    msgQ(size_t maxMsgs, 
	     size_t maxMsgLength, int options)
	{
	id = ::msgQCreate (maxMsgs, maxMsgLength, options);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    // open an existing queue
    msgQ(const char * name)
	{
	named = true;
	id = ::msgQOpen( name, 0, 0, 0, 0, NULL);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}

    _Vx_STATUS send 
    	(
	 char *    buffer,         /* message to send */
	 size_t    nBytes,         /* length of message */
	 _Vx_ticks_t timeout,      /* ticks to wait */
	 int       priority        /* MSG_PRI_NORMAL or MSG_PRI_URGENT */
    	)
	{
	return ::msgQSend(id, buffer, nBytes, timeout, priority);
	}

    _Vx_STATUS send 
    	(
	 char *    buffer,         /* message to send */
	 size_t    nBytes         /* length of message */
    	)
	{
	return ::msgQSend(id, buffer, nBytes, WAIT_FOREVER, MSG_PRI_NORMAL);
	}
    
    ssize_t recieve(char *      buffer,       /* buffer to receive message */
		    size_t      maxNBytes,    /* length of buffer */
		    _Vx_ticks_t timeout       /* ticks to wait */
		    )
	{
	 return ::msgQReceive( id, buffer, maxNBytes, timeout);
	}
    
    ssize_t recieve(char *      buffer,       /* buffer to receive message */
		    size_t      maxNBytes    /* length of buffer */
		    )
	{
	 return ::msgQReceive( id, buffer, maxNBytes, WAIT_FOREVER);
	}
    
    ssize_t poll(char *      buffer,       /* buffer to receive message */
		    size_t      maxNBytes    /* length of buffer */
		    )
	{
	 return ::msgQReceive( id, buffer, maxNBytes, NO_WAIT);
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
public:    
    
    /*! Instantiate a named queue with an optional *context* token.   
    */
    queue(const char * name, size_t maxMsgs, 
    			     int options, int mode,
    			     void * context)
    	{
	named = true;
    	id = ::msgQOpen( name, maxMsgs, sizeM,  options, mode, context);
    	if (id == MSG_Q_ID_NULL)
    	    throw;
    	}

    /*! Instantiate a named queue that holds up to *maxMsgs* in FIFO order.   
    */
    queue(const char * name, size_t maxMsgs, 
			      int options, int mode)
	{
	named = true;
	id = ::msgQOpen( name, maxMsgs, sizeM,  options, mode, NULL);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

    /*! Instantiate a unnamed queue that holds up to *maxMsgs* in FIFO order.   
    */
    queue(size_t maxMsgs, 
	      int options)
	{
	id = ::msgQCreate (maxMsgs, sizeM, options);
	if (id == MSG_Q_ID_NULL)
		    throw;
	}

     /*! Open an existing named queue from a second context
         */
    queue(const char * name)
	{
	named = true;
	id = ::msgQOpen( name, 0, 0, 0, 0, NULL);
	if (id == MSG_Q_ID_NULL)
	    throw;
	}
    
    
    _Vx_STATUS send 
    	(
	M& message,
	 _Vx_ticks_t timeout,      /* ticks to wait */
	 int       priority        /* MSG_PRI_NORMAL or MSG_PRI_URGENT */
    	)
	{
	return ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, timeout, priority);
	}

    STATUS send 
    	(
	M& message
	     )
	{
	return ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL);
	}
    
    void push( 
	     const M& message 
	     )
	{
	if ( OK != ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL))
	    throw;
	}
    
    ssize_t recieve(
		    M& message,    /* pointer to message */
		    _Vx_ticks_t timeout       /* ticks to wait */
		    )
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, timeout);
	}
    
    ssize_t recieve(
		    M& message
		    )
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER);
	}

    ssize_t poll(
		M& message 
		)
	{
	 return ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, NO_WAIT);
	}

    void operator<< ( M& message)
 	{
	if (OK != ::msgQSend(id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER, MSG_PRI_NORMAL))
	    throw;
	}
	 
    void operator>> (  M& message)
 	{
	if (OK != ::msgQReceive( id, reinterpret_cast<char *>(&message), sizeM, WAIT_FOREVER) )
	    throw;
	}
    };  // queue
}      // vxworks
#endif // __cplusplus 
#endif // __INCqueuehpp    
