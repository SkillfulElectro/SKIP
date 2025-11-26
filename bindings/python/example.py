import skip

# Create a configuration
config = skip.Config()
config.add_field("x", skip.DataType.FLOAT)
config.add_field("y", skip.DataType.FLOAT)
config.add_field("z", skip.DataType.FLOAT)
config.add_field("id", skip.DataType.INT32)

# Create an object and set its values
obj = config.create_object()
obj["x"] = 1.1
obj["y"] = 2.2
obj["z"] = 3.3
obj["id"] = 123

# Write the object to a buffer
buffer = obj.to_bytes()

# Read the object from the buffer
new_obj = config.from_bytes(buffer)

print(f"x: {new_obj['x']}")
print(f"y: {new_obj['y']}")
print(f"z: {new_obj['z']}")
print(f"id: {new_obj['id']}")

# Verify the values
assert abs(new_obj['x'] - 1.1) < 1e-6
assert abs(new_obj['y'] - 2.2) < 1e-6
assert abs(new_obj['z'] - 3.3) < 1e-6
assert new_obj['id'] == 123

print("Python example ran successfully!")
