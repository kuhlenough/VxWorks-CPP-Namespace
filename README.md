## VxWorks C++ Namespace

While VxWorks is unique AFAKT in that it supports use of C++ in both the kernel and userspace, suppling a resonable STL and gradually optimising it over time.
However the basic VxWorks native API is based in C, and the C++ Wrappers know as the *Wind Foundation Classes* are long discontinued.  

This is a native C++ namespace that provides access to the classes for the VxWorks APIs as a template based C++. 
The classes follow the naming style of the STL, prefering lower case method names rather than camelCase names of the C functions they wrap, with few exceptions.

