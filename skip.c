#include <stdlib.h>
#include <stdint.h>
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

static uint64_t host_to_network_uint64(uint64_t host_long_long) {
    if (is_little_endian()) {
        return swap_uint64(host_long_long);
    }
    return host_long_long;
}

static uint32_t host_to_network_uint32(uint32_t host_long) {
    if (is_little_endian()) {
        return swap_uint32(host_long);
    }
    return host_long;
}

static uint16_t host_to_network_uint16(uint16_t host_short) {
    if (is_little_endian()) {
        return swap_uint16(host_short);
    }
    return host_short;
}

static uint64_t network_to_host_uint64(uint64_t network_long_long) {
    return host_to_network_uint64(network_long_long);
}

static uint32_t network_to_host_uint32(uint32_t network_long) {
    return host_to_network_uint32(network_long);
}

static uint16_t network_to_host_uint16(uint16_t network_short) {
    return host_to_network_uint16(network_short);
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

    config->endian = SKIP_LITTLE_ENDIAN;

    return config;
}

int skip_get_system_endian() {
    return is_little_endian() ? SKIP_LITTLE_ENDIAN : SKIP_BIG_ENDIAN;
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
    uint64_t body_size = skip_get_export_body_size(config);

    SkipHeader header;
    header.magic = host_to_network_uint32(SKIP_MAGIC);
    header.version = host_to_network_uint32(SKIP_CONFIG_VERSION);
    header.body_size = host_to_network_uint64(body_size);
    header.endian = config->endian;
    memset(header.reserved, 0, sizeof(header.reserved));

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
    header.magic = network_to_host_uint32(header.magic);
    header.version = network_to_host_uint32(header.version);
    header.body_size = network_to_host_uint64(header.body_size);

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
        case skip_char: return 1;
        default: return 0;
    }
}

uint64_t skip_get_cfg_size(void* cfg) {
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
    int32_t type_code = config->types[index].type_code;

    if (offset + (type_size * count) > buffer_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    int system_endian = skip_get_system_endian();
    int config_endian = config->endian;

    if (type_size == 1 || system_endian == config_endian) {
        memcpy((uint8_t*)buffer + offset, value, (size_t)(count * type_size));
        return SKIP_SUCCESS;
    }

    uint8_t* current_val_ptr = (uint8_t*)value;
    for (uint64_t i = 0; i < count; ++i) {
        uint64_t current_offset = offset + (i * type_size);
        switch (type_code) {
            case skip_int16:
            case skip_uint16: {
                uint16_t val = host_to_network_uint16(*(uint16_t*)current_val_ptr);
                memcpy((uint8_t*)buffer + current_offset, &val, sizeof(uint16_t));
                break;
            }
            case skip_int32:
            case skip_uint32:
            case skip_float32: {
                uint32_t val = host_to_network_uint32(*(uint32_t*)current_val_ptr);
                memcpy((uint8_t*)buffer + current_offset, &val, sizeof(uint32_t));
                break;
            }
            case skip_int64:
            case skip_uint64:
            case skip_float64: {
                uint64_t val = host_to_network_uint64(*(uint64_t*)current_val_ptr);
                memcpy((uint8_t*)buffer + current_offset, &val, sizeof(uint64_t));
                break;
            }
        }
        current_val_ptr += type_size;
    }

    return SKIP_SUCCESS;
}

uint64_t skip_get_export_body_size(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    return config->types_size * (sizeof(int32_t) + sizeof(uint64_t));
}

int skip_export_cfg_body(void* cfg, char* buffer, uint64_t buffer_size) {
    SkipConfig* config = (SkipConfig*)cfg;
    uint64_t required_size = skip_get_export_body_size(cfg);
    if (buffer_size < required_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    char* current_pos = buffer;
    for (uint64_t i = 0; i < config->types_size; ++i) {
        int32_t type_code = host_to_network_uint32(config->types[i].type_code);
        memcpy(current_pos, &type_code, sizeof(int32_t));
        current_pos += sizeof(int32_t);

        uint64_t count = host_to_network_uint64(config->types[i].count);
        memcpy(current_pos, &count, sizeof(uint64_t));
        current_pos += sizeof(uint64_t);
    }

    return SKIP_SUCCESS;
}

int skip_import_cfg_body(void* cfg, const char* buffer, uint64_t buffer_size) {
    if (!cfg) {
        return SKIP_ERROR_INVALID_ARGUMENT;
    }

    const char* current_pos = buffer;
    const char* end_pos = buffer + buffer_size;

    while (current_pos < end_pos) {
        int32_t type_code_net;
        memcpy(&type_code_net, current_pos, sizeof(int32_t));
        int32_t type_code = network_to_host_uint32(type_code_net);
        current_pos += sizeof(int32_t);

        uint64_t count_net;
        memcpy(&count_net, current_pos, sizeof(uint64_t));
        uint64_t count = network_to_host_uint64(count_net);
        current_pos += sizeof(uint64_t);

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
    int32_t type_code = config->types[index].type_code;

    if (offset + (type_size * count) > buffer_size) {
        return SKIP_ERROR_BUFFER_TOO_SMALL;
    }

    int system_endian = skip_get_system_endian();
    int config_endian = config->endian;

    if (type_size == 1 || system_endian == config_endian) {
        memcpy(value, (uint8_t*)buffer + offset, (size_t)(count * type_size));
        return SKIP_SUCCESS;
    }
    
    uint8_t* current_val_ptr = (uint8_t*)value;
    for (uint64_t i = 0; i < count; ++i) {
        uint64_t current_offset = offset + (i * type_size);
        switch (type_code) {
            case skip_int16:
            case skip_uint16: {
                uint16_t net_val;
                memcpy(&net_val, (uint8_t*)buffer + current_offset, sizeof(uint16_t));
                *(uint16_t*)current_val_ptr = network_to_host_uint16(net_val);
                break;
            }
            case skip_int32:
            case skip_uint32:
            case skip_float32: {
                uint32_t net_val;
                memcpy(&net_val, (uint8_t*)buffer + current_offset, sizeof(uint32_t));
                *(uint32_t*)current_val_ptr = network_to_host_uint32(net_val);
                break;
            }
            case skip_int64:
            case skip_uint64:
            case skip_float64: {
                uint64_t net_val;
                memcpy(&net_val, (uint8_t*)buffer + current_offset, sizeof(uint64_t));
                *(uint64_t*)current_val_ptr = network_to_host_uint64(net_val);
                break;
            }
        }
        current_val_ptr += type_size;
    }

    return SKIP_SUCCESS;
}
