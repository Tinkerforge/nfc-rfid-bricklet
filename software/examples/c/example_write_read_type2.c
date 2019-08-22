#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_nfc_rfid.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your NFC/RFID Bricklet

// Callback function for state changed callback
void cb_state_changed(uint8_t state, bool idle, void *user_data) {
	NFCRFID *nr = (NFCRFID *)user_data;

	(void)idle; // avoid unused parameter warning

	if(state == NFC_RFID_STATE_REQUEST_TAG_ID_READY) {
		printf("Tag found\n");

		// Write 16 byte to pages 5-8
		uint8_t data_write[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

		nfc_rfid_write_page(nr, 5, data_write);

		printf("Writing data...\n");
	} else if(state == NFC_RFID_STATE_WRITE_PAGE_READY) {
		// Request pages 5-8
		nfc_rfid_request_page(nr, 5);

		printf("Requesting data...\n");
	} else if(state == NFC_RFID_STATE_REQUEST_PAGE_READY) {
		uint8_t data_read[16];

		// Get and print pages 5-8
		nfc_rfid_get_page(nr, data_read);
		printf("Read data: [%d", data_read[0]);

		int i;
		for(i = 1; i < 16; i++) {
			printf(" %d", data_read[i]);
		}

		printf("]\n");
	} else if(state & (1 << 6)) {
		// All errors have bit 6 set
		printf("Error: %d\n", state);
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

	// Select NFC Forum Type 2 tag
	nfc_rfid_request_tag_id(&nr, NFC_RFID_TAG_TYPE_TYPE2);

	printf("Press key to exit\n");
	getchar();
	nfc_rfid_destroy(&nr);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
