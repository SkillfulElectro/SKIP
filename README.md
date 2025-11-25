<p align="center">
  <img alt="SKIP Logo" src="skip_logo.png" style="width:auto;height:auto;">
</p>


<h1 align="center">SKIP : Simple Knockout Interaction Protocol</h1> 
SKIP is a simple, fast, and lightweight data serialization library. It provides a protocol for defining the structure of your data and then reading and writing that data to a buffer. This makes it easy to send and receive structured data over any network protocol, or to store it in a file.

The core of SKIP is the concept of a "config", which is a description of the data's layout. You define the types and sizes of the data fields, and SKIP handles the details of reading and writing those fields to and from a buffer.

## Key Features

- **Simplicity:** SKIP is designed to be easy to use, with a minimal API.
- **Speed:** SKIP is written in C++ and is designed for performance.
- **Lightweight:** The SKIP library is small and has no external dependencies.
- **Portability:** SKIP can be used with any programming language that can call C functions. The serialization format is platform-independent and supports both Big-Endian and Little-Endian byte orders.
- **Security:** As long as the SKIP config is not exposed, there is no way to decode the underlying data.
- **Versioning:** The exported configuration format includes a version number and endianness information, ensuring backward compatibility.
- **Configurable Endianness:** By default, SKIP uses the system's native endianness for serialization, but you can explicitly set it to either big-endian or little-endian, ensuring consistent data representation across different systems.

## Benchmarking

A benchmark was performed to compare the performance of SKIP against JSON and XML. The test involved serializing and deserializing a dataset containing 1000 integers, 1000 doubles, and 1000 strings.

| Format | Output Size (bytes) | Encoding Time (ms) | Decoding Time (ms) |
|--------|---------------------|--------------------|--------------------|
| SKIP   | 22890               | 0.094              | 0.200              |
| JSON   | 29358               | 1.466              | 6.778              |
| XML    | 76591               | 3.067              | 1.703              |

As the results show, SKIP is significantly faster and more lightweight than both JSON and XML.

## Building

To build the SKIP library and the example `main` executable, you will need to have `cmake` and `make` installed.

1.  Create a build directory:
    ```bash
    mkdir build
    ```
2.  Navigate to the build directory:
    ```bash
    cd build
    ```
3.  Run cmake to generate the Makefiles:
    ```bash
    cmake ..
    ```
4.  Run make to build the project:
    ```bash
    make
    ```

This will create a `libskip.so` shared library, a `tests` executable, and a `benchmark` executable in the `build` directory.

To run the tests, execute the following command from the `build` directory:
```bash
./tests
```

To run the benchmark, execute the following command from the `build` directory:
```bash
./benchmark
```

## API Reference

The SKIP C API provides a set of functions for creating and managing data serialization configurations, and for reading and writing data to buffers based on these configurations.

### Enums and Structs

#### `SkipDataTypeCode`

This enum defines the supported data types in SKIP.

```c
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
```

- `skip_int8`: 1-byte signed integer.
- `skip_uint8`: 1-byte unsigned integer (can be used for characters).
- `skip_int16`: 2-byte signed integer.
- `skip_uint16`: 2-byte unsigned integer.
- `skip_int32`: 4-byte signed integer.
- `skip_uint32`: 4-byte unsigned integer.
- `skip_int64`: 8-byte signed integer.
- `skip_uint64`: 8-byte unsigned integer.
- `skip_float32`: 4-byte single-precision float.
- `skip_float64`: 8-byte double-precision float.
- `skip_char`: 1-byte character.

#### `SkipEndian`

This enum defines the endianness options for data serialization.

```c
enum SkipEndian {
    SKIP_BIG_ENDIAN = 0,
    SKIP_LITTLE_ENDIAN = 1
};
```

#### `SkipError`

This enum defines the error codes returned by SKIP functions.

```c
enum SkipError {
    SKIP_SUCCESS = 0,
    SKIP_ERROR_INVALID_ARGUMENT = -1,
    SKIP_ERROR_OUT_OF_BOUNDS = -2,
    SKIP_ERROR_ALLOCATION_FAILED = -3,
    SKIP_ERROR_BUFFER_TOO_SMALL = -4,
    SKIP_ERROR_INVALID_CONFIG = -5,
};
```

- `SKIP_SUCCESS`: The operation completed successfully.
- `SKIP_ERROR_INVALID_ARGUMENT`: A function was called with an invalid argument.
- `SKIP_ERROR_OUT_OF_BOUNDS`: An out-of-bounds access was attempted.
- `SKIP_ERROR_ALLOCATION_FAILED`: A memory allocation failed.
- `SKIP_ERROR_BUFFER_TOO_SMALL`: The provided buffer was too small to complete the operation.
- `SKIP_ERROR_INVALID_CONFIG`: The provided configuration was invalid or corrupted.

#### `SkipInternalType`

This struct represents a type and its count within the SKIP configuration.

```c
struct SkipInternalType {
    int32_t type_code;
    uint64_t count;
};
```

- `type_code`: The data type, as defined by `SkipDataTypeCode`.
- `count`: The number of elements of this type.

### Functions

#### `void* skip_create_base_config()`

Creates a new, empty SKIP configuration.

- **Returns:** A pointer to the newly created config. This pointer should be freed using `skip_free_cfg` when it's no longer needed.

#### `int skip_push_type_to_config(void* cfg, int32_t type_code, uint64_t len)`

Adds a new data type entry to the end of the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `type_code`: The data type code from `SkipDataTypeCode`.
    - `len`: The number of elements of this type.
- **Returns:** `0` on success.

#### `int skip_pop_type_from_config(void* cfg)`

Removes the last data type entry from the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** `0` on success.

#### `SkipInternalType* skip_get_type_at_index(void* cfg, uint64_t index)`

Retrieves the type information at a specific index in the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `index`: The index of the type entry to retrieve.
- **Returns:** A pointer to a `SkipInternalType` struct, or `nullptr` if the index is out of bounds.

#### `int skip_free_cfg(void* cfg)`

Frees the memory used by a SKIP configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** `0` on success.

#### `uint64_t skip_get_data_size(void* cfg)`

Calculates the total size in bytes required for a buffer based on the current configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** The total buffer size in bytes.

#### `uint64_t skip_get_datatype_size(int32_t type_code)`

Gets the size in bytes of a single element of a given data type.

- **Parameters:**
    - `type_code`: The data type code from `SkipDataTypeCode`.
- **Returns:** The size of the data type in bytes.

#### `int skip_write_index_to_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index)`

Writes data to a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the destination buffer.
    - `buffer_size`: The size of the destination buffer.
    - `value`: A pointer to the data to be written.
    - `index`: The index in the config that specifies where and how to write the data.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_read_index_from_buffer(void* cfg, void* buffer, uint64_t buffer_size, void* value, uint64_t index)`

Reads data from a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the source buffer.
    - `buffer_size`: The size of the source buffer.
    - `value`: A pointer to the destination variable where the data will be read.
    - `index`: The index in the config that specifies where and how to read the data.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `void* skip_get_index_ptr(void* cfg, void* buffer, uint64_t index)`

Retrieves a direct pointer to the start of the data for a given index within the buffer. This is useful for in-place access to data without needing a separate copy.

> **Warning:** On architectures with strict memory alignment requirements, dereferencing the returned pointer for multi-byte types may cause performance issues or crashes. For portable and safe access, prefer using `skip_read_index_from_buffer`, which handles alignment and endianness correctly.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the source buffer.
    - `index`: The index in the config that specifies which data segment to point to.
- **Returns:** A `void*` pointer to the data segment, or `nullptr` if the index is out of bounds.

#### `uint64_t skip_get_header_export_size()`

Gets the fixed size of the configuration header.

- **Returns:** The size of the header in bytes (currently 32 bytes).

#### `int skip_export_header(void* cfg, char* buffer, uint64_t buffer_size, uint64_t* out_body_size)`

Exports the configuration header to a buffer. This function serializes the metadata about the configuration, including a magic number for identification, the format version, the size of the configuration body, and the endianness.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `buffer`: The buffer to write the header to.
  - `buffer_size`: The size of the buffer, which must be at least `skip_get_header_export_size()`.
  - `out_body_size`: A pointer to a `uint64_t` where the size of the configuration body will be stored.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `void* skip_import_header(const char* buffer, uint64_t buffer_size, uint64_t* out_body_size)`

Imports a configuration header from a buffer and creates a new SKIP config object. It reads the metadata, validates the magic number and version, and prepares a config object with the correct endianness.

- **Parameters:**
  - `buffer`: The buffer to read the header from.
  - `buffer_size`: The size of the buffer.
  - `out_body_size`: A pointer to a `uint64_t` where the size of the configuration body (as read from the header) will be stored. This tells you how large the buffer for the body needs to be.
- **Returns:** A pointer to a new SKIP config, or `nullptr` on failure (e.g., invalid magic number, version mismatch).

#### `uint64_t skip_get_export_body_size(void* cfg)`

Calculates the size of the buffer needed to export the configuration body (the actual type information).

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
- **Returns:** The required buffer size for the body in bytes.

#### `int skip_export_header_body(void* cfg, char* buffer, uint64_t buffer_size)`

Exports the SKIP configuration body to a buffer. This function should be called after `skip_export_header`.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `buffer`: The buffer to write the configuration body to.
  - `buffer_size`: The size of the buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_import_header_body(void* cfg, const char* buffer, uint64_t buffer_size)`

Imports a SKIP configuration body into a pre-existing config object. This function should be called after `skip_import_header`.

- **Parameters:**
  - `cfg`: A pointer to the config object created by `skip_import_header`.
  - `buffer`: The buffer to read the configuration body from.
  - `buffer_size`: The size of the buffer.
- **Returns:** `SKIP_SUCCESS` on success, or `SKIP_ERROR_INVALID_CONFIG` on failure.

#### `int skip_get_system_endian()`

Detects the endianness of the host machine.

- **Returns:** `SKIP_BIG_ENDIAN` or `SKIP_LITTLE_ENDIAN`.

#### `int skip_set_endian_value_cfg(void* cfg, int endian)`

Sets the desired endianness for the SKIP configuration. This will affect how multi-byte data is written to and read from buffers.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `endian`: The desired endianness, either `SKIP_BIG_ENDIAN` or `SKIP_LITTLE_ENDIAN`.
- **Returns:** `SKIP_SUCCESS` on success.

#### `int skip_get_cfg_endian(void* cfg)`

Gets the endianness of the SKIP configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** `SKIP_BIG_ENDIAN` or `SKIP_LITTLE_ENDIAN`.

### Nesting Functions

#### `int skip_create_nest_buffer(void* final_res, uint64_t final_res_size, void* meta_buffer, uint64_t meta_size, void* data_buffer, uint64_t data_size)`

Creates a nested buffer by combining a metadata buffer and a data buffer.

- **Parameters:**
  - `final_res`: A pointer to the destination buffer.
  - `final_res_size`: The size of the destination buffer.
  - `meta_buffer`: A pointer to the metadata buffer.
  - `meta_size`: The size of the metadata buffer.
  - `data_buffer`: A pointer to the data buffer.
  - `data_size`: The size of the data buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_get_nest_cfg(void* cfg, void* nest_base_cfg, void* nest_buffer, uint64_t nest_size)`

Extracts the configuration from a nested buffer.

- **Parameters:**
  - `cfg`: A pointer to the parent SKIP config.
  - `nest_base_cfg`: A pointer to the destination config for the nested data.
  - `nest_buffer`: A pointer to the nested buffer.
  - `nest_size`: The size of the nested buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_get_nested_data_buffer(void* nested_cfg_buffer, void* nest_buffer, uint64_t nest_size, void* data_buffer, uint64_t data_size)`

Extracts the data buffer from a nested buffer.

- **Parameters:**
  - `nested_cfg_buffer`: A pointer to the configuration of the nested buffer.
  - `nest_buffer`: A pointer to the nested buffer.
  - `nest_size`: The size of the nested buffer.
  - `data_buffer`: A pointer to the destination buffer for the data.
  - `data_size`: The size of the destination data buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

### Standalone Functions

#### `uint64_t skip_export_standalone_size(void* cfg)`

Calculates the total size required to export a standalone buffer (header + body + data).

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
- **Returns:** The required size in bytes.

#### `int skip_export_standalone(void* cfg, void* data_buffer, uint64_t data_size, void* standalone_buffer, uint64_t standalone_size)`

Exports a standalone buffer containing the header, body, and data.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `data_buffer`: A pointer to the data buffer.
  - `data_size`: The size of the data buffer.
  - `standalone_buffer`: A pointer to the destination buffer.
  - `standalone_size`: The size of the destination buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_fill_import_standalone_cfg(void* void_null_ptr, void* buffer, uint64_t buffer_size)`

Imports the configuration from a standalone buffer.

- **Parameters:**
  - `void_null_ptr`: A null pointer that will be populated with the new config.
  - `buffer`: A pointer to the standalone buffer.
  - `buffer_size`: The size of the standalone buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

#### `int skip_fill_data_buffer_import_standalone(void* cfg, void* buffer, uint64_t buffer_size, void* data_buffer, uint64_t data_buffer_size)`

Imports the data from a standalone buffer.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `buffer`: A pointer to the standalone buffer.
  - `buffer_size`: The size of the standalone buffer.
  - `data_buffer`: A pointer to the destination data buffer.
  - `data_buffer_size`: The size of the destination data buffer.
- **Returns:** `SKIP_SUCCESS` on success, or an error code on failure.

## Usage Example

Here is a simple example of how to use the SKIP library to serialize and deserialize a struct-like object with multiple data types.

```cpp
#include <iostream>
#include <cstdint>
#include <cstring>
#include "skip.h"

int main() {
    // 1. Create a new SKIP configuration.
    void* config = skip_create_base_config();

    // 2. Define the data structure: a string, an integer, and a double.
    const char* message = "Hello SKIP!";
    size_t message_len = strlen(message) + 1; // Include null terminator
    skip_push_type_to_config(config, skip_uint8, message_len);
    skip_push_type_to_config(config, skip_int32, 1);
    skip_push_type_to_config(config, skip_float64, 1);

    // 3. Allocate a buffer to hold the serialized data.
    size_t buffer_size = skip_get_data_size(config);
    char* buffer = new char[buffer_size];

    // 4. Prepare the data to be serialized.
    int32_t year = 2024;
    double pi = 3.14159265359;

    // 5. Write the data to the buffer.
    skip_write_index_to_buffer(config, buffer, buffer_size, (void*)message, 0);
    skip_write_index_to_buffer(config, buffer, buffer_size, &year, 1);
    skip_write_index_to_buffer(config, buffer, buffer_size, &pi, 2);

    // 6. Use skip_get_index_ptr for direct, in-place access.
    char* message_ptr = (char*)skip_get_index_ptr(config, buffer, 0);
    int32_t* year_ptr = (int32_t*)skip_get_index_ptr(config, buffer, 1);

    // 7. Alternatively, read data into a separate variable.
    double pi_copy;
    skip_read_index_from_buffer(config, buffer, buffer_size, &pi_copy, 2);

    // 8. Print the deserialized data.
    std::cout << "Buffer size: " << buffer_size << std::endl;
    std::cout << "Message (from ptr): " << message_ptr << std::endl;
    std::cout << "Year (from ptr): " << *year_ptr << std::endl;
    std::cout << "PI (from copy): " << pi_copy << std::endl;

    // 9. Clean up allocated memory.
    skip_free_cfg(config);
    delete[] buffer;

    return 0;
}
```

When you build and run the `main` executable, the output will be:

```
Buffer size: 24
Message (from ptr): Hello SKIP!
Year (from ptr): 2024
PI (from copy): 3.14159
```

## Contribution
- all contributions are welcomed :)
