#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_laser_range_finder import LaserRangeFinder

# Callback for distance greater than 200 cm
def cb_reached(distance):
    print('Distance ' + str(distance) + ' cm.')

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    lrf = LaserRangeFinder(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enable_laser()
    time.sleep(0.25)

    # Get threshold callbacks with a debounce time of 1 seconds (1000ms)
    lrf.set_debounce_period(1000)

    # Register threshold reached callback to function cb_reached
    lrf.register_callback(lrf.CALLBACK_DISTANCE_REACHED, cb_reached)

    # Configure threshold for "greater than 200 cm"
    lrf.set_distance_callback_threshold('>', 200, 0)

    raw_input('Press key to exit\n') # Use input() in Python 3
    lrf.disable_laser() # Turn laser off
    ipcon.disconnect()
