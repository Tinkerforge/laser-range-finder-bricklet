#include <stdio.h>

#ifdef WIN32
 #include <windows.h>
#else
 #include <unistd.h>
#endif

#include "ip_connection.h"
#include "bricklet_laser_range_finder.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change to your UID

// Callback function for distance callback (parameter has unit cm)
void cb_distance(uint16_t distance, void *user_data) {
	(void)user_data; // avoid unused parameter warning

	printf("Distance: %d cm\n", distance);
}

int main() {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	LaserRangeFinder lrf;
	laser_range_finder_create(&lrf, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		exit(1);
	}
	// Don't use device before ipcon is connected

	// Turn laser on and wait 250ms for very first measurement to be ready
	laser_range_finder_enable_laser(&lrf);

#ifdef WIN32
	Sleep(250);
#else
 	usleep(250 * 1000);
#endif

	// Set Period for distance callback to 1s (1000ms)
	// Note: The distance callback is only called every second if the
	//       distance has changed since the last call!
	laser_range_finder_set_distance_callback_period(&lrf, 1000);

	// Register distance callback to function cb_distance
	laser_range_finder_register_callback(&lrf,
	                                     LASER_RANGE_FINDER_CALLBACK_DISTANCE,
	                                     (void *)cb_distance,
	                                     NULL);

	printf("Press key to exit\n");
	getchar();
	laser_range_finder_disable_laser(&lrf); // Turn laser off
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
