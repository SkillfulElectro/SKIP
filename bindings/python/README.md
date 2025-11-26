# Python Bindings for SKIP

This directory contains Python bindings for the SKIP library, allowing you to use its fast and efficient serialization capabilities in your Python applications. The bindings are implemented using the `ctypes` library, which provides a straightforward way to call C functions from Python.

## Prerequisites

- Python 3.x
- A C compiler (e.g., GCC, Clang)
- CMake

## Building the C Library

Before using the Python bindings, you need to build the core SKIP C library. From the root of the repository, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will compile the C library and create a shared object file (`libskip.so` on Linux, `libskip.dylib` on macOS) in the `build` directory.

## Usage

The `skip.py` module provides a Pythonic interface to the underlying C library. It automatically locates and loads the shared library, so you can start using it right away.

### Example

Here's a simple example of how to use the bindings:

```python
import skip

# Create a configuration
config = skip.Config()
config.add_field("x", skip.DataType.FLOAT)
config.add_field("y", skip.DataType.FLOAT)
config.add_field("z", skip.DataType.FLOAT)

# Create an object and set its values
obj = config.create_object()
obj["x"] = 1.0
obj["y"] = 2.0
obj["z"] = 3.0

# Write the object to a buffer
buffer = obj.to_bytes()

# Read the object from the buffer
new_obj = config.from_bytes(buffer)

print(f"x: {new_obj['x']}")
print(f"y: {new_obj['y']}")
print(f"z: {new_obj['z']}")
```

To run this example, execute the following command from the `python` directory:

```bash
python3 example.py
```
