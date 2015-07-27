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
current_tag_type = 0

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register state changed callback
nr.register_callback(BrickletNFCRFID::CALLBACK_STATE_CHANGED) do |state, idle|
  if idle
    current_tag_type = (current_tag_type + 1) % 3
    nr.request_tag_id current_tag_type
  end

  if state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY
    tag_type, tid_length, tid = nr.get_tag_id
    puts "Found tag of type #{tag_type} with ID " +
         "#{tid.take(tid_length).each.map { |b| b.to_s(16) }.join ' '}"
  end
end

nr.request_tag_id(BrickletNFCRFID::TAG_TYPE_MIFARE_CLASSIC)

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
