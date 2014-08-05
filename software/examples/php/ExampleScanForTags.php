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
$tag_type = 0;

// Callback function for state changed callback
function cb_state_changed($state, $idle)
{
	global $nfc;

	if($idle) {
		global $tag_type;
		$tag_type = ($tag_type + 1) % 3;
		$nfc->requestTagID($tag_type);
	}

	if($state == BrickletNFCRFID::STATE_REQUEST_TAG_ID_READY) {
		$ret = $nfc->getTagID();
		echo "Found tag of type " . $ret["tag_type"] . " with ID [" . $ret["tid"][0];

		for($i = 1; $i < $ret["tid_length"]; $i++) {
			echo " " . dechex($ret["tid"][$i]);
		}

		echo "]\n";
	}
}

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register state chaged callback to function cb_state_changed
$nfc->registerCallback(BrickletNFCRFID::CALLBACK_STATE_CHANGED, 'cb_state_changed');

$nfc->requestTagID(BrickletNFCRFID::TAG_TYPE_MIFARE_CLASSIC);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
