Imports System
Imports Tinkerforge

Module ExampleScanForTags
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your NFC/RFID Bricklet

    Private currentTagType As Byte = 0

    ' Callback subroutine for state changed callback
    Sub StateChangedCB(ByVal sender As BrickletNFCRFID, ByVal state As Byte, _
                       ByVal idle As Boolean)
        If state = BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY Then
            Dim tagType As Byte
            Dim tidLength As Byte
            Dim tid() As Byte

            sender.GetTagID(tagType, tidLength, tid)

            If tidLength = 7 Then
                Console.WriteLine("Fount tag of type {0} with ID [{1:X} {2:X} {3:X} {4:X} {5:X} {6:X} {7:X}]",
                                  tagType, tid(0), tid(1), tid(2), tid(3), tid(4), tid(5), tid(6))
            Else
                Console.WriteLine("Fount tag of type {0} with ID [{1:X} {2:X} {3:X} {4:X}]",
                                  tagType, tid(0), tid(1), tid(2), tid(3))
            End If
        End If

        ' Cycle through all types
        If idle Then
            currentTagType = Convert.ToByte((currentTagType + 1) Mod 3)
            sender.requestTagID(currentTagType)
        End If
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim nr As New BrickletNFCRFID(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register state changed callback to subroutine StateChangedCB
        AddHandler nr.StateChangedCallback, AddressOf StateChangedCB

        ' Start scan loop
        nr.RequestTagID(BrickletNFCRFID.TAG_TYPE_MIFARE_CLASSIC)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
