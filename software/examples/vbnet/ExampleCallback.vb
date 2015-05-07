Imports Tinkerforge
Imports System.Threading

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    ' Callback function for distance callback (parameter has unit cm)
    Sub DistanceCB(ByVal sender As BrickletLaserRangeFinder, ByVal distance As Integer)
        System.Console.WriteLine("Distance: " + distance.ToString() + " cm")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim lrf As New BrickletLaserRangeFinder(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

		' Turn laser on and wait 250ms for very first measurement to be ready
		lrf.EnableLaser()
		Thread.Sleep(250)

        ' Set Period for distance callback to 1s (1000ms)
        ' Note: The distance callback is only called every second if the
        '       distance has changed since the last call!
        lrf.SetDistanceCallbackPeriod(1000)

        ' Register distance callback to function DistanceCB
        AddHandler lrf.Distance, AddressOf DistanceCB

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
