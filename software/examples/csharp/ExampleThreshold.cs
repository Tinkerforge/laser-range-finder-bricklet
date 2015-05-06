using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	// Callback for distance greater than 20 cm
	static void ReachedCB(BrickletLaserRangeFinder sender, int distance)
	{
		System.Console.WriteLine("Distance: " + distance + " cm.");
	}

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletLaserRangeFinder lrf = new BrickletLaserRangeFinder(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Turn laser on and wait 250ms for very first measurement to be ready
		lrf.EnableLaser();
		System.Threading.Thread.Sleep(250);

		// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
		lrf.SetDebouncePeriod(10000);

		// Register threshold reached callback to function ReachedCB
		lrf.DistanceReached += ReachedCB;

		// Configure threshold for "greater than 20 cm"
		lrf.SetDistanceCallbackThreshold('>', 20, 0);

		System.Console.WriteLine("Press enter to exit");
		System.Console.ReadLine();
		ipcon.Disconnect();
	}
}
