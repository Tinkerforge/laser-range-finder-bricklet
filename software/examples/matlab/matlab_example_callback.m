function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletLaserRangeFinder;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change to your UID

    ipcon = IPConnection(); % Create IP connection
    lrf = handle(BrickletLaserRangeFinder(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enableLaser();
    pause(0.25);

    % Register distance callback to function cb_distance
    set(lrf, 'DistanceCallback', @(h, e) cb_distance(e));

    % Set period for distance callback to 0.2s (200ms)
    % Note: The distance callback is only called every 0.2 seconds
    %       if the distance has changed since the last call!
    lrf.setDistanceCallbackPeriod(200);

    input('Press key to exit\n', 's');
    lrf.disableLaser(); % Turn laser off
    ipcon.disconnect();
end

% Callback function for distance callback (parameter has unit cm)
function cb_distance(e)
    fprintf('Distance: %i cm\n', e.distance);
end
