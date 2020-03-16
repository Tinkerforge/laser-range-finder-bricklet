#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_laser_range_finder'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Laser Range Finder Bricklet

ipcon = IPConnection.new # Create IP connection
lrf = BrickletLaserRangeFinder.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Turn laser on and wait 250ms for very first measurement to be ready
lrf.enable_laser
sleep 0.25

# Register distance callback
lrf.register_callback(BrickletLaserRangeFinder::CALLBACK_DISTANCE) do |distance|
  puts "Distance: #{distance} cm"
end

# Set period for distance callback to 0.2s (200ms)
# Note: The distance callback is only called every 0.2 seconds
#       if the distance has changed since the last call!
lrf.set_distance_callback_period 200

puts 'Press key to exit'
$stdin.gets

lrf.disable_laser # Turn laser off

ipcon.disconnect
