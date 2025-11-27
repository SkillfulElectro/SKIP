#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "skip.h"

#define SKIP_MAGIC 0x534B4950 // "SKIP" in ASCII

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint64_t body_size;
    uint8_t endian;
    uint8_t reserved[15];
} SkipHeader;


static int is_little_endian() {
    volatile uint32_t i = 0x01234567;
    return (*((volatile uint8_t*)(&i))) == 0x67;
}

static uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

static uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

static uint64_t swap_uint64(uint64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}



typedef struct {
    SkipInternalType* types;
    uint64_t types_size;
    uint64_t types_capacity;

    uint64_t* offsets;
    uint64_t offsets_size;
    uint64_t offsets_capacity;
    int endian;
} SkipConfig;

static int ensure_capacity(void** array, uint64_t* capacity, uint64_t element_size, uint64_t new_capacity) {
    if (*capacity < new_capacity) {
        void* new_array = realloc(*array, (size_t)(new_capacity * element_size));
        if (!new_array) {
            return SKIP_ERROR_ALLOCATION_FAILED;
        }
        *array = new_array;
        *capacity = new_capacity;
    }
    return SKIP_SUCCESS;
}

#define SKIP_INITIAL_CAPACITY 16
#define SKIP_CONFIG_VERSION 1

void* skip_create_base_config() {
    SkipConfig* config = (SkipConfig*)malloc(sizeof(SkipConfig));
    if (!config) return NULL;

    config->types = NULL;
    config->types_size = 0;
    config->types_capacity = 0;
    if (ensure_capacity((void**)&config->types, &config->types_capacity, sizeof(SkipInternalType), SKIP_INITIAL_CAPACITY) != 0) {
        free(config);
        return NULL;
    }

    config->offsets = NULL;
    config->offsets_size = 0;
    config->offsets_capacity = 0;
    if (ensure_capacity((void**)&config->offsets, &config->offsets_capacity, sizeof(uint64_t), SKIP_INITIAL_CAPACITY) != 0) {
        free(config->types);
        free(config);
        return NULL;
    }
    config->offsets[0] = 0;
    config->offsets_size = 1;

    config->endian = skip_get_system_endian();

    return config;
}

int skip_get_system_endian() {
    return is_little_endian() ? SKIP_LITTLE_ENDIAN : SKIP_BIG_ENDIAN;
}

int skip_get_cfg_endian(void* cfg) {
    SkipConfig* conf = cfg;

    return conf->endian;
}

int skip_set_endian_value_cfg(void* cfg, int endian) {
    if (!cfg) {
        return SKIP_ERROR_INVALID_ARGUMENT;
    }
    SkipConfig* config = (SkipConfig*)cfg;
    if (endian != SKIP_BIG_ENDIAN && endian != SKIP_LITTLE_ENDIAN) {
        return SKIP_ERROR_INVALID_ARGUMENT;
    }
    config->endian = endian;
    return SKIP_SUCCESS;
}

int skip_push_type_to_config(void* cfg, int32_t type_code, uint64_t count) {
    SkipConfig* config = (SkipConfig*)cfg;

    if (config->types_size == config->types_capacity) {
        uint64_t new_cap = config->types_capacity == 0 ? 8 : config->types_capacity * 2;
        int ret = ensure_capacity((void**)&config->types, &config->types_capacity, sizeof(SkipInternalType), new_cap);
        if (ret != SKIP_SUCCESS) return ret;
    }

    if (config->offsets_size == config->offsets_capacity) {
        uint64_t new_cap = config->offsets_capacity == 0 ? 8 : config->offsets_capacity * 2;
        int ret = ensure_capacity((void**)&config->offsets, &config->offsets_capacity, sizeof(uint64_t), new_cap);
        if (ret != SKIP_SUCCESS) return ret;
    }

    uint64_t type_size = skip_get_datatype_size(type_code);
    uint64_t new_offset = config->offsets[config->offsets_size - 1] + (type_size * count);

    config->types[config->types_size].type_code = type_code;
    config->types[config->types_size].count = count;
    config->types_size++;

    config->offsets[config->offsets_size] = new_offset;
    config->offsets_size++;

    return SKIP_SUCCESS;
}

uint64_t skip_get_header_export_size() {
    return sizeof(SkipHeader);
}

int skip_export_header(void* cfg, char* buffer, uint64_t buffer_size, uint64_t* out_body_size) {
    if (buffer_size < sizeof(SkipHeader)) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    SkipConfig* config = (SkipConfig*)cfg;
    uint64_t body_size = skip_get_export_header_body_size(config);

    SkipHeader header;
    header.magic = SKIP_MAGIC;
    header.version = SKIP_CONFIG_VERSION;
    header.body_size = body_size;
    header.endian = config->endian;
    memset(header.reserved, 0, sizeof(header.reserved));

    if (skip_get_system_endian() != config->endian) {
        header.magic = swap_uint32(header.magic);
        header.version = swap_uint32(header.version);
        header.body_size = swap_uint64(header.body_size);
    }

    memcpy(buffer, &header, sizeof(SkipHeader));
    *out_body_size = body_size;

    return SKIP_SUCCESS;
}

void* skip_import_header(const char* buffer, uint64_t buffer_size, uint64_t* out_body_size) {
    if (buffer_size < sizeof(SkipHeader)) {
        return NULL;
    }

    SkipHeader header;
    memcpy(&header, buffer, sizeof(SkipHeader));

    if (skip_get_system_endian() != header.endian) {
        header.magic = swap_uint32(header.magic);
        header.version = swap_uint32(header.version);
        header.body_size = swap_uint64(header.body_size);
    }

    if (header.magic != SKIP_MAGIC || header.version != SKIP_CONFIG_VERSION) {
        return NULL;
    }

    void* config_ptr = skip_create_base_config();
    if (!config_ptr) {
        return NULL;
    }

    skip_set_endian_value_cfg(config_ptr, header.endian);
    *out_body_size = header.body_size;

    return config_ptr;
}

int skip_pop_type_from_config(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (config->types_size > 0) {
        config->types_size--;
        config->offsets_size--;
    }
    return SKIP_SUCCESS;
}

SkipInternalType* skip_get_type_at_index(void* cfg, uint64_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return NULL;
    return &config->types[index];
}

int skip_free_cfg(void* cfg) {
    if (cfg) {
        SkipConfig* config = (SkipConfig*)cfg;
        free(config->types);
        free(config->offsets);
        free(config);
    }
    return SKIP_SUCCESS;
}

uint64_t skip_get_datatype_size(int32_t type_code) {
    switch (type_code) {
        case skip_int8: return 1;
        case skip_uint8: return 1;
        case skip_int16: return 2;
        case skip_uint16: return 2;
        case skip_int32: return 4;
        case skip_uint32: return 4;
        case skip_int64: return 8;
        case skip_uint64: return 8;
        case skip_float32: return 4;
        case skip_float64: return 8;
        case skip_nest:
        case skip_char: return 1;
        default: return 0;
    }
}

uint64_t skip_get_data_size(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (config->offsets_size == 0) return 0;
    return config->offsets[config->offsets_size - 1];
}

int skip_write_index_to_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return SKIP_ERROR_OUT_OF_BOUNDS;

    uint64_t offset = config->offsets[index];
    uint64_t type_size = skip_get_datatype_size(config->types[index].type_code);
    uint64_t count = config->types[index].count;

    if (offset + (type_size * count) > buffer_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    int system_endian = skip_get_system_endian();
    int config_endian = config->endian;

    if (type_size == 1 || system_endian == config_endian) {
        memcpy((uint8_t*)buffer + offset, value, (size_t)(count * type_size));
    } else {
        uint8_t* dst_ptr = (uint8_t*)buffer + offset;
        uint8_t* src_ptr = (uint8_t*)value;
        for (uint64_t i = 0; i < count; ++i) {
            switch (type_size) {
                case 2: {
                    uint16_t val = swap_uint16(*(uint16_t*)src_ptr);
                    memcpy(dst_ptr, &val, 2);
                    break;
                }
                case 4: {
                    uint32_t val = swap_uint32(*(uint32_t*)src_ptr);
                    memcpy(dst_ptr, &val, 4);
                    break;
                }
                case 8: {
                    uint64_t val = swap_uint64(*(uint64_t*)src_ptr);
                    memcpy(dst_ptr, &val, 8);
                    break;
                }
            }
            src_ptr += type_size;
            dst_ptr += type_size;
        }
    }

    return SKIP_SUCCESS;
}

uint64_t skip_get_export_header_body_size(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    return config->types_size * (sizeof(int32_t) + sizeof(uint64_t));
}

int skip_export_header_body(void* cfg, char* buffer, uint64_t buffer_size) {
    SkipConfig* config = (SkipConfig*)cfg;
    uint64_t required_size = skip_get_export_header_body_size(cfg);
    if (buffer_size < required_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    char* current_pos = buffer;
    for (uint64_t i = 0; i < config->types_size; ++i) {
        int32_t type_code = config->types[i].type_code;
        uint64_t count = config->types[i].count;

        if (skip_get_system_endian() != config->endian) {
            type_code = swap_uint32(type_code);
            count = swap_uint64(count);
        }

        memcpy(current_pos, &type_code, sizeof(int32_t));
        current_pos += sizeof(int32_t);
        memcpy(current_pos, &count, sizeof(uint64_t));
        current_pos += sizeof(uint64_t);
    }

    return SKIP_SUCCESS;
}

int skip_import_header_body(void* cfg, const char* buffer, uint64_t buffer_size) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (!config) {
        return SKIP_ERROR_INVALID_ARGUMENT;
    }

    const char* current_pos = buffer;
    const char* end_pos = buffer + buffer_size;

    if (buffer_size % (sizeof(int32_t) + sizeof(uint64_t)) {
        return SKIP_ERROR_INVALID_CONFIG;
    }
    
    int system_endian = skip_get_system_endian();
    int config_endian = config->endian;

    while (current_pos < end_pos) {
        int32_t type_code;
        uint64_t count;

        memcpy(&type_code, current_pos, sizeof(int32_t));
        current_pos += sizeof(int32_t);
        memcpy(&count, current_pos, sizeof(uint64_t));
        current_pos += sizeof(uint64_t);

        if (system_endian != config_endian) {
            type_code = swap_uint32(type_code);
            count = swap_uint64(count);
        }

        if (skip_push_type_to_config(cfg, type_code, count) != SKIP_SUCCESS) {
            return SKIP_ERROR_INVALID_CONFIG;
        }
    }

    return SKIP_SUCCESS;
}

void* skip_get_index_ptr(void* cfg, void* buffer, uint64_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return NULL;
    uint64_t offset = config->offsets[index];
    return (uint8_t*)buffer + offset;
}

int skip_read_index_from_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return SKIP_ERROR_OUT_OF_BOUNDS;

    uint64_t offset = config->offsets[index];
    uint64_t type_size = skip_get_datatype_size(config->types[index].type_code);
    uint64_t count = config->types[index].count;

    if (offset + (type_size * count) > buffer_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    int system_endian = skip_get_system_endian();
    int config_endian = config->endian;

    if (type_size == 1 || system_endian == config_endian) {
        memcpy(value, (uint8_t*)buffer + offset, (size_t)(count * type_size));
    } else {
        uint8_t* dst_ptr = (uint8_t*)value;
        uint8_t* src_ptr = (uint8_t*)buffer + offset;
        for (uint64_t i = 0; i < count; ++i) {
            switch (type_size) {
                case 2: {
                    uint16_t val;
                    memcpy(&val, src_ptr, 2);
                    *(uint16_t*)dst_ptr = swap_uint16(val);
                    break;
                }
                case 4: {
                    uint32_t val;
                    memcpy(&val, src_ptr, 4);
                    *(uint32_t*)dst_ptr = swap_uint32(val);
                    break;
                }
                case 8: {
                    uint64_t val;
                    memcpy(&val, src_ptr, 8);
                    *(uint64_t*)dst_ptr = swap_uint64(val);
                    break;
                }
            }
            src_ptr += type_size;
            dst_ptr += type_size;
        }
    }

    return SKIP_SUCCESS;
}


int skip_create_nest_buffer(void* cfg, void* final_res, uint64_t final_res_size, void* data_buffer, uint64_t data_size) {
    uint64_t header_body_size = skip_get_export_header_body_size(cfg);
    if (final_res_size < header_body_size + data_size + sizeof(uint64_t)) {
        return (int)SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    SkipConfig* config = (SkipConfig*)cfg;
    uint64_t meta_size_to_write = header_body_size;

    if (skip_get_system_endian() != config->endian) {
        meta_size_to_write = swap_uint64(meta_size_to_write);
    }


    void* header_body_buffer = malloc(header_body_size);
    if (!header_body_buffer) {
         return SKIP_ERROR_ALLOCATION_FAILED;
    }
    int err = skip_export_header_body(cfg, header_body_buffer, header_body_size);

    if (err != SKIP_SUCCESS) {
        free(header_body_buffer);
        return err;
    }

    memcpy(final_res, &meta_size_to_write, sizeof(uint64_t));
    memcpy((uint8_t*)final_res + sizeof(uint64_t), header_body_buffer, header_body_size);
    memcpy((uint8_t*)final_res + sizeof(uint64_t) + header_body_size, data_buffer, data_size);

    free(header_body_buffer);
    return (int)SKIP_SUCCESS;
}

int skip_get_nest_cfg(void* cfg, void* nest_base_cfg, void* nest_buffer, uint64_t nest_size) {
    SkipConfig* parent_config = (SkipConfig*)cfg;
    uint64_t meta_size;
    memcpy(&meta_size, nest_buffer, sizeof(uint64_t));

    if (skip_get_system_endian() != parent_config->endian) {
        meta_size = swap_uint64(meta_size);
    }

    if (nest_size < meta_size + sizeof(uint64_t)) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    void* new_buffer_start = (uint8_t*)nest_buffer + sizeof(uint64_t);

    int err = skip_set_endian_value_cfg(nest_base_cfg, skip_get_cfg_endian(cfg));
    if (err != SKIP_SUCCESS) {
        return err;
    }

    err = skip_import_header_body(nest_base_cfg, new_buffer_start, meta_size);
    if (err != SKIP_SUCCESS) {
        return err;
    }

    return SKIP_SUCCESS;
}

int skip_get_nested_data_buffer(void* cfg, void* nest_buffer, uint64_t nest_size, void* data_buffer, uint64_t data_size) {
    SkipConfig* parent_config = (SkipConfig*)cfg;
    uint64_t meta_size;
    memcpy(&meta_size, nest_buffer, sizeof(uint64_t));

    if (skip_get_system_endian() != parent_config->endian) {
        meta_size = swap_uint64(meta_size);
    }

    if (nest_size < meta_size + sizeof(uint64_t)) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    uint64_t actual_data_size = nest_size - meta_size - sizeof(uint64_t);

    if (actual_data_size > data_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(data_buffer, (uint8_t*)nest_buffer + meta_size + sizeof(uint64_t), actual_data_size);

    return SKIP_SUCCESS;
}

uint64_t skip_export_standalone_size(void* cfg) {
    return skip_get_header_export_size() + skip_get_export_header_body_size(cfg) + skip_get_data_size(cfg);
}

int skip_export_standalone(void* cfg, void* data_buffer, uint64_t data_size, void* standalone_buffer, uint64_t standalone_size) {
    uint64_t header_size = skip_get_header_export_size();
    uint64_t header_body_size;
    
    int err = skip_export_header(cfg, standalone_buffer, header_size, &header_body_size);
    if (err != SKIP_SUCCESS) {
        return err;
    }

    void* header_body_mem = malloc(header_body_size);
    if (!header_body_mem) {
        return SKIP_ERROR_ALLOCATION_FAILED;
    }

    err = skip_export_header_body(cfg, header_body_mem, header_body_size);
    if (err == SKIP_SUCCESS) {
        memcpy((uint8_t*)standalone_buffer + header_size, header_body_mem, header_body_size);
        memcpy((uint8_t*)standalone_buffer + header_size + header_body_size, data_buffer, data_size);
    }

    free(header_body_mem);
    return err;
}

int skip_import_standalone_get_cfg(void** out_cfg, void* buffer, uint64_t buffer_size) {
    uint64_t header_body_size;
    *out_cfg = skip_import_header(buffer, buffer_size, &header_body_size);
    if (!*out_cfg) {
        return SKIP_ERROR_INVALID_CONFIG;
    }

    void* new_buffer = (uint8_t*)buffer + skip_get_header_export_size();
    int err = skip_import_header_body(*out_cfg, new_buffer, header_body_size);
    if (err != SKIP_SUCCESS) {
        skip_free_cfg(*out_cfg);
        *out_cfg = NULL;
        return err;
    }

    return SKIP_SUCCESS;
}


int skip_import_standalone_get_data_buffer(void* cfg , void* buffer , uint64_t buffer_size , void* data_buffer , uint64_t data_buffer_size) {
    uint64_t header_size = skip_get_header_export_size();
    uint64_t header_body_size = skip_get_export_header_body_size(cfg);
    uint64_t body_size = skip_get_data_size(cfg);
    
    if (body_size > data_buffer_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    void* new_buffer = (uint8_t*) buffer + header_size + header_body_size;

    memcpy(data_buffer, new_buffer, body_size);

    return SKIP_SUCCESS;
}
