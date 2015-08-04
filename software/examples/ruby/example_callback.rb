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

# Set Period for distance callback to 1s (1000ms)
# Note: The distance callback is only called every second if the 
#       distance has changed since the last call!
lrf.set_distance_callback_period 1000

# Register distance callback (parameter has unit cm)
lrf.register_callback(BrickletLaserRangeFinder::CALLBACK_DISTANCE) do |distance|
  puts "Distance: #{distance} cm"
end

puts 'Press key to exit'
$stdin.gets
lrf.disable_laser # Turn laser off
ipcon.disconnect
