#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Laser Range Finder Bricklet

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 0.25

# Get current distance
tinkerforge call laser-range-finder-bricklet $uid get-distance

tinkerforge call laser-range-finder-bricklet $uid disable-laser # Turn laser off
