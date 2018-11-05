object fmIEC104M: TfmIEC104M
  Left = 719
  Top = 706
  Width = 612
  Height = 452
  Caption = 'fmIEC104M'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lbPackt: TLabel
    Left = 8
    Top = 8
    Width = 54
    Height = 13
    Caption = 'Packets ok'
  end
  object lbPacktNok: TLabel
    Left = 8
    Top = 32
    Width = 56
    Height = 13
    Caption = 'lbPacktNok'
  end
  object mmLOG: TMemo
    Left = 8
    Top = 72
    Width = 577
    Height = 329
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object cbLog: TCheckBox
    Left = 8
    Top = 56
    Width = 97
    Height = 17
    Caption = 'Log'
    TabOrder = 1
  end
  object Timer1: TTimer
    Interval = 20000
    OnTimer = Timer1Timer
    Left = 472
  end
  object Timer2: TTimer
    Interval = 2000
    OnTimer = Timer2Timer
    Left = 520
  end
  object IdUDPServer1: TIdUDPServer
    Active = True
    BufferSize = 163840
    Bindings = <>
    DefaultPort = 8099
    OnUDPRead = IdUDPServer1UDPRead
    Left = 328
  end
end
