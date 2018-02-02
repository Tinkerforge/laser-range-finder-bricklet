<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletLaserRangeFinder.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletLaserRangeFinder;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Laser Range Finder Bricklet

// Callback function for distance callback
function cb_distance($distance)
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

// Register distance callback to function cb_distance
$lrf->registerCallback(BrickletLaserRangeFinder::CALLBACK_DISTANCE, 'cb_distance');

// Set period for distance callback to 0.2s (200ms)
// Note: The distance callback is only called every 0.2 seconds
//       if the distance has changed since the last call!
$lrf->setDistanceCallbackPeriod(200);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
