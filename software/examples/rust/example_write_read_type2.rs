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

    //Create receiver for state changed events.
    let state_changed_receiver = nr.get_state_changed_receiver();
    // Spawn thread to handle received events. This thread ends when the nr
    // is dropped, so there is no need for manual cleanup.
    let nr_copy = nr.clone(); //Device objects don't implement Sync, so they can't be shared between threads (by reference). So clone the device and move the copy.
    thread::spawn(move || {
        for state_change in state_changed_receiver {
            if state_change.state == NFC_RFID_BRICKLET_STATE_REQUEST_TAG_ID_READY {
                println!("Tag found");

                // Write 16 byte to pages 5-8
                let data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

                nr_copy.write_page(5, data_write);
                println!("Writing data...");
            } else if state_change.state == NFC_RFID_BRICKLET_STATE_WRITE_PAGE_READY {
                // Request pages 5-8
                nr_copy.request_page(5);
                println!("Requesting data...");
            } else if state_change.state == NFC_RFID_BRICKLET_STATE_REQUEST_PAGE_READY {
                // Get and print pages
                let data = nr_copy.get_page().recv().unwrap();

                println!("Read data: {:x?}", data);
            } else if (state_change.state & (1 << 6)) == (1 << 6) {
                // All errors have bit 6 set
                println!("Error: {}", state_change.state);
            }
        }
    });

    // Select NFC Forum Type 2 tag
    nr.request_tag_id(NFC_RFID_BRICKLET_TAG_TYPE_TYPE2);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
