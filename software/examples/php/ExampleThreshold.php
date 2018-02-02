<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletLaserRangeFinder.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletLaserRangeFinder;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Laser Range Finder Bricklet

// Callback function for distance reached callback
function cb_distanceReached($distance)
{
    echo "Distance: $distance cm\n";
}

$ipcon = new IPConnection(); // Create IP connection
$lrf = new BrickletLaserRangeFinder(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Turn laser on and wait 250ms for very first measurement to be ready
$lrf->enableLaser();
usleep(250*1000);

// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
$lrf->setDebouncePeriod(10000);

// Register distance reached callback to function cb_distanceReached
$lrf->registerCallback(BrickletLaserRangeFinder::CALLBACK_DISTANCE_REACHED,
                       'cb_distanceReached');

// Configure threshold for distance "greater than 20 cm"
$lrf->setDistanceCallbackThreshold('>', 20, 0);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
