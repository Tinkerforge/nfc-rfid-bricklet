function octave_example_write_read_type2()
    more off;

    global nfc;

    HOST = "localhost";
    PORT = 4223;
    UID = "fobar"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    nfc = java_new("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don"t use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    nfc.addStateChangedCallback(@cb_state_changed);

    % Select NFC Forum Type 2 tag
    nfc.requestTagID(nfc.TAG_TYPE_TYPE2);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end

% Callback function for state changed
function cb_state_changed(e)
    global nfc

    if short2int(e.state) == short2int(nfc.STATE_REQUEST_TAG_ID_READY)
        fprintf("Tag found\n");

        % Write 16 byte to pages 5-8
        data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
        nfc.writePage(5, data_write);
        fprintf("Writing data...\n");
    elseif short2int(e.state) == short2int(nfc.STATE_WRITE_PAGE_READY)
        % Request pages 5-8
        nfc.requestPage(5);
        fprintf("Requesting data...\n");
    elseif short2int(e.state) == short2int(nfc.STATE_REQUEST_PAGE_READY)
        % Get and print pages
        data = nfc.getPage();
        fprintf("Read data: [%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d]\n",
                short2int(data(1)), short2int(data(2)), short2int(data(3)), short2int(data(4)),
                short2int(data(5)), short2int(data(6)), short2int(data(7)), short2int(data(8)),
                short2int(data(9)), short2int(data(10)), short2int(data(11)), short2int(data(12)),
                short2int(data(13)), short2int(data(14)), short2int(data(15)), short2int(data(16)));
    elseif (bitand(short2int(e.state), 64) == 64)
        % All errors have bit 6 set
        fprintf("Error: %d\n", short2int(e.state));
    end
end

function int = short2int(short)
    if compare_versions(version(), "3.8", "<=")
        int = short.intValue();
    else
        int = short;
    end
end
