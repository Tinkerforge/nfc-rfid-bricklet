<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletNFCRFID.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletNFCRFID;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your NFC/RFID Bricklet

$tag_type = 0;

// Callback function for state changed callback
function cb_stateChanged($state, $idle, $user_data)
{
	$nr = $user_data;

	if($idle) {
		global $tag_type;
		$tag_type = ($tag_type + 1) % 3;
		$nr->requestTagID($tag_type);
	}

	if($state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY) {
		$ret = $nr->getTagID();
		echo "Found tag of type " . $ret["tag_type"] . " with ID [" . dechex($ret["tid"][0]);

		for($i = 1; $i < $ret["tid_length"]; $i++) {
			echo " " . dechex($ret["tid"][$i]);
		}

		echo "]\n";
	}
}

$ipcon = new IPConnection(); // Create IP connection
$nr = new BrickletNFCRFID(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register state changed callback to function cb_stateChanged
$nr->registerCallback(BrickletNFCRFID::CALLBACK_STATE_CHANGED, 'cb_stateChanged', $nr);

// Start scan loop
$nr->requestTagID(BrickletNFCRFID::TAG_TYPE_MIFARE_CLASSIC);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
