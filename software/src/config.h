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
#define DEBUG_BRICKLET 1

#define BRICKLET_NO_OFFSET
#define BRICKLET_VALUE_APPLIED_OUTSIDE
#define INVOCATION_IN_BRICKLET_CODE
#define BRICKLET_HAS_NO_DESTRUCTOR

#define BOARD_MCK 64000000

#define PIN_MOSI (BS->pin1_ad)
#define PIN_NSS  (BS->pin2_da)
#define PIN_SCK  (BS->pin3_pwm)
#define PIN_MISO (BS->pin4_io)

typedef struct {
	uint8_t state;
	uint32_t tick;
	uint8_t counter;
} BrickContext;

#endif
