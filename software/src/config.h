/* nfc/rfid-bricklet
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: NFC/RFID Bricklet specific configuration
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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/board/sam3s/SAM3S.h"
#include "bricklib/drivers/pio/pio.h"

#include "nfc-rfid.h"

#define BRICKLET_FIRMWARE_VERSION_MAJOR 2
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 0

#define BRICKLET_HARDWARE_VERSION_MAJOR 1
#define BRICKLET_HARDWARE_VERSION_MINOR 0
#define BRICKLET_HARDWARE_VERSION_REVISION 0

#define BRICKLET_DEVICE_IDENTIFIER 246

#define LOGGING_LEVEL LOGGING_DEBUG
#define DEBUG_BRICKLET 0

#define BRICKLET_NO_OFFSET
#define BRICKLET_VALUE_APPLIED_OUTSIDE
#define INVOCATION_IN_BRICKLET_CODE
#define BRICKLET_HAS_NO_DESTRUCTOR

#define BOARD_MCK 64000000

#define PIN_MOSI (BS->pin1_ad)
#define PIN_NSS  (BS->pin2_da)
#define PIN_SCK  (BS->pin3_pwm)
#define PIN_MISO (BS->pin4_io)

#define BUFFER_LENGTH 32

typedef struct {
	State state_before;
	State state;
	uint8_t state_inner;
	uint16_t state_wait;
	uint16_t state_wait_inner;
	uint16_t state_wait_ack;

	uint8_t tag_type;

	uint8_t buffer[32];

	uint8_t read_byte_i;
	uint8_t write_byte_i;

	uint8_t authenticate_page;
	uint8_t authenticate_key[KEY_MAX_LENGTH];
	uint8_t authenticate_key_number;
	uint8_t authenticate_tid[TID_MAX_LENGTH];

	uint8_t tid_length;
	uint8_t tid_tag_type;
	uint8_t tid[TID_MAX_LENGTH];

	uint16_t data_write_page;
	uint8_t data_write[PAGE_MAX_LENGTH];

	uint16_t data_read_page;
	uint8_t data_read[PAGE_MAX_LENGTH];
} BrickContext;

#endif
