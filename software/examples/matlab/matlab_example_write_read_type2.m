function matlab_example_write_read_type2()
    global nr;

    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletNFCRFID;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change to your UID

    ipcon = IPConnection(); % Create IP connection
    nr = BrickletNFCRFID(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
    set(nr, 'StateChangedCallback', @(h, e) cb_state_changed(e));

    % Select NFC Forum Type 2 tag
    nr.requestTagID(BrickletNFCRFID.TAG_TYPE_TYPE2);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for state changed callback
function cb_state_changed(e)
    global nr;
    
    if e.state == nr.STATE_REQUEST_TAG_ID_READY
        fprintf('Tag found\n');
        
        % Write 16 byte to pages 5-8
        data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
        nr.writePage(5, data_write);
        fprintf('Writing data...\n');
    elseif e.state == nr.STATE_WRITE_PAGE_READY
        % Request pages 5-8
        nr.requestPage(5);
        fprintf('Requesting data...\n');
    elseif e.state == nr.STATE_REQUEST_PAGE_READY
        % Get and print pages
        data = nr.getPage();
        fprintf('Read data: [%d]\n', data);
    elseif (bitand(e.state, 64) == 64)
        % All errors have bit 6 set
        fprintf('Error: %d\n', e.state);
    end
end
