#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_laser_range_finder import LaserRangeFinder

# Callback function for distance callback (parameter has unit cm)
def cb_distance(distance):
    print('Distance: ' + str(distance) + ' cm')

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    lrf = LaserRangeFinder(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set Period for distance callback to 200ms
    # Note: The distance callback is only called every second if the 
    #       distance has changed since the last call!
    lrf.set_distance_callback_period(200)

    # Register distance callback to function cb_distance
    lrf.register_callback(lrf.CALLBACK_DISTANCE, cb_distance)

    lrf.enable_laser()

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
