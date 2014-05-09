/* nfc/rfid-bricklet
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * nfc-rfid.c: Implementation of NFC/RFID Bricklet messages
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "nfc-rfid.h"

#include "brickletlib/bricklet_entry.h"
#include "brickletlib/bricklet_debug.h"
#include "bricklib/bricklet/bricklet_communication.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/util_definitions.h"
#include "config.h"
#include "bricklib/logging/logging.h"

#define I2C_EEPROM_ADDRESS_HIGH 84


void invocation(const ComType com, const uint8_t *data) {
}

void constructor(void) {
	PIN_NSS.type = PIO_OUTPUT_1;
	PIN_NSS.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_NSS, 1);

	PIN_SCK.type = PIO_OUTPUT_1;
	PIN_SCK.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_SCK, 1);

	PIN_MOSI.type = PIO_OUTPUT_1;
	PIN_MOSI.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_MOSI, 1);

	PIN_MISO.type = PIO_INPUT;
	PIN_MISO.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_MISO, 1);

	BC->state = 0;
	BC->tick = 0;
	BC->counter = 0;

	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;
	SLEEP_MS(1100);

   	pn532_sam_configure_mode_normal();
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		BC->tick++;
        if(BC->tick % 500 == 0) {
        	pn532_read_passive_target_id();
        }
	}
}

bool pn532_read_passive_target_id(void) {
	uint8_t command[] = {
		PN532_COMMAND_INLISTPASSIVETARGET,
		1, // One card
		PN532_MIFARE_ISO14443A
	};

	if(pn532_send_data(command, sizeof(command))) {
		SLEEP_MS(1);
		uint8_t response[20] = {0};
		if(pn532_read_response(response, 20)) {

			uint8_t length = response[12];
			uint8_t uid[4] = {0};
			for(uint8_t i = 0; i < length; i++) {
				uid[i] = response[13+i];
			}

			BA->printf("UID: ");
			for(uint8_t i = 0; i < 4; i++) {
				BA->printf("%x ", uid[i]);
			}
			BA->printf("\n\r");
		} else {
			BA->printf("No response\n\r");
		}
	} else {
		BA->printf("Could not send data\n\r");
		return false;
	}

	return true;
}

bool pn532_sam_configure_mode_normal(void) {
	uint8_t command[] = {
		PN532_COMMAND_SAMCONFIGURATION,
		PN532_SAMC_MODE_NORMAL,
		PN532_SAMC_TIMEOUT_1S,
		PN532_SAMC_IRQ_OFF
	};

	if(pn532_send_data(command, sizeof(command))) {
		uint8_t response[9];
		if(pn532_read_response(response, 9)) {
			if(response[5] == 0xD5 && response[6] == 0x15) {
				BA->printf("pn532_sam_configure_mode_normal OK\n\r");
				return true;
			}
		}
	}

	BA->printf("pn532_sam_configure_mode_normal FAIL\n\r");
	return false;
}

bool pn532_wait_until_ready(uint32_t timeout) {
	while(timeout--) {
		uint8_t status = pn532_status();
		if(status == PN532_SPI_READY) {
			return true;
		}
	}

	return false;
}

uint8_t pn532_status(void) {
	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;

	spibb_transceive_byte(PN532_SPI_STATREAD);
	const uint8_t ret = spibb_transceive_byte(0x00);

	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;

	return ret;
}

bool pn532_read_ack(uint32_t timeout) {
	if(!pn532_wait_until_ready(PN532_WAIT_UNTIL_READY_TIMEOUT)) {
		return false;
	}

	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;
	spibb_transceive_byte(PN532_SPI_DATAREAD);

	uint8_t state = 0;
	while(timeout--) {
		switch(state) {
			case 0: {
				uint8_t data = spibb_transceive_byte(0x00);
				if(data == 0x00) { // Start of Packet Code (first byte)
					state = 1;
				}

				break;
			}

			case 1: {
				uint8_t data = spibb_transceive_byte(0x00);
				if(data == 0xFF) { // Start of Packet Code (second byte)
					state = 2;
				} else if(data == 0x00) {
					state = 1;
				} else {
					state = 0;
				}

				break;
			}

			case 2: {
				uint8_t data[3];
				data[0] = spibb_transceive_byte(0x00);
				data[1] = spibb_transceive_byte(0x00);
				data[2] = spibb_transceive_byte(0x00);

				// Test for ACK Packet Code, ignore Postamble
				// see 6.2.1.6: The postamble field is composed of an undetermined number of bytes
				if(data[0] == 0x00 && data[1] == 0xFF) {
					PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;
					return true;
				}

				PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;
				return false;
			}
		}
	}

	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;
	return false;
}

bool pn532_read_response(uint8_t *data, uint8_t length) {
	if(!pn532_wait_until_ready(PN532_WAIT_UNTIL_READY_TIMEOUT)) {
		return false;
	}

	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;

	spibb_transceive_byte(PN532_SPI_DATAREAD);

	for(uint8_t i = 0; i < length; i++) {
		data[i] = spibb_transceive_byte(0x00);
	}

	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;

	return true;
}

bool pn532_send_data(uint8_t *data, uint8_t length) {
	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;

	spibb_transceive_byte(PN532_SPI_DATAWRITE);

	spibb_transceive_byte(PN532_PREAMBLE);
	spibb_transceive_byte(PN532_PREAMBLE);
	spibb_transceive_byte(PN532_STARTCODE2);
	spibb_transceive_byte(length+1);
	spibb_transceive_byte(256 - (length+1));
	spibb_transceive_byte(PN532_HOSTTOPN532);

	uint8_t checksum = PN532_HOSTTOPN532;

	for(uint8_t i = 0; i < length; i++) {
		spibb_transceive_byte(data[i]);
		checksum += data[i];
	}

	spibb_transceive_byte(256 - checksum);
	spibb_transceive_byte(PN532_POSTAMBLE);
	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;

	if(pn532_read_ack(PN532_READ_ACK_TIMEOUT)) {
		return true;
	}

	return false;
}

uint8_t spibb_transceive_byte(const uint8_t value) {
	uint8_t recv = 0;

	for(int8_t i = 0; i < 8; i++) {
		PIN_SCK.pio->PIO_CODR = PIN_SCK.mask;
		if((value >> i) & 1) {
			PIN_MOSI.pio->PIO_SODR = PIN_MOSI.mask;
		} else {
			PIN_MOSI.pio->PIO_CODR = PIN_MOSI.mask;
		}

		SLEEP_US(1);
		if(PIN_MISO.pio->PIO_PDSR & PIN_MISO.mask) {
			recv |= (1 << i);
		}

		PIN_SCK.pio->PIO_SODR = PIN_SCK.mask;
		SLEEP_US(1);
	}

	PIN_SCK.pio->PIO_CODR = PIN_SCK.mask;
	PIN_MOSI.pio->PIO_CODR = PIN_MOSI.mask;
	return recv;
}
