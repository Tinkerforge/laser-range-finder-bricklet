package main

import (
	"fmt"
	"github.com/tinkerforge/go-api-bindings/ipconnection"
	"github.com/tinkerforge/go-api-bindings/laser_range_finder_bricklet"
	"time"
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

	lrf.RegisterDistanceCallback(func(distance uint16) {
		fmt.Printf("Distance: %d cm\n", distance)
	})

	// Set period for distance receiver to 0.2s (200ms).
	// Note: The distance callback is only called every 0.2 seconds
	//       if the distance has changed since the last call!
	lrf.SetDistanceCallbackPeriod(200)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	lrf.DisableLaser() // Turn laser off
}
