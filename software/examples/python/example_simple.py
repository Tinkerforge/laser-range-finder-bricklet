#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_laser_range_finder import LaserRangeFinder

import time

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    lrf = LaserRangeFinder(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enable_laser()
    time.sleep(0.25)

    # Get current distance (unit is cm)
    distance = lrf.get_distance()

    # Turn laser off
    lrf.disable_laser()

    print('Distance: ' + str(distance) + ' cm')

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
