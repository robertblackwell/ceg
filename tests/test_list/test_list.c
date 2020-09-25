#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <c_eg/unittest.h>
#include <c_eg/buffer/contig_buffer.h>
#include <c_eg/logger.h>
#include <c_eg/list.h>

typedef struct DummyObj_s {
    long value;
} DummyObj, *DummyObjRef;

DummyObjRef DummyObj_new(long val)
{
    DummyObjRef dref = malloc(sizeof(DummyObj));
    dref->value = val;
}
void DummyObj_free(DummyObjRef* dref)
{

    free((void*)*dref);
    *dref = NULL;
}

#define DOList_new() List_new(dealloc)
#define DOList_free(lref) List_free(lref)
#define DOList_first(lref) (DummyObjRef)List_first(lref)
#define DOList_last(lref) (DummyObjRef)List_last(lref)
#define DOList_remove_first(lref) (DummyObjRef)List_remove_first(lref)
#define DOList_remove_last(lref) (DummyObjRef)List_remove_last(lref)
#define DOList_itr_unpack(lref, iter) (DummyObjRef)List_itr_unpack(lref, iter)
#define DOList_iterator(lref) List_iterator(lref)
#define DOList_itr_next(lref, iter) List_itr_next(lref, iter)

typedef ListRef DObjListRef;
typedef ListNodeRef DObjListIter, ListIter;

DObjListRef  DObj_List_new();
void         DObj_List_free(DObjListRef* lref) ;
DummyObjRef  DObj_List_first(DObjListRef lref);
DummyObjRef  DObj_List_last(DObjListRef lref) ;
DummyObjRef  DObj_List_remove_first(DObjListRef lref);
DummyObjRef  DObj_List_remove_last(DObjListRef lref);
DummyObjRef  DObj_List_itr_unpack(DObjListRef lref, DObjListIter iter);
DObjListIter DObj_List_iterator(DObjListRef lref);
DObjListIter DObj_List_itr_next(DObjListRef lref, DObjListIter iter);

static void dealloc(void** ptr)
{
    DummyObj_free((DummyObjRef*) ptr);
}

DObjListRef  DObj_List_new() {return (DObjListRef)List_new(dealloc);}
void         DObj_List_free(DObjListRef* lref) {List_free(*((ListRef**)lref)); *lref = NULL;}
DummyObjRef  DObj_List_first(DObjListRef lref) { return (DummyObjRef)List_first(lref);}
DummyObjRef  DObj_List_last(DObjListRef lref)  { return (DummyObjRef)List_last(lref);}
DummyObjRef  DObj_List_remove_first(DObjListRef lref) { return (DummyObjRef)List_remove_first(lref);}
DummyObjRef  DObj_List_remove_last(DObjListRef lref) { return (DummyObjRef)List_remove_last(lref);}
DummyObjRef  DObj_List_itr_unpack(DObjListRef lref, DObjListIter iter) { return (DummyObjRef)List_itr_unpack(lref, iter);}
DObjListIter DObj_List_iterator(DObjListRef lref) { return List_iterator(lref);}
DObjListIter DObj_List_itr_next(DObjListRef lref, DObjListIter iter) { return List_itr_next(lref, iter);}



///////////////////////////////////////////////////
int test_List_new()
{
    ListRef lref = List_new(dealloc);
    int sz = List_size(lref);
    UT_EQUAL_INT(sz, 0);
	return 0;
}
int test_list_add_back()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_back(lref, (void*) dref);
    int sz = List_size(lref);
    int v1 = ((DummyObjRef)List_first(lref))->value;
    int v2 = ((DummyObjRef)List_last(lref))->value;
    UT_EQUAL_INT(sz, 1);
    UT_EQUAL_INT(v1, 333);
    UT_EQUAL_INT(v2, 333);
    DummyObjRef dref2 = DummyObj_new(444);
    List_add_back(lref, (void*) dref2);
    int v11 = ((DummyObjRef)List_first(lref))->value;
    int v12 = ((DummyObjRef)List_last(lref))->value;
    UT_EQUAL_INT((List_size(lref)), 2);
    UT_EQUAL_INT(v11, 333);
    UT_EQUAL_INT(v12, 444);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;
}
int test_list_add_front()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_front(lref, (void*) dref);
    int sz = List_size(lref);
    int v1 = ((DummyObjRef)List_first(lref))->value;
    int v2 = ((DummyObjRef)List_last(lref))->value;
    UT_EQUAL_INT(sz, 1);
    UT_EQUAL_INT(v1, 333);
    UT_EQUAL_INT(v2, 333);
    DummyObjRef dref2 = DummyObj_new(444);
    List_add_front(lref, (void*) dref2);
    int v11 = ((DummyObjRef)List_first(lref))->value;
    int v12 = ((DummyObjRef)List_last(lref))->value;
    UT_EQUAL_INT((List_size(lref)), 2);
    UT_EQUAL_INT(v11, 444);
    UT_EQUAL_INT(v12, 333);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}
int test_list_remove_front()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_front(lref, (void*) dref);
    List_remove_first(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    DummyObjRef dref1 = DummyObj_new(111);
    DummyObjRef dref2 = DummyObj_new(222);
    DummyObjRef dref3= DummyObj_new(333);
    List_display(lref);
    List_add_front(lref, (void*) dref1);
    List_display(lref);
    List_add_front(lref, (void*) dref2);
    List_display(lref);
    List_add_front(lref, (void*) dref3);
    List_display(lref);
    UT_EQUAL_INT((List_size(lref)), 3);
    int v1 = (int)((DummyObjRef)List_remove_first(lref))->value;
    int v2 = (int)((DummyObjRef)List_remove_first(lref))->value;
    int v3 = (int)((DummyObjRef)List_remove_first(lref))->value;
    UT_EQUAL_INT((List_size(lref)), 0);
    UT_EQUAL_INT(v1, 333);
    UT_EQUAL_INT(v2, 222);
    UT_EQUAL_INT(v3, 111);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}

int test_list_remove_back()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_front(lref, (void*) dref);
    List_remove_first(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    DummyObjRef dref1 = DummyObj_new(111);
    DummyObjRef dref2 = DummyObj_new(222);
    DummyObjRef dref3= DummyObj_new(333);
    List_display(lref);
    List_add_back(lref, (void*) dref1);
    List_display(lref);
    List_add_back(lref, (void*) dref2);
    List_display(lref);
    List_add_back(lref, (void*) dref3);
    List_display(lref);
    UT_EQUAL_INT((List_size(lref)), 3);
    int v1 = (int)((DummyObjRef)List_remove_last(lref))->value;
    int v2 = (int)((DummyObjRef)List_remove_last(lref))->value;
    int v3 = (int)((DummyObjRef)List_remove_last(lref))->value;
    UT_EQUAL_INT((List_size(lref)), 0);
    UT_EQUAL_INT(v1, 333);
    UT_EQUAL_INT(v2, 222);
    UT_EQUAL_INT(v3, 111);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}


int test_iter()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_front(lref, (void*) dref);
    List_remove_first(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    DummyObjRef dref1 = DummyObj_new(111);
    DummyObjRef dref2 = DummyObj_new(222);
    DummyObjRef dref3= DummyObj_new(333);
    List_add_front(lref, (void*) dref1);
    List_add_front(lref, (void*) dref2);
    List_add_front(lref, (void*) dref3);
    UT_EQUAL_INT((List_size(lref)), 3);
    ListIterator iter = List_iterator(lref);
    for(int i = 3; i != 0;i--) {
        DummyObjRef dref = (DummyObjRef)List_itr_unpack(lref, iter);
        int v1 = i*100 + i*10 + i;
        int v2 = dref->value;
        UT_EQUAL_INT(v1, v2);
        iter = List_itr_next(lref, iter);
    }
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}
int test_list_remove_backx()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_back(lref, (void*) dref);
    List_remove_last(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    DummyObjRef dref1 = DummyObj_new(111);
    DummyObjRef dref2 = DummyObj_new(222);
    DummyObjRef dref3= DummyObj_new(333);
    List_add_back(lref, (void*) dref1);
    List_add_back(lref, (void*) dref2);
    List_add_back(lref, (void*) dref3);
    UT_EQUAL_INT((List_size(lref)), 3);
    DummyObjRef oref1 = (DummyObjRef)List_remove_last(lref);
    DummyObjRef oref2 = (DummyObjRef)List_remove_last(lref);
    DummyObjRef oref3 = (DummyObjRef)List_remove_last(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    UT_EQUAL_INT((oref1->value), 333);
    UT_EQUAL_INT((oref2->value), 222);
    UT_EQUAL_INT((oref3->value), 111);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}


int test_list_remove_back_one()
{
    ListRef lref = List_new(dealloc);
    DummyObjRef dref = DummyObj_new(333);
    List_add_front(lref, (void*) dref);
    List_remove_last(lref);
    UT_EQUAL_INT((List_size(lref)), 0);
    List_free(&lref);
    UT_EQUAL_PTR(lref, NULL);
    return 0;

    return 0;
}

int main()
{
	UT_ADD(test_List_new);
    UT_ADD(test_list_add_back);
    UT_ADD(test_list_add_front);
    UT_ADD(test_list_remove_front);
    UT_ADD(test_list_remove_back);
    UT_ADD(test_iter);
	int rc = UT_RUN();
	return rc;
}