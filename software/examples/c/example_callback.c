#define IPCON_EXPOSE_MILLISLEEP

#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_laser_range_finder.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your Laser Range Finder Bricklet

// Callback function for distance callback
void cb_distance(uint16_t distance, void *user_data) {
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

	// Register distance callback to function cb_distance
	laser_range_finder_register_callback(&lrf,
	                                     LASER_RANGE_FINDER_CALLBACK_DISTANCE,
	                                     (void (*)(void))cb_distance,
	                                     NULL);

	// Set period for distance callback to 0.2s (200ms)
	// Note: The distance callback is only called every 0.2 seconds
	//       if the distance has changed since the last call!
	laser_range_finder_set_distance_callback_period(&lrf, 200);

	printf("Press key to exit\n");
	getchar();

	laser_range_finder_disable_laser(&lrf); // Turn laser off

	laser_range_finder_destroy(&lrf);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
