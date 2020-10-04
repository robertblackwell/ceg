#define _GNU_SOURCE
#include <c_eg/kvpair.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <c_eg/alloc.h>
#include <c_eg/utils.h>

struct KVPair_s {
    char* label_ptr;
    int   label_len;
    char* value_ptr;
    int   value_len;
};

//char* make_upper(char* src)
//{
//    int srclen = strlen(src);
//    char* dest = eg_alloc(srclen+1);
//    char* s = src;
//    char* p = dest;
//    for(int i = 0; i < srclen; i++) {
//        p[i] = toupper((unsigned char) dest[i]);
//    }
//    p[srclen+1] = (unsigned char)'\0';
//    return dest;
//}
// creates and initializes a new KVPair obj. Returns NULL on allocation failure
KVPair* KVPair_new(char* labptr, int lablen, char* valptr, int vallen)
{
    // store {label}: {value}\r\n\0
    KVPair* hlref = eg_alloc(sizeof(KVPair));
    if(hlref  == NULL) goto mem_error_1;
    hlref->label_len = lablen;
    hlref->value_len = vallen;

    hlref->label_ptr = eg_alloc(lablen+1);
    if(hlref->label_ptr == NULL) goto mem_error_2;
    memcpy(hlref->label_ptr, labptr, lablen);
        // Convert to upper case
    char* s = labptr;
    char* p = hlref->label_ptr;
    for(int i = 0; i < lablen; i++) {
        p[i] = toupper((unsigned char) labptr[i]);
    }
    p[lablen] = '\0';

    hlref->value_ptr = eg_alloc(vallen+1);
    if(hlref->value_ptr == NULL) goto mem_error_2;
    memcpy(hlref->value_ptr, valptr, vallen);
    hlref->value_ptr[vallen] = (char)'\0';

    return  hlref;
    mem_error_1:
        // nothing got allocated
        return NULL;
    mem_error_2:
        // hlref ok one of the otehrs failed
        if(hlref->label_ptr != NULL) free((void*)hlref->label_ptr);
        if(hlref->value_ptr != NULL) free((void*)hlref->value_ptr);
        free((void*)hlref);
        return NULL;
}
KVPair* KVPair_from_cstrs(char* key, char* value)
{
    return KVPair_new(key, strlen(key), value, strlen(value));
}
KVPair* KVPair_from_cstrpair(CStrPair cstrp)
{
    return KVPair_from_cstrs(cstrp.key, cstrp.value);
}

void KVPair_free(KVPair** hlref_ptr)
{
    KVPair* hlref = *hlref_ptr;
    eg_free(hlref->label_ptr);
    hlref->label_len = 0;
//    hlref->label_ptr = NULL;
    eg_free(hlref->value_ptr);
    hlref->value_len = 0;
//    hlref->value_ptr = NULL;
    eg_free((void*) hlref);
    *hlref_ptr = NULL;
}
void KVPair_dealloc(void* ptr) { KVPair_free((KVPair**)(ptr));}
char* KVPair_label(KVPair* hlref)
{
    return hlref->label_ptr;
}
char* KVPair_value(KVPair* hlref)
{
    return hlref->value_ptr;
}
void KVPair_set_value(KVPair* hlref, char* valptr, int vallen)
{
    char* oldvalptr = hlref->value_ptr;
    hlref->value_ptr = eg_alloc(vallen+1);
    memcpy(hlref->value_ptr, valptr, vallen);
    hlref->value_ptr[vallen] = '\0';
    free(oldvalptr);
}
