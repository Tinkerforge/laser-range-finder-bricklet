use std::{io, error::Error};
use std::thread;
use std::time::Duration;
use tinkerforge::{ip_connection::IpConnection, 
                  laser_range_finder_bricklet::*};


const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Laser Range Finder Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let lrf = LaserRangeFinderBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
    // Don't use device before ipcon is connected.

		// Turn laser on and wait 250ms for very first measurement to be ready
		lrf.enable_laser();
		thread::sleep(Duration::from_millis(250));

		// Get current distance.
let distance = lrf.get_distance().recv()?;
		println!("Distance: {} cm", distance);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
		lrf.disable_laser(); // Turn laser off
    ipcon.disconnect();
    Ok(())
}
