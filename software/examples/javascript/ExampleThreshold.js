var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Laser Range Finder Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var lrf = new Tinkerforge.BrickletLaserRangeFinder(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Turn laser on and wait 250ms for very first measurement to be ready
        lrf.enableLaser();

        setTimeout(function () {
            // Get threshold callbacks with a debounce time of 10 seconds (10000ms)
            lrf.setDebouncePeriod(10000);

            // Configure threshold for distance "greater than 20 cm"
            lrf.setDistanceCallbackThreshold('>', 20, 0);
        }, 250);
    }
);

// Register distance reached callback
lrf.on(Tinkerforge.BrickletLaserRangeFinder.CALLBACK_DISTANCE_REACHED,
    // Callback function for distance reached callback
    function (distance) {
        console.log('Distance: ' + distance + ' cm');
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        lrf.disableLaser(); // Turn laser off
        ipcon.disconnect();
        process.exit(0);
    }
);
