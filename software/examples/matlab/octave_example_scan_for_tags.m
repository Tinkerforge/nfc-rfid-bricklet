function octave_example_scan_for_tags()
    more off;

    global nfc;
    global tag_type;

    HOST = "localhost";
    PORT = 4223;
    UID = "fobar"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    nfc = java_new("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object
    tag_type = 0;

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don"t use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    nfc.addStateChangedCallback(@cb_state_changed);

    nfc.requestTagID(nfc.TAG_TYPE_MIFARE_CLASSIC);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end

% Callback function for state changed
function cb_state_changed(e)
    global tag_type
    global nfc

    % Cycle through all types
    if e.idle
        tag_type = mod((tag_type + 1), 3);
        nfc.requestTagID(tag_type);
    end

    if short2int(e.state) == short2int(nfc.STATE_REQUEST_TAG_ID_READY)
        ret = nfc.getTagID();
        tid = ret.tid;

        if short2int(ret.tidLength) == 4
            fprintf("Found tag of type %d with ID [%02x %02x %02x %02x]\n", ...
                    short2int(ret.tagType), short2int(tid(1)), short2int(tid(2)), ...
                    short2int(tid(3)), short2int(tid(4)));
        else
            fprintf("Found tag of type %d with ID [%02x %02x %02x %02x %02x %02x %02x]\n", ...
                    short2int(ret.tagType), short2int(tid(1)), short2int(tid(2)), ...
                    short2int(tid(3)), short2int(tid(4)), short2int(tid(5)), ...
                    short2int(tid(6)), short2int(tid(7)));
        end
    end
end

function int = short2int(short)
    if compare_versions(version(), "3.8", "<=")
        int = short.intValue();
    else
        int = short;
    end
end
