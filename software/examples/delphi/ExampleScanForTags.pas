program ExampleCallback;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletNFCRFID;

type
  TExample = class
  private
    ipcon: TIPConnection;
    nfc: TBrickletNFCRFID;
    tagType: byte;
  public
    procedure StateChangedCB(sender: TBrickletNFCRFID; const state: byte; const idle: boolean);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'hjw'; { Change to your UID }

var
  e: TExample;

{ Callback function for state changed callback }
procedure TExample.StateChangedCB(sender: TBrickletNFCRFID; const state: byte; const idle: boolean);
  var newTagType: byte;
  var tidLength: byte;
  var tid: Array[0..6] of byte;
  var s: String;
  var i: byte;
begin
  if idle then begin
    tagType := (tagType + 1) Mod 3;
    sender.RequestTagID(tagType);
  end;

  if state = BRICKLET_NFC_RFID_STATE_REQUEST_TAG_ID_READY then begin
    sender.GetTagID(newTagType, tidLength, tid);
	s := 'Fount tag of type ' + IntToStr(newTagType);
	s := s + ' with ID [' + IntToHex(tid[0], 2);
    if tidLength = 7 then begin
      for i:= 1 to 6 do begin
        s := s + ', ' + IntToHex(tid[i], 2);
      end;
    end
    else begin
      for i:= 1 to 3 do begin
        s := s + ', ' + IntToHex(tid[i], 2);
      end;
    end;
    s := s + ']';
    WriteLn(s);
  end;
end;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  nfc := TBrickletNFCRFID.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Register state changed callback to procedure StateChangedCB }
  nfc.OnStateChanged := {$ifdef FPC}@{$endif}StateChangedCB;
  
  tagType := BRICKLET_NFC_RFID_TAG_TYPE_MIFARE_CLASSIC;
  nfc.RequestTagID(tagType);

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
