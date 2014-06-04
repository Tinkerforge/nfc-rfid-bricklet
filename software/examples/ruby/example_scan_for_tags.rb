#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_nfc_rfid'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'hjw' # Change to your UID

ipcon = IPConnection.new # Create IP connection
nfc = BrickletNFCRFID.new UID, ipcon # Create device object
tag_type = 0

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register state changed callback
nfc.register_callback(BrickletNFCRFID::CALLBACK_STATE_CHANGED) do |state, idle|
  if idle
    tag_type = (tag_type + 1) % 3
	nfc.request_tag_id tag_type
  end

  if state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY
    tag_type_new, tid_length, tid = nfc.get_tag_id
    puts "Found tag of type #{tag_type_new} with ID " +
         "#{tid.take(tid_length).each.map { |b| b.to_s(16) }.join ' '}"
  end
end

nfc.request_tag_id(BrickletNFCRFID::TAG_TYPE_MIFARE_CLASSIC)

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
