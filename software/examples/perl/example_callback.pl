#!/usr/bin/perl  

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletLaserRangeFinder;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $lrf = Tinkerforge::BrickletLaserRangeFinder->new(&UID, $ipcon); # Create device object

# Callback function for distance callback (parameter has unit cm)
sub cb_distance
{
    my ($distance) = @_;

    print "Distance: ".$distance." cm\n";
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set Period for distance callback to 1s (1000ms)
# Note: The distance callback is only called every second if the 
#       distance has changed since the last call!
$lrf->set_distance_callback_period(1000);

# Register distance callback to function cb_distance
$lrf->register_callback($lrf->CALLBACK_DISTANCE, 'cb_distance');

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
