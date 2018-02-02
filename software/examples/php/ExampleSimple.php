<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletLaserRangeFinder.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletLaserRangeFinder;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Laser Range Finder Bricklet

$ipcon = new IPConnection(); // Create IP connection
$lrf = new BrickletLaserRangeFinder(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Turn laser on and wait 250ms for very first measurement to be ready
$lrf->enableLaser();
usleep(250*1000);

// Get current distance
$distance = $lrf->getDistance();
echo "Distance: $distance cm\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$lrf->disableLaser(); // Turn laser off
$ipcon->disconnect();

?>
