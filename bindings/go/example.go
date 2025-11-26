package main

import (
	"fmt"
	"skip"
)

func main() {
	// Create a configuration
	config := skip.NewConfig()
	config.AddField("x", skip.Float, 1)
	config.AddField("y", skip.Float, 1)
	config.AddField("z", skip.Float, 1)
	defer config.Free()

	// Create an object and set its values
	obj := config.CreateObject()
	defer obj.Free()

	obj.SetFloat("x", 1.1)
	obj.SetFloat("y", 2.2)
	obj.SetFloat("z", 3.3)

	// Write the object to a buffer
	buffer := obj.ToBytes()

	// Read the object from the buffer
	newObj := config.FromBytes(buffer)
	defer newObj.Free()

	x, _ := newObj.GetFloat("x")
	y, _ := newObj.GetFloat("y")
	z, _ := newObj.GetFloat("z")

	fmt.Printf("x: %f\n", x)
	fmt.Printf("y: %f\n", y)
	fmt.Printf("z: %f\n", z)

	// Verify the values
	if x != 1.1 || y != 2.2 || z != 3.3 {
		panic("Verification failed!")
	}

	fmt.Println("Go example ran successfully!")
}
