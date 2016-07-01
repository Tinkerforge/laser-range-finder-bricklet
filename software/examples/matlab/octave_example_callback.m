function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Laser Range Finder Bricklet

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    lrf = java_new("com.tinkerforge.BrickletLaserRangeFinder", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enableLaser();
    pause(0.25);

    % Register distance callback to function cb_distance
    lrf.addDistanceCallback(@cb_distance);

    % Set period for distance callback to 0.2s (200ms)
    % Note: The distance callback is only called every 0.2 seconds
    %       if the distance has changed since the last call!
    lrf.setDistanceCallbackPeriod(200);

    input("Press key to exit\n", "s");
    lrf.disableLaser(); % Turn laser off
    ipcon.disconnect();
end

% Callback function for distance callback (parameter has unit cm)
function cb_distance(e)
    fprintf("Distance: %d cm\n", e.distance);
end
