#define IPCON_EXPOSE_MILLISLEEP

#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_laser_range_finder.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your Laser Range Finder Bricklet

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

	// Get current distance
	uint16_t distance;
	if(laser_range_finder_get_distance(&lrf, &distance) < 0) {
		fprintf(stderr, "Could not get distance, probably timeout\n");
		return 1;
	}

	printf("Distance: %u cm\n", distance);

	printf("Press key to exit\n");
	getchar();

	laser_range_finder_disable_laser(&lrf); // Turn laser off

	laser_range_finder_destroy(&lrf);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
