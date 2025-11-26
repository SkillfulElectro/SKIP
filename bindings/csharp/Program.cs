using System;
using System.Runtime.InteropServices;

public static class Skip
{
    private const string LibName = "skip";

    public enum DataType
    {
        INT8 = 0,
        UINT8 = 1,
        INT16 = 2,
        UINT16 = 3,
        INT32 = 4,
        UINT32 = 5,
        INT64 = 6,
        UINT64 = 7,
        FLOAT = 8,
        DOUBLE = 9,
        CHAR = 10,
        NEST = 11,
    }

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr skip_create_base_config();

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int skip_push_type_to_config(IntPtr cfg, int type_code, ulong len);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern ulong skip_get_data_size(IntPtr cfg);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void skip_free_cfg(IntPtr cfg);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int skip_write_index_to_buffer(IntPtr cfg, IntPtr buffer, ulong buffer_size, float[] value, ulong index);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int skip_write_index_to_buffer(IntPtr cfg, IntPtr buffer, ulong buffer_size, int[] value, ulong index);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int skip_read_index_from_buffer(IntPtr cfg, IntPtr buffer, ulong buffer_size, float[] value, ulong index);

    [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int skip_read_index_from_buffer(IntPtr cfg, IntPtr buffer, ulong buffer_size, int[] value, ulong index);
}

public class Example
{
    public static void Main(string[] args)
    {
        // Create a configuration
        IntPtr config = Skip.skip_create_base_config();
        Skip.skip_push_type_to_config(config, (int)Skip.DataType.FLOAT, 1);
        Skip.skip_push_type_to_config(config, (int)Skip.DataType.FLOAT, 1);
        Skip.skip_push_type_to_config(config, (int)Skip.DataType.FLOAT, 1);
        Skip.skip_push_type_to_config(config, (int)Skip.DataType.INT32, 1);

        // Create an object and set its values
        ulong size = Skip.skip_get_data_size(config);
        IntPtr buffer = Marshal.AllocHGlobal((int)size);

        float[] x = { 1.1f };
        float[] y = { 2.2f };
        float[] z = { 3.3f };
        int[] id = { 123 };

        Skip.skip_write_index_to_buffer(config, buffer, size, x, 0);
        Skip.skip_write_index_to_buffer(config, buffer, size, y, 1);
        Skip.skip_write_index_to_buffer(config, buffer, size, z, 2);
        Skip.skip_write_index_to_buffer(config, buffer, size, id, 3);

        // Read the values back
        float[] newX = new float[1];
        float[] newY = new float[1];
        float[] newZ = new float[1];
        int[] newId = new int[1];

        Skip.skip_read_index_from_buffer(config, buffer, size, newX, 0);
        Skip.skip_read_index_from_buffer(config, buffer, size, newY, 1);
        Skip.skip_read_index_from_buffer(config, buffer, size, newZ, 2);
        Skip.skip_read_index_from_buffer(config, buffer, size, newId, 3);

        Console.WriteLine("x: " + newX[0]);
        Console.WriteLine("y: " + newY[0]);
        Console.WriteLine("z: " + newZ[0]);
        Console.WriteLine("id: " + newId[0]);

        // Verify the values
        if (Math.Abs(newX[0] - 1.1) > 1e-6 ||
            Math.Abs(newY[0] - 2.2) > 1e-6 ||
            Math.Abs(newZ[0] - 3.3) > 1e-6 ||
            newId[0] != 123)
        {
            throw new Exception("Verification failed!");
        }

        Console.WriteLine("C# example ran successfully!");

        Marshal.FreeHGlobal(buffer);
        Skip.skip_free_cfg(config);
    }
}
