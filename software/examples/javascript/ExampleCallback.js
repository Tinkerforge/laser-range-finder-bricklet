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
            // Set period for distance callback to 0.2s (200ms)
            // Note: The distance callback is only called every 0.2 seconds
            //       if the distance has changed since the last call!
            lrf.setDistanceCallbackPeriod(200);
        }, 250);
    }
);

// Register distance callback
lrf.on(Tinkerforge.BrickletLaserRangeFinder.CALLBACK_DISTANCE,
    // Callback function for distance callback
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
