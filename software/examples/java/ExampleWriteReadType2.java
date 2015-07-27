import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletNFCRFID;

public class ExampleWriteReadType2 {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;
	private static final String UID = "XYZ"; // Change to your UID

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions you
	//       might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		final BrickletNFCRFID nfcrfid = new BrickletNFCRFID(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Add and implement state changed listener (called if state changes)
		nfc.addStateChangedListener(new BrickletNFCRFID.StateChangedListener() {
			public void stateChanged(short state, boolean idle) {
				try {
					if(state == BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
						System.out.println("Tag found");

						// Write 16 byte to pages 5-8
						short[] dataWrite = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

						nfcrfid.writePage(5, dataWrite);
						System.out.println("Writing data...");
					} else if(state == BrickletNFCRFID.STATE_WRITE_PAGE_READY) {
						// Request pages 5-8
						nfcrfid.requestPage(5);
						System.out.println("Requesting data...");
					} else if(state == BrickletNFCRFID.STATE_REQUEST_PAGE_READY) {
						// Get and print pages
						short[] data = nfcrfid.getPage();
						String s = "Read data: [" + data[0];

						for(int i = 1; i < data.length; i++) {
							s += " " + data[i];
						}

						s += "]";
						System.out.println(s);
					} else if((state & (1 << 6)) == (1 << 6)) {
						// All errors have bit 6 set
						System.out.println("Error: " + state);
					}

				} catch(Exception e) {
					System.out.println(e);
				}
			}
		});

		// Select NFC Forum Type 2 tag
		nfcrfid.requestTagID(BrickletNFCRFID.TAG_TYPE_TYPE2);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
