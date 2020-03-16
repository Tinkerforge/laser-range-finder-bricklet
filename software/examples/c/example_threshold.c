#define IPCON_EXPOSE_MILLISLEEP

#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_laser_range_finder.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your Laser Range Finder Bricklet

// Callback function for distance reached callback
void cb_distance_reached(uint16_t distance, void *user_data) {
	(void)user_data; // avoid unused parameter warning

	printf("Distance: %u cm\n", distance);
}

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	LaserRangeFinder lrf;
	laser_range_finder_create(&lrf, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Turn laser on and wait 250ms for very first measurement to be ready
	laser_range_finder_enable_laser(&lrf);
	millisleep(250);

	// Get threshold callbacks with a debounce time of 10 seconds (10000ms)
	laser_range_finder_set_debounce_period(&lrf, 10000);

	// Register distance reached callback to function cb_distance_reached
	laser_range_finder_register_callback(&lrf,
	                                     LASER_RANGE_FINDER_CALLBACK_DISTANCE_REACHED,
	                                     (void (*)(void))cb_distance_reached,
	                                     NULL);

	// Configure threshold for distance "greater than 20 cm"
	laser_range_finder_set_distance_callback_threshold(&lrf, '>', 20, 0);

	printf("Press key to exit\n");
	getchar();

	laser_range_finder_disable_laser(&lrf); // Turn laser off

	laser_range_finder_destroy(&lrf);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
