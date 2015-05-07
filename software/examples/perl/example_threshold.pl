#!/usr/bin/perl  

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletLaserRangeFinder;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $lrf = Tinkerforge::BrickletLaserRangeFinder->new(&UID, $ipcon); # Create device object

# Callback for distance greater than 20 cm
sub cb_reached
{
    my ($distance) = @_;

    print "Distance ".$distance." cm\n";
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get threshold callbacks with a debounce time of 10 seconds (10000ms)
$lrf->set_debounce_period(10000);

# Register threshold reached callback to function cb_reached
$lrf->register_callback($lrf->CALLBACK_DISTANCE_REACHED, 'cb_reached');

# Configure threshold for "greater than 20 cm"
$lrf->set_distance_callback_threshold('>', 20, 0);

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
