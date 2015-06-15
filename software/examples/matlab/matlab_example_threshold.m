function matlab_example_threshold()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletLaserRangeFinder;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change to your UID

    ipcon = IPConnection(); % Create IP connection
    lrf = BrickletLaserRangeFinder(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

	% Turn laser on and wait 250ms for very first measurement to be ready
	lrf.enableLaser();
	pause(0.25);

    % Set threshold callbacks with a debounce time of 10 seconds (10000ms)
    lrf.setDebouncePeriod(10000);

    % Register threshold reached callback to function cb_reached
    set(lrf, 'DistanceReachedCallback', @(h, e) cb_reached(e));

    % Configure threshold for "greater than 20 cm"
    lrf.setDistanceCallbackThreshold('>', 20, 0);

    input('Press any key to exit...\n', 's');
    ipcon.disconnect();
end

% Callback for distance greater than 20 cm
function cb_reached(e)
    fprintf('Distance: %g cm\n', e.distance);
end
