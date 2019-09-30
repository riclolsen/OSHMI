object fmMicroHttpd: TfmMicroHttpd
  Left = 251
  Top = 40
  Width = 690
  Height = 702
  Caption = 'fmMicroHttpd'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object lbLog: TLabel
    Left = 8
    Top = 8
    Width = 26
    Height = 13
    Caption = 'lbLog'
  end
  object mmLog: TMemo
    Left = 8
    Top = 48
    Width = 617
    Height = 585
    Anchors = [akLeft, akTop, akRight, akBottom]
    Lines.Strings = (
      'mmLog')
    TabOrder = 0
  end
  object cbLog: TCheckBox
    Left = 8
    Top = 24
    Width = 97
    Height = 17
    Caption = 'cbLog'
    TabOrder = 1
  end
  object btGetSync: TButton
    Left = 128
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Get Sync'
    TabOrder = 2
    OnClick = btGetSyncClick
  end
  object Timer2: TTimer
    Interval = 10000
    OnTimer = Timer2Timer
    Left = 1288
    Top = 16
  end
end
