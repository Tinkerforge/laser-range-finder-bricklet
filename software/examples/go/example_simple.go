package main

import (
	"fmt"
	"time"
	"tinkerforge/ipconnection"
	"tinkerforge/laser_range_finder_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Laser Range Finder Bricklet.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	lrf, _ := laser_range_finder_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Turn laser on and wait 250ms for very first measurement to be ready
	lrf.EnableLaser()
	time.Sleep(250 * time.Millisecond)

	// Get current distance.
	distance, _ := lrf.GetDistance()
	fmt.Printf("Distance:  cm\n", distance)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	lrf.DisableLaser() // Turn laser off
}
