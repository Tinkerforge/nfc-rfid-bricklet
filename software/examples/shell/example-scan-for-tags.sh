#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your NFC/RFID Bricklet
tmp=$(mktemp)

# Create temporary file to keep track of current tag type
echo "0" > $tmp

# Handle incoming state changed callbacks
tinkerforge dispatch nfc-rfid-bricklet $uid state-changed\
 --execute "if [ {idle} ]
            then 
              tt=\$(cat $tmp)
              if [ \$tt -eq 0 ] 
                then tt=1
              elif [ \$tt -eq 1 ] 
                then tt=2
              elif [ \$tt -eq 2 ] 
                then tt=0
              fi
              echo \"\$tt\" > $tmp
              tinkerforge call nfc-rfid-bricklet $uid request-tag-id \$tt
            fi;
            if [ {state} == request-tag-id-ready ]
              then tinkerforge call nfc-rfid-bricklet $uid get-tag-id
            fi" &

# Start scan loop
tinkerforge call nfc-rfid-bricklet $uid request-tag-id mifare-classic

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
