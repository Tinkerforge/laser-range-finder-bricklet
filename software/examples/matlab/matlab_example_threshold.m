function matlab_example_threshold()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletLaserRangeFinder;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Laser Range Finder Bricklet

    ipcon = IPConnection(); % Create IP connection
    lrf = handle(BrickletLaserRangeFinder(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enableLaser();
    pause(0.25);

    % Get threshold callbacks with a debounce time of 10 seconds (10000ms)
    lrf.setDebouncePeriod(10000);

    % Register distance reached callback to function cb_distance_reached
    set(lrf, 'DistanceReachedCallback', @(h, e) cb_distance_reached(e));

    % Configure threshold for distance "greater than 20 cm"
    lrf.setDistanceCallbackThreshold('>', 20, 0);

    input('Press key to exit\n', 's');

    lrf.disableLaser(); % Turn laser off

    ipcon.disconnect();
end

% Callback function for distance reached callback
function cb_distance_reached(e)
    fprintf('Distance: %i cm\n', e.distance);
end
