import ctypes
import os
from enum import IntEnum

# Load the shared library
lib_path = os.path.join(os.path.dirname(__file__), '..', 'build', 'libskip.so')
if not os.path.exists(lib_path):
    lib_path = os.path.join(os.path.dirname(__file__), '..', 'build', 'libskip.dylib')
if not os.path.exists(lib_path):
    # try windows
    lib_path = os.path.join(os.path.dirname(__file__), '..', 'build', 'Release', 'skip.dll')

try:
    lib = ctypes.CDLL(lib_path)
except OSError:
    raise ImportError("Could not find the SKIP shared library. Please build it first.")

# Define data types
class DataType(IntEnum):
    INT8 = 0
    UINT8 = 1
    INT16 = 2
    UINT16 = 3
    INT32 = 4
    UINT32 = 5
    INT64 = 6
    UINT64 = 7
    FLOAT = 8
    DOUBLE = 9
    CHAR = 10
    NEST = 11

# Define C function prototypes
lib.skip_create_base_config.restype = ctypes.c_void_p
lib.skip_push_type_to_config.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_ulonglong]
lib.skip_get_data_size.argtypes = [ctypes.c_void_p]
lib.skip_get_data_size.restype = ctypes.c_ulonglong
lib.skip_write_index_to_buffer.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_void_p, ctypes.c_ulonglong]
lib.skip_read_index_from_buffer.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_void_p, ctypes.c_ulonglong]
lib.skip_free_cfg.argtypes = [ctypes.c_void_p]

# Pythonic wrapper classes
class Config:
    def __init__(self):
        self.ptr = lib.skip_create_base_config()
        self.fields = []

    def add_field(self, name, data_type, count=1):
        if not isinstance(data_type, DataType):
            raise TypeError("data_type must be an instance of DataType")
        lib.skip_push_type_to_config(self.ptr, data_type, count)
        self.fields.append({'name': name, 'type': data_type, 'count': count})

    def create_object(self):
        return SkipObject(self)

    def from_bytes(self, buffer):
        obj = self.create_object()
        obj.from_bytes(buffer)
        return obj

    def __del__(self):
        if self.ptr:
            lib.skip_free_cfg(self.ptr)

class SkipObject:
    def __init__(self, config):
        self.config = config
        self.size = lib.skip_get_data_size(self.config.ptr)
        self.buffer = ctypes.create_string_buffer(self.size)

    def _get_field_info(self, name):
        for i, field in enumerate(self.config.fields):
            if field['name'] == name:
                return i, field['type']
        raise KeyError(f"Field not found: {name}")

    def __setitem__(self, key, value):
        index, data_type = self._get_field_info(key)
        c_type = self._get_c_type(data_type)
        c_value = c_type(value)
        lib.skip_write_index_to_buffer(self.config.ptr, self.buffer, self.size, ctypes.byref(c_value), index)

    def __getitem__(self, key):
        index, data_type = self._get_field_info(key)
        c_type = self._get_c_type(data_type)
        c_value = c_type()
        lib.skip_read_index_from_buffer(self.config.ptr, self.buffer, self.size, ctypes.byref(c_value), index)
        return c_value.value

    def to_bytes(self):
        return self.buffer.raw

    def from_bytes(self, data):
        ctypes.memmove(self.buffer, data, len(data))

    def _get_c_type(self, data_type):
        return {
            DataType.INT8: ctypes.c_int8,
            DataType.UINT8: ctypes.c_uint8,
            DataType.INT16: ctypes.c_int16,
            DataType.UINT16: ctypes.c_uint16,
            DataType.INT32: ctypes.c_int32,
            DataType.UINT32: ctypes.c_uint32,
            DataType.INT64: ctypes.c_int64,
            DataType.UINT64: ctypes.c_uint64,
            DataType.FLOAT: ctypes.c_float,
            DataType.DOUBLE: ctypes.c_double,
            DataType.CHAR: ctypes.c_char,
        }[data_type]
