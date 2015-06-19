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

// Callback for distance greater than 20 cm
void cb_reached(uint16_t distance, void *user_data) {
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

	// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
	laser_range_finder_set_debounce_period(&lrf, 10000);

	// Register threshold reached callback to function cb_reached
	laser_range_finder_register_callback(&lrf,
	                                     LASER_RANGE_FINDER_CALLBACK_DISTANCE_REACHED,
	                                     (void *)cb_reached,
	                                     NULL);

	// Configure threshold for "greater than 20 cm"
	laser_range_finder_set_distance_callback_threshold(&lrf, '>', 20, 0);

	printf("Press key to exit\n");
	getchar();
	laser_range_finder_disable_laser(&lrf); // Turn laser off
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
