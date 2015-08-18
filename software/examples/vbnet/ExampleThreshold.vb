Imports Tinkerforge
Imports System.Threading

Module ExampleThreshold
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    ' Callback for distance greater than 20 cm
    Sub ReachedCB(ByVal sender As BrickletLaserRangeFinder, ByVal distance As Integer)
        System.Console.WriteLine("Distance " + distance.ToString() + " cm")
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

        ' Register threshold reached callback to function ReachedCB
        AddHandler lrf.DistanceReached, AddressOf ReachedCB

        ' Configure threshold for "greater than 20 cm" (unit is cm)
        lrf.SetDistanceCallbackThreshold(">"C, 20, 0)

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        lrf.DisableLaser() ' Turn laser off
        ipcon.Disconnect()
    End Sub
End Module
