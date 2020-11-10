
#ifndef c_http_ll_parser_h
#define c_http_ll_parser_h
/**
 * This modules wraps the c language http parser provided by github.com/joyent/http-parser
 * for use in this project.
 */
#include <llhttp.h>
#include <c_http/message.h>
#include <c_http/buffer/cbuffer.h>

/**
 * state values used in parsing http header lines
 */
#define kHEADER_STATE_NOTHING 10
#define kHEADER_STATE_FIELD   11
#define kHEADER_STATE_VALUE   12
/**
 * Holds  details of a parse failure
 */
struct LL_ParserError_s {
    char*               m_name;
    char*               m_description;
    enum http_errno     m_err_number;
};
typedef struct LL_ParserError_s LL_ParserError;

/**
 * \brief  Return code used as part of the value returned by Parser_consume() when processing data.
 */
enum LL_ParserRC {
    ParserRC_error,          /// got a parse error
    ParserRC_end_of_header,  /// encountered enf of header
    ParserRC_end_of_message, /// encountered end of message
    ParserRC_end_of_data     /// processed all the data given
};
typedef enum LL_ParserRC LL_ParserRC;

/**
 * Value object return by Parser_consume()
 */
struct LL_ParserReturnValue {
    LL_ParserRC   return_code;
    int           bytes_remaining;
};

typedef struct LL_ParserReturnValue LL_ParserReturnValue;

/**
 * Type holding context data for Parser functions. Allows for parsing to continue
 * over buffer and message boundaries
 */
struct LL_Parser_s;
typedef struct LL_Parser_s LL_Parser, *LL_ParserRef;

struct LL_Parser_s {
    bool m_started;
    bool m_header_done;
    bool m_message_done;
    /*
     * These are required to run the http-parser
     */
    llhttp_t*                m_llhttp_ptr;
    llhttp_settings*         m_llhttp_settings_ptr;
    MessageRef               m_current_message_ptr;

    int                      m_header_state;
    ///////////////////////////////////////////////////////////////////////////////////
    // String buffers used to accumulate values from http-parser
    ///////////////////////////////////////////////////////////////////////////////////
    CbufferRef             m_url_buf;
    CbufferRef             m_status_buf;
    CbufferRef             m_name_buf;
    CbufferRef             m_value_buf;
};

LL_ParserRef LL_Parser_new();
void LL_Parser_free(LL_ParserRef* parser_p);

void LL_Parser_begin(LL_ParserRef parser, MessageRef msg_ref);

/**
 * The guts of the http message parsing process.
 *
 * Call this function repeatedly with successive buffers of data.
 * These successive buffers may represent one or more messages and a single buffer is permitted to hold
 * the end of one message and the start of the successive message.
 *
 * The returned value indicates the state the parser is in after processing a buffer, and under some circumstances
 * how much of the provided buffer was consumed.
 *
 * Except under error conditions, the only time a buffer will be only partially consumed is when
 * a messages ends part way through a buffer. This is indicated by message complete being returned
 * along with number of bytes consumed being less that the size of the buffer provided.
 *
 * Under such situation the completed message should be retreived from the parser, the parser begin()
 * method called again and the remainder of the incomplete buffer presented to the parser to start
 * the next message.
 *
 * @param parser ParserRef
 * @param buffer A buffer of data presumable read from a tcp connectin
 * @param length Length of the data ba=uffer
 * @return ParserReturnValue - a struct
 */
LL_ParserReturnValue LL_Parser_consume(LL_ParserRef parser, const void* buffer, int length);

/**
 * Returns the message currently being worked on. Only valid after Parser_consume() returns ParserReturnValue.return_code == ReturnRC_end_of_message
 * \param parser ParserRef
 * \return MessageRef or NULL
 */
MessageRef      LL_Parser_current_message(LL_ParserRef parser);

/**
 * Gather details of latest error
 * \param parser
 * \return
 */
bool            LL_Parser_is_error(LL_ParserRef parser);
enum http_errno LL_Parser_get_errno(LL_ParserRef parser);
ParserError     LL_Parser_get_error(LL_ParserRef parser);

/**
 * C parser class callback functions that interface with the C language parser
 * http-parser from github.com/joyent/http-parser.
 *
 * These could have been hidden in the parser.c file
*
int ll_message_begin_cb(llhttp_t* parser);
int ll_url_data_cb(llhttp_t* parser, const char* at, size_t length);
int ll_status_data_cb(llhttp_t parser, const char* at, size_t length);
int ll_header_field_data_cb(llhttp_t parser, const char* at, size_t length);
int ll_header_value_data_cb(llhttp_t parser, const char* at, size_t length);
int ll_headers_complete_cb(llhttp_t parser);
int ll_chunk_header_cb(llhttp_t parser);
int ll_body_data_cb(llhttp_t parser, const char* at, size_t length);
int ll_chunk_complete_cb(llhttp_t parser);
int ll_message_complete_cb(llhttp_t parser);
*/
#endif

