use std::{error::Error, io, thread, time::Duration};
use tinkerforge::{ip_connection::IpConnection, laser_range_finder_bricklet::*};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Laser Range Finder Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let lrf = LaserRangeFinderBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Turn laser on and wait 250ms for very first measurement to be ready
    lrf.enable_laser().recv()?;
    thread::sleep(Duration::from_millis(250));

    // Get threshold receivers with a debounce time of 10 seconds (10000ms).
    lrf.set_debounce_period(10000);

    let distance_reached_receiver = lrf.get_distance_reached_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `lrf` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for distance_reached in distance_reached_receiver {
            println!("Distance: {} cm", distance_reached);
        }
    });

    // Configure threshold for distance "greater than 20 cm".
    lrf.set_distance_callback_threshold('>', 20, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;

    lrf.disable_laser().recv()?; // Turn laser off

    ipcon.disconnect();
    Ok(())
}
