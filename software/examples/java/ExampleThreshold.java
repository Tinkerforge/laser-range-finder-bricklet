import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletLaserRangeFinder;

public class ExampleThreshold {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Laser Range Finder Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletLaserRangeFinder lrf =
		  new BrickletLaserRangeFinder(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Turn laser on and wait 250ms for very first measurement to be ready
		lrf.enableLaser();
		Thread.sleep(250);

		// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
		lrf.setDebouncePeriod(10000);

		// Add distance reached listener
		lrf.addDistanceReachedListener(new BrickletLaserRangeFinder.DistanceReachedListener() {
			public void distanceReached(int distance) {
				System.out.println("Distance: " + distance + " cm");
			}
		});

		// Configure threshold for distance "greater than 20 cm"
		lrf.setDistanceCallbackThreshold('>', 20, 0);

		System.out.println("Press key to exit"); System.in.read();

		lrf.disableLaser(); // Turn laser off

		ipcon.disconnect();
	}
}
