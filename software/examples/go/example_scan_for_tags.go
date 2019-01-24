package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/nfc_rfid_bricklet"
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

	currentTagType := nfc_rfid_bricklet.TagTypeMifareClassic

	nr.RegisterStateChangedCallback(func(state nfc_rfid_bricklet.State, idle bool) {
		if idle {
			currentTagType = (currentTagType + 1) % 3
			nr.RequestTagID(currentTagType)
		}
		if state == nfc_rfid_bricklet.StateRequestTagIDReady {
			tagType, IDLength, tagID, _ := nr.GetTagID()
			fmt.Printf("Found tag of type %d with ID %v\n", tagType, tagID[:IDLength])
		}
	})

	// Start scan loop
	nr.RequestTagID(nfc_rfid_bricklet.TagTypeMifareClassic)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
