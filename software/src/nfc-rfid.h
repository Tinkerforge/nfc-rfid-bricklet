/* nfc/rfid-bricklet
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * nfc/rfid.h: Implementation of NFC/RFID Bricklet messages
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

#ifndef NFC_RFID_H
#define NFC_RFID_H

#include <stdint.h>

#include "bricklib/com/com_common.h"

// Timeouts
#define PN532_WAIT_UNTIL_READY_TIMEOUT      100
#define PN532_READ_ACK_TIMEOUT              100

#define PN532_PREAMBLE                      0x00
#define PN532_STARTCODE1                    0x00
#define PN532_STARTCODE2                    0xFF
#define PN532_POSTAMBLE                     0x00

#define PN532_HOSTTOPN532                   0xD4

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              0x00
#define PN532_COMMAND_GETFIRMWAREVERSION    0x02
#define PN532_COMMAND_GETGENERALSTATUS      0x04
#define PN532_COMMAND_READREGISTER          0x06
#define PN532_COMMAND_WRITEREGISTER         0x08
#define PN532_COMMAND_READGPIO              0x0C
#define PN532_COMMAND_WRITEGPIO             0x0E
#define PN532_COMMAND_SETSERIALBAUDRATE     0x10
#define PN532_COMMAND_SETPARAMETERS         0x12
#define PN532_COMMAND_SAMCONFIGURATION      0x14
#define PN532_COMMAND_POWERDOWN             0x16
#define PN532_COMMAND_RFCONFIGURATION       0x32
#define PN532_COMMAND_RFREGULATIONTEST      0x58
#define PN532_COMMAND_INJUMPFORDEP          0x56
#define PN532_COMMAND_INJUMPFORPSL          0x46
#define PN532_COMMAND_INLISTPASSIVETARGET   0x4A
#define PN532_COMMAND_INATR                 0x50
#define PN532_COMMAND_INPSL                 0x4E
#define PN532_COMMAND_INDATAEXCHANGE        0x40
#define PN532_COMMAND_INCOMMUNICATETHRU     0x42
#define PN532_COMMAND_INDESELECT            0x44
#define PN532_COMMAND_INRELEASE             0x52
#define PN532_COMMAND_INSELECT              0x54
#define PN532_COMMAND_INAUTOPOLL            0x60
#define PN532_COMMAND_TGINITASTARGET        0x8C
#define PN532_COMMAND_TGSETGENERALBYTES     0x92
#define PN532_COMMAND_TGGETDATA             0x86
#define PN532_COMMAND_TGSETDATA             0x8E
#define PN532_COMMAND_TGSETMETADATA         0x94
#define PN532_COMMAND_TGGETINITIATORCOMMAND 0x88
#define PN532_COMMAND_TGRESPONSETOINITIATOR 0x90
#define PN532_COMMAND_TGGETTARGETSTATUS     0x8A

#define PN532_WAKEUP                        0x55

#define PN532_SPI_STATREAD                  0x02
#define PN532_SPI_DATAWRITE                 0x01
#define PN532_SPI_DATAREAD                  0x03
#define PN532_SPI_READY                     0x01

#define PN532_MIFARE_ISO14443A              0x00

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   0x60
#define MIFARE_CMD_AUTH_B                   0x61
#define MIFARE_CMD_READ                     0x30
#define MIFARE_CMD_WRITE                    0xA0
#define MIFARE_CMD_TRANSFER                 0xB0
#define MIFARE_CMD_DECREMENT                0xC0
#define MIFARE_CMD_INCREMENT                0xC1
#define MIFARE_CMD_STORE                    0xC2


// SAMConfiguration
#define PN532_SAMC_MODE_NORMAL       0x01
#define PN532_SAMC_MODE_VIRTUAL_CARD 0x02
#define PN532_SAMC_MODE_WIRED_CARD   0x03
#define PN532_SAMC_MODE_DUAL_CARD    0x04
#define PN532_SAMC_TIMEOUT_1S        0x14
#define PN532_SAMC_IRQ_OFF           0x00
#define PN532_SAMC_IRQ_ON            0x01


void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);

bool pn532_read_passive_target_id(void);
bool pn532_sam_configure_mode_normal(void);
bool pn532_wait_until_ready(uint32_t timeout);
uint8_t pn532_status(void);
bool pn532_read_ack(uint32_t timeout);
bool pn532_read_response(uint8_t *data, uint8_t length);
bool pn532_send_data(uint8_t *data, uint8_t length);
uint8_t spibb_transceive_byte(const uint8_t value);


#endif
