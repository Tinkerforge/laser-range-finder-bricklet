function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "amb"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    lrf = java_new("com.tinkerforge.BrickletLaserRangeFinder", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

	% Turn laser on and wait 250ms for very first measurement to be ready
	lrf.enableLaser();
	pause(0.25);

    % Get current distance (unit is cm)
    distance = lrf.getDistance();
    fprintf("Distance: %g cm\n", distance);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end
