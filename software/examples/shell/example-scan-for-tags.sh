#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=hjw

echo "0" > /tmp/tt_save

# handle incoming state changed callbacks
tinkerforge dispatch nfc-rfid-bricklet $uid state-changed\
 --execute "if [ {idle} ]
            then 
              tt=\`cat /tmp/tt_save\`
              if [ \$tt -eq 0 ] 
                then tt=1
              elif [ \$tt -eq 1 ] 
                then tt=2
              elif [ \$tt -eq 2 ] 
                then tt=0
              fi
              echo \"\$tt\" > /tmp/tt_save
              tinkerforge call nfc-rfid-bricklet $uid request-tag-id \$tt
            fi;
            if [ {state} -eq 130 ]
              then tinkerforge call nfc-rfid-bricklet $uid get-tag-id
            fi" &

tinkerforge call nfc-rfid-bricklet $uid request-tag-id 0
