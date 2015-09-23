#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletLaserRangeFinder;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

# Callback subroutine for distance callback (parameter has unit cm)
sub cb_distance
{
    my ($distance) = @_;

    print "Distance: $distance cm\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $lrf = Tinkerforge::BrickletLaserRangeFinder->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Turn laser on and wait 250ms for very first measurement to be ready
$lrf->enable_laser();
select(undef, undef, undef, 0.25);

# Register distance callback to subroutine cb_distance
$lrf->register_callback($lrf->CALLBACK_DISTANCE, 'cb_distance');

# Set period for distance callback to 0.2s (200ms)
# Note: The distance callback is only called every 0.2 seconds
#       if the distance has changed since the last call!
$lrf->set_distance_callback_period(200);

print "Press key to exit\n";
<STDIN>;
$lrf->disable_laser(); # Turn laser off
$ipcon->disconnect();
