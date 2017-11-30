Imports System
Imports Tinkerforge

Module ExampleWriteReadType2
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your NFC/RFID Bricklet

    ' Callback subroutine for state changed callback
    Sub StateChangedCB(ByVal sender As BrickletNFCRFID, ByVal state As Byte, _
                       ByVal idle As Boolean)
        If state = BrickletNFCRFID.STATE_REQUEST_TAG_ID_READY Then
            System.Console.WriteLine("Tag found")

            ' Write 16 byte to pages 5-8
            Dim dataWrite() As Byte = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
            sender.WritePage(5, dataWrite)
            System.Console.WriteLine("Writing data...")
        ElseIf state = BrickletNFCRFID.STATE_WRITE_PAGE_READY Then
            ' Request pages 5-8
            sender.RequestPage(5)
            System.Console.WriteLine("Requesting data...")
        ElseIf state = BrickletNFCRFID.STATE_REQUEST_PAGE_READY Then
            ' Get and print pages

            Dim data As Byte() = sender.GetPage()
            Dim str As String = "" & data(0).ToString()
            For i As Integer = 0 To data.Length-1
                str = str & " " & data(i).ToString()
            Next
            System.Console.WriteLine("Read data: [{0}]", str)
        ElseIf (state And (1 << 6)) = (1 << 6) Then
            ' All errors have bit 6 set
            System.Console.WriteLine("Error: {0}", state)
        End If
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim nr As New BrickletNFCRFID(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register state changed callback to subroutine StateChangedCB
        AddHandler nr.StateChangedCallback, AddressOf StateChangedCB

        ' Select NFC Forum Type 2 tag
        nr.RequestTagID(BrickletNFCRFID.TAG_TYPE_TYPE2)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
