#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your Laser Range Finder Bricklet

import time

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_laser_range_finder import BrickletLaserRangeFinder

# Callback function for distance reached callback
def cb_distance_reached(distance):
    print("Distance: " + str(distance) + " cm")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    lrf = BrickletLaserRangeFinder(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enable_laser()
    time.sleep(0.25)

    # Get threshold callbacks with a debounce time of 10 seconds (10000ms)
    lrf.set_debounce_period(10000)

    # Register distance reached callback to function cb_distance_reached
    lrf.register_callback(lrf.CALLBACK_DISTANCE_REACHED, cb_distance_reached)

    # Configure threshold for distance "greater than 20 cm"
    lrf.set_distance_callback_threshold(">", 20, 0)

    raw_input("Press key to exit\n") # Use input() in Python 3
    lrf.disable_laser() # Turn laser off
    ipcon.disconnect()
