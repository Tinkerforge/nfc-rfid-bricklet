#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_nfc_rfid'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change to your UID

ipcon = IPConnection.new # Create IP connection
nr = BrickletNFCRFID.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register state changed callback
nr.register_callback(BrickletNFCRFID::CALLBACK_STATE_CHANGED) do |state, idle|
  if state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY
    puts "Tag found"

    # Write 16 byte to pages 5-8
    data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
    nr.write_page 5, data_write
    puts "Writing data..."
  elsif state == BrickletNFCRFID::STATE_WRITE_PAGE_READY
    # Request pages 5-8
    nr.request_page 5
    puts "Requesting data..."
  elsif state == BrickletNFCRFID::STATE_REQUEST_PAGE_READY
    # Get and print pages
    data = nr.get_page
    puts "Read data: #{data}"
  elsif (state & (1 << 6)) == (1 << 6)
    # All errors have bit 6 set
    puts "Error: #{state}"
  end
end

# Select NFC Forum Type 2 tag
nr.request_tag_id BrickletNFCRFID::TAG_TYPE_TYPE2

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
