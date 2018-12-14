package main

import (
	"fmt"
	"tinkerforge/ipconnection"
    "tinkerforge/nfc_rfid_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your NFC/RFID Bricklet.

func main() {
	ipcon := ipconnection.New()
    defer ipcon.Close()
	nr, _ := nfc_rfid_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
    defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	nr.RegisterStateChangedCallback(func(state nfc_rfid_bricklet.State, idle bool) {
		if state == nfc_rfid_bricklet.StateRequestTagIDReady {
			fmt.Println("Tag found")

			dataWrite := [16]uint8{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
			nr.WritePage(5, dataWrite)
			fmt.Println("Writing data...")
		} else if state == nfc_rfid_bricklet.StateWritePageReady {
			nr.RequestPage(5)
			fmt.Println("Requesting data...")
		} else if state == nfc_rfid_bricklet.StateRequestPageReady {
			data, _ := nr.GetPage()
			fmt.Printf("Read data: %v\n", data)
		} else if state&(1<<6) == (1 << 6) {
			fmt.Printf("Error: %v\n", state)
		}
	})

	// Select NFC Forum Type 2 tag
	nr.RequestTagID(nfc_rfid_bricklet.TagTypeType2)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

	ipcon.Disconnect()
}
