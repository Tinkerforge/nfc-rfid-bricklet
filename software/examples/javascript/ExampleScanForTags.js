var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'hjw'; // Change to your UID

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var nfc = new Tinkerforge.BrickletNFCRFID(UID, ipcon); // Create device object
var tagType = 0;

ipcon.connect(HOST, PORT,
    function(error) {
        console.log('Error: '+error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        nfc.requestTagID(Tinkerforge.BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC);
    }
);

// Register state changed callback
nfc.on(Tinkerforge.BrickletNFCRFID.CALLBACK_STATE_CHANGED,
    // Callback function for state change callback
    function(state, idle) {
        if(idle) {
            tagType = (tagType + 1) % 3;
            nfc.requestTagID(tagType);
        }

        if(state == Tinkerforge.BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
            nfc.getTagID(
                function(tagType, tidLength, tid) {
                    var s = 'Found tag of type ' + tagType +
                            ' with ID [' + tid[0].toString(16);
                    if(tidLength == 7) {
                        for(var i = 1; i < 7; i++) {
                            s += ', ' + tid[i].toString(16);
                        }
                    } else {
                        for(var i = 1; i < 4; i++) {
                            s += ', ' + tid[i].toString(16);
                        }
                    }

                    s += ']';
                    console.log(s);
                },
                function(error) {
                    console.log('Error: ' + error);
                }
            )
        }
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data',
    function(data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
