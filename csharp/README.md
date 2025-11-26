# C# Bindings for SKIP

This directory contains C# bindings for the SKIP library, allowing you to use its fast and efficient serialization capabilities in your .NET applications. The bindings are implemented using P/Invoke, which is the standard way to call native code from C#.

## Prerequisites

- .NET SDK 6.0 or later
- A C compiler (e.g., GCC, Clang)
- CMake

## Building the C Library

Before using the C# bindings, you need to build the core SKIP C library. From the root of the repository, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will compile the C library and create a shared object file (`libskip.so` on Linux, `libskip.dylib` on macOS) in the `build` directory.

## Usage

The `Skip.cs` class provides a C#-friendly API for the underlying C library. P/Invoke handles the dynamic loading of the shared library and the marshalling of data between C# and C.

### Example

Here's a simple example of how to use the bindings:

```csharp
using System;

public class Example
{
    public static void Main(string[] args)
    {
        // Create a configuration
        IntPtr config = Skip.skip_create_base_config();
        Skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        Skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        Skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);

        // Create an object and set its values
        long size = Skip.skip_get_data_size(config);
        IntPtr buffer = Marshal.AllocHGlobal((int)size);

        float[] x = {1.1f};
        Skip.skip_write_index_to_buffer(config, buffer, size, x, 0);

        // Read the value back
        float[] newX = new float[1];
        Skip.skip_read_index_from_buffer(config, buffer, size, newX, 0);

        Console.WriteLine("x: " + newX[0]);

        Marshal.FreeHGlobal(buffer);
        Skip.skip_free_cfg(config);
    }
}
```

To run this example, you'll need to create a C# project and add a reference to the `Skip.cs` file. You can then build and run the project using the `dotnet` CLI.
