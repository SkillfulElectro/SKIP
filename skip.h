#ifndef SKIP_H
#define SKIP_H

#include <stddef.h>

enum SkipDataTypeCode {
    skip_int = 0,
    skip_double = 1,
    skip_char = 2,
    skip_size_t = 3,
};

struct SkipInternalType {
    int type_code;
    size_t count;
};

void* skip_create_base_config();

int skip_push_type_to_config(void* cfg , int type_code , size_t len);

int skip_pop_type_from_config(void* cfg);

SkipInternalType* skip_get_type_at_index(void* cfg , size_t index);

int skip_free_cfg(void* cfg);

size_t skip_get_cfg_size(void* cfg);

size_t skip_get_datatype_size(int type_code);

int skip_write_index_to_buffer(void* cfg , void* buffer , void* value , size_t index);

int skip_read_index_from_buffer(void* cfg , void* buffer , void* value , size_t index);

#endif
