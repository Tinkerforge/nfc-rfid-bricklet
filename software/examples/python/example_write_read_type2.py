#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "hjw" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_nfc_rfid import NFCRFID

# Callback function for state changed callback
def cb_state_changed(state, idle, nfc):
    if state == nfc.STATE_REQUEST_TAG_ID_READY:
        print('Tag found')

        # Write 16 byte to pages 5-8
        data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
        nfc.write_page(5, data_write)
        print('Writing data...')
    elif state == nfc.STATE_WRITE_PAGE_READY:
        # Request pages 5-8
        nfc.request_page(5)
        print('Requesting data...')
    elif state == nfc.STATE_REQUEST_PAGE_READY:
        # Get and print pages
        data = nfc.get_page()
        print('Read data: [' + ' '.join(map(str, data)) + ']')
    elif state & (1 << 6):
        # All errors have bit 6 set
        print('Error: ' + str(state))


if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    nfc = NFCRFID(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register state changed callback to function cb_state_changed
    nfc.register_callback(nfc.CALLBACK_STATE_CHANGED, 
                          lambda x, y: cb_state_changed(x, y, nfc))

    # Select NFC Forum Type 2 tag
    nfc.request_tag_id(nfc.TAG_TYPE_TYPE2)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
