function matlab_example_scan_for_tags()
    global nr;
    global tag_type;

    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletNFCRFID;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your NFC/RFID Bricklet

    ipcon = IPConnection(); % Create IP connection
    nr = handle(BrickletNFCRFID(UID, ipcon), 'CallbackProperties'); % Create device object
    tag_type = 0;

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    set(nr, 'StateChangedCallback', @(h, e) cb_state_changed(e));

    % Start scan loop
    nr.requestTagID(BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC);

    input('Press key to exit\n', 's');
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
    
    if e.state == nr.STATE_REQUEST_TAG_ID_READY
        ret = nr.getTagID();

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
