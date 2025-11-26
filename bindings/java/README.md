# Java Bindings for SKIP

This directory contains Java bindings for the SKIP library, allowing you to use its fast and efficient serialization capabilities in your Java applications. The bindings are implemented using Java Native Access (JNA), which provides easy access to native shared libraries.

## Prerequisites

- Java JDK 8 or later
- A C compiler (e.g., GCC, Clang)
- CMake
- Maven

## Building the C Library

Before using the Java bindings, you need to build the core SKIP C library. From the root of the repository, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will compile the C library and create a shared object file (`libskip.so` on Linux, `libskip.dylib` on macOS) in the `build` directory.

## Usage

The `Skip.java` interface provides a Java-friendly API for the underlying C library. JNA handles the dynamic loading of the shared library and the mapping of Java types to native types.

### Example

Here's a simple example of how to use the bindings:

```java
import com.sun.jna.Pointer;

public class Example {
    public static void main(String[] args) {
        Skip skip = Skip.INSTANCE;

        // Create a configuration
        Pointer config = skip.skip_create_base_config();
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);

        // Create an object and set its values
        long size = skip.skip_get_data_size(config);
        Pointer buffer = new Memory(size);

        float[] x = {1.1f};
        skip.skip_write_index_to_buffer(config, buffer, size, x, 0);

        // Read the value back
        float[] newX = new float[1];
        skip.skip_read_index_from_buffer(config, buffer, size, newX, 0);

        System.out.println("x: " + newX[0]);

        skip.skip_free_cfg(config);
    }
}
```

To run this example, you'll need to have the JNA dependency in your project. You can add it to your `pom.xml` like this:

```xml
<dependencies>
  <dependency>
    <groupId>net.java.dev.jna</groupId>
    <artifactId>jna</artifactId>
    <version>5.14.0</version>
  </dependency>
</dependencies>
```
