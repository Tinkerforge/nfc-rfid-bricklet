#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletNFCRFID;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
our $nr = Tinkerforge::BrickletNFCRFID->new(&UID, $ipcon); # Create device object

# Callback function for state changed callback
sub cb_state_changed
{
	my ($state, $idle) = @_;

	if($state == Tinkerforge::BrickletNFCRFID->STATE_REQUEST_TAG_ID_READY) {
		print "Tag found\n";

		# Write 16 byte to pages 5-8
		my @data_write = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

		$nr->write_page(5, @data_write);
		print "Writing data...\n";
	} elsif($state == Tinkerforge::BrickletNFCRFID->STATE_WRITE_PAGE_READY) {
		# Request pages 5-8
		$nr->request_page(5);
		print "Requesting data...\n";
	} elsif($state == Tinkerforge::BrickletNFCRFID->STATE_REQUEST_PAGE_READY) {
		# Get and print pages
		my @data = $nr->get_page();
		my $s = "Read data: [" . $data[0];

		for(my $i = 1; $i < scalar @data; $i++) {
			$s = $s . " " . $data[$i];
		}

		$s = $s . "]\n";

		print $s;
	} elsif(($state & (1 << 6)) == (1 << 6)) {
		# All errors have bit 6 set
		print "Error: " . $state . "\n";
	}
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register state changed callback to function cb_state_changed
$nr->register_callback($nr->CALLBACK_STATE_CHANGED, 'cb_state_changed');

# Select NFC Forum Type 2 tag
$nr->request_tag_id($nr->TAG_TYPE_TYPE2);

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
