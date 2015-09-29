import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletNFCRFID;

public class ExampleScanForTags {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;
	private static final String UID = "XYZ"; // Change to your UID
	private static short currentTagType = 0;

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		// Note: Declare nr as final, so the listener can access it
		final BrickletNFCRFID nr = new BrickletNFCRFID(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Add state changed listener
		nr.addStateChangedListener(new BrickletNFCRFID.StateChangedListener() {
			public void stateChanged(short state, boolean idle) {
				try {
					if(idle) {
						currentTagType = (short)((currentTagType + 1) % 3);
						nr.requestTagID(currentTagType);
					}

					if(state == BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY) {
						BrickletNFCRFID.TagID tagID = nr.getTagID();
						String s = "Found tag of type " + tagID.tagType + 
						           " with ID [" + Integer.toHexString(tagID.tid[0]);

						for(int i = 1; i < tagID.tidLength; i++) {
							s += " " + Integer.toHexString(tagID.tid[i]);
						}

						s += "]";
						System.out.println(s);
					}

				} catch(Exception e) {
					System.out.println(e);
				}
			}
		});

		// Start scan loop
		nr.requestTagID(BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
