function octave_example_scan_for_tags()
    more off;

    global nr;
    global tag_type;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your NFC/RFID Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    nr = javaObject("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object
    tag_type = 0;

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    nr.addStateChangedCallback(@cb_state_changed);

    % Start scan loop
    nr.requestTagID(nr.TAG_TYPE_MIFARE_CLASSIC);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for state changed callback
function cb_state_changed(e)
    global nr;
    global tag_type;

    % Cycle through all types
    if e.idle
        tag_type = mod((tag_type + 1), 3);
        nr.requestTagID(tag_type);
    end

    if java2int(e.state) == java2int(nr.STATE_REQUEST_TAG_ID_READY)
        ret = nr.getTagID();
        tid = ret.tid;

        if java2int(ret.tidLength) == 4
            fprintf("Found tag of type %d with ID [%02x %02x %02x %02x]\n", ...
                    java2int(ret.tagType), java2int(tid(1)), java2int(tid(2)), ...
                    java2int(tid(3)), java2int(tid(4)));
        else
            fprintf("Found tag of type %d with ID [%02x %02x %02x %02x %02x %02x %02x]\n", ...
                    java2int(ret.tagType), java2int(tid(1)), java2int(tid(2)), ...
                    java2int(tid(3)), java2int(tid(4)), java2int(tid(5)), ...
                    java2int(tid(6)), java2int(tid(7)));
        end
    end
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
