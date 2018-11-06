use std::{error::Error, io, thread, time::Duration};
use tinkerforge::{ipconnection::IpConnection, laser_range_finder_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Laser Range Finder Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let laser_range_finder_bricklet = LaserRangeFinderBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Turn laser on and wait 250ms for very first measurement to be ready
    laser_range_finder_bricklet.enable_laser();
    thread::sleep(Duration::from_millis(250));

    // Get current distance
    let distance = laser_range_finder_bricklet.get_distance().recv()?;
    println!("Distance: {}{}", distance, " cm");

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    laser_range_finder_bricklet.disable_laser(); // Turn laser off
    ipcon.disconnect();
    Ok(())
}
