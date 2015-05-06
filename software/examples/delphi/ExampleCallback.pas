program ExampleCallback;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletLaserRangeFinder;

type
  TExample = class
  private
    ipcon: TIPConnection;
    lrf: TBrickletLaserRangeFinder;
  public
    procedure DistanceCB(sender: TBrickletLaserRangeFinder; const distance: word);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change to your UID }

var
  e: TExample;

{ Callback function for distance callback (parameter has unit cm) }
procedure TExample.DistanceCB(sender: TBrickletLaserRangeFinder; const distance: word);
begin
  WriteLn(Format('Distance: %d cm', [distance]));
end;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  lrf := TBrickletLaserRangeFinder.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Turn laser on and wait 250ms for very first measurement to be ready }
  lrf.EnableLaser;
  Sleep(250);

  { Set Period for distance callback to 1s (1000ms)
    Note: The distance callback is only called every second if the
          distance has changed since the last call! }
  lrf.SetDistanceCallbackPeriod(1000);

  { Register distance callback to procedure DistanceCB }
  lrf.OnDistance := {$ifdef FPC}@{$endif}DistanceCB;

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
