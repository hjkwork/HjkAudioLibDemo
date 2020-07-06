#pragma once
/** This is a shorthand macro for declaring stubs for a class's copy constructor and operator=.

    For example, instead of
    @code
    class MyClass
    {
        etc..

    private:
        MyClass (const MyClass&);
        MyClass& operator= (const MyClass&);
    };@endcode

    ..you can just write:

    @code
    class MyClass
    {
        etc..

    private:
        JUCE_DECLARE_NON_COPYABLE (MyClass)
    };@endcode
*/
#define JUCE_DECLARE_NON_COPYABLE(className) \
    className (const className&) = delete;\
    className& operator= (const className&) = delete;
/** This macro can be added to class definitions to disable the use of new/delete to
    allocate the object on the heap, forcing it to only be used as a stack or member variable.
*/
#define JUCE_PREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t) = delete; \
    static void operator delete (void*) = delete;