#define _GNU_SOURCE
#include <c_http/api/message.h>
#include <c_http/test_helpers/message_private.h>
#include <c_http/dsl/alloc.h>
#include <c_http/details/hdrlist.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//#define TYPE Message
//#define Message_TAG "_MSG_"
//#include <c_http/check_tag.h>
//#undef TYPE
//#define CHECK_MESSAGE(p) CHECK_TAG(Message, p)
/**
 * @addtogroup group_message
 * @{
 */
struct Message_s
{
    MESSAGE_DECLARE_TAG;
    BufferChainRef body;
    HdrListRef headers;
    int major_vers;
    HttpMinorVersion minor_vers;
    bool is_request;
    HttpStatus status_code;
    CbufferRef reason;
    HttpMethod method;
    CbufferRef target;
};

MessageRef Message_new ()
{
    MessageRef mref = (MessageRef) eg_alloc(sizeof(Message));
    MESSAGE_SET_TAG(mref)
    if(mref == NULL) goto error_label_1;
    SET_TAG(Message, mref)
    mref->body = NULL;
    mref->headers = HdrList_new();
    if(mref->headers == NULL) goto error_label_2;
    mref->minor_vers = minor_version1;
    mref->major_vers = major_version1;
    mref->target = Cbuffer_new();
    mref->reason = Cbuffer_new();
    return mref;

    error_label_2:
        Message_dispose(&mref);
    error_label_1:
        return NULL;
}
/**
 * @brief Create a new request message instance
 * @return MessageRef
 */
MessageRef Message_new_request()
{
    MessageRef mref = Message_new();
    if(mref != NULL) {
        mref->is_request = true;
//        mref->target = Cbuffer_new();
        return mref;
    }
    return NULL;
}
MessageRef Message_new_response()
{

    MessageRef mref = Message_new();
    if(mref != NULL) {
        mref->is_request = false;
//        mref->reason = Cbuffer_new();
        return mref;
    }
    return NULL;
}
void Message_dispose(MessageRef* this_p)
{
    MESSAGE_CHECK_TAG(*this_p)
    MessageRef this = *this_p;
    MESSAGE_CHECK_TAG(this)
    HdrList_dispose(&(this->headers));
    Cbuffer_dispose(&(this->target));
    Cbuffer_dispose(&(this->reason));

    eg_free(*this_p);
    *this_p = NULL;
}
void Message_dispose_anonymous(void* p)
{
    Message_dispose((MessageRef*)&p);
}
MessageRef MessageResponse(HttpStatus status, void* body)
{
    MessageRef mref = Message_new(false);
    if(mref == NULL) goto error_1;
    mref->is_request = false;
    mref->status_code = status;
    mref->body = body;
    if(mref->target != NULL) Cbuffer_dispose(&(mref->target));
    if(mref->reason != NULL) Cbuffer_dispose(&(mref->reason));
    return mref;
    error_1:
        return NULL;
}
IOBufferRef Message_serialize(MessageRef mref)
{
    MESSAGE_CHECK_TAG(mref)
    BufferChainRef bc_result = BufferChain_new();
    char* first_line;
    int first_line_len;
    if(mref->is_request) {
        const char* meth = llhttp_method_name(mref->method);
        first_line_len = asprintf(&first_line,"%s %s HTTP/%d.%d\r\n", meth, (char*)Cbuffer_data(mref->target), mref->major_vers, mref->minor_vers);
    } else {
        first_line_len = asprintf(&first_line, "HTTP/%d.%d  %d %s\r\n",mref->major_vers, mref->minor_vers, mref->status_code, (char*)Cbuffer_data(mref->reason));
    }
    BufferChain_append_cstr(bc_result, first_line);
    free(first_line);
    HdrListRef hdrs = mref->headers;
    ListIterator iter = HdrList_iterator(hdrs);
    while(iter != NULL) {
        KVPairRef item = HdrList_itr_unpack(hdrs, iter);
        char* s;
        int len = asprintf(&s,"%s: %s\r\n", KVPair_label(item), KVPair_value(item));
        BufferChain_append_cstr(bc_result, s);
        ListIterator next = HdrList_itr_next(hdrs, iter);
        iter = next;
        free(s);
    }
    BufferChain_append_cstr(bc_result, "\r\n");
    if((mref->body != NULL) && (BufferChain_size(mref->body) != 0)) {
        IOBufferRef iob_body = BufferChain_compact(mref->body);
        BufferChain_add_back(bc_result, iob_body);
    }
    IOBufferRef result = BufferChain_compact(bc_result);
    BufferChain_dispose(&bc_result);
    return result;
}
void Message_add_header_cstring(MessageRef this, const char* key, const char* value)
{
    HdrListRef hdrlist = Message_get_headerlist(this);
    HdrList_add_cstr(hdrlist, key, value);
}
void Message_add_header_cbuf(MessageRef this, CbufferRef key, CbufferRef value)
{
    HdrList_add_cbuf(Message_get_headerlist(this), key, value);
}
HttpStatus Message_get_status(MessageRef this)
{
    MESSAGE_CHECK_TAG(this)
    return this->status_code;
}
void Message_set_status(MessageRef this, HttpStatus status)
{
    MESSAGE_CHECK_TAG(this);
    this->status_code = status;
}
bool Message_get_is_request(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return this->is_request;
}
void Message_set_is_request(MessageRef this, bool yn)
{
    MESSAGE_CHECK_TAG(this);
    this->is_request = yn;
}
HttpMinorVersion Message_get_minor_version(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return this->minor_vers;
}

void Message_set_minor_version(MessageRef this, HttpMinorVersion mv)
{
    MESSAGE_CHECK_TAG(this);
    this->minor_vers = mv;
}
void Message_set_version(MessageRef this, int major_vers, int minor_vers)
{
    MESSAGE_CHECK_TAG(this);
    this->minor_vers = minor_vers;
}
void Message_set_method(MessageRef this, HttpMethod method)
{
    MESSAGE_CHECK_TAG(this);
    this->method = method;
}
HttpMethod Message_get_method(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return this->method;
}

// target
const char* Message_get_target(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return (const char*)Cbuffer_cstr(this->target);
}
//void Message_move_target(MessageRef this, CbufferRef target)
//{
//    if(this->target == NULL) {
//        this->target = Cbuffer_new();
//    }
//    Cbuffer_move(this->target, target);
//}
void Message_set_target(MessageRef this, const char* targ)
{
    MESSAGE_CHECK_TAG(this);
    assert((this->target != NULL) && (Cbuffer_size(this->target) == 0));
    Cbuffer_append_cstr(this->target, (const char*)targ);
}
CbufferRef Message_get_target_cbuffer(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return Cbuffer_from_cstring(Cbuffer_cstr(this->target));
}
void Message_set_target_cbuffer(MessageRef this, CbufferRef target)
{
    MESSAGE_CHECK_TAG(this);
    assert((this->target != NULL) && (Cbuffer_size(this->target) == 0));
    Cbuffer_append_cstr(this->target, (const char*)Cbuffer_cstr(target));

}
// reason
void Message_set_reason(MessageRef this, const char* reason_cstr)
{
    MESSAGE_CHECK_TAG(this);
    assert((this->reason != NULL) && (Cbuffer_size(this->reason) == 0));
    Cbuffer_append_cstr(this->reason, (const char*)reason_cstr);
}
//void Message_move_reason(MessageRef this, CbufferRef reason)
//{
//    if(this->reason == NULL)
//        this->reason = Cbuffer_new();
//    Cbuffer_move(this->reason, reason);
//}
const char* Message_get_reason(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    (const char*)Cbuffer_cstr(this->reason);
}
CbufferRef Message_get_reason_cbuffer(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return Cbuffer_from_cstring(Cbuffer_cstr(this->reason));
}
void Message_set_reason_cbuffer(MessageRef this, CbufferRef target)
{
    MESSAGE_CHECK_TAG(this);
    assert((this->reason != NULL) && (Cbuffer_size(this->reason) == 0));
    Cbuffer_append_cstr(this->reason, (const char*)Cbuffer_cstr(target));
}
int Message_get_content_length(MessageRef this)
{
    assert(false);
}
void Message_set_content_length(MessageRef this, int length)
{
    char buf[10];
    assert(length >= 0);
    int r = sprintf(buf, "%d", length);
    HdrListRef hdrlist_ref = this->headers;
    KVPairRef kvp = HdrList_find(hdrlist_ref, "Content-length");
    if(kvp != NULL) {
        KVPair_set_value(kvp, buf, strlen(buf));
    } else {
        HdrList_add_cstr(hdrlist_ref, "Content-length", buf);
    }
}

// headers
HdrListRef Message_get_headerlist(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return this->headers;
}
const char* Message_get_header_value(MessageRef mref, const char* labptr)
{
    KVPairRef kvp = HdrList_find(Message_get_headerlist(mref), labptr);
    if(kvp == NULL) {
        return NULL;
    }
    return KVPair_value(kvp);
}

BufferChainRef Message_get_body(MessageRef this)
{
    MESSAGE_CHECK_TAG(this);
    return this->body;
}
void Message_set_body(MessageRef this, BufferChainRef bc)
{
    MESSAGE_CHECK_TAG(this);
    this->body = bc;
}
void Message_set_headers_arr(MessageRef this, const char* ar[][2])
{
    MESSAGE_CHECK_TAG(this);
    HdrList_add_arr(this->headers, ar);
}
/**@}*/