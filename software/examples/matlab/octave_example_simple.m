function octave_example_simple()
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

    % Get current distance (unit is cm)
    distance = lrf.getDistance();
    fprintf("Distance: %d cm\n", distance);

    input("Press key to exit\n", "s");
    lrf.disableLaser(); % Turn laser off
    ipcon.disconnect();
end
