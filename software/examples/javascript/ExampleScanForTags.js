var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your NFC/RFID Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var nr = new Tinkerforge.BrickletNFCRFID(UID, ipcon); // Create device object
var tagType = 0;

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Start scan loop
        nr.requestTagID(Tinkerforge.BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC);
    }
);

// Register state changed callback
nr.on(Tinkerforge.BrickletNFCRFID.CALLBACK_STATE_CHANGED,
    // Callback function for state changed callback
    function (state, idle) {
        if(state == Tinkerforge.BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
            nr.getTagID(
                function (tagType, tidLength, tid) {
                    var s = 'Found tag of type ' + tagType +
                            ' with ID [' + tid[0].toString(16);

                    for(var i = 1; i < tidLength; i++) {
                        s += ' ' + tid[i].toString(16);
                    }

                    s += ']';
                    console.log(s);
                },
                function (error) {
                    console.log('Error: ' + error);
                }
            )
        }

        // Cycle through all types
        if(idle) {
            tagType = (tagType + 1) % 3;
            nr.requestTagID(tagType);
        }
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
