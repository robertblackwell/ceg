#ifndef c_http_xr_conn_list_h
#define c_http_xr_conn_list_h

#include <c_http/xr/conn.h>
#include <c_http/list.h>


typedef ListRef XrConnListRef;
typedef ListIter XrConnListIter;

XrConnListRef XrConnList_new();
void XrConnList_free();

int XrConnList_size (XrConnListRef lref);

XrConnectionRef XrConnList_first (XrConnListRef lref);

XrConnectionRef XrConnList_last (XrConnListRef lref);

XrConnectionRef XrConnList_remove_first (XrConnListRef lref);

XrConnectionRef XrConnList_remove_last (XrConnListRef lref);

XrConnectionRef XrConnList_itr_unpack (XrConnListRef lref, XrConnListIter iter);

XrConnListIter XrConnList_iterator (XrConnListRef lref);

XrConnListIter XrConnList_itr_next (XrConnListRef lref, XrConnListIter iter);

void XrConnList_itr_remove (XrConnListRef lref, XrConnListIter *iter);

void XrConnList_add_back (XrConnListRef lref, XrConnectionRef item);

void XrConnList_add_front (XrConnListRef lref, XrConnectionRef item);

#endif