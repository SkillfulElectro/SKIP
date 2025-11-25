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

/**
 * @brief Imports a configuration body into a pre-existing config object.
 *
 * This function reads the body of a a configuration from the given buffer and populates the
 * provided config object. This should be called after skip_import_header.
 *
 * @param cfg A pointer to the config object created by skip_import_header.
 * @param buffer A pointer to the buffer containing the configuration body.
 * @param buffer_size The size of the buffer.
 * @return SKIP_SUCCESS on success, or an error code on failure.
 */
int skip_import_header_body(void* cfg, const char* buffer, uint64_t buffer_size);

/**
 * @brief Gets the required buffer size for the configuration body.
 *
 * @param cfg A pointer to the SKIP config.
 * @return The size in bytes required for the configuration body.
 */
uint64_t skip_get_export_body_size(void* cfg);

/**
 * @brief Exports the configuration body to a buffer.
 *
 * This function serializes the main body of the configuration, excluding the header.
 *
 * @param cfg A pointer to the SKIP config.
 * @param buffer A pointer to the destination buffer.
 * @param buffer_size The size of the destination buffer.
 * @return SKIP_SUCCESS on success, or an error code on failure.
 */
int skip_export_header_body(void* cfg, char* buffer, uint64_t buffer_size);

/**
 * @brief Gets the fixed size of the configuration header.
 *
 * @return The size of the header in bytes.
 */
uint64_t skip_get_header_export_size();

/**
 * @brief Exports the configuration header to a buffer.
 *
 * @param cfg A pointer to the SKIP config.
 * @param buffer A pointer to the destination buffer.
 * @param buffer_size The size of the destination buffer.
 * @param out_body_size A pointer to a uint64_t to store the size of the configuration body.
 * @return SKIP_SUCCESS on success, or an error code on failure.
 */
int skip_export_header(void* cfg, char* buffer, uint64_t buffer_size, uint64_t* out_body_size);

/**
 * @brief Imports a configuration header and creates a new config object.
 *
 * @param buffer A pointer to the buffer containing the header.
 * @param buffer_size The size of the buffer.
 * @param out_body_size A pointer to a uint64_t to store the expected size of the configuration body.
 * @return A pointer to the newly created config object, or NULL on failure.
 */
void* skip_import_header(const char* buffer, uint64_t buffer_size, uint64_t* out_body_size);

int skip_get_system_endian();

int skip_set_endian_value_cfg(void* cfg, int endian);



int skip_get_cfg_endian(void* cfg);

int skip_create_nest_buffer(void* final_res , uint64_t final_res_size , void* meta_buffer , uint64_t meta_size , void* data_buffer , uint64_t data_size);

int skip_get_nest_cfg(void* cfg , void* nest_base_cfg ,  void* nest_buffer , uint64_t nest_size);

int skip_get_nested_data_buffer(void* nested_cfg_buffer , void* nest_buffer , uint64_t nest_size , void* data_buffer , uint64_t data_size);

uint64_t skip_export_standalone_size(void* cfg);


int skip_export_standalone(void* cfg , void* data_buffer , uint64_t data_size , void* standalone_buffer , uint64_t standalone_size);

int skip_fill_import_standalone_cfg(void* void_null_ptr , void* buffer , uint64_t buffer_size);


int skip_fill_data_buffer_import_standalone(void* cfg , void* buffer , uint64_t buffer_size , void* data_buffer , uint64_t data_buffer_size);

#ifdef __cplusplus
}
#endif

#endif
