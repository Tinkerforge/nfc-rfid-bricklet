import com.tinkerforge.BrickletNFCRFID;
import com.tinkerforge.IPConnection;

public class ExampleScanForTags {
	private static final String host = "localhost";
	private static final int port = 4223;
	private static final String UID = "hjw"; // Change to your UID
	private static short tagType = 0;
	
	// Note: To make the example code cleaner we do not handle exceptions. Exceptions you
	//       might normally want to catch are described in the commnents below
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		final BrickletNFCRFID nfc = new BrickletNFCRFID(UID, ipcon); // Create device object

		ipcon.connect(host, port); // Connect to brickd
		// Don't use device before ipcon is connected


		// Add and implement state changed listener (called if state changes)
		nfc.addStateChangedListener(new BrickletNFCRFID.StateChangedListener() {
			public void stateChanged(short state, boolean idle) {
				try {
					if(idle) {
						tagType = (short)((tagType + 1) % 3);
						nfc.requestTagID(tagType);
					}

					if(state == BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
						BrickletNFCRFID.TagID tagID = nfc.getTagID();
						String s = "Found tag of type " + tagID.tagType + 
						           " with ID [" + Integer.toHexString(tagID.tid[0]);
						if(tagID.tidLength == 7) {
							for(int i = 1; i < 7; i++) {
								s += ", " + Integer.toHexString(tagID.tid[i]);
							}
						} else {
							for(int i = 1; i < 4; i++) {
								s += ", " + Integer.toHexString(tagID.tid[i]);
							}
						}
						s += "]";
						System.out.println(s);
					}

				} catch(Exception e) {
					System.out.println(e);
				}
			}
		});

		nfc.requestTagID(BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
