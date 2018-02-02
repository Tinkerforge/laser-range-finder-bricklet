#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Laser Range Finder Bricklet

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 0.25

# Handle incoming distance callbacks
tinkerforge dispatch laser-range-finder-bricklet $uid distance &

# Set period for distance callback to 0.2s (200ms)
# Note: The distance callback is only called every 0.2 seconds
#       if the distance has changed since the last call!
tinkerforge call laser-range-finder-bricklet $uid set-distance-callback-period 200

echo "Press key to exit"; read dummy

tinkerforge call laser-range-finder-bricklet $uid disable-laser # Turn laser off

kill -- -$$ # Stop callback dispatch in background
