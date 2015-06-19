function matlab_example_callback()
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

    % Set Period for distance callback to 1s (1000ms)
    % Note: The callback is only called every second if the
    %       distance has changed since the last call!
    lrf.setDistanceCallbackPeriod(1000);

    % Register distance callback to function cb_distance
    set(lrf, 'DistanceCallback', @(h, e) cb_distance(e));

    input('Press any key to exit...\n', 's');
    lrf.disableLaser(); % Turn laser off
    ipcon.disconnect();
end

% Callback function for distance callback (parameter has unit cm)
function cb_distance(e)
    fprintf('Distance: %g cm\n', e.distance);
end
