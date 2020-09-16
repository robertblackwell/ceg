/*
 * THis is a temmplate file from which a correctly typed doubdly link list
 * implementation can be generated by symbol substitution.
 *
 * There are 3 symbols that must be replaced
 *  ${type} : replace this symbol with the name of the type to be held by the list
 *     (cannot be struct AName - use type def to turn into a single identifier)
 *     This will result in a TypeName for the generated list.
 *     If ${type} is replaced with Sock the list type will be SockList and SockList* will be typdef'd to SockListRef
 *
 *  ${prefix}: This template generated a set of functions and a set of macros that "overload" the function names exported by List.h
 *      If ${prefix} is replaced by SOK then
 *          functions of the form
 *              SockListRef SOKList_new() and
 *              SOKList_free(SockListRef sref) .. and one for each List function will ge generated
 *
 *          and macros of the form
 *              SockListRef M_SOKList_new() and
 *              M_SOKList_free(SockListRef sref) .. and one for each List function will ge generated
 */

// include the base implementation of a doublde list
//#include <List.h>

#define M_${prefix}List_new() List_new(dealloc)
#define M_${prefix}List_free(lref) List_free(lref)
#define M_${prefix}List_first(lref) (${type}ListRef)List_first(lref)
#define M_${prefix}List_last(lref) (${type}ListRef)List_last(lref)
#define M_${prefix}List_remove_first(lref) (${type}ListRef)List_remove_first(lref)
#define M_${prefix}List_remove_last(lref) (${type}ListRef)List_remove_last(lref)
#define M_${prefix}List_itr_unpack(lref, iter) (${type}ListRef)List_itr_unpack(lref, iter)
#define M_${prefix}List_iterator(lref) List_iterator(lref)
#define M_${prefix}List_itr_next(lref, iter) List_itr_next(lref, iter)

typedef ListRef ${type}ListRef;
typedef ListNodeRef ${type}ListIter, ListIter;

DObjListRef  ${prefix}List_new();
void         ${prefix}_List_free(DObjListRef lref) ;
${type}Ref  ${prefix}_List_first(${type}ListRef lref);
${type}Ref  ${prefix}_List_last(${type}ListRef lref) ;
${type}Ref  ${prefix}_List_remove_first(${type}ListRef lref);
${type}Ref  ${prefix}_List_remove_last(${type}ListRef lref);
${type}Ref  ${prefix}_List_itr_unpack(${type}ListRef lref, DObjListIter iter);
${type}ListIter ${prefix}_List_iterator(${type}ListRef lref);
${type}ListIter ${prefix}_List_itr_next(${type}ListRef lref, DObjListIter iter);
