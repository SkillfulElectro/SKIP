use skip_rs::{Config, DataType};

fn main() {
    // Create a configuration
    let mut config = Config::new();
    config.add_field("x", DataType::skip_float32, 1);
    config.add_field("y", DataType::skip_float32, 1);
    config.add_field("z", DataType::skip_float32, 1);
    config.add_field("id", DataType::skip_int32, 1);

    // Create an object and set its values
    let mut obj = config.create_object();
    obj.set("x", 1.1f32).unwrap();
    obj.set("y", 2.2f32).unwrap();
    obj.set("z", 3.3f32).unwrap();
    obj.set("id", 123i32).unwrap();

    // Write the object to a buffer
    let buffer = obj.to_bytes();

    // Read the object from the buffer
    let new_obj = config.from_bytes(&buffer);

    let x: f32 = new_obj.get("x").unwrap();
    let y: f32 = new_obj.get("y").unwrap();
    let z: f32 = new_obj.get("z").unwrap();
    let id: i32 = new_obj.get("id").unwrap();

    println!("x: {}", x);
    println!("y: {}", y);
    println!("z: {}", z);
    println!("id: {}", id);

    // Verify the values
    assert!((x - 1.1).abs() < 1e-6);
    assert!((y - 2.2).abs() < 1e-6);
    assert!((z - 3.3).abs() < 1e-6);
    assert_eq!(id, 123);

    println!("Rust example ran successfully!");
}
