#define _GNU_SOURCE

#include <c_eg/datasource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <c_eg/alloc.h>

#include <c_eg/unittest.h>
#include <c_eg/utils.h>
#include <c_eg/buffer/iobuffer.h>
#include <c_eg/rdsocket.h>
#include <c_eg/reader.h>

/**
 *
 * @param this DataSourceRef
 * @param blocks a pointer to an array of char* (an array of const cstring pointers)
 */
void DataSource_init(DataSourceRef this, char** blocks)
{
    this->m_block_count = 0;
    this->m_blocks = blocks;
}

/**
 * Returns the next block of utf-8 null terminated data, NULL when done
 * @return char*, NULL when done
 */
char* DataSource_next(DataSourceRef this)
{
    char* block = this->m_blocks[this->m_block_count];
    this->m_block_count++;
    return block;
}
/**
 * @return bool true when no more data
 */
bool DataSource_finished(DataSourceRef this)
{
    return (this->m_blocks[this->m_block_count] == NULL);
}

int DataSource_read(DataSourceRef this, void* buffer, int length)
{
    char* block = this->m_blocks[this->m_block_count];
    if (block == NULL) {
        return 0;
    } else if (strcmp(block, "error") == 0) {
        return -1;
    } else {
        this->m_block_count++;
        int block_len = strlen(block);
        assert(block_len < length);
        memcpy((void*)buffer, block, block_len);
        return block_len;
    }
}
