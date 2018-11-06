object fmJSON: TfmJSON
  Left = 1144
  Top = 387
  Width = 787
  Height = 554
  Caption = 'fmJSON'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object mmJSONLog: TMemo
    Left = 32
    Top = 64
    Width = 681
    Height = 425
    ReadOnly = True
    TabOrder = 0
  end
  object cbJSONLog: TCheckBox
    Left = 32
    Top = 40
    Width = 97
    Height = 17
    Caption = 'Log'
    TabOrder = 1
  end
  object IdUDPServer1: TIdUDPServer
    Active = True
    BufferSize = 163840
    Bindings = <>
    DefaultPort = 9100
    OnUDPRead = IdUDPServer1UDPRead
    Left = 704
    Top = 8
  end
end
