package skip

/*
#cgo CFLAGS: -I../../
#cgo LDFLAGS: -L../../build -lskip
#include "skip.h"
#include <stdlib.h>
#include <string.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// DataType represents the SKIP data types.
type DataType C.enum_SkipDataTypeCode

const (
	Int8   DataType = C.skip_int8
	UInt8  DataType = C.skip_uint8
	Int16  DataType = C.skip_int16
	UInt16 DataType = C.skip_uint16
	Int32  DataType = C.skip_int32
	UInt32 DataType = C.skip_uint32
	Int64  DataType = C.skip_int64
	UInt64 DataType = C.skip_uint64
	Float  DataType = C.skip_float32
	Double DataType = C.skip_float64
	Char   DataType = C.skip_char
	Nest   DataType = C.skip_nest
)

// Config represents a SKIP configuration.
type Config struct {
	ptr    unsafe.Pointer
	fields map[string]int
}

// NewConfig creates a new SKIP configuration.
func NewConfig() *Config {
	return &Config{
		ptr:    C.skip_create_base_config(),
		fields: make(map[string]int),
	}
}

// AddField adds a new field to the configuration.
func (c *Config) AddField(name string, dataType DataType, count uint64) {
	C.skip_push_type_to_config(c.ptr, C.int(dataType), C.uint64_t(count))
	c.fields[name] = len(c.fields)
}

// CreateObject creates a new SkipObject based on the configuration.
func (c *Config) CreateObject() *SkipObject {
	size := C.skip_get_data_size(c.ptr)
	buffer := C.malloc(C.size_t(size))
	return &SkipObject{
		config: c,
		buffer: buffer,
		size:   size,
	}
}

// FromBytes creates a SkipObject from a byte slice.
func (c *Config) FromBytes(data []byte) *SkipObject {
	obj := c.CreateObject()
	C.memcpy(obj.buffer, unsafe.Pointer(&data[0]), C.size_t(len(data)))
	return obj
}

// Free releases the memory used by the configuration.
func (c *Config) Free() {
	C.skip_free_cfg(c.ptr)
}

// SkipObject represents an object created from a SKIP configuration.
type SkipObject struct {
	config *Config
	buffer unsafe.Pointer
	size   C.uint64_t
}

// SetFloat sets the value of a float field.
func (o *SkipObject) SetFloat(name string, value float32) error {
	index, ok := o.config.fields[name]
	if !ok {
		return fmt.Errorf("field not found: %s", name)
	}
	cValue := C.float(value)
	C.skip_write_index_to_buffer(o.config.ptr, o.buffer, o.size, unsafe.Pointer(&cValue), C.uint64_t(index))
	return nil
}

// GetFloat retrieves the value of a float field.
func (o *SkipObject) GetFloat(name string) (float32, error) {
	index, ok := o.config.fields[name]
	if !ok {
		return 0, fmt.Errorf("field not found: %s", name)
	}
	var value C.float
	C.skip_read_index_from_buffer(o.config.ptr, o.buffer, o.size, unsafe.Pointer(&value), C.uint64_t(index))
	return float32(value), nil
}

// ToBytes returns the raw byte representation of the object.
func (o *SkipObject) ToBytes() []byte {
	return C.GoBytes(o.buffer, C.int(o.size))
}

// Free releases the memory used by the object's buffer.
func (o *SkipObject) Free() {
	C.free(o.buffer)
}
