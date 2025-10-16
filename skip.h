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

uint64_t skip_get_cfg_size(void* cfg);

uint64_t skip_get_datatype_size(int32_t type_code);

int skip_write_index_to_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index);

int skip_read_index_from_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index);

/**
 * @brief Retrieves a direct pointer to the start of the data for a given index within the buffer.
 * 
 * @warning On architectures with strict memory alignment requirements, dereferencing the returned pointer
 * for multi-byte types may cause performance issues or crashes. For portable and safe access,
 * prefer using skip_read_index_from_buffer, which handles alignment and endianness correctly.
 * This function is primarily for use cases where in-place, read-only access to byte arrays
 * (e.g., strings) is needed and performance is critical.
 *
 * @param cfg A pointer to the SKIP config.
 * @param buffer A pointer to the source buffer.
 * @param index The index in the config that specifies which data segment to point to.
 * @return A void* pointer to the data segment, or `nullptr` if the index is out of bounds.
 */
void* skip_get_index_ptr(void* cfg, void* buffer, uint64_t index);

void* skip_import_cfg(const char* buffer, uint64_t buffer_size);

uint64_t skip_get_export_buffer_size(void* cfg);

int skip_export_cfg(void* cfg, char* buffer, uint64_t buffer_size);

int skip_get_system_endian();

int skip_set_endian_value_cfg(void* cfg, int endian);

#ifdef __cplusplus
}
#endif

#endif
