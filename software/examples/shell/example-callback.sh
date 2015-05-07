#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# Turn laser on and wait 250ms for very first measurement to be ready
tinkerforge call laser-range-finder-bricklet $uid enable-laser
sleep 1

# set period for distance callback to 1s (1000ms)
# note: the distance callback is only called every second if the
#       distance has changed since the last call!
tinkerforge call laser-range-finder-bricklet $uid set-distance-callback-period 1000

# handle incoming distance callbacks (unit is cm)
tinkerforge dispatch laser-range-finder-bricklet $uid distance
