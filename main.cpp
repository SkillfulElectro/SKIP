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

    uint64_t buffer_size = skip_get_data_size(config);
    std::cout << "Buffer size: " << buffer_size << std::endl;
    assert(buffer_size == 1 + 2 + 4 + 8);

    char* buffer = new char[buffer_size];

    // --- Write data ---
    int8_t i8 = -120;
    uint16_t u16 = 65000;
    float f32 = 3.14f;
    int64_t i64 = -9000000000000000000;

    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &i8, 0) == SKIP_SUCCESS);
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &u16, 1) == SKIP_SUCCESS);
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &f32, 2) == SKIP_SUCCESS);
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &i64, 3) == SKIP_SUCCESS);

    // --- Read data back and verify ---
    int8_t i8_res;
    uint16_t u16_res;
    float f32_res;
    int64_t i64_res;

    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &i8_res, 0) == SKIP_SUCCESS);
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &u16_res, 1) == SKIP_SUCCESS);
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &f32_res, 2) == SKIP_SUCCESS);
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &i64_res, 3) == SKIP_SUCCESS);

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

    uint64_t buffer_size = skip_get_data_size(config);
    assert(buffer_size == 1);

    char* buffer = new char[buffer_size];

    char c = 'A';
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &c, 0) == SKIP_SUCCESS);

    char c_res;
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &c_res, 0) == SKIP_SUCCESS);

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

    uint64_t buffer_size = skip_get_data_size(config);
     std::cout << "Buffer size: " << buffer_size << std::endl;
    assert(buffer_size == 13 + 4);

    char* buffer = new char[buffer_size];

    // --- Write data ---
    const char* message = "Hello World";
    int32_t number = 2024;
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, (void*)message, 0) == SKIP_SUCCESS);
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &number, 1) == SKIP_SUCCESS);


    // --- Use skip_get_index_ptr to check pointers ---
    char* msg_ptr = (char*)skip_get_index_ptr(config, buffer, 0);
    int32_t* num_ptr = (int32_t*)skip_get_index_ptr(config, buffer, 1);
    
    // Check that the pointers point to the correct offsets
    assert(msg_ptr == &buffer[0]);
    assert((char*)num_ptr == &buffer[13]);
    std::cout << "Pointer offsets are correct." << std::endl;

    // --- Verify content using pointers (where safe) and read functions ---
    // Direct string comparison is safe for char arrays
    std::cout << "Message from ptr: " << msg_ptr << " (expected " << message << ")" << std::endl;
    assert(strcmp(msg_ptr, message) == 0);
    
    // For multi-byte types, direct dereferencing is unsafe due to endianness.
    // Use the read function to verify the content was written correctly.
    int32_t number_read;
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &number_read, 1) == SKIP_SUCCESS);
    std::cout << "Number from read: " << number_read << " (expected " << number << ")" << std::endl;
    assert(number_read == number);

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

    // 1. Create and configure the original config object
    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_int32, 1);
    skip_push_type_to_config(config, skip_float64, 2);
    skip_push_type_to_config(config, skip_char, 10);

    // 2. Export the header
    uint64_t header_size = skip_get_header_export_size();
    char* header_buffer = new char[header_size];
    uint64_t exported_body_size;
    assert(skip_export_header(config, header_buffer, header_size, &exported_body_size) == SKIP_SUCCESS);

    // 3. Export the body
    uint64_t body_size = skip_get_export_body_size(config);
    char* body_buffer = new char[body_size];
    assert(skip_export_header_body(config, body_buffer, body_size) == SKIP_SUCCESS);
    assert(exported_body_size == body_size);

    // 4. Import the header
    uint64_t imported_body_size;
    void* imported_config = skip_import_header(header_buffer, header_size, &imported_body_size);
    assert(imported_config != NULL);
    assert(body_size == imported_body_size);

    // 5. Import the body
    assert(skip_import_header_body(imported_config, body_buffer, body_size) == SKIP_SUCCESS);

    // 6. Verify the imported config
    assert(skip_get_data_size(config) == skip_get_data_size(imported_config));
    SkipInternalType* original_type = skip_get_type_at_index(config, 1);
    SkipInternalType* imported_type = skip_get_type_at_index(imported_config, 1);
    assert(original_type->type_code == imported_type->type_code);
    assert(original_type->count == imported_type->count);
    std::cout << "Config sizes and types match." << std::endl;

    // 7. Cleanup
    skip_free_cfg(config);
    skip_free_cfg(imported_config);
    delete[] header_buffer;
    delete[] body_buffer;

    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}


void test_endianness() {
    std::cout << "--- Testing Endianness ---" << std::endl;

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_uint32, 1);

    uint64_t buffer_size = skip_get_data_size(config);
    char* buffer = new char[buffer_size];

    uint32_t original_value = 0x12345678;
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &original_value, 0) == SKIP_SUCCESS);

    // Manually check the byte order in the buffer (should be little-endian by default)
    assert((unsigned char)buffer[0] == 0x78);
    assert((unsigned char)buffer[1] == 0x56);
    assert((unsigned char)buffer[2] == 0x34);
    assert((unsigned char)buffer[3] == 0x12);
    std::cout << "Byte order in buffer is correct (little-endian)." << std::endl;

    uint32_t read_value;
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &read_value, 0) == SKIP_SUCCESS);

    std::cout << "Read value: " << std::hex << read_value << " (expected " << original_value << ")" << std::endl;
    assert(original_value == read_value);

    skip_free_cfg(config);
    delete[] buffer;
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

void test_error_handling() {
    std::cout << "--- Testing Error Handling ---" << std::endl;

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_int32, 1);

    uint64_t buffer_size = skip_get_data_size(config);
    char* buffer = new char[buffer_size];

    // Test writing to a buffer that is too small
    int32_t value = 123;
    assert(skip_write_index_to_buffer(config, buffer, buffer_size - 1, &value, 0) == SKIP_ERROR_BUFFER_TOO_SMALL);
    std::cout << "Buffer too small write test passed." << std::endl;

    // Test reading from a buffer that is too small
    assert(skip_read_index_from_buffer(config, buffer, buffer_size - 1, &value, 0) == SKIP_ERROR_BUFFER_TOO_SMALL);
    std::cout << "Buffer too small read test passed." << std::endl;

    // Test out of bounds access
    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &value, 1) == SKIP_ERROR_OUT_OF_BOUNDS);
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &value, 1) == SKIP_ERROR_OUT_OF_BOUNDS);
    std::cout << "Out of bounds access test passed." << std::endl;

    // Test exporting header to a buffer that is too small
    uint64_t header_size = skip_get_header_export_size();
    char* header_buffer = new char[header_size];
    uint64_t body_size;
    assert(skip_export_header(config, header_buffer, header_size - 1, &body_size) == SKIP_ERROR_BUFFER_TOO_SMALL);
    std::cout << "Export header buffer too small test passed." << std::endl;

    // Test exporting body to a buffer that is too small
    body_size = skip_get_export_body_size(config);
    char* body_buffer = new char[body_size];
    assert(skip_export_header_body(config, body_buffer, body_size - 1) == SKIP_ERROR_BUFFER_TOO_SMALL);
    std::cout << "Export body buffer too small test passed." << std::endl;

    // Test importing header from a buffer that is too small
    uint64_t imported_body_size;
    assert(skip_import_header(header_buffer, header_size - 1, &imported_body_size) == NULL);
    std::cout << "Import header buffer too small test passed." << std::endl;


    skip_free_cfg(config);
    delete[] buffer;
    delete[] header_buffer;
    delete[] body_buffer;
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

void test_endianness_config() {
    std::cout << "--- Testing Endianness Configuration ---" << std::endl;

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_uint32, 1);

    // Test setting to big-endian
    skip_set_endian_value_cfg(config, SKIP_BIG_ENDIAN);
    uint64_t buffer_size = skip_get_data_size(config);
    char* buffer = new char[buffer_size];
    uint32_t original_value = 0x12345678;

    assert(skip_write_index_to_buffer(config, buffer, buffer_size, &original_value, 0) == SKIP_SUCCESS);

    if (skip_get_system_endian() == SKIP_LITTLE_ENDIAN) {
        assert((unsigned char)buffer[0] == 0x12);
        assert((unsigned char)buffer[1] == 0x34);
        assert((unsigned char)buffer[2] == 0x56);
        assert((unsigned char)buffer[3] == 0x78);
        std::cout << "Big-endian write on little-endian system is correct." << std::endl;
    }

    uint32_t read_value;
    assert(skip_read_index_from_buffer(config, buffer, buffer_size, &read_value, 0) == SKIP_SUCCESS);
    assert(original_value == read_value);
    std::cout << "Big-endian read is correct." << std::endl;

    // Test import/export preserves endianness
    uint64_t header_size = skip_get_header_export_size();
    char* header_buffer = new char[header_size];
    uint64_t exported_body_size;
    assert(skip_export_header(config, header_buffer, header_size, &exported_body_size) == SKIP_SUCCESS);

    uint64_t body_size = skip_get_export_body_size(config);
    char* body_buffer = new char[body_size];
    assert(skip_export_header_body(config, body_buffer, body_size) == SKIP_SUCCESS);

    uint64_t imported_body_size;
    void* imported_config = skip_import_header(header_buffer, header_size, &imported_body_size);
    assert(imported_config != NULL);
    assert(skip_import_header_body(imported_config, body_buffer, body_size) == SKIP_SUCCESS);

    // Use the imported config to write a value and check if the byte order is correct.
    // This behaviorally verifies that the endianness setting was imported correctly.
    char* verification_buffer = new char[buffer_size];
    uint32_t verification_value = 0xAABBCCDD;
    assert(skip_write_index_to_buffer(imported_config, verification_buffer, buffer_size, &verification_value, 0) == SKIP_SUCCESS);

    if (skip_get_system_endian() == SKIP_LITTLE_ENDIAN) {
        assert((unsigned char)verification_buffer[0] == 0xAA);
        assert((unsigned char)verification_buffer[1] == 0xBB);
        assert((unsigned char)verification_buffer[2] == 0xCC);
        assert((unsigned char)verification_buffer[3] == 0xDD);
    }
    std::cout << "Import/export correctly preserves endianness setting." << std::endl;
    delete[] verification_buffer;

    delete[] buffer;
    delete[] header_buffer;
    delete[] body_buffer;
    skip_free_cfg(config);
    skip_free_cfg(imported_config);
    std::cout << "--- Test Passed ---" << std::endl << std::endl;
}

int main() {
    test_new_datatypes();
    test_get_index_ptr();
    test_endianness_config();
    test_char_type();
    test_import_export();
    test_endianness();
    test_error_handling();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
