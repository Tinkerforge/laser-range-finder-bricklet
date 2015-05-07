#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 1

# get current distance (unit is cm/10)
tinkerforge call laser-range-finder-bricklet $uid get-distance
