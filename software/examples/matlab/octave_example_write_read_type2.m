function octave_example_write_read_type2()
    more off;

    global nr;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your NFC/RFID Bricklet

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    nr = java_new("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    nr.addStateChangedCallback(@cb_state_changed);

    % Select NFC Forum Type 2 tag
    nr.requestTagID(nr.TAG_TYPE_TYPE2);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for state changed callback
function cb_state_changed(e)
    global nr;

    if java2int(e.state) == java2int(nr.STATE_REQUEST_TAG_ID_READY)
        fprintf("Tag found\n");

        % Write 16 byte to pages 5-8
        data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
        nr.writePage(5, data_write);
        fprintf("Writing data...\n");
    elseif java2int(e.state) == java2int(nr.STATE_WRITE_PAGE_READY)
        % Request pages 5-8
        nr.requestPage(5);
        fprintf("Requesting data...\n");
    elseif java2int(e.state) == java2int(nr.STATE_REQUEST_PAGE_READY)
        % Get and print pages
        data = nr.getPage();
        fprintf("Read data: [%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d]\n",
                java2int(data(1)), java2int(data(2)), java2int(data(3)), java2int(data(4)),
                java2int(data(5)), java2int(data(6)), java2int(data(7)), java2int(data(8)),
                java2int(data(9)), java2int(data(10)), java2int(data(11)), java2int(data(12)),
                java2int(data(13)), java2int(data(14)), java2int(data(15)), java2int(data(16)));
    elseif (bitand(java2int(e.state), 64) == 64)
        % All errors have bit 6 set
        fprintf("Error: %d\n", java2int(e.state));
    end
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
