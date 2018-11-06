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

    // Get threshold listeners with a debounce time of 10 seconds (10000ms)
    laser_range_finder_bricklet.set_debounce_period(10000);

    //Create listener for distance reached events.
    let distance_reached_listener = laser_range_finder_bricklet.get_distance_reached_receiver();
    // Spawn thread to handle received events. This thread ends when the laser_range_finder_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in distance_reached_listener {
            println!("Distance: {}{}", event, " cm");
        }
    });

    // Configure threshold for distance "greater than 20 cm"
    laser_range_finder_bricklet.set_distance_callback_threshold('>', 20, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    laser_range_finder_bricklet.disable_laser(); // Turn laser off
    ipcon.disconnect();
    Ok(())
}
