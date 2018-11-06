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

    //Create listener for distance events.
    let distance_listener = laser_range_finder_bricklet.get_distance_receiver();
    // Spawn thread to handle received events. This thread ends when the laser_range_finder_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in distance_listener {
            println!("Distance: {}{}", event, " cm");
        }
    });

    // Set period for distance listener to 0.2s (200ms)
    // Note: The distance callback is only called every 0.2 seconds
    //       if the distance has changed since the last call!
    laser_range_finder_bricklet.set_distance_callback_period(200);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    laser_range_finder_bricklet.disable_laser(); // Turn laser off
    ipcon.disconnect();
    Ok(())
}
