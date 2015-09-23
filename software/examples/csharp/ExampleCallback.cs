using System;
using System.Threading;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	// Callback function for distance callback (parameter has unit cm)
	static void DistanceCB(BrickletLaserRangeFinder sender, int distance)
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

		// Register distance callback to function DistanceCB
		lrf.Distance += DistanceCB;

		// Set period for distance callback to 0.2s (200ms)
		// Note: The distance callback is only called every 0.2 seconds
		//       if the distance has changed since the last call!
		lrf.SetDistanceCallbackPeriod(200);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		lrf.DisableLaser(); // Turn laser off
		ipcon.Disconnect();
	}
}
