import com.sun.jna.Memory;
import com.sun.jna.Pointer;

public class Example {
    public static void main(String[] args) {
        // Set the JNA library path to the build directory
        System.setProperty("jna.library.path", "../../build");

        Skip skip = Skip.INSTANCE;

        // Create a configuration
        Pointer config = skip.skip_create_base_config();
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        skip.skip_push_type_to_config(config, Skip.DataType.FLOAT, 1);
        skip.skip_push_type_to_config(config, Skip.DataType.INT32, 1);


        // Create an object and set its values
        long size = skip.skip_get_data_size(config);
        Pointer buffer = new Memory(size);

        float[] x = {1.1f};
        float[] y = {2.2f};
        float[] z = {3.3f};
        int[] id = {123};

        skip.skip_write_index_to_buffer(config, buffer, size, x, 0);
        skip.skip_write_index_to_buffer(config, buffer, size, y, 1);
        skip.skip_write_index_to_buffer(config, buffer, size, z, 2);
        // JNA doesn't have a direct mapping for int[], so we need to use a Pointer
        Pointer idPtr = new Memory(4);
        idPtr.setInt(0, id[0]);
        skip.skip_write_index_to_buffer(config, buffer, size, idPtr, 3);


        // Read the values back
        float[] newX = new float[1];
        float[] newY = new float[1];
        float[] newZ = new float[1];
        Pointer newIdPtr = new Memory(4);


        skip.skip_read_index_from_buffer(config, buffer, size, newX, 0);
        skip.skip_read_index_from_buffer(config, buffer, size, newY, 1);
        skip.skip_read_index_from_buffer(config, buffer, size, newZ, 2);
        skip.skip_read_index_from_buffer(config, buffer, size, newIdPtr, 3);
        int newId = newIdPtr.getInt(0);

        System.out.println("x: " + newX[0]);
        System.out.println("y: " + newY[0]);
        System.out.println("z: " + newZ[0]);
        System.out.println("id: " + newId);

        // Verify the values
        if (Math.abs(newX[0] - 1.1) > 1e-6 ||
            Math.abs(newY[0] - 2.2) > 1e-6 ||
            Math.abs(newZ[0] - 3.3) > 1e-6 ||
            newId != 123) {
            throw new RuntimeException("Verification failed!");
        }

        System.out.println("Java example ran successfully!");

        skip.skip_free_cfg(config);
    }
}
