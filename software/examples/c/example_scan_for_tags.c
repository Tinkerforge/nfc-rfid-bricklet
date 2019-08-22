#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_nfc_rfid.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your NFC/RFID Bricklet

uint8_t current_tag_type = NFC_RFID_TAG_TYPE_MIFARE_CLASSIC;

// Callback function for state changed callback
void cb_state_changed(uint8_t state, bool idle, void *user_data) {
	NFCRFID *nr = (NFCRFID *)user_data;

	if(state == NFC_RFID_STATE_REQUEST_TAG_ID_READY) {
		uint8_t tag_type;
		uint8_t tid_length;
		uint8_t tid[7];

		nfc_rfid_get_tag_id(nr, &tag_type, &tid_length, tid);

		if(tid_length == 4) {
			printf("Found tag of type %d with ID [%x %x %x %x]\n", 
			       tag_type, tid[0], tid[1], tid[2], tid[3]);
		} else {
			printf("Found tag of type %d with ID [%x %x %x %x %x %x %x]\n", 
			       tag_type, tid[0], tid[1], tid[2], tid[3], tid[4], tid[5], tid[6]);
		}
	}

	// Cycle through all types
	if(idle) {
		current_tag_type = (current_tag_type + 1) % 3;

		nfc_rfid_request_tag_id(nr, current_tag_type);
	}
}

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	NFCRFID nr;
	nfc_rfid_create(&nr, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Register state changed callback to function cb_state_changed
	nfc_rfid_register_callback(&nr,
	                           NFC_RFID_CALLBACK_STATE_CHANGED,
	                           (void (*)(void))cb_state_changed,
	                           &nr);

	// Start scan loop
	nfc_rfid_request_tag_id(&nr, NFC_RFID_TAG_TYPE_MIFARE_CLASSIC);

	printf("Press key to exit\n");
	getchar();
	nfc_rfid_destroy(&nr);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
