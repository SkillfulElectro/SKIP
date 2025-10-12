#ifndef SKIP_H
#define SKIP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SkipDataTypeCode {
    skip_int8 = 0,
    skip_uint8 = 1,
    skip_int16 = 2,
    skip_uint16 = 3,
    skip_int32 = 4,
    skip_uint32 = 5,
    skip_int64 = 6,
    skip_uint64 = 7,
    skip_float32 = 8,
    skip_float64 = 9,
    skip_char = 10,
};

typedef struct SkipInternalType {
    int32_t type_code;
    uint64_t count;
} SkipInternalType;

void* skip_create_base_config();

int skip_push_type_to_config(void* cfg , int type_code , size_t len);

int skip_pop_type_from_config(void* cfg);

SkipInternalType* skip_get_type_at_index(void* cfg , size_t index);

int skip_free_cfg(void* cfg);

size_t skip_get_cfg_size(void* cfg);

size_t skip_get_datatype_size(int type_code);

int skip_write_index_to_buffer(void* cfg , void* buffer , void* value , size_t index);

int skip_read_index_from_buffer(void* cfg , void* buffer , void* value , size_t index);

void* skip_get_index_ptr(void* cfg, void* buffer, size_t index);

void* skip_import_cfg(const char* cfg);

size_t skip_get_export_buffer_size(void* cfg);

int skip_export_cfg(void* cfg, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
