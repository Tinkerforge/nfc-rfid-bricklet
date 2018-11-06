use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, nfc_rfid_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your NFC/RFID Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let nfc_rfid_bricklet = NFCRFIDBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    //Create listener for state changed events.
    let state_changed_listener = nfc_rfid_bricklet.get_state_changed_receiver();
    // Spawn thread to handle received events. This thread ends when the nfc_rfid_bricklet
    // is dropped, so there is no need for manual cleanup.
    let nfc_rfid_bricklet_copy = nfc_rfid_bricklet.clone(); //Device objects don't implement Sync, so they can't be shared between threads (by reference). So clone the device and move the copy.
    thread::spawn(move || {
        let mut current_tag_type = NFC_RFID_BRICKLET_TAG_TYPE_MIFARE_CLASSIC;
        for event in state_changed_listener {
            if event.idle {
                current_tag_type = (current_tag_type + 1) % 3;
                nfc_rfid_bricklet_copy.request_tag_id(current_tag_type);
            }

            if event.state == NFC_RFID_BRICKLET_STATE_REQUEST_TAG_ID_READY {
                let tag_id = nfc_rfid_bricklet_copy.get_tag_id().recv().unwrap();
                println!("Found tag of type {} with ID {:x?}", tag_id.tag_type, tag_id.tid[0..tag_id.tid_length as usize].to_vec());
            }
        }
    });

    // Start scan loop
    nfc_rfid_bricklet.request_tag_id(NFC_RFID_BRICKLET_TAG_TYPE_MIFARE_CLASSIC);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
