function octave_example_threshold()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    lrf = java_new("com.tinkerforge.BrickletLaserRangeFinder", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enableLaser();
    pause(0.25);

    % Set threshold callbacks with a debounce time of 10 seconds (10000ms)
    lrf.setDebouncePeriod(10000);

    % Configure threshold for "greater than 20 cm"
    lrf.setDistanceCallbackThreshold(lrf.THRESHOLD_OPTION_GREATER, 20, 0);

    % Register threshold reached callback to function cb_reached
    lrf.addDistanceReachedCallback(@cb_reached);

    input("Press any key to exit...\n", "s");
    lrf.disableLaser(); % Turn laser off
    ipcon.disconnect();
end

% Callback function for distance callback (parameter has unit cm)
function cb_reached(e)
    fprintf("Distance: %g cm\n", e.distance);
end
