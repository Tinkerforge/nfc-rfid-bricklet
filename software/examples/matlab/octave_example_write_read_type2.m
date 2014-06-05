function matlab_example_write_read_type2()
	more off;

    global nfc;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'hjw'; % Change to your UID
    
    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    nfc = java_new("com.tinkerforge.BrickletNFCRFID", UID, ipcon); % Create device object
    
    ipcon = IPConnection(); % Create IP connection
    nfc = BrickletNFCRFID(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register state changed callback to function cb_state_changed
	nfc.addStateChangedCallback(@cb_state_changed);
    
    % Select NFC Forum Type 2 tag
    nfc.requestTagID(nfc.TAG_TYPE_TYPE2);

    input('Press any key to exit...\n', 's');
    ipcon.disconnect();
end

% Callback function for state changed
function cb_state_changed(e)
    global nfc
    
    if e.state == nfc.STATE_REQUEST_TAG_ID_READY
        fprintf('Tag found\n');
        
        % Write 16 byte to pages 5-8
        data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
        nfc.writePage(5, data_write);
        fprintf('Writing data...\n');
    elseif e.state == nfc.STATE_WRITE_PAGE_READY
        % Request pages 5-8
        nfc.requestPage(5);
        fprintf('Requesting data...\n');
    elseif e.state == nfc.STATE_REQUEST_PAGE_READY
        % Get and print pages
        data = nfc.getPage();
        fprintf('Read data: [%d]\n', data);
    elseif (bitand(e.state, 64) == 64)
        % All errors have bit 6 set
        fprintf('Error: %d\n', e.state);
    end
end
