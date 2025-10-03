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
    skip_int = 0,
    skip_double = 1,
    skip_char = 2,
    skip_size_t = 3,
};
```

- `skip_int`: Represents a 4-byte integer.
- `skip_double`: Represents an 8-byte double.
- `skip_char`: Represents a 1-byte character.
- `skip_size_t`: Represents an 8-byte size_t.

#### `SkipInternalType`

This struct represents a type and its count within the SKIP configuration.

```c
struct SkipInternalType {
    int type_code;
    size_t count;
};
```

- `type_code`: The data type, as defined by `SkipDataTypeCode`.
- `count`: The number of elements of this type.

### Functions

#### `void* skip_create_base_config()`

Creates a new, empty SKIP configuration.

- **Returns:** A pointer to the newly created config. This pointer should be freed using `skip_free_cfg` when it's no longer needed.

#### `int skip_push_type_to_config(void* cfg, int type_code, size_t len)`

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

#### `SkipInternalType* skip_get_type_at_index(void* cfg, size_t index)`

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

#### `size_t skip_get_cfg_size(void* cfg)`

Calculates the total size in bytes required for a buffer based on the current configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
- **Returns:** The total buffer size in bytes.

#### `size_t skip_get_datatype_size(int type_code)`

Gets the size in bytes of a single element of a given data type.

- **Parameters:**
    - `type_code`: The data type code from `SkipDataTypeCode`.
- **Returns:** The size of the data type in bytes.

#### `int skip_write_index_to_buffer(void* cfg, void* buffer, void* value, size_t index)`

Writes data to a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the destination buffer.
    - `value`: A pointer to the data to be written.
    - `index`: The index in the config that specifies where and how to write the data.
- **Returns:** `0` on success, `-1` if the index is out of bounds.

#### `int skip_read_index_from_buffer(void* cfg, void* buffer, void* value, size_t index)`

Reads data from a specific index in the buffer, according to the configuration.

- **Parameters:**
    - `cfg`: A pointer to the SKIP config.
    - `buffer`: A pointer to the source buffer.
    - `value`: A pointer to the destination variable where the data will be read.
    - `index`: The index in the config that specifies where and how to read the data.
- **Returns:** `0` on success, `-1` if the index is out of bounds.

## Usage Example

Here is a simple example of how to use the SKIP library to serialize and deserialize a string.

```cpp
#include <iostream>
#include "skip.h"

int main() {
    // 1. Create a new SKIP configuration.
    void* config = skip_create_base_config();

    // 2. Define the data structure. In this case, it's a single
    //    field containing a string of 12 characters.
    skip_push_type_to_config(config, skip_char, 12);

    // 3. Allocate a buffer to hold the serialized data. The size
    //    is determined by the configuration.
    size_t buffer_size = skip_get_cfg_size(config);
    char* buffer = new char[buffer_size];

    // 4. The data to be serialized.
    const char* message = "Hello World";

    // 5. Write the data to the buffer. We're writing to the first
    //    field (index 0) in our configuration.
    skip_write_index_to_buffer(config, (void*)buffer, (void*)message, 0);

    // 6. Create a variable to hold the deserialized data.
    char* deserialized_message = new char[12];

    // 7. Read the data from the buffer back into our variable.
    skip_read_index_from_buffer(config, (void*)buffer, (void*)deserialized_message, 0);

    // 8. Print the deserialized message.
    std::cout << "Buffer size: " << buffer_size << std::endl;
    std::cout << "Deserialized message: " << deserialized_message << std::endl;

    // 9. Clean up allocated memory.
    skip_free_cfg(config);
    delete[] buffer;
    delete[] deserialized_message;

    return 0;
}
```

When you build and run the `main` executable, the output will be:

```
Buffer size: 12
Deserialized message: Hello World
```
