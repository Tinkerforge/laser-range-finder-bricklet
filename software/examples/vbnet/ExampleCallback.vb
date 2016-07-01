Imports System
Imports System.Threading
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Laser Range Finder Bricklet

    ' Callback subroutine for distance callback (parameter has unit cm)
    Sub DistanceCB(ByVal sender As BrickletLaserRangeFinder, ByVal distance As Integer)
        Console.WriteLine("Distance: " + distance.ToString() + " cm")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim lrf As New BrickletLaserRangeFinder(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Turn laser on and wait 250ms for very first measurement to be ready
        lrf.EnableLaser()
        Thread.Sleep(250)

        ' Register distance callback to subroutine DistanceCB
        AddHandler lrf.Distance, AddressOf DistanceCB

        ' Set period for distance callback to 0.2s (200ms)
        ' Note: The distance callback is only called every 0.2 seconds
        '       if the distance has changed since the last call!
        lrf.SetDistanceCallbackPeriod(200)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        lrf.DisableLaser() ' Turn laser off
        ipcon.Disconnect()
    End Sub
End Module
