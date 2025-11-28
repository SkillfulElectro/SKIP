#ifndef SKIP_H
#define SKIP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SkipEndian {
    SKIP_BIG_ENDIAN = 0,
    SKIP_LITTLE_ENDIAN = 1
};

enum SkipError {
    SKIP_SUCCESS = 0,
    SKIP_ERROR_INVALID_ARGUMENT = -1,
    SKIP_ERROR_OUT_OF_BOUNDS = -2,
    SKIP_ERROR_ALLOCATION_FAILED = -3,
    SKIP_ERROR_BUFFER_TOO_SMALL = -4,
    SKIP_ERROR_INVALID_CONFIG = -5,
};

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
    skip_nest = 11,
};

typedef struct SkipInternalType {
    int32_t type_code;
    uint64_t count;
} SkipInternalType;

void* skip_create_base_config();

int skip_push_type_to_config(void* cfg , int32_t type_code , uint64_t len);

int skip_pop_type_from_config(void* cfg);

SkipInternalType* skip_get_type_at_index(void* cfg , uint64_t index);

int skip_free_cfg(void* cfg);

uint64_t skip_get_data_size(void* cfg);

uint64_t skip_get_datatype_size(int32_t type_code);

int skip_write_index_to_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index);

int skip_read_index_from_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index);

void* skip_get_index_ptr(void* cfg, void* buffer, uint64_t index);

int skip_import_header_body(void* cfg, const char* buffer, uint64_t buffer_size);

uint64_t skip_get_export_header_body_size(void* cfg);

int skip_export_header_body(void* cfg, char* buffer, uint64_t buffer_size);

uint64_t skip_get_header_export_size();

int skip_export_header(void* cfg, char* buffer, uint64_t buffer_size, uint64_t* out_body_size);

void* skip_import_header(const char* buffer, uint64_t buffer_size, uint64_t* out_body_size);

int skip_get_system_endian();

int skip_set_endian_value_cfg(void* cfg, int endian);



int skip_get_cfg_endian(void* cfg);

int skip_create_nest_buffer(void* cfg, void* final_res, uint64_t final_res_size, void* data_buffer, uint64_t data_size);

int skip_get_nest_cfg(void* cfg, void* nest_base_cfg, void* nest_buffer, uint64_t nest_size);

int skip_get_nested_data_buffer(void* cfg, void* nest_buffer, uint64_t nest_size, void* data_buffer, uint64_t data_size);

uint64_t skip_export_standalone_size(void* cfg);


int skip_export_standalone(void* cfg , void* data_buffer , uint64_t data_size , void* standalone_buffer , uint64_t standalone_size);

int skip_import_standalone_get_cfg(void** void_null_ptr, void* buffer, uint64_t buffer_size);


int skip_import_standalone_get_data_buffer(void* cfg , void* buffer , uint64_t buffer_size , void* data_buffer , uint64_t data_buffer_size);

#ifdef __cplusplus
}
#endif

#endif
