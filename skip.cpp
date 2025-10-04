#include <vector>
#include <cstdint>
#include <cstring>
#include "skip.h"

struct SkipConfig {
    std::vector<SkipInternalType> types;
    std::vector<size_t> offsets;
};

void* skip_create_base_config() {
    SkipConfig* config = new SkipConfig{};
    
    config->offsets.push_back(0);  // Initial offset
    
    return config;
}

int skip_push_type_to_config(void* cfg, int type_code, size_t count) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    size_t type_size = skip_get_datatype_size(type_code);
    size_t new_offset = config->offsets.back() + (type_size * count);
    
    config->types.push_back({type_code, count});
    config->offsets.push_back(new_offset);

    return 0;
}

int skip_pop_type_from_config(void* cfg) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    if (!config->types.empty()) {
        config->types.pop_back();
        config->offsets.pop_back();
    }

    return 0;
}

SkipInternalType* skip_get_type_at_index(void* cfg, size_t index) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    if (index >= config->types.size()) {
        return nullptr;
    }

    return &config->types[index];
}

int skip_free_cfg(void* cfg) {
    delete static_cast<SkipConfig*>(cfg);
    return 0;
}

size_t skip_get_datatype_size(int type_code) {
    switch (type_code) {
    case SkipDataTypeCode::skip_int8:
        return 1;
    case SkipDataTypeCode::skip_uint8:
        return 1;
    case SkipDataTypeCode::skip_int16:
        return 2;
    case SkipDataTypeCode::skip_uint16:
        return 2;
    case SkipDataTypeCode::skip_int32:
        return 4;
    case SkipDataTypeCode::skip_uint32:
        return 4;
    case SkipDataTypeCode::skip_int64:
        return 8;
    case SkipDataTypeCode::skip_uint64:
        return 8;
    case SkipDataTypeCode::skip_float32:
        return 4;
    case SkipDataTypeCode::skip_float64:
        return 8;
    case SkipDataTypeCode::skip_char:
        return 1;
    default:
        return 0;
    }
}

size_t skip_get_cfg_size(void* cfg) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    return config->offsets.back();
}

int skip_write_index_to_buffer(void* cfg, void* buffer, void* value, size_t index) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    if (index >= config->types.size()) {
        return -1;
    }

    size_t offset = config->offsets[index];
    size_t type_size = skip_get_datatype_size(config->types[index].type_code);

    size_t value_len = type_size * config->types[index].count;
    std::memcpy(static_cast<uint8_t*>(buffer) + offset, value, value_len);
    
    return 0;
}

void* skip_get_index_ptr(void* cfg, void* buffer, size_t index) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    if (index >= config->types.size()) {
        return nullptr;
    }

    size_t offset = config->offsets[index];
    return static_cast<uint8_t*>(buffer) + offset;
}

int skip_read_index_from_buffer(void* cfg, void* buffer, void* value, size_t index) {
    SkipConfig* config = static_cast<SkipConfig*>(cfg);
    
    if (index >= config->types.size()) {
        return -1;
    }

    size_t offset = config->offsets[index];
    size_t type_size = skip_get_datatype_size(config->types[index].type_code);

    size_t value_len = type_size * config->types[index].count;
    std::memcpy(value, static_cast<uint8_t*>(buffer) + offset, value_len);
    
    return 0;
}
