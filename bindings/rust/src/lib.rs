#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::collections::HashMap;
use std::ffi::{c_void, CString};
use std::marker::PhantomData;

// Re-export the enum for easier use
pub use SkipDataTypeCode as DataType;

pub struct Config<'a> {
    ptr: *mut c_void,
    fields: HashMap<String, (usize, DataType)>,
    _marker: PhantomData<&'a ()>,
}

impl<'a> Config<'a> {
    pub fn new() -> Self {
        let ptr = unsafe { skip_create_base_config() };
        Config {
            ptr,
            fields: HashMap::new(),
            _marker: PhantomData,
        }
    }

    pub fn add_field(&mut self, name: &str, data_type: DataType, count: u64) {
        unsafe {
            skip_push_type_to_config(self.ptr, data_type as i32, count);
        }
        self.fields.insert(name.to_string(), (self.fields.len(), data_type));
    }

    pub fn create_object(&self) -> SkipObject<'_> {
        let size = unsafe { skip_get_data_size(self.ptr) };
        let buffer = unsafe { libc::malloc(size as usize) };
        SkipObject {
            config: self,
            buffer,
            size,
        }
    }

    pub fn from_bytes(&self, data: &[u8]) -> SkipObject<'_> {
        let obj = self.create_object();
        unsafe {
            libc::memcpy(obj.buffer, data.as_ptr() as *const c_void, data.len());
        }
        obj
    }
}

impl<'a> Drop for Config<'a> {
    fn drop(&mut self) {
        unsafe { skip_free_cfg(self.ptr) };
    }
}

pub struct SkipObject<'a> {
    config: &'a Config<'a>,
    buffer: *mut c_void,
    size: u64,
}

impl<'a> SkipObject<'a> {
    pub fn set<T: SkipType>(&mut self, name: &str, value: T) -> Result<(), &str> {
        if let Some((index, data_type)) = self.config.fields.get(name) {
            if *data_type == T::data_type() {
                unsafe {
                    skip_write_index_to_buffer(
                        self.config.ptr,
                        self.buffer,
                        self.size,
                        &value as *const T as *mut c_void,
                        *index as u64,
                    );
                }
                Ok(())
            } else {
                Err("Type mismatch")
            }
        } else {
            Err("Field not found")
        }
    }

    pub fn get<T: SkipType>(&self, name: &str) -> Result<T, &str> {
        if let Some((index, data_type)) = self.config.fields.get(name) {
            if *data_type == T::data_type() {
                let mut value: T = unsafe { std::mem::zeroed() };
                unsafe {
                    skip_read_index_from_buffer(
                        self.config.ptr,
                        self.buffer,
                        self.size,
                        &mut value as *mut T as *mut c_void,
                        *index as u64,
                    );
                }
                Ok(value)
            } else {
                Err("Type mismatch")
            }
        } else {
            Err("Field not found")
        }
    }

    pub fn to_bytes(&self) -> Vec<u8> {
        let mut vec = Vec::with_capacity(self.size as usize);
        unsafe {
            vec.set_len(self.size as usize);
            libc::memcpy(
                vec.as_mut_ptr() as *mut c_void,
                self.buffer,
                self.size as usize,
            );
        }
        vec
    }
}

impl<'a> Drop for SkipObject<'a> {
    fn drop(&mut self) {
        unsafe { libc::free(self.buffer) };
    }
}

pub trait SkipType {
    fn data_type() -> DataType;
}

impl SkipType for i8 { fn data_type() -> DataType { DataType::skip_int8 } }
impl SkipType for u8 { fn data_type() -> DataType { DataType::skip_uint8 } }
impl SkipType for i16 { fn data_type() -> DataType { DataType::skip_int16 } }
impl SkipType for u16 { fn data_type() -> DataType { DataType::skip_uint16 } }
impl SkipType for i32 { fn data_type() -> DataType { DataType::skip_int32 } }
impl SkipType for u32 { fn data_type() -> DataType { DataType::skip_uint32 } }
impl SkipType for i64 { fn data_type() -> DataType { DataType::skip_int64 } }
impl SkipType for u64 { fn data_type() -> DataType { DataType::skip_uint64 } }
impl SkipType for f32 { fn data_type() -> DataType { DataType::skip_float32 } }
impl SkipType for f64 { fn data_type() -> DataType { DataType::skip_float64 } }
