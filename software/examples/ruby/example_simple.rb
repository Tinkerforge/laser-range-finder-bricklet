#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_laser_range_finder'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change to your UID

ipcon = IPConnection.new # Create IP connection
lrf = BrickletLaserRangeFinder.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Turn laser on and wait 250ms for very first measurement to be ready
lrf.enable_laser
sleep 0.25

# Get current distance (unit is cm)
distance = lrf.get_distance
puts "Distance: #{distance} cm"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
