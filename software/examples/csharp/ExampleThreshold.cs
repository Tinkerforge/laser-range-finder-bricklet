using System;
using System.Threading;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Laser Range Finder Bricklet

	// Callback function for distance reached callback
	static void DistanceReachedCB(BrickletLaserRangeFinder sender, int distance)
	{
		Console.WriteLine("Distance: " + distance + " cm");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletLaserRangeFinder lrf =
		  new BrickletLaserRangeFinder(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Turn laser on and wait 250ms for very first measurement to be ready
		lrf.EnableLaser();
		Thread.Sleep(250);

		// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
		lrf.SetDebouncePeriod(10000);

		// Register distance reached callback to function DistanceReachedCB
		lrf.DistanceReachedCallback += DistanceReachedCB;

		// Configure threshold for distance "greater than 20 cm"
		lrf.SetDistanceCallbackThreshold('>', 20, 0);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		lrf.DisableLaser(); // Turn laser off
		ipcon.Disconnect();
	}
}
