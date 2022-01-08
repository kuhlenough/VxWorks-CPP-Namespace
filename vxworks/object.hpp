/* object.hpp - VxWorks object class */

/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 */

#ifndef __INCobjecthpp
#define __INCobjecthpp

#include <objLib.h>

#ifdef __cplusplus

namespace vxworks
{
#ifdef __RTP__
#define __OBJ(_id)  static_cast<OBJ_HANDLE>(_id)
#else
#define __OBJ(_id)  static_cast<OBJ_ID>(_id)
#endif

/*! 
 * The parent object class of all VxWorks classes 
 *   
 *  A VxWorks C system object is analogous a  C++ class
 * this "object" class is parent of other classes in 
 * the **vxworks::** namespace. It provides access
 * to the native object ID and show() methods available
 * in the kernel.  
 */ 

template<typename T> class object
    {
protected:
    bool named = false;
    T id;
public:


#ifndef __RTP__
	
    /*!
    Show debug information about a class instance
    */
    _Vx_STATUS show( int type)
	{
	return ::objShow(__OBJ(id), type);
	}

    /*!
    Show debug information about a class instance
    */
    _Vx_STATUS show()
	{
	return ::objShow(__OBJ(id), 0 );
	}

    /*!
    Show debug information about a class instance
    */
    _Vx_STATUS showAll(int type)
	{
	return ::objShowAll(__OBJ(id), type );
	}

    /*!
    Show debug information about a class instance
    */
    _Vx_STATUS showAll()
	{
	return ::objShowAll(__OBJ(id), 0 );
	}

    /*!
    Return the length of the name of a VxWorks class instance.
    If the object is not named or an error occurs it returns -1.     
    */
    ssize_t nameLen()
	{
	return ::objNameLenGet(__OBJ(id));
	}

    /*!
    Return the name of a VxWorks class instance.
    If the object is not named  an error is thrown     
    */
    string name(size_t capacity  ) 
	{
        char * nameBuf = malloc (capacity);
	string ret;
	if nameBuf == NULL
		throw ;
	if ( ERROR == ::objNameGet(__OBJ(id), nameBuf, capacity ))
		throw ;
	ret = nameBuf;
	free(nameBuf);
	return ret;
	}
#else


#endif
    /*! Return the underlying VxWorks object ID.     
     */
    T handle()
	{
	return id;
	}
    };  // object
}	// vxworks
#endif  // __cplusplus
#endif  // __INCeventhpp
