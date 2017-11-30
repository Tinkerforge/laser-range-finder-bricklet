Imports System
Imports System.Threading
Imports Tinkerforge

Module ExampleThreshold
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Laser Range Finder Bricklet

    ' Callback subroutine for distance reached callback (parameter has unit cm)
    Sub DistanceReachedCB(ByVal sender As BrickletLaserRangeFinder, _
                          ByVal distance As Integer)
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

        ' Get threshold callbacks with a debounce time of 10 seconds (10000ms)
        lrf.SetDebouncePeriod(10000)

        ' Register distance reached callback to subroutine DistanceReachedCB
        AddHandler lrf.DistanceReachedCallback, AddressOf DistanceReachedCB

        ' Configure threshold for distance "greater than 20 cm" (unit is cm)
        lrf.SetDistanceCallbackThreshold(">"C, 20, 0)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        lrf.DisableLaser() ' Turn laser off
        ipcon.Disconnect()
    End Sub
End Module
