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
	switch(((MessageHeader*)data)->fid) {
		case FID_REQUEST_TAG_ID: {
			request_tag_id(com, (RequestTagID*)data);
			return;
		}

		case FID_GET_TAG_ID: {
			get_tag_id(com, (GetTagID*)data);
			return;
		}

		case FID_GET_STATE: {
			get_state(com, (GetState*)data);
			return;
		}

		case FID_AUTHENTICATE_MIFARE_CLASSIC_PAGE: {
			authenticate_mifare_classic_page(com, (AuthenticateMifareClassicPage*)data);
			return;
		}

		case FID_WRITE_PAGE: {
			write_page(com, (WritePage*)data);
			return;
		}

		case FID_REQUEST_PAGE: {
			request_page(com, (RequestPage*)data);
			return;
		}

		case FID_GET_PAGE: {
			get_page(com, (GetPage*)data);
			return;
		}

		default: {
			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			return;
		}
	}
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

	BC->state = STATE_INITIALIZATION;
	BC->state_before = STATE_INITIALIZATION;

	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;
	SLEEP_MS(1100);
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(BC->state_wait_inner > 0) {
			BC->state_wait_inner--;
		}

		if(BC->state_wait == 0) {
			switch(BC->state) {
				case STATE_INITIALIZATION: {
					pn532_sam_configure_mode_normal();
					break;
				}

				case STATE_REQUEST_TAG_ID: {
					pn532_read_tag_id();
					break;
				}

				case STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE: {
					pn532_authenticate_mifare_classic();
					break;
				}

				case STATE_WRITE_PAGE: {
					pn532_write_page();
					break;
				}

				case STATE_REQUEST_PAGE: {
					pn532_read_page();
					break;
				}

				default: break;
			}
		} else {
			BC->state_wait--;
		}
	}

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(BC->state != BC->state_before) {
			GetStateReturn gsr;
			BA->com_make_default_header(&gsr, BS->uid, sizeof(GetStateReturn), FID_STATE_CHANGED);
			gsr.idle  = (BC->state & STATE_IDLE_MASK) ? true : false;
			gsr.state = BC->state;

			BA->send_blocking_with_timeout(&gsr,
										   sizeof(GetStateReturn),
										   *BA->com_current);

			BC->state_before = BC->state;
		}
	}
}


void request_tag_id(const ComType com, const RequestTagID *data) {
	if(BC->state & STATE_IDLE_MASK) {
		BC->tag_type = data->tag_type;
		BC->state = STATE_REQUEST_TAG_ID;
		BA->com_return_setter(com, data);
	} else {
		BA->com_return_error(data, sizeof(RequestTagID), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
	}
}

void get_tag_id(const ComType com, const GetTagID *data) {
	GetTagIDReturn gptidr;
	gptidr.header        = data->header;
	gptidr.header.length = sizeof(GetTagIDReturn);
	gptidr.tag_type      = BC->tid_tag_type;
	gptidr.tid_length    = BC->tid_length;

	for(uint8_t i = 0; i < TID_MAX_LENGTH; i++) {
		if(i < gptidr.tid_length) {
			gptidr.tid[i] = BC->tid[i];
		} else {
			gptidr.tid[i] = 0;
		}
	}

	BA->send_blocking_with_timeout(&gptidr, sizeof(GetTagIDReturn), com);
}

void get_state(const ComType com, const GetState *data) {
	GetStateReturn gsr;
	gsr.header        = data->header;
	gsr.header.length = sizeof(GetStateReturn);
	gsr.idle          = (BC->state & STATE_IDLE_MASK) ? true : false;
	gsr.state         = BC->state;

	BA->send_blocking_with_timeout(&gsr, sizeof(GetStateReturn), com);
}

void authenticate_mifare_classic_page(const ComType com, const AuthenticateMifareClassicPage *data) {
	if((BC->state & STATE_IDLE_MASK) && BC->tag_type == TAG_TYPE_MIFARE) {
		BC->authenticate_key_number = data->key_number;
		BC->authenticate_page = data->page;
		memcpy(BC->authenticate_key, data->key, KEY_MAX_LENGTH);
		memcpy(BC->authenticate_tid, BC->tid, TID_MIFARE_LENGTH);
		BC->state = STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE;
		BA->com_return_setter(com, data);
	} else {
		BA->com_return_error(data, sizeof(AuthenticateMifareClassicPage), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
	}
}

void write_page(const ComType com, const WritePage *data) {
	if(BC->state & STATE_IDLE_MASK) {
		BC->write_byte_i = 0;
		memcpy(BC->data_write, data->data, PAGE_MAX_LENGTH);
		BC->data_write_page = data->page;
		BC->state = STATE_WRITE_PAGE;
		BA->com_return_setter(com, data);
	} else {
		BA->com_return_error(data, sizeof(WritePage), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
	}
}

void request_page(const ComType com, const RequestPage *data) {
	if(BC->state & STATE_IDLE_MASK) {
		BC->read_byte_i = 0;
		BC->data_read_page = data->page;
		BC->state = STATE_REQUEST_PAGE;
		BA->com_return_setter(com, data);
	} else {
		BA->com_return_error(data, sizeof(RequestPage), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
	}
}

void get_page(const ComType com, const GetPage *data) {
	GetPageReturn gpr;
	gpr.header        = data->header;
	gpr.header.length = sizeof(GetPageReturn);
	memcpy(gpr.data, BC->data_read, PAGE_MAX_LENGTH);

	BA->send_blocking_with_timeout(&gpr, sizeof(GetPageReturn), com);
}

void pn532_authenticate_mifare_classic(void) {
	switch(BC->state_inner) {
		case STATE_INNER_SEND_DATA: {
			uint8_t *command = BC->buffer;
			command[0] = PN532_COMMAND_INDATAEXCHANGE;
			command[1] = 1;
			command[2] = BC->authenticate_key_number == 0 ? MIFARE_CMD_AUTH_A : MIFARE_CMD_AUTH_B;
			command[3] = BC->authenticate_page;
			memcpy(&command[4], BC->authenticate_key, KEY_MAX_LENGTH);
			memcpy(&command[10], BC->authenticate_tid, TID_MIFARE_LENGTH);

			if(BC->state_wait_ack == 0) {
				pn532_send_data_wo_ack(command, 14);
				BC->state_wait_ack = TIMEOUT_WAIT_FOR_ACK;
			} else {
				if(pn532_read_ack(TIMEOUT_WAIT_READ_ACK_DATA)) {
					BC->state_wait = 1;
					BC->state_wait_inner = TIMEOUT_WAIT_INNER_STATE;
					BC->state_wait_ack = 0;
					BC->state_inner = STATE_INNER_READ_RESPONSE;
				} else {
					BC->state_wait_ack--;
					if(BC->state_wait_ack == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_ERROR;
					}
				}
			}

			break;
		}

		case STATE_INNER_READ_RESPONSE: {
			uint8_t *response = BC->buffer;
			if(pn532_read_response(response, BUFFER_LENGTH) >= 0) {

				if(response[1] == 0) {
					BC->state_inner = 0;
					BC->state_wait = 1;
					BC->state = STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_READY;
				} else {
					BC->state_inner = 0;
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state = STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_ERROR;
				}
			} else {
				if(BC->state_wait_inner == 0) {
					BC->state_inner = 0;
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state = STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_ERROR;
				}
			}
			break;
		}
	}
}

void pn532_write_page(void) {
	switch(BC->state_inner) {
		case STATE_INNER_SEND_DATA: {
			uint8_t *command = BC->buffer;
			command[0] = PN532_COMMAND_INDATAEXCHANGE;
			command[1] = 1;
			uint8_t write_length = 0;
			switch(BC->tag_type) {
				case TAG_TYPE_MIFARE: {
					command[2] = MIFARE_CMD_WRITE;
					command[3] = BC->data_write_page;
					memcpy(&command[4], BC->data_write, PAGE_MAX_LENGTH);
					write_length = 4 + 16;

					break;
				}

				case TAG_TYPE_TYPE1: {
					command[2] = JEWEL_CMD_WRITEE;
					command[3] = ((BC->data_write_page + BC->write_byte_i/8) << 3) + (BC->write_byte_i % 8);
					command[4] = BC->data_write[BC->write_byte_i];
					write_length = 4 + 1;

					break;
				}

				case TAG_TYPE_TYPE2: {
					command[2] = TYPE2_CMD_WRITE;
					command[3] = BC->data_write_page + BC->write_byte_i/4;
					for(uint8_t i = 0; i < 4; i++) {
						command[4 + i] = BC->data_write[i + BC->write_byte_i];
					}
					write_length = 4 + 4;

					break;
				}

				default: {
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state_inner = 0;
					BC->state_wait_ack = 0;
					BC->state = STATE_WRITE_PAGE_ERROR;

					return;
				}
			}

			if(BC->state_wait_ack == 0) {
				pn532_send_data_wo_ack(command, write_length);
				BC->state_wait_ack = TIMEOUT_WAIT_FOR_ACK;
			} else {
				if(pn532_read_ack(TIMEOUT_WAIT_READ_ACK_DATA)) {
					BC->state_wait = 1;
					BC->state_wait_inner = TIMEOUT_WAIT_INNER_STATE;
					BC->state_wait_ack = 0;
					BC->state_inner = STATE_INNER_READ_RESPONSE;
				} else {
					BC->state_wait_ack--;
					if(BC->state_wait_ack == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_WRITE_PAGE_ERROR;
					}
				}
			}

			break;
		}

		case STATE_INNER_READ_RESPONSE: {
			uint8_t *response = BC->buffer;
			if(pn532_read_response(response, BUFFER_LENGTH) >= 0) {
				switch(BC->tag_type) {
					case TAG_TYPE_MIFARE: {
						BC->write_byte_i = 16;
						break;
					}

					case TAG_TYPE_TYPE1: {
						BC->write_byte_i++;
						break;
					}

					case TAG_TYPE_TYPE2: {
						BC->write_byte_i+=4;
						break;
					}

					default: {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_WRITE_PAGE_ERROR;

						return;
					}
				}

				if(BC->write_byte_i == 16) {
					BC->state_inner = 0;
					BC->state_wait = 1;
					BC->state = STATE_WRITE_PAGE_READY;
				} else {
					BC->state_inner = STATE_INNER_SEND_DATA;
					BC->state_wait = 0;
				}
			} else {
				if(BC->state_wait_inner == 0) {
					BC->state_inner = 0;
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state = STATE_WRITE_PAGE_ERROR;
				}
			}
			break;
		}
	}
}

void pn532_read_page(void) {
	switch(BC->state_inner) {
		case STATE_INNER_SEND_DATA: {
			uint8_t command[4] = {
				PN532_COMMAND_INDATAEXCHANGE,
				1,
				MIFARE_CMD_READ,
				BC->data_read_page
			};

			if(BC->tag_type == TAG_TYPE_TYPE1) {
				command[2] = JEWEL_CMD_READ;
				command[3] = ((BC->data_read_page + BC->read_byte_i/8) << 3) + (BC->read_byte_i % 8);
			}

			if(BC->state_wait_ack == 0) {
				pn532_send_data_wo_ack(command, sizeof(command));
				BC->state_wait_ack = TIMEOUT_WAIT_FOR_ACK;
			} else {
				if(pn532_read_ack(TIMEOUT_WAIT_READ_ACK_DATA)) {
					BC->state_wait = 1;
					BC->state_wait_inner = TIMEOUT_WAIT_INNER_STATE;
					BC->state_wait_ack = 0;
					BC->state_inner = STATE_INNER_READ_RESPONSE;
				} else {
					BC->state_wait_ack--;
					if(BC->state_wait_ack == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_REQUEST_PAGE_ERROR;
					}
				}
			}

			break;
		}

		case STATE_INNER_READ_RESPONSE: {
			uint8_t *response = BC->buffer;

			if(BC->tag_type == TAG_TYPE_TYPE1) {
				if(pn532_read_response(response, BUFFER_LENGTH) >= 0) {
					BC->data_read[BC->read_byte_i] = response[2];

					BC->read_byte_i++;
					if(BC->read_byte_i == 16) {
						BC->state_inner = 0;
						BC->state_wait = 1;
						BC->state = STATE_REQUEST_PAGE_READY;
					} else {
						BC->state_inner = STATE_INNER_SEND_DATA;
						BC->state_wait = 0;
					}
				} else {
					if(BC->state_wait_inner == 0) {
						BC->state_inner = 0;
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state = STATE_REQUEST_PAGE_ERROR;
					}
				}
			} else {
				if(pn532_read_response(response, BUFFER_LENGTH) > 3) {
					for(uint8_t i = 0; i < 16; i++) {
						BC->data_read[i] = response[i+2];
					}

					BC->state_wait = 1;
					BC->state_inner = 0;
					BC->state = STATE_REQUEST_PAGE_READY;
				} else {
					if(BC->state_wait_inner == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_REQUEST_PAGE_ERROR;
					}
				}
			}
			break;
		}
	}
}

void pn532_read_tag_id(void) {
	switch(BC->state_inner) {
		case STATE_INNER_SEND_DATA: {
			const uint8_t command[] = {
				PN532_COMMAND_INLISTPASSIVETARGET,
				1, // One card
				BC->tag_type == TAG_TYPE_TYPE1 ? PN532_INNOVISION_JEWEL : PN532_MIFARE_ISO14443A
			};

			if(BC->state_wait_ack == 0) {
				pn532_send_data_wo_ack(command, sizeof(command));
				BC->state_wait_ack = TIMEOUT_WAIT_FOR_ACK;
			} else {
				if(pn532_read_ack(TIMEOUT_WAIT_READ_ACK_DATA)) {
					BC->state_wait = 1;
					BC->state_wait_inner = TIMEOUT_WAIT_INNER_STATE;
					BC->state_wait_ack = 0;
					BC->state_inner = STATE_INNER_READ_RESPONSE;
				} else {
					BC->state_wait_ack--;
					if(BC->state_wait_ack == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_REQUEST_TAG_ID_ERROR;
					}
				}
			}

			break;
		}

		case STATE_INNER_READ_RESPONSE: {
			uint8_t *response = BC->buffer;

			const uint8_t tid_start = BC->tag_type == TAG_TYPE_TYPE1 ? 5 : 7;

			if(pn532_read_response(response, BUFFER_LENGTH) >= 0) {
				// TODO: Sanity check of response values
				// TODO: Check for target type
				const uint8_t num_tags = response[1];
				const uint8_t tag = response[2];
				const uint16_t sens = (response[3] << 8) | response[4];
				uint8_t sel = 0;
				uint8_t tid_length = 4;

				// Differentiate between Mifare and Type 2 by ID length
				if((BC->tag_type == TAG_TYPE_MIFARE && response[6] != 4) ||
				   (BC->tag_type == TAG_TYPE_TYPE2 && response[6] != 7)) {
					BC->state_inner = 0;
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state = STATE_REQUEST_TAG_ID_ERROR;
				} else {
					if(BC->tag_type != TAG_TYPE_TYPE1) {
						sel = response[5];
						tid_length = MIN(response[6], TID_MAX_LENGTH);
					}

					for(uint8_t i = 0; i < TID_MAX_LENGTH; i++) {
						if(i < tid_length) {
							BC->tid[i] = response[tid_start+i];
						} else {
							BC->tid[i] = 0;
						}
					}
					BC->tid_length = tid_length;
					BC->tid_tag_type = BC->tag_type;

					BC->state_inner = 0;
					BC->state_wait = 1;
					BC->state = STATE_REQUEST_TAG_ID_READY;
				}
			} else {
				if(BC->state_wait_inner == 0) {
					BC->state_inner = 0;
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state = STATE_REQUEST_TAG_ID_ERROR;
				}
			}
			break;
		}
	}
}

void pn532_sam_configure_mode_normal() {
	switch(BC->state_inner) {
		case STATE_INNER_SEND_DATA: {
			const uint8_t command[] = {
				PN532_COMMAND_SAMCONFIGURATION,
				PN532_SAMC_MODE_NORMAL,
				PN532_SAMC_TIMEOUT_1S,
				PN532_SAMC_IRQ_OFF
			};

			if(BC->state_wait_ack == 0) {
				pn532_send_data_wo_ack(command, sizeof(command));
				BC->state_wait_ack = TIMEOUT_WAIT_FOR_ACK;
			} else {
				if(pn532_read_ack(TIMEOUT_WAIT_READ_ACK_DATA)) {
					BC->state_wait = 1;
					BC->state_wait_inner = TIMEOUT_WAIT_INNER_STATE;
					BC->state_wait_ack = 0;
					BC->state_inner = STATE_INNER_READ_RESPONSE;
				} else {
					BC->state_wait_ack--;
					if(BC->state_wait_ack == 0) {
						BC->state_wait = TIMEOUT_WAIT_ERROR;
						BC->state_inner = 0;
						BC->state = STATE_ERROR;
					}
				}
			}


			break;
		}

		case STATE_INNER_READ_RESPONSE: {
			uint8_t response = 0;
			pn532_read_response(&response, 1);

			if(response == 0x15) {
				BC->state_wait = 1;
				BC->state_inner = 0;
				BC->state = STATE_IDLE;
			} else {
				if(BC->state_wait_inner == 0) {
					BC->state_wait = TIMEOUT_WAIT_ERROR;
					BC->state_inner = 0;
					BC->state = STATE_ERROR;
				}
			}

			break;
		}
	}
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
	if(!pn532_wait_until_ready(1)) {
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

int16_t pn532_read_response(uint8_t *data, uint8_t length) {
	if(!pn532_wait_until_ready(1)) {
		return -1;
	}

	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;

	// Read data (see 6.2.5)
	spibb_transceive_byte(PN532_SPI_DATAREAD);

	// Read and check preamble and start code (see 6.2.1.1)
	const uint8_t preamble = spibb_transceive_byte(0x00);
	const uint8_t start_code1 = spibb_transceive_byte(0x00);
	const uint8_t start_code2 = spibb_transceive_byte(0x00);
	if(preamble != 0x00 || start_code1 != 0x00 || start_code2 != 0xFF) {
		logbld("Preamble/Start Code not found: %x %x %x\n\r", preamble, start_code1, start_code2);
		return -1;
	}

	// Read length and check length checksum (see 6.2.1.1)
	const uint8_t length_to_read = spibb_transceive_byte(0x00);
	const uint8_t lcs = spibb_transceive_byte(0x00) + length_to_read;
	if(lcs != 0) {
		logbld("Length checksum wrong: %x %x\n\r", length_to_read, lcs);
		return -1;
	}

	if((length_to_read-1) > length) {
		logbld("Buffer not big enough: %x > %x\n\r", (length_to_read-1), length);
		return -1;
	}

	// Read and check frame identifier (see 6.2.1.1)
	const uint8_t tfi = spibb_transceive_byte(0x00);
	if(tfi != PN532_PN532TOHOST) {
		if(tfi == 0x7f) { // Error frame
			const uint8_t error_code = spibb_transceive_byte(0x00);
			const uint8_t postamble = spibb_transceive_byte(0x00);
			logbld("Error frame: %x %x\n\r", error_code, postamble);
		} else {
			logbld("Wrong frame identifier: %x\n\r", tfi);
		}
		return -1;
	}

	// Read data and check checksum (see 6.2.1.1)
	uint8_t checksum = PN532_PN532TOHOST;
	for(uint8_t i = 0; i < length_to_read-1; i++) {
		data[i] = spibb_transceive_byte(0x00);
		checksum += data[i];
	}

	const uint8_t dcs = spibb_transceive_byte(0x00) + checksum;
	if(dcs != 0) {
		logbld("Wrong checksum: %x %x\n\r", dcs, checksum);
		return -1;
	}

	// Read and check postamble (see 6.2.1.1)
	const uint8_t postamble = spibb_transceive_byte(0x00);
	if(postamble != 0x00) {
		logbld("Wrong postamble: %x\n\r", postamble);
		return -1;
	}

	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;

	return length_to_read;
}

void pn532_send_data_wo_ack(const uint8_t *data, uint8_t length) {
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
}

bool pn532_send_data(const uint8_t *data, uint8_t length) {
	pn532_send_data_wo_ack(data, length);

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
