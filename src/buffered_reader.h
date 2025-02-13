#ifndef BUFFERED_READER_H
#define BUFFERED_READER_H

#include<stdio.h>
#include<stdbool.h>

#include "result.h"

typedef struct buffered_reader* buffered_reader_ptr;
DEFINE_RESULT_TYPE(buffered_reader_ptr);

enum bufferered_reader_codes {
    BF_OK,
    BF_READ_ERROR,
    BF_EOF,
    BF_NULL_PTR
};


typedef struct buffered_reader buffered_reader;
struct buffered_reader {
 //   FILE *fd;
    char *buffer;
    size_t index, buffer_len;
    size_t line, column;

    bool has_had_error;
};

result(buffered_reader_ptr) new_buffered_reader(FILE *fd);
result(bool) delete_buffered_reader(buffered_reader *br);
result(char) bf_get_char(buffered_reader *br);
result(char) bf_peek_char(buffered_reader *br);
result(char_ptr) bf_get_buffer(buffered_reader *br);
//result(bool) bf_buffer_read(buffered_reader *br);


//enum bufferered_reader_codes _bf_read_into_buffer(buffered_reader *br); 


#endif /* BUFFERED_READER_H */

