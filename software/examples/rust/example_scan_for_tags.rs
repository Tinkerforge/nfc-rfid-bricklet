use std::{error::Error, io, thread};
use tinkerforge::{ip_connection::IpConnection, nfc_rfid_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your NFC/RFID Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let nr = NfcRfidBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd
                                          // Don't use device before ipcon is connected

    let state_changed_receiver = nr.get_state_changed_callback_receiver();

    // Spawn thread to handle received events.
    // This thread ends when the `nr` object
    // is dropped, so there is no need for manual cleanup.
    let nr_copy = nr.clone(); //Device objects don't implement Sync, so they can't be shared between threads (by reference). So clone the device and move the copy.
    thread::spawn(move || {
        let mut current_tag_type = NFC_RFID_BRICKLET_TAG_TYPE_MIFARE_CLASSIC;
        for state_change in state_changed_receiver {
            if state_change.idle {
                current_tag_type = (current_tag_type + 1) % 3;
                nr_copy.request_tag_id(current_tag_type);
            }

            if state_change.state == NFC_RFID_BRICKLET_STATE_REQUEST_TAG_ID_READY {
                let tag_id = nr_copy.get_tag_id().recv().unwrap();
                println!("Found tag of type {} with ID {:x?}", tag_id.tag_type, tag_id.tid[0..tag_id.tid_length as usize].to_vec());
            }
        }
    });

    // Start scan loop
    nr.request_tag_id(NFC_RFID_BRICKLET_TAG_TYPE_MIFARE_CLASSIC);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
