var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change to your UID

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var nr = new Tinkerforge.BrickletNFCRFID(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Select NFC Forum Type 2 tag
        nr.requestTagID(Tinkerforge.BrickletNFCRFID.TAG_TYPE_TYPE2);
    }
);

// Register state changed callback
nr.on(Tinkerforge.BrickletNFCRFID.CALLBACK_STATE_CHANGED,
    // Callback function for state changed callback
    function (state, idle) {
        if(state == Tinkerforge.BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
            console.log('Tag found');

            // Write 16 byte to pages 5-8
            var dataWrite = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

            nr.writePage(5, dataWrite);
            console.log('Writing data...');
        } else if(state == Tinkerforge.BrickletNFCRFID.STATE_WRITE_PAGE_READY) {
            // Request pages 5-8
            nr.requestPage(5);
            console.log('Requesting data...');
        } else if(state == Tinkerforge.BrickletNFCRFID.STATE_REQUEST_PAGE_READY) {
            // Get and print pages
            nr.getPage(
                function (data) {
                    var s = 'Read data: [' + data[0];

                    for(var i = 1; i < data.length; i++) {
                        s += ' ' + data[i].toString();
                    }

                    s += ']';
                    console.log(s);
                }
            );
        } else if((state & (1 << 6)) == (1 << 6)) {
            // All errors have bit 6 set
            console.log('Error: ' + state);
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
