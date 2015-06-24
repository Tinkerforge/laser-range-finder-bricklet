#ifndef _BSD_SOURCE
 #define _BSD_SOURCE // for usleep
#endif

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

	// Get current distance (unit is cm)
	uint16_t distance;
	if(laser_range_finder_get_distance(&lrf, &distance) < 0) {
		fprintf(stderr, "Could not get value, probably timeout\n");
		exit(1);
	}

	printf("Distance: %d cm\n", distance);

	printf("Press key to exit\n");
	getchar();
	laser_range_finder_disable_laser(&lrf); // Turn laser off
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
