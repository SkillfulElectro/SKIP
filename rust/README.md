# Rust Bindings for SKIP

This directory contains Rust bindings for the SKIP library, enabling you to use its high-performance serialization capabilities in your Rust applications. The bindings are generated using `bindgen` and wrapped in a safe, idiomatic Rust API.

## Prerequisites

- Rust 1.x
- A C compiler (e.g., GCC, Clang)
- CMake

## Building the C Library

Before using the Rust bindings, you must build the core SKIP C library. From the root of the repository, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will compile the C library and create a shared object file (`libskip.so` on Linux, `libskip.dylib` on macOS) in the `build` directory.

## Usage

The `skip-rs` crate provides a safe and convenient interface to the underlying C library. It handles the unsafe FFI calls, allowing you to work with SKIP in a more natural Rust style.

### Example

Here's a simple example of how to use the bindings:

```rust
use skip_rs::{Config, DataType};

fn main() {
    // Create a configuration
    let mut config = Config::new();
    config.add_field("x", DataType::Float, 1);
    config.add_field("y", DataType::Float, 1);
    config.add_field("z", DataType::Float, 1);

    // Create an object and set its values
    let mut obj = config.create_object();
    obj.set("x", 1.0f32).unwrap();
    obj.set("y", 2.0f32).unwrap();
    obj.set("z", 3.0f32).unwrap();

    // Write the object to a buffer
    let buffer = obj.to_bytes();

    // Read the object from the buffer
    let new_obj = config.from_bytes(&buffer);

    let x: f32 = new_obj.get("x").unwrap();
    let y: f32 = new_obj.get("y").unwrap();
    let z: f32 = new_obj.get("z").unwrap();

    println!("x: {}", x);
    println!("y: {}", y);
    println!("z: {}", z);
}
```

To run this example, execute the following command from the `rust` directory:

```bash
cargo run --example simple
```
