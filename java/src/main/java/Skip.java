import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface Skip extends Library {
    Skip INSTANCE = Native.load("skip", Skip.class);

    // Enums
    interface DataType {
        int INT8 = 0;
        int UINT8 = 1;
        int INT16 = 2;
        int UINT16 = 3;
        int INT32 = 4;
        int UINT32 = 5;
        int INT64 = 6;
        int UINT64 = 7;
        int FLOAT = 8;
        int DOUBLE = 9;
        int CHAR = 10;
        int NEST = 11;
    }

    // Functions
    Pointer skip_create_base_config();
    int skip_push_type_to_config(Pointer cfg, int type_code, long len);
    int skip_pop_type_from_config(Pointer cfg);
    long skip_get_data_size(Pointer cfg);
    void skip_free_cfg(Pointer cfg);
    int skip_write_index_to_buffer(Pointer cfg, Pointer buffer, long buffer_size, float[] value, long index);
    int skip_read_index_from_buffer(Pointer cfg, Pointer buffer, long buffer_size, float[] value, long index);
    int skip_write_index_to_buffer(Pointer cfg, Pointer buffer, long buffer_size, Pointer value, long index);
    int skip_read_index_from_buffer(Pointer cfg, Pointer buffer, long buffer_size, Pointer value, long index);
    int skip_get_system_endian();
}
