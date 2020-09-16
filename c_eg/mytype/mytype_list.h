#ifndef c_ceg_mytype_list_h
#define c_ceg_mytype_list_h

struct MyType_s;

typedef MyType, *MyTypeRef;

Found it
///
/// The remainder of this file is generated code and will be over written at the next build
///
/*
 * THis is a temmplate file from which a correctly typed doubdly link list
 * implementation can be generated by symbol substitution.
 *
 * There are 3 symbols that must be replaced
 *  MyType : replace this symbol with the name of the type to be held by the list
 *     (cannot be struct AName - use type def to turn into a single identifier)
 *     This will result in a TypeName for the generated list.
 *     If MyType is replaced with Sock the list type will be SockList and SockList* will be typdef'd to SockListRef
 *
 *  MT: This template generated a set of functions and a set of macros that "overload" the function names exported by List.h
 *      If MT is replaced by SOK then
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

#define M_MTList_new() List_new(dealloc)
#define M_MTList_free(lref) List_free(lref)
#define M_MTList_first(lref) (MyTypeListRef)List_first(lref)
#define M_MTList_last(lref) (MyTypeListRef)List_last(lref)
#define M_MTList_remove_first(lref) (MyTypeListRef)List_remove_first(lref)
#define M_MTList_remove_last(lref) (MyTypeListRef)List_remove_last(lref)
#define M_MTList_itr_unpack(lref, iter) (MyTypeListRef)List_itr_unpack(lref, iter)
#define M_MTList_iterator(lref) List_iterator(lref)
#define M_MTList_itr_next(lref, iter) List_itr_next(lref, iter)

typedef ListRef MyTypeListRef;
typedef ListNodeRef MyTypeListIter, ListIter;

DObjListRef  MTList_new();
void         MTList_free(DObjListRef lref) ;
MyTypeRef  MTList_first(MyTypeListRef lref);
MyTypeRef  MTList_last(MyTypeListRef lref) ;
MyTypeRef  MTList_remove_first(MyTypeListRef lref);
MyTypeRef  MTList_remove_last(MyTypeListRef lref);
MyTypeRef  MTList_itr_unpack(MyTypeListRef lref, DObjListIter iter);
MyTypeListIter MTList_iterator(MyTypeListRef lref);
MyTypeListIter MTList_itr_next(MyTypeListRef lref, DObjListIter iter);

MyTypeRef MyType_new();

//this is after the include
#endif
