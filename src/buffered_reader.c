
#include "buffered_reader.h"

#include <stdio.h>
#include<stdlib.h>
#include<string.h>

result(buffered_reader_ptr) new_buffered_reader(FILE *fd) {
    NULL_ARGUMENT_ERROR_RETURN(buffered_reader_ptr, fd);

    buffered_reader *new_buffered_reader = (buffered_reader*)malloc(sizeof(buffered_reader));
    memset(new_buffered_reader, 0, sizeof(buffered_reader));

    fseek(fd, 0L, SEEK_END);
    new_buffered_reader->buffer_len = ftell(fd);
    new_buffered_reader->buffer = calloc(new_buffered_reader->buffer_len + 1, 1);
    rewind(fd);
    fread(new_buffered_reader->buffer, 1, new_buffered_reader->buffer_len, fd);

    new_buffered_reader->has_had_error = false;
    fclose(fd);

    RESULT_RETURN(buffered_reader_ptr, new_buffered_reader);
}

result(bool) delete_buffered_reader(buffered_reader *br) {
    NULL_ARGUMENT_ERROR_RETURN(bool, br);

    free(br->buffer);
    free(br);
    
    RESULT_RETURN(bool, true);
}

result(char) bf_get_char(buffered_reader *br) {
    NULL_ARGUMENT_ERROR_RETURN(char, br);

//    if(br->index >= br->buffer_len) {
//        enum bufferered_reader_codes result = _bf_read_into_buffer(br);
//
//        switch(result) {
//            case BF_EOF:
//               RESULT_ERROR_RETURN(char, "Buffer reader reached EOF", REACHED_EOF); 
//            case BF_READ_ERROR:
//               RESULT_ERROR_RETURN(char, "Buffer reader failed to read from file", READ_OPERATION_FAILED); 
//            default:
//        }
//
//        br->line = br->column = 0;
//    }

    if(br->index == br->buffer_len) {
        RESULT_ERROR_RETURN(char, "Reached EOF", REACHED_EOF);
    }

    result(char) r = {
        .status = OK,
        .result = br->buffer[br->index]
    };

    br->index += 1;
    if(r.result == '\n') {
        br->line += 1;
        br->column = 0;
    }else {
        br->column += 1;
    }

    return r;
}

result(char) bf_peek_char(buffered_reader *br) {
    NULL_ARGUMENT_ERROR_RETURN(char, br);

//    if(br->index >= br->buffer_len) {
//        enum bufferered_reader_codes result = _bf_read_into_buffer(br);
//
//        switch(result) {
//            case BF_EOF:
//               RESULT_ERROR_RETURN(char, "Buffer reader reached EOF", REACHED_EOF); 
//            case BF_READ_ERROR:
//               RESULT_ERROR_RETURN(char, "Buffer reader failed to read from file", READ_OPERATION_FAILED); 
//            default:
//        }
//    }
    RESULT_RETURN(char, br->buffer[br->index]);
}

result(char_ptr) bf_get_buffer(buffered_reader *br) {
    NULL_ARGUMENT_ERROR_RETURN(char_ptr, br);

    if(br->buffer_len == 0) {
        RESULT_ERROR_RETURN(char_ptr,
                            "Reader buffer is empty, either read hasn't been called or it's at EOF",
                            BUFFER_IS_EMPTY);
    }

    RESULT_RETURN(char_ptr, br->buffer);
}

//result(bool) bf_buffer_read(buffered_reader *br) {
//    NULL_ARGUMENT_ERROR_RETURN(bool, br);
//
//    switch(_bf_read_into_buffer(br)) {
//        case BF_EOF:
//            RESULT_ERROR_RETURN(bool, "Buffer reader reached EOF", REACHED_EOF);
//        case BF_READ_ERROR:
//            RESULT_ERROR_RETURN(bool, "Failed to read from buffer", READ_OPERATION_FAILED);
//        default:
//    }
//
//    RESULT_RETURN(bool, true);
//}

//enum bufferered_reader_codes _bf_read_into_buffer(buffered_reader *br) {
//    clearerr(br->fd);
//    size_t read = fread(br->buffer, sizeof(char), BUFFER_SIZE, br->fd);
//
//    if(read == 0) {
//        br->has_had_error = true;
//        if(feof(br->fd)) {
//            clearerr(br->fd);
//            return BF_EOF;
//        }else {
//            clearerr(br->fd);
//            return BF_READ_ERROR;
//        }
//    }
//
//    br->buffer_len = read;
//    br->index = 0;
//
//    return BF_OK;
//} 
