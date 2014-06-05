function octave_example_scan_for_tags()
	more off;

    global nfc;
    global tag_type;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'hjw'; % Change to your UID
    
    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    nfc = java_new("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object
    tag_type = 0;

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
	nfc.addStateChangedCallback(@cb_state_changed);
    
    nfc.requestTagID(nfc.TAG_TYPE_MIFARE_CLASSIC);

    input('Press any key to exit...\n', 's');
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
    
    if e.state == nfc.STATE_REQUEST_TAG_ID_READY
        ret = nfc.getTagID();
        if ret.tidLength == 4
            fprintf('Found tag of type %d with ID [%x %x %x %x]\n', ...
                    ret.tagType, ret.tid(1), ret.tid(2), ret.tid(3), ret.tid(4));
        else
            fprintf('Found tag of type %d with ID [%x %x %x %x %x %x %x]\n', ...
                    ret.tagType, ret.tid(1), ret.tid(2), ret.tid(3), ret.tid(4), ...
                    ret.tid(5), ret.tid(6), ret.tid(7));
        end
        
    end
end
