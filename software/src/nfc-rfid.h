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
#define PN532_WAIT_UNTIL_READY_TIMEOUT      1000
#define PN532_READ_ACK_TIMEOUT              100

#define TIMEOUT_WAIT_FOR_ACK                50
#define TIMEOUT_WAIT_INNER_STATE            50
#define TIMEOUT_WAIT_ERROR                  100
#define TIMEOUT_WAIT_READ_ACK_DATA          100

#define PN532_PREAMBLE                      0x00
#define PN532_STARTCODE1                    0x00
#define PN532_STARTCODE2                    0xFF
#define PN532_POSTAMBLE                     0x00

#define PN532_HOSTTOPN532                   0xD4
#define PN532_PN532TOHOST                   0xD5

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
#define PN532_FELICA_212                    0x01
#define PN532_FELICA_424                    0x02
#define PN532_ISO14443B                     0x03
#define PN532_INNOVISION_JEWEL              0x04

// NFC Forum Type 2 Commands
#define TYPE2_CMD_READ                      0x30
#define TYPE2_CMD_WRITE                     0xA2

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   0x60
#define MIFARE_CMD_AUTH_B                   0x61
#define MIFARE_CMD_READ                     0x30
#define MIFARE_CMD_WRITE                    0xA0
#define MIFARE_CMD_TRANSFER                 0xB0
#define MIFARE_CMD_DECREMENT                0xC0
#define MIFARE_CMD_INCREMENT                0xC1
#define MIFARE_CMD_STORE                    0xC2

// Jewel Commands (constants from libnfc: utils/jewel.h)
#define JEWEL_CMD_RID                       0x78 // Read ID
  // List of commands (Static memory model)
#define JEWEL_CMD_RALL                      0x00 // Real All
#define JEWEL_CMD_READ                      0x01 // Read (single byte)
#define JEWEL_CMD_WRITEE                    0x53 // Write-with-Erase (single byte)
#define JEWEL_CMD_WRITENE                   0x1A // Write-without-Erase (single byte)

// SAMConfiguration
#define PN532_SAMC_MODE_NORMAL       0x01
#define PN532_SAMC_MODE_VIRTUAL_CARD 0x02
#define PN532_SAMC_MODE_WIRED_CARD   0x03
#define PN532_SAMC_MODE_DUAL_CARD    0x04
#define PN532_SAMC_TIMEOUT_1S        0x14
#define PN532_SAMC_IRQ_OFF           0x00
#define PN532_SAMC_IRQ_ON            0x01

#define TID_MAX_LENGTH 7
#define TID_MIFARE_LENGTH 4
#define KEY_MAX_LENGTH 6

#define PAGE_MAX_LENGTH 16


#define FID_REQUEST_TAG_ID 1
#define FID_GET_TAG_ID 2
#define FID_GET_STATE 3
#define FID_AUTHENTICATE_MIFARE_CLASSIC_PAGE 4
#define FID_WRITE_PAGE 5
#define FID_REQUEST_PAGE 6
#define FID_GET_PAGE 7
#define FID_STATE_CHANGED 8

typedef struct {
	MessageHeader header;
	uint8_t tag_type;
} __attribute__((__packed__)) RequestTagID;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetTagID;

typedef struct {
	MessageHeader header;
	uint8_t tag_type;
	uint8_t tid_length;
	uint8_t tid[TID_MAX_LENGTH];
} __attribute__((__packed__)) GetTagIDReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetState;

typedef struct {
	MessageHeader header;
	uint8_t state;
	uint8_t idle;
} __attribute__((__packed__)) GetStateReturn;

typedef struct {
	MessageHeader header;
	uint16_t page;
	uint8_t key_number;
	uint8_t key[KEY_MAX_LENGTH];
} __attribute__((__packed__)) AuthenticateMifareClassicPage;

typedef struct {
	MessageHeader header;
	uint16_t page;
	uint8_t data[PAGE_MAX_LENGTH];
} __attribute__((__packed__)) WritePage;

typedef struct {
	MessageHeader header;
	uint16_t page;
} __attribute__((__packed__)) RequestPage;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetPage;

typedef struct {
	MessageHeader header;
	uint8_t data[PAGE_MAX_LENGTH];
} __attribute__((__packed__)) GetPageReturn;

void request_tag_id(const ComType com, const RequestTagID *data);
void get_tag_id(const ComType com, const GetTagID *data);
void get_state(const ComType com, const GetState *data);
void authenticate_mifare_classic_page(const ComType com, const AuthenticateMifareClassicPage *data);
void write_page(const ComType com, const WritePage *data);
void request_page(const ComType com, const RequestPage *data);
void get_page(const ComType com, const GetPage *data);

#define STATE_IDLE_MASK (1 << 7)
#define STATE_ERROR_MASK ((1 << 7) | (1 << 6))

typedef enum {
	STATE_INITIALIZATION = 0,
	STATE_IDLE = STATE_IDLE_MASK,
	STATE_ERROR = STATE_ERROR_MASK,
	STATE_REQUEST_TAG_ID = 2,
	STATE_REQUEST_TAG_ID_READY = STATE_IDLE_MASK | STATE_REQUEST_TAG_ID,
	STATE_REQUEST_TAG_ID_ERROR = STATE_ERROR_MASK | STATE_REQUEST_TAG_ID,
	STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE = 3,
	STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_READY = STATE_IDLE_MASK | STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE,
	STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE_ERROR = STATE_ERROR_MASK | STATE_AUTHENTICATING_MIFARE_CLASSIC_PAGE,
	STATE_WRITE_PAGE = 4,
	STATE_WRITE_PAGE_READY = STATE_IDLE_MASK | STATE_WRITE_PAGE,
	STATE_WRITE_PAGE_ERROR = STATE_ERROR_MASK | STATE_WRITE_PAGE,
	STATE_REQUEST_PAGE = 5,
	STATE_REQUEST_PAGE_READY = STATE_IDLE_MASK | STATE_REQUEST_PAGE,
	STATE_REQUEST_PAGE_ERROR = STATE_ERROR_MASK | STATE_REQUEST_PAGE
} State;

typedef enum {
	STATE_INNER_SEND_DATA = 0,
	STATE_INNER_READ_RESPONSE = 1
} StateInner;

typedef enum {
	TAG_TYPE_MIFARE = 0,
	TAG_TYPE_TYPE1 = 1,
	TAG_TYPE_TYPE2 = 2
} TagType;


void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);

void pn532_authenticate_mifare_classic(void);
void pn532_write_page(void);
void pn532_read_page(void);
void pn532_read_tag_id(void);
void pn532_sam_configure_mode_normal(void);

bool pn532_wait_until_ready(uint32_t timeout);
uint8_t pn532_status(void);
bool pn532_read_ack(uint32_t timeout);
int16_t pn532_read_response(uint8_t *data, uint8_t length);
void pn532_send_data_wo_ack(const uint8_t *data, uint8_t length);
bool pn532_send_data(const uint8_t *data, uint8_t length);
uint8_t spibb_transceive_byte(const uint8_t value);


#endif
