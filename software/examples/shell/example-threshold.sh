#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Laser Range Finder Bricklet

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 0.25

# Get threshold callbacks with a debounce time of 10 seconds (10000ms)
tinkerforge call laser-range-finder-bricklet $uid set-debounce-period 10000

# Handle incoming distance reached callbacks
tinkerforge dispatch laser-range-finder-bricklet $uid distance-reached &

# Configure threshold for distance "greater than 20 cm"
tinkerforge call laser-range-finder-bricklet $uid set-distance-callback-threshold threshold-option-greater 20 0

echo "Press key to exit"; read dummy

tinkerforge call laser-range-finder-bricklet $uid disable-laser # Turn laser off

kill -- -$$ # Stop callback dispatch in background
