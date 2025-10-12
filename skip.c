#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "skip.h"

typedef struct {
    SkipInternalType* types;
    size_t types_size;
    size_t types_capacity;

    size_t* offsets;
    size_t offsets_size;
    size_t offsets_capacity;
} SkipConfig;

static int ensure_capacity(void** array, size_t* capacity, size_t element_size, size_t new_capacity) {
    if (*capacity < new_capacity) {
        void* new_array = realloc(*array, new_capacity * element_size);
        if (!new_array) {
            return -1; // Allocation failed
        }
        *array = new_array;
        *capacity = new_capacity;
    }
    return 0;
}

void* skip_create_base_config() {
    SkipConfig* config = (SkipConfig*)malloc(sizeof(SkipConfig));
    if (!config) return NULL;

    config->types = NULL;
    config->types_size = 0;
    config->types_capacity = 0;

    config->offsets = NULL;
    config->offsets_size = 0;
    config->offsets_capacity = 0;

    if (ensure_capacity((void**)&config->offsets, &config->offsets_capacity, sizeof(size_t), 1) != 0) {
        free(config);
        return NULL;
    }
    config->offsets[0] = 0;
    config->offsets_size = 1;

    return config;
}

int skip_push_type_to_config(void* cfg, int type_code, size_t count) {
    SkipConfig* config = (SkipConfig*)cfg;

    if (config->types_size == config->types_capacity) {
        size_t new_cap = config->types_capacity == 0 ? 8 : config->types_capacity * 2;
        if (ensure_capacity((void**)&config->types, &config->types_capacity, sizeof(SkipInternalType), new_cap) != 0) return -1;
    }

    if (config->offsets_size == config->offsets_capacity) {
        size_t new_cap = config->offsets_capacity == 0 ? 8 : config->offsets_capacity * 2;
        if (ensure_capacity((void**)&config->offsets, &config->offsets_capacity, sizeof(size_t), new_cap) != 0) return -1;
    }

    size_t type_size = skip_get_datatype_size(type_code);
    size_t new_offset = config->offsets[config->offsets_size - 1] + (type_size * count);

    config->types[config->types_size].type_code = type_code;
    config->types[config->types_size].count = count;
    config->types_size++;

    config->offsets[config->offsets_size] = new_offset;
    config->offsets_size++;

    return 0;
}

int skip_pop_type_from_config(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (config->types_size > 0) {
        config->types_size--;
        config->offsets_size--;
    }
    return 0;
}

SkipInternalType* skip_get_type_at_index(void* cfg, size_t index) {
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
    return 0;
}

size_t skip_get_datatype_size(int type_code) {
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

size_t skip_get_cfg_size(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (config->offsets_size == 0) return 0;
    return config->offsets[config->offsets_size - 1];
}

int skip_write_index_to_buffer(void* cfg, void* buffer, void* value, size_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return -1;

    size_t offset = config->offsets[index];
    size_t type_size = skip_get_datatype_size(config->types[index].type_code);
    size_t value_len = type_size * config->types[index].count;
    memcpy((uint8_t*)buffer + offset, value, value_len);
    return 0;
}

size_t skip_get_export_buffer_size(void* cfg) {
    SkipConfig* config = (SkipConfig*)cfg;
    return sizeof(size_t) + (config->types_size * sizeof(SkipInternalType));
}

int skip_export_cfg(void* cfg, char* buffer, size_t buffer_size) {
    SkipConfig* config = (SkipConfig*)cfg;
    size_t required_size = skip_get_export_buffer_size(cfg);
    if (buffer_size < required_size) return -1;

    size_t num_types = config->types_size;
    *(size_t*)buffer = num_types;
    memcpy(buffer + sizeof(size_t), config->types, num_types * sizeof(SkipInternalType));
    return 0;
}

void* skip_import_cfg(const char* buffer) {
    size_t num_types = *(const size_t*)buffer;
    void* config_ptr = skip_create_base_config();
    if (!config_ptr) return NULL;
    SkipConfig* config = (SkipConfig*)config_ptr;

    // A bit of a hack to resize and copy
    free(config->types); // free the initial null
    config->types_capacity = num_types;
    config->types_size = num_types;
    config->types = (SkipInternalType*)malloc(num_types * sizeof(SkipInternalType));
    if (!config->types) {
        skip_free_cfg(config);
        return NULL;
    }
    memcpy(config->types, buffer + sizeof(size_t), num_types * sizeof(SkipInternalType));
    
    // Rebuild offsets
    for (size_t i = 0; i < num_types; ++i) {
        size_t type_size = skip_get_datatype_size(config->types[i].type_code);
        size_t count = config->types[i].count;
        size_t new_offset = config->offsets[config->offsets_size - 1] + (type_size * count);
        
        if (config->offsets_size == config->offsets_capacity) {
            size_t new_cap = config->offsets_capacity * 2;
            if(ensure_capacity((void**)&config->offsets, &config->offsets_capacity, sizeof(size_t), new_cap) != 0) {
                skip_free_cfg(config);
                return NULL;
            }
        }
        config->offsets[config->offsets_size] = new_offset;
        config->offsets_size++;
    }

    return config;
}

void* skip_get_index_ptr(void* cfg, void* buffer, size_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return NULL;
    size_t offset = config->offsets[index];
    return (uint8_t*)buffer + offset;
}

int skip_read_index_from_buffer(void* cfg, void* buffer, void* value, size_t index) {
    SkipConfig* config = (SkipConfig*)cfg;
    if (index >= config->types_size) return -1;
    size_t offset = config->offsets[index];
    size_t type_size = skip_get_datatype_size(config->types[index].type_code);
    size_t value_len = type_size * config->types[index].count;
    memcpy(value, (uint8_t*)buffer + offset, value_len);
    return 0;
}
