<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletNFCRFID.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletNFCRFID;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'hjw'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$nfc = new BrickletNFCRFID(UID, $ipcon); // Create device object

// Callback function for state changed callback
function cb_state_changed($state, $idle)
{
	global $nfc;

	if($state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY) {
		echo "Tag found\n";

		// Write 16 byte to pages 5-8
		$data_write = array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		$nfc->writePage(5, $data_write);
		echo "Writing data...\n";
	} else if($state == BrickletNFCRFID::STATE_WRITE_PAGE_READY) {
		// Request pages 5-8
		$nfc->requestPage(5);
		echo "Requesting data...\n";
	} else if($state == BrickletNFCRFID::STATE_REQUEST_PAGE_READY) {
		// Get and print pages
		$data = $nfc->getPage();
		echo "Read data: [" . $data[0];
		for($i = 1; $i < count($data); $i++) {
			echo ", " . $data[$i];
		}
		echo "]\n";
	} else if(($state & (1 << 6)) == (1 << 6)) {
		// All errors have bit 6 set
		echo "Error: " . $state . "\n";
	}
}


$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register state chaged callback to function cb_state_changed
$nfc->registerCallback(BrickletNFCRFID::CALLBACK_STATE_CHANGED, 'cb_state_changed');

// Select NFC Forum Type 2 tag
$nfc->requestTagID(BrickletNFCRFID::TAG_TYPE_TYPE2);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
