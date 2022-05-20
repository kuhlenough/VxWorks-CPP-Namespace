## VxWorks C++ Namespace

While VxWorks is unique AFAKT in that it supports use of C++ in both the kernel and userspace, suppling a reasonable STL and gradually optimizing it over time.
However the basic VxWorks native API is based in C, and the C++ Wrappers know as the *Wind Foundation Classes* are long discontinued.  So this namespace provides access to functionality of VxWorks that is not part of the STL, or Boost libraries and is currently available in the C API. This is a native C++ namespace that provides access to the classes for the VxWorks APIs as a template based C++.  

This includes: 

- The very performant and lightweight event library, **eventLib**
- The kernel lightweight asynchronous callback library, **wdLib**   
- The use of named semaphores, queues, and mutexes shared between processes (and/or the kernel), conceptually similar to named POSIX entities. 

The classes follow the naming style of the STL, preferring lower case method names rather than camelCase names of the C functions they wrap, with few exceptions.

For more detail see the  [Doxygen Reference](../) 

TODO:  needs some test code, figure out move/copy constructible support

