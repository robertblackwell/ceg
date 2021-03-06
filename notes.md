# C programming Notes  

Below are some notes an observations from my efforts to make a C implementation of a simple webserver.

This project was undertaken as part of a larger exercise of implementing a test project in a  number of languages in
order to explore a number of languages.
### Pupose and goal

This project is an exercise in using C in a not-entirely-trivial "systems style" project. 

I want a project that would be:
-   multi-tasking (so as to use sync primitives), 
-   long running so that resource leaks would be an issue that needed to be handled,
-   a long a running service so that error detection and handling in order that the code must cleanup after errors and find a way to keep providing its services.

### Error Handling

These last two itens would force me to address C style error handling and recovery, an area in which C is renowned for trying 
the patience of all but the most detail minded programmers.

Out-of-memory conditions are a particular focus. While modern Linux systems have almost infinite memory, processes
that run for days without restarting can still cause system problems even with only minor memory leaks. I wanted to 
ensure that every memory allocation provided a correct error processing path. To test this I put a wrapper
around malloc/free so that during testing I could force alloction failures and verify a viable recovery path. 

This last para comes about because a number of the proposed C replacements tout better error management as one of their 
goals and advatages. Thus I wanted to see by experience how difficult C error handling really was.
 

### Classes

I have used the "standard" C approach to classes. An opaque struct reference passed as the first argument to
systematically named functions as a way of implmenting methods.

This leads to more verbose code than more OO languages but is definitely workable.

I have avoided vtables as a means of implementing inheritance and instead used composition; which leads to the
next topic __Generics__. 

### Generics
As a learning exercise I implemented a generic doubly linked  List type using `void*` as the anonamous 
content of the list. I then derived a number of typed lists structures from this base list.

Research unearthed a number of approaches to creating a Typed List (or other structure) but in the end I created 
my own approach (rudimentary code generator and cmake custom_command) as I wanted to be able to view the code for the resulting typed list structure to facilitate debugging.

If I wrote a lot of C I would build a much much smart preprocessor/code_generator for this purpose.  
 
### Memory Allocation Failures

Initially I started coding without checking the return value from `malloc()` and other memory allocators but in the 
interest of realism I have retro-fitted such checks on all allocation.

This has led me to adopt the classic C pattern of using labels and goto to implement a single scope clanup block.

In conjunction with this __EVERY__ function tests all pointer arguments for NULL using

```
    ASSERT_NOT_NULL(p)
```

The goal is that during development this will catch bugs early but in "production" the macro `ASSERT_NOT_NULL()` 
can be conditionally turned into a noop.

### Deallocation and dangling pointers

Leaving a pointer pointing at free'd memory is a common C problem and it is facilitated by the calling interface
to the `void free(void* p)` function. 

A safer way is to have deallocation functions take a reference to the pointer 
such as `void free(void** p)` and after freeing the memory pointed to by `*p` to set `*p = NULL`.

I have ensured that all calls to the system `free()` are inside functions of of the form `TypeName_free(TypeName** p)`
which implement the strategy outlined in the previous sentence.

### Lack of standard tools

It did not take long to discover a scarcity of good pure C, and lightweight Logger and Unit Test frameworks that would 
easily integrate with my project.

Hence I built my own, each as a single `.h` and `.c` file.

## Some conventions

###Opaque types

This project has a number of `Opaque` types, such a __List__, __Queue__.

-   The headers file declare such types as 
```
struct Type_s;
typedef Type_s Type, *TypeRef;
```
 -  Only the 'c' file has a full definition of the struct members.
 -  Functions that operate of such an opaque type are defined thus:
 ```
TypeRef Type_new();
int Type_count_something(TypeRef this);
void Type_free(TypeRef* p);
```
 - The `TypeRef` argument is __always__ called `this` to remind me that the function is a `method` and which object 
 it is working on. 
 - a few points to note. 
    - TypeRef is deliberately not Type* as I want to obscure the fact that its a point to Type_s 
    - all "methods" functions have a name that starts with the type name or an abbreviation for that type name.
    - all `Type_free` functions take a pointer to a TypeRef and set that variable to NULL on return. This is a 
    token effort ot ease the dangling pointer problem
    - the `Type_free` function deallocates all memory pointed to from within the type.
    - char* arguments to functions are references that only need to be valid for the duration of the function call. 
    If a function needs to save the string pointed to by the char* argument, in a struct say, 
    for use after the function returns then the string is copied. An example of this 
    ```
        HeaderLineRef HeaderLine_new(char* key, char* value);
    ``` 
     