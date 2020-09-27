#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <c_eg/alloc.h>
#include <c_eg/utils.h>
#include <c_eg/buffer/cbuffer.h>


#define CBUFFER_MAX_CAPACITY 10000
#define CBUFFER_MIN_CAPACITY 1000

size_t max_of_two(size_t a, size_t b)
{
    return (a >= b) ? a : b;
}

size_t min_of_two(size_t a, size_t b)
{
    return (a <= b) ? a : b;
}

/**
* This struct implements a strategy for growing contiguous buffers
* Cbuffer instances can be parameterized with different strategies
*/
typedef struct BufferStrategy_s {

    size_t m_min_size;
    size_t m_max_size;

} BufferStrategy, *BufferStrategyRef;


typedef struct Cbuffer_s
{
    void*       m_memPtr;     /// points to the start of the memory slab managed by the instance
    char*       m_cPtr;       /// same as memPtr but makes it easier in debugger to see whats in the buffer
    size_t      m_length;    ///
    size_t      m_capacity;  /// the capacity of the buffer, the value used for the eg_alloc call
    size_t      m_size;      /// size of the currently filled portion of the memory slab
    BufferStrategyRef m_strategy;
} Cbuffer;

/**
* Allocates memory - BufferStrategyRef determines how big the memory block
* is based on the requested size and its own internal strategy
*/
void* BufferStrategy_allocate(BufferStrategyRef bsref, size_t required_size)
{
    if (required_size > bsref->m_max_size) assert(0);
    return eg_alloc(max_of_two(required_size, bsref->m_min_size));

}
/**
* Apply a buffer strategy to deallocation - provision for recording allocation/deallocation history
*/
void BufferStrategy_deallocate(BufferStrategyRef bsref, void* dataptr)
{
    free(dataptr);
}
/**
* Strategy determines size of memory reallocation based on current and requested sizes 
* assert is would be too big
*/
size_t BufferStrategy_reallocate_size(BufferStrategyRef bsref, size_t current_capacity, size_t requested_new_size)
{
        if (requested_new_size > bsref->m_max_size) assert(0);
        return max_of_two(requested_new_size, min_of_two(bsref->m_max_size, 2*current_capacity));

}
/**
* Perform a reallocation of the current_memptr to new_size
* Assert is too big 
*/
void* BufferStrategy_reallocate(BufferStrategyRef bsref, void* current_memptr, size_t new_size)
{
        if (new_size > bsref->m_max_size) assert(0);
        return realloc(current_memptr, new_size);
}

BufferStrategy common_strategy = {.m_min_size=256, .m_max_size=1024*1024};

Cbuffer* Cbuffer_new()
{
    Cbuffer* cb_ptr = (Cbuffer*)eg_alloc(sizeof(Cbuffer));
    cb_ptr->m_strategy=&common_strategy;
    size_t tmp_cap = cb_ptr->m_strategy->m_min_size;
    cb_ptr->m_memPtr = BufferStrategy_allocate(cb_ptr->m_strategy, tmp_cap);
    cb_ptr->m_cPtr = (char*) cb_ptr->m_memPtr;
    cb_ptr->m_length = 0;
    cb_ptr->m_size = 0;
    cb_ptr->m_capacity = tmp_cap;
    return cb_ptr;
}

Cbuffer* Cbuffer_from_cstring(char* c_str)
{
    Cbuffer* cbuf = Cbuffer_new();
    Cbuffer_append(cbuf, (void*)c_str, strlen(c_str));
    return cbuf;
}

void Cbuffer_free(Cbuffer** cbuf)
{
    Cbuffer* this = *cbuf;
    assert(*cbuf != NULL);
    eg_free(this->m_memPtr);
    eg_free(*cbuf);
    *cbuf = NULL;
}
/**
 * gets a pointer to the start of the memory slab being managed by the instance
 */
void* Cbuffer_data(Cbuffer* cbuf)
{
    return cbuf->m_memPtr;
}
/**
 * gets the size of used portion of the buffer
*/
size_t Cbuffer_size(Cbuffer* cbuf)
{
    return cbuf->m_length;
}
/**
 * capacity of the buffer - max value of size
*/
size_t Cbuffer_capacity(Cbuffer* cbuf)
{
    return cbuf->m_capacity;
}
/**
 * returns a pointer to the next available unused position in the buffer
*/
void* Cbuffer_next_available(Cbuffer* cbuf)
{
    void* x = (void*) (cbuf->m_cPtr + cbuf->m_length);
    return x;
}
/**
 * Resets the buffer so that it is again an empty buffer
 */
void Cbuffer_clear(Cbuffer* cbuf)
{

    cbuf->m_length = 0; cbuf->m_length = 0; cbuf->m_cPtr[0] = (char)0;
}

void Cbuffer_append(Cbuffer* cbuf, void* data, size_t len)
{
    char* tmp = (char*)data;
    if(len == 0)
        return;
    if ( ( (cbuf->m_length + len) >= cbuf->m_capacity )  ) {
        size_t new_capacity = BufferStrategy_reallocate_size(cbuf->m_strategy, cbuf->m_capacity, cbuf->m_length + len);
        void* tmp = BufferStrategy_reallocate(cbuf->m_strategy, cbuf->m_memPtr, new_capacity);
        cbuf->m_memPtr = tmp;
        cbuf->m_cPtr = (char*) cbuf->m_memPtr;
        cbuf->m_capacity = new_capacity;
    }
    void* na = Cbuffer_next_available(cbuf);
    memcpy(na, data, len);
    cbuf->m_length = cbuf->m_length + len;
    cbuf->m_size = cbuf->m_length;
    
    cbuf->m_cPtr = (char*) cbuf->m_memPtr;
}
void Cbuffer_append_cstr(Cbuffer* cbuf, char* cstr)
{
    Cbuffer_append(cbuf, (void*)cstr, strlen(cstr));
}
void Cbuffer_setSize(Cbuffer* cbuf, size_t n)
{
    cbuf->m_length = n;
    cbuf->m_size = n;
}

/**
 * Returns a string that has the same value as the used portion of the buffer
 * This is a reference to an internal string so dont free or change it
 * there is a bug here as no zero on the end
 */
char* Cbuffer_toString(Cbuffer* cbuf)
{
    char* p = cbuf->m_cPtr;
    return p;
}
// c++ move semantics - saves a copy
void Cbuffer_move(Cbuffer* dest, Cbuffer* src)
{
    ASSERT_NOT_NULL(src);
    ASSERT_NOT_NULL(dest);
    Cbuffer_clear(dest);
//    Cbuffer_append(dest, src->m_memPtr, src->m_length );
    Cbuffer tmp = *dest;
    *dest = *src;
    *src = tmp;
    Cbuffer_clear(src);
}
/**
 * Detremines if an address value (pointer) is within the address range of the
 * the buffer ie
 *      buffer.dada() < = ptr < buffer.data() + buffer.capacity();
 *  or, should it be
 *      buffer.dada() < = ptr < buffer.data() + buffer.size();
 *
 */
bool Cbuffer_contains_voidptr(Cbuffer* cbuf, void* ptr)
{
    char* p = (char*) ptr;
    return Cbuffer_contains_charptr(cbuf, p);
}
bool Cbuffer_contains_charptr(Cbuffer* cbuf, char* ptr)
{
    char* endPtr = cbuf->m_cPtr + (long)cbuf->m_capacity;
    char* sPtr = cbuf->m_cPtr;
//    bool r1 = ptr <= endPtr;
//    bool r2 = ptr >= sPtr;
    bool r = ( ptr <= endPtr && ptr >= sPtr);
    return r;
}
