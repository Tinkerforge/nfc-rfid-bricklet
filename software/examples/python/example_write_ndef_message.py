#!/usr/bin/env python
# -*- coding: utf-8 -*-  

# The following specifications have been used
# as a basis for writing this example.
#
# NFC Data Exchange Format (NDEF), NDEF 1.0:
# https://github.com/Tinkerforge/nfc-rfid-bricklet/raw/master/datasheets/specification_ndef.pdf
#
# Type 1 Tag Operation Specification, T1TOP 1.1:
# https://github.com/Tinkerforge/nfc-rfid-bricklet/raw/master/datasheets/specification_type1.pdf
#
# Type 2 Tag Operation Specification, T2TOP 1.1:
# https://github.com/Tinkerforge/nfc-rfid-bricklet/raw/master/datasheets/specification_type2.pdf

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_nfc_rfid import NFCRFID

try:
    from Queue import Queue
except ImportError:
    from queue import Queue

from pprint import pprint
import os

class NdefMessage:
    tag_type = None
    records = []
    capability_container = [0, 0, 0, 0]

    def __init__(self, tag_type):
        self.tag_type = tag_type

    def add_record(self, record):
        self.records.append(record)

        # Set end and begin flags as needed
        if len(self.records) == 1:
            record.begin = True
            record.end = True
        else:
            self.records[-2].end = False
            self.records[-1].end = True

    def set_capability_container(self, version, tag_size, read_write_access):
        # Magic number to indicate NFC Forum defined data is stored
        self.capability_container[0] = 0xE1
        self.capability_container[1] = version
        if self.tag_type == NFCRFID.TAG_TYPE_TYPE1:
            self.capability_container[2] = tag_size/8 - 1
        else:
            self.capability_container[2] = tag_size/8

        self.capability_container[3] = read_write_access

    def get_raw_data_in_chunks(self):
        raw_data = []
        for record in self.records:
            raw_data.extend(record.get_raw_data())

        # Use Three consecutive byte format if necessary, see 2.3 TLV blocks
        data_len = len(raw_data)
        if data_len < 0xFF:
            tlv_ndef = [0x03, data_len]
        else:
            tlv_ndef = [0x03, 0xFF, data_len >> 8, data_len % 256]

        if self.tag_type == NFCRFID.TAG_TYPE_TYPE1:
            # CC set by set_capability_container
            # default lock and memory TLVs
            # NDEF TLV
            # NDEF message
            # Terminator TLV
            raw_data = self.capability_container + \
                       [0x01, 0x03, 0xF2, 0x30, 0x33, 0x02, 0x03, 0xF0, 0x02, 0x03] + \
                       tlv_ndef + \
                       raw_data + \
                       [0xFE] 
        elif self.tag_type == NFCRFID.TAG_TYPE_TYPE2:
            # CC set by set_capability_container
            # NDEF TLV
            # NDEF message
            # Terminator TLV
            raw_data = self.capability_container + \
                       tlv_ndef + \
                       raw_data + \
                       [0xFE]
        else:
            # TODO: We could support TAG_TYPE_MIFARE_CLASSIC here, but mifare classic
            # it is not supported in modern smart phones anyway.
            return [[]]


        chunks = []
        for i in range(0, len(raw_data), 16):
            chunks.append(raw_data[i:i+16])

        last_chunk_length = len(chunks[-1])
        if last_chunk_length < 16:
            chunks[-1].extend([0]*(16-last_chunk_length))

        return chunks

class NdefRecord:
    FLAG_ID_LENGTH     = 1 << 3
    FLAG_SHORT_RECORD  = 1 << 4
    FLAG_CHUNK         = 1 << 5
    FLAG_MESSAGE_END   = 1 << 6
    FLAG_MESSAGE_BEGIN = 1 << 7

    TNF_EMPTY          = 0x0
    TNF_WELL_KNOWN     = 0x01
    TNF_MIME_MEDIA     = 0x02
    TNF_ABSOLUTE_URI   = 0x03
    TNF_EXTERNAL_TYPE  = 0x04
    TNF_UNKNOWN        = 0x05
    TNF_UNCHANGED      = 0x06
    TNF_RESERVED       = 0x07

    NDEF_URIPREFIX_NONE         = 0x00
    NDEF_URIPREFIX_HTTP_WWWDOT  = 0x01
    NDEF_URIPREFIX_HTTPS_WWWDOT = 0x02
    NDEF_URIPREFIX_HTTP         = 0x03
    NDEF_URIPREFIX_HTTPS        = 0x04
    NDEF_URIPREFIX_TEL          = 0x05
    NDEF_URIPREFIX_MAILTO       = 0x06
    NDEF_URIPREFIX_FTP_ANONAT   = 0x07
    NDEF_URIPREFIX_FTP_FTPDOT   = 0x08
    NDEF_URIPREFIX_FTPS         = 0x09
    NDEF_URIPREFIX_SFTP         = 0x0A
    NDEF_URIPREFIX_SMB          = 0x0B
    NDEF_URIPREFIX_NFS          = 0x0C
    NDEF_URIPREFIX_FTP          = 0x0D
    NDEF_URIPREFIX_DAV          = 0x0E
    NDEF_URIPREFIX_NEWS         = 0x0F
    NDEF_URIPREFIX_TELNET       = 0x10
    NDEF_URIPREFIX_IMAP         = 0x11
    NDEF_URIPREFIX_RTSP         = 0x12
    NDEF_URIPREFIX_URN          = 0x13
    NDEF_URIPREFIX_POP          = 0x14
    NDEF_URIPREFIX_SIP          = 0x15
    NDEF_URIPREFIX_SIPS         = 0x16
    NDEF_URIPREFIX_TFTP         = 0x17
    NDEF_URIPREFIX_BTSPP        = 0x18
    NDEF_URIPREFIX_BTL2CAP      = 0x19
    NDEF_URIPREFIX_BTGOEP       = 0x1A
    NDEF_URIPREFIX_TCPOBEX      = 0x1B
    NDEF_URIPREFIX_IRDAOBEX     = 0x1C
    NDEF_URIPREFIX_FILE         = 0x1D
    NDEF_URIPREFIX_URN_EPC_ID   = 0x1E
    NDEF_URIPREFIX_URN_EPC_TAG  = 0x1F
    NDEF_URIPREFIX_URN_EPC_PAT  = 0x20
    NDEF_URIPREFIX_URN_EPC_RAW  = 0x21
    NDEF_URIPREFIX_URN_EPC      = 0x22
    NDEF_URIPREFIX_URN_NFC      = 0x23

    NDEF_TYPE_MIME              = 0x02
    NDEF_TYPE_URI               = 0x55
    NDEF_TYPE_TEXT              = 0x54

    begin = False
    end = False

    tnf = 0
    record_type = None
    payload = None
    identifier = None

    def set_identifier(self, identifier):
        self.identifier = identifier

    def get_raw_data(self):
        if self.record_type == None or self.payload == None:
            return []

        # Construct tnf and flags (byte 0 of header)
        header = [self.tnf]
        if self.begin:
            header[0] |= NdefRecord.FLAG_MESSAGE_BEGIN
        if self.end:
            header[0] |= NdefRecord.FLAG_MESSAGE_END
        if len(self.payload) < 256:
            header[0] |= NdefRecord.FLAG_SHORT_RECORD
        if self.identifier != None:
            header[0] |= NdefRecord.FLAG_ID_LENGTH

        # Type length (byte 1 of header)
        header.append(len(self.record_type))

        # Payload length (byte 2 of header)
        # TODO: payload > 255?
        header.append(len(self.payload))

        # ID length (byte 3 of header)
        if self.identifier != None:
            header.append(len(self.identifier))

        # Record type (byte 4ff of header)
        header.extend(self.record_type)

        # ID
        if self.identifier != None:
            header.extend(self.identifier)

        return header + self.payload

class NdefTextRecord(NdefRecord):
    # Call with text and ISO/IANA language code
    def __init__(self, text, language='en'):
        self.tnf = NdefRecord.TNF_WELL_KNOWN
        self.record_type = [NdefRecord.NDEF_TYPE_TEXT]

        lang_list = map(ord, language)
        text_list = map(ord, text)

        # Text Record Content: Status byte, ISO/IANA language code, text
        # See NDEF 1.0: 3.2.1
        self.payload = [len(lang_list)] + lang_list + text_list

class NdefUriRecord(NdefRecord):
    def __init__(self, uri, uri_prefix=NdefRecord.NDEF_URIPREFIX_NONE):
        self.tnf = NdefRecord.TNF_WELL_KNOWN
        self.record_type = [NdefRecord.NDEF_TYPE_URI]

        uri_list = map(ord, uri)

        # Text Record Content: URI prefix, URI 
        # See NDEF 1.0: 3.2.2
        self.payload = [uri_prefix] + uri_list

class NdefMediaRecord(NdefRecord):
    def __init__(self, mime_type, data):
        self.tnf = NdefRecord.TNF_MIME_MEDIA

        self.record_type = map(ord, mime_type)
        if type(data) == str:
            self.payload = map(ord, data)
        else:
            self.payload = data

class ExampleNdef:
    HOST = "localhost"
    PORT = 4223
    UID = "hjw" # Change to your UID

    state_queue = Queue()
    tag_type = None
    tag_size = None

    def __init__(self, tag_type, tag_size=512):
        self.tag_type = tag_type
        self.tag_size = tag_size
        self.ipcon = IPConnection() # Create IP connection
        self.nfc = NFCRFID(self.UID, self.ipcon) # Create device object

        self.ipcon.connect(self.HOST, self.PORT) # Connect to brickd

        self.nr.register_callback(self.nr.CALLBACK_STATE_CHANGED, self.state_changed)

    def write_message(self):
        chunks = self.message.get_raw_data_in_chunks()
        print("Trying to write the follwing data to the tag:")
        pprint(chunks)

        self.nr.request_tag_id(self.tag_type)
        state = self.state_queue.get()
        if state != self.nr.STATE_REQUEST_TAG_ID:
            return -1

        state = self.state_queue.get()
        if state != self.nr.STATE_REQUEST_TAG_ID_READY:
            return -2
        
        # NFC Forum Type 1 start page is 1 (start of capability container)
        # NFC Forum Type 2 start page is 3 (start of capability container)
        if self.tag_type == NFCRFID.TAG_TYPE_TYPE1:
            current_page = 1
        else:
            current_page = 3

        for chunk in chunks:
            self.nr.write_page(current_page, chunk)
            state = self.state_queue.get()
            if state != self.nr.STATE_WRITE_PAGE:
                return -3

            state = self.state_queue.get()
            if state != self.nr.STATE_WRITE_PAGE_READY:
                return -4

            # NFC Forum Type 1 has 2 pages per chunk (16 byte)
            # NFC Forum Type 2 has 4 pages per chunk (16 byte)
            if self.tag_type == NFCRFID.TAG_TYPE_TYPE1 :       
                current_page += 2
            else:
                current_page += 4

        return 0

    def state_changed(self, state, idle):
        self.state_queue.put(state)

    def make_message_small(self):
        self.message = NdefMessage(self.tag_type)

        # Capabilities: 
        # Version 1.0               (0x10) 
        # Tag size bytes            (given by self.tag_size)
        # Read/write access for all (0x00)
        self.message.set_capability_container(0x10, self.tag_size, 0x00)

        record = NdefUriRecord('tinkerforge.com', NdefRecord.NDEF_URIPREFIX_HTTP_WWWDOT)
        self.message.add_record(record)

    def make_message_large(self):
        self.message = NdefMessage(self.tag_type)

        # Capabilities: 
        # Version 1.0               (0x10) 
        # Tag size bytes            (given by self.tag_size)
        # Read/write access for all (0x00)
        self.message.set_capability_container(0x10, self.tag_size, 0x00)

        rec1 = NdefTextRecord('Hello World', 'en')
        self.message.add_record(rec1)
        rec2 = NdefTextRecord('Hallo Welt', 'de')
        self.message.add_record(rec2)
        rec3 = NdefUriRecord('tinkerforge.com', NdefRecord.NDEF_URIPREFIX_HTTP_WWWDOT)
        self.message.add_record(rec3)
        text = '<html><head><title>Hello</title></head><body>World!</body></html>'
        rec4 = NdefMediaRecord('text/html', text)
        self.message.add_record(rec4)

        # To test an "image/png" you can can download the file from:
        # http://download.tinkerforge.com/_stuff/tf_16x16.png
        if os.path.isfile('tf_16x16.png'):
            logo = file('tf_16x16.png').read()
            rec5 = NdefMediaRecord('image/png', map(ord, logo))
            self.message.add_record(rec5)

if __name__ == '__main__':
    example = ExampleNdef(tag_type=NFCRFID.TAG_TYPE_TYPE2, tag_size=888)
#    example.make_message_large() # Writes different texts, URI, html site and image
    example.make_message_small() # Writes simple URI record
    ret = example.write_message()
    if ret < 0:
        print('Could not write NDEF Message: ' + str(ret))
    else:
        print('NDEF Message written successfully')
