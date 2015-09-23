#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletLaserRangeFinder;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $lrf = Tinkerforge::BrickletLaserRangeFinder->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Turn laser on and wait 250ms for very first measurement to be ready
$lrf->enable_laser();
select(undef, undef, undef, 0.25);

# Get current distance (unit is cm)
my $distance = $lrf->get_distance();
print "Distance: $distance cm\n";

print "Press key to exit\n";
<STDIN>;
$lrf->disable_laser(); # Turn laser off
$ipcon->disconnect();
