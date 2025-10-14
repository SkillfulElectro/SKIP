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
- **Portability:** SKIP can be used with any programming language that can call C functions.
- **Security:** As long as the SKIP config is not exposed, there is no way to decode the underlying data.

> being sure about the buffer sizes you pass to SKIP functions is your responsibility ⚠️ 
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

This will create a `libskip.so` shared library and a `main` executable in the `build` directory.

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

#### `uint64_t skip_get_cfg_size(void* cfg)`

Calculates the total size in bytes required for a buffer based on the current configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** The total buffer size in bytes.

#### `uint64_t skip_get_datatype_size(int32_t type_code)`

Gets the size in bytes of a single element of a given data type.

- **Parameters:**
    - `type_code`: The data type code from `SkipDataTypeCode`.
- **Returns:** The size of the data type in bytes.

#### `int skip_write_index_to_buffer(void* cfg, void* buffer, void* value, uint64_t index)`

Writes data to a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the destination buffer.
    - `value`: A pointer to the data to be written.
    - `index`: The index in the config that specifies where and how to write the data.
- **Returns:** `0` on success, `-1` if the index is out of bounds.

#### `int skip_read_index_from_buffer(void* cfg, void* buffer, void* value, uint64_t index)`

Reads data from a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the source buffer.
    - `value`: A pointer to the destination variable where the data will be read.
    - `index`: The index in the config that specifies where and how to read the data.
- **Returns:** `0` on success, `-1` if the index is out of bounds.

#### `void* skip_get_index_ptr(void* cfg, void* buffer, uint64_t index)`

Retrieves a direct pointer to the start of the data for a given index within the buffer. This is useful for in-place access to data without needing a separate copy.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the source buffer.
    - `index`: The index in the config that specifies which data segment to point to.
- **Returns:** A `void*` pointer to the data segment, or `nullptr` if the index is out of bounds.

#### `uint64_t skip_get_export_buffer_size(void* cfg)`

Calculates the size of the buffer needed to export the configuration.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
- **Returns:** The required buffer size in bytes.

#### `int skip_export_cfg(void* cfg, char* buffer, uint64_t buffer_size)`

Exports the SKIP configuration to a buffer. This allows you to save the configuration and reuse it later, or send it over a network.

- **Parameters:**
  - `cfg`: A pointer to the SKIP config.
  - `buffer`: The buffer to write the configuration to.
  - `buffer_size`: The size of the buffer.
- **Returns:** `0` on success, `-1` if the buffer is too small.

#### `void* skip_import_cfg(const char* buffer)`

Imports a SKIP configuration from a buffer.

- **Parameters:**
  - `buffer`: The buffer to read the configuration from.
- **Returns:** A pointer to a new SKIP config, or `nullptr` on failure.

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
    size_t buffer_size = skip_get_cfg_size(config);
    char* buffer = new char[buffer_size];

    // 4. Prepare the data to be serialized.
    int32_t year = 2024;
    double pi = 3.14159265359;

    // 5. Write the data to the buffer.
    skip_write_index_to_buffer(config, buffer, (void*)message, 0);
    skip_write_index_to_buffer(config, buffer, &year, 1);
    skip_write_index_to_buffer(config, buffer, &pi, 2);

    // 6. Use skip_get_index_ptr for direct, in-place access.
    char* message_ptr = (char*)skip_get_index_ptr(config, buffer, 0);
    int32_t* year_ptr = (int32_t*)skip_get_index_ptr(config, buffer, 1);

    // 7. Alternatively, read data into a separate variable.
    double pi_copy;
    skip_read_index_from_buffer(config, buffer, &pi_copy, 2);

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
