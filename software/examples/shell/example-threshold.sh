#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 1

# get threshold callbacks with a debounce time of 10 seconds (10000ms)
tinkerforge call laser-range-finder-bricklet $uid set-debounce-period 10000

# configure threshold for "greater than 20 cm" (unit is cm)
tinkerforge call laser-range-finder-bricklet $uid set-distance-callback-threshold greater 20 0

# handle incoming distance-reached callbacks (unit is cm)
tinkerforge dispatch laser-range-finder-bricklet $uid distance-reached\
 --execute "echo Distance: {distance} cm"
