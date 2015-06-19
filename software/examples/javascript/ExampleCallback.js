var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change to your UID

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var lrf = new Tinkerforge.BrickletLaserRangeFinder(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function(error) {
        console.log('Error: '+error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        // Turn laser on and wait 250ms for very first measurement to be ready
        lrf.enableLaser();
        setTimeout(function() {
            // Set Period for distance callback to 1s (1000ms)
            // Note: The distance callback is only called every second if the
            // distance has changed since the last call!
            lrf.setDistanceCallbackPeriod(1000);
        }, 250);
    }
);

// Register position callback
lrf.on(Tinkerforge.BrickletLaserRangeFinder.CALLBACK_DISTANCE,
    // Callback function for distance callback (parameter has unit cm)
    function(distance) {
        console.log('Distance: '+distance+' cm');
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data',
    function(data) {
        lrf.disableLaser(); // Turn laser off
        ipcon.disconnect();
        process.exit(0);
    }
);
