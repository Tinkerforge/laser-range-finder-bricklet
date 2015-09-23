#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletLaserRangeFinder;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

# Callback subroutine for distance reached callback (parameter has unit cm)
sub cb_distance_reached
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

# Get threshold callbacks with a debounce time of 10 seconds (10000ms)
$lrf->set_debounce_period(10000);

# Register distance reached callback to subroutine cb_distance_reached
$lrf->register_callback($lrf->CALLBACK_DISTANCE_REACHED, 'cb_distance_reached');

# Configure threshold for distance "greater than 20 cm" (unit is cm)
$lrf->set_distance_callback_threshold('>', 20, 0);

print "Press key to exit\n";
<STDIN>;
$lrf->disable_laser(); # Turn laser off
$ipcon->disconnect();
