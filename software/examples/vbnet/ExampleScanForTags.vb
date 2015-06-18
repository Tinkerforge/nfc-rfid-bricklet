Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "hjw" ' Change to your UID

    Private currentTagType As Byte = 0

    ' Callback function for state changed callback
    Sub StateChangedCB(ByVal sender As BrickletNFCRFID, _
                       ByVal state As Byte, ByVal idle As Boolean)
        If idle Then
            currentTagType = Convert.ToByte((currentTagType + 1) Mod 3)
            sender.requestTagID(currentTagType)
        End If

        If state = BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY Then
            Dim tagType As Byte
            Dim tidLength As Byte
            Dim tid() As Byte

            sender.GetTagID(tagType, tidLength, tid)

            If tidLength = 7 Then
                System.Console.WriteLine("Fount tag of type {0} with ID [{1:X} {2:X} {3:X} {4:X} {5:X} {6:X} {7:X}]",
                                         tagType, tid(0), tid(1), tid(2), tid(3), tid(4), tid(5), tid(6))
            Else
                System.Console.WriteLine("Fount tag of type {0} with ID [{1:X} {2:X} {3:X} {4:X}]",
                                         tagType, tid(0), tid(1), tid(2), tid(3))
            End If
        End If

    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim nfc As New BrickletNFCRFID(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register state changed callback to function StateChangedCB
        AddHandler nfc.StateChanged, AddressOf StateChangedCB

        nfc.RequestTagID(BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC)

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
