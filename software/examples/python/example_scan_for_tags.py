#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_nfc_rfid import BrickletNFCRFID

tag_type = 0

# Callback function for state changed callback
def cb_state_changed(state, idle, nr):
    # Cycle through all types
    if idle:
        global tag_type
        tag_type = (tag_type + 1) % 3
        nr.request_tag_id(tag_type)

    if state == nr.STATE_REQUEST_TAG_ID_READY:
        ret = nr.get_tag_id()
        print('Found tag of type ' +
              str(ret.tag_type) +
              ' with ID [' +
              ' '.join(map(str, map(hex, ret.tid[:ret.tid_length]))) +
              "]")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    nr = BrickletNFCRFID(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register state changed callback to function cb_state_changed
    nr.register_callback(nr.CALLBACK_STATE_CHANGED,
                         lambda x, y: cb_state_changed(x, y, nr))

    nr.request_tag_id(nr.TAG_TYPE_MIFARE_CLASSIC)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
