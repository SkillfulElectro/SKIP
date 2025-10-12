#include <iostream>
#include <cstdint>
#include <cstring>
#include <cassert>
#include "skip.h"

void test_new_datatypes() {
    std::cout << "--- Testing New Data Types ---" << std::endl;

    void* config = skip_create_base_config();

    // Push various data types
    skip_push_type_to_config(config, skip_int8, 1);
    skip_push_type_to_config(config, skip_uint16, 1);
    skip_push_type_to_config(config, skip_float32, 1);
    skip_push_type_to_config(config, skip_int64, 1);

    size_t buffer_size = skip_get_cfg_size(config);
    std::cout << "Buffer size: " << buffer_size << std::endl;
    assert(buffer_size == 1 + 2 + 4 + 8);

    char* buffer = new char[buffer_size];

    // --- Write data ---
    int8_t i8 = -120;
    uint16_t u16 = 65000;
    float f32 = 3.14f;
    int64_t i64 = -9000000000000000000;

    skip_write_index_to_buffer(config, buffer, &i8, 0);
    skip_write_index_to_buffer(config, buffer, &u16, 1);
    skip_write_index_to_buffer(config, buffer, &f32, 2);
    skip_write_index_to_buffer(config, buffer, &i64, 3);

    // --- Read data back and verify ---
    int8_t i8_res;
    uint16_t u16_res;
    float f32_res;
    int64_t i64_res;

    skip_read_index_from_buffer(config, buffer, &i8_res, 0);
    skip_read_index_from_buffer(config, buffer, &u16_res, 1);
    skip_read_index_from_buffer(config, buffer, &f32_res, 2);
    skip_read_index_from_buffer(config, buffer, &i64_res, 3);

    std::cout << "int8: " << (int)i8_res << " (expected " << (int)i8 << ")" << std::endl;
    assert(i8 == i8_res);
    std::cout << "uint16: " << u16_res << " (expected " << u16 << ")" << std::endl;
    assert(u16 == u16_res);
    std::cout << "float32: " << f32_res << " (expected " << f32 << ")" << std::endl;
    assert(f32 == f32_res);
    std::cout << "int64: " << i64_res << " (expected " << i64 << ")" << std::endl;
    assert(i64 == i64_res);

    skip_free_cfg(config);
    delete[] buffer;
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

void test_char_type() {
    std::cout << "--- Testing skip_char ---" << std::endl;

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_char, 1);

    size_t buffer_size = skip_get_cfg_size(config);
    assert(buffer_size == 1);

    char* buffer = new char[buffer_size];

    char c = 'A';
    skip_write_index_to_buffer(config, buffer, &c, 0);

    char c_res;
    skip_read_index_from_buffer(config, buffer, &c_res, 0);

    std::cout << "char: " << c_res << " (expected " << c << ")" << std::endl;
    assert(c == c_res);

    skip_free_cfg(config);
    delete[] buffer;
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

void test_get_index_ptr() {
    std::cout << "--- Testing skip_get_index_ptr ---" << std::endl;

    void* config = skip_create_base_config();

    // Config: [char[13], int32_t]
    skip_push_type_to_config(config, skip_uint8, 13); // "Hello World"
    skip_push_type_to_config(config, skip_int32, 1);

    size_t buffer_size = skip_get_cfg_size(config);
     std::cout << "Buffer size: " << buffer_size << std::endl;
    assert(buffer_size == 13 + 4);

    char* buffer = new char[buffer_size];

    // --- Write data ---
    const char* message = "Hello World";
    int32_t number = 2024;
    skip_write_index_to_buffer(config, buffer, (void*)message, 0);
    skip_write_index_to_buffer(config, buffer, &number, 1);


    // --- Use skip_get_index_ptr to read data ---
    char* msg_ptr = (char*)skip_get_index_ptr(config, buffer, 0);
    int32_t* num_ptr = (int32_t*)skip_get_index_ptr(config, buffer, 1);

    std::cout << "Message from ptr: " << msg_ptr << " (expected " << message << ")" << std::endl;
    assert(strcmp(msg_ptr, message) == 0);
    std::cout << "Number from ptr: " << *num_ptr << " (expected " << number << ")" << std::endl;
    assert(*num_ptr == number);

    // --- Test out of bounds ---
    void* null_ptr = skip_get_index_ptr(config, buffer, 2);
    assert(null_ptr == nullptr);
    std::cout << "Out of bounds test passed." << std::endl;


    skip_free_cfg(config);
    delete[] buffer;
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

void test_import_export() {
    std::cout << "--- Testing Import/Export ---" << std::endl;

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_int32, 1);
    skip_push_type_to_config(config, skip_float64, 2);
    skip_push_type_to_config(config, skip_char, 10);

    size_t buffer_size = skip_get_export_buffer_size(config);
    char* exported_buffer = new char[buffer_size];
    int export_result = skip_export_cfg(config, exported_buffer, buffer_size);
    assert(export_result == 0);

    void* imported_config = skip_import_cfg(exported_buffer);

    assert(skip_get_cfg_size(config) == skip_get_cfg_size(imported_config));

    SkipInternalType* original_type = skip_get_type_at_index(config, 1);
    SkipInternalType* imported_type = skip_get_type_at_index(imported_config, 1);

    assert(original_type->type_code == imported_type->type_code);
    assert(original_type->count == imported_type->count);

    std::cout << "Config sizes and types match." << std::endl;

    skip_free_cfg(config);
    skip_free_cfg(imported_config);
    delete[] exported_buffer;

    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}


int main() {
    test_new_datatypes();
    test_get_index_ptr();
    test_char_type();
    test_import_export();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
