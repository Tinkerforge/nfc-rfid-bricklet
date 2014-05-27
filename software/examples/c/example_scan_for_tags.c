
#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_nfc_rfid.h"

#define HOST "localhost"
#define PORT 4223
#define UID "hjw" // Change to your UID

// Callback function for state changed callback 
void cb_state_changed(uint8_t state, bool idle, void *user_data) {
	static uint8_t tag_type = 0;
	NFCRFID *nfc = (NFCRFID*)user_data;

	// Cycle through all types
	if(idle) {
		tag_type = (tag_type + 1) % 3;
		nfc_rfid_request_tag_id(nfc, tag_type);
	}

	if(state == NFC_RFID_STATE_REQUEST_TAG_ID_READY) {
		uint8_t ret_tag_type;
		uint8_t ret_tid_length;
		uint8_t ret_tid[7];
		nfc_rfid_get_tag_id(nfc, &ret_tag_type, &ret_tid_length, ret_tid);
		if(ret_tid_length == 4) {
			printf("Found tag of type %d with ID [%x %x %x %x]\n", 
			       ret_tag_type, ret_tid[0], ret_tid[1], ret_tid[2], ret_tid[3]);
		} else {
			printf("Found tag of type %d with ID [%x %x %x %x %x %x %x]\n", 
			       ret_tag_type, ret_tid[0], ret_tid[1], ret_tid[2], ret_tid[3], 
			       ret_tid[4], ret_tid[5], ret_tid[6]);
		}
	}
}

int main() {
	// Create ip connection to brickd
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	NFCRFID nfc;
	nfc_rfid_create(&nfc, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		exit(1);
	}
	// Don't use device before ipcon is connected

	// Register state changed callback to function cb_state_changed
	nfc_rfid_register_callback(&nfc,
	                           NFC_RFID_CALLBACK_STATE_CHANGED,
	                           (void *)cb_state_changed,
	                           &nfc);

	nfc_rfid_request_tag_id(&nfc, NFC_RFID_TAG_TYPE_MIFARE_CLASSIC);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
