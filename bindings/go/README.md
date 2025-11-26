# Go Bindings for SKIP

This directory contains Go bindings for the SKIP library, allowing you to leverage its high-performance serialization capabilities within your Go applications. The bindings are implemented using `cgo`, which enables Go programs to call C code.

## Prerequisites

- Go 1.x
- A C compiler (e.g., GCC, Clang)
- CMake

## Building the C Library

Before using the Go bindings, you must build the core SKIP C library. From the root of the repository, execute the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will compile the C library and create a shared object file (`libskip.so` on Linux, `libskip.dylib` on macOS) in the `build` directory.

## Usage

The `skip.go` module provides a Go-native interface to the underlying C library. It handles the `cgo` boilerplate, allowing you to work with SKIP in a more idiomatic Go style.

### Example

Here is a simple example demonstrating how to use the bindings:

```go
package main

import (
	"fmt"
	"skip"
)

func main() {
	// Create a configuration
	config := skip.NewConfig()
	config.AddField("x", skip.Float)
	config.AddField("y", skip.Float)
	config.AddField("z", skip.Float)
	defer config.Free()

	// Create an object and set its values
	obj := config.CreateObject()
	defer obj.Free()

	obj.SetFloat("x", 1.0)
	obj.SetFloat("y", 2.0)
	obj.SetFloat("z", 3.0)

	// Write the object to a buffer
	buffer := obj.ToBytes()

	// Read the object from the buffer
	newObj := config.FromBytes(buffer)
	defer newObj.Free()

	x, _ := newObj.GetFloat("x")
	y, _ := newObj.GetFloat("y")
	z, _ := newObj.GetFloat("z")

	fmt.Printf("x: %f\n", x)
	fmt.Printf("y: %f\n", y)
	fmt.Printf("z: %f\n", z)
}
```

To run this example, execute the following command from the `go` directory:

```bash
go run example.go
```
