#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your Laser Range Finder Bricklet

import time

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_laser_range_finder import BrickletLaserRangeFinder

# Callback function for distance callback
def cb_distance(distance):
    print("Distance: " + str(distance) + " cm")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    lrf = BrickletLaserRangeFinder(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enable_laser()
    time.sleep(0.25)

    # Register distance callback to function cb_distance
    lrf.register_callback(lrf.CALLBACK_DISTANCE, cb_distance)

    # Set period for distance callback to 0.2s (200ms)
    # Note: The distance callback is only called every 0.2 seconds
    #       if the distance has changed since the last call!
    lrf.set_distance_callback_period(200)

    raw_input("Press key to exit\n") # Use input() in Python 3
    lrf.disable_laser() # Turn laser off
    ipcon.disconnect()
