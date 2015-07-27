#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_nfc_rfid import NFCRFID

tag_type = 0

# Callback function for state changed callback
def cb_state_changed(state, idle, nfc):
    # Cycle through all types
    if idle:
        global tag_type
        tag_type = (tag_type + 1) % 3
        nfc.request_tag_id(tag_type)

    if state == nfc.STATE_REQUEST_TAG_ID_READY:
        ret = nfc.get_tag_id()
        print('Found tag of type ' +
              str(ret.tag_type) +
              ' with ID [' +
              ' '.join(map(str, map(hex, ret.tid[:ret.tid_length]))) +
              "]")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    nfcrfid = NFCRFID(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register state changed callback to function cb_state_changed
    nfc.register_callback(nfc.CALLBACK_STATE_CHANGED, 
                          lambda x, y: cb_state_changed(x, y, nfc))

    nfc.request_tag_id(nfc.TAG_TYPE_MIFARE_CLASSIC)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
