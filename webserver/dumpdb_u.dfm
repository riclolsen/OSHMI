object fmDumpdb: TfmDumpdb
  Left = 515
  Top = 288
  Width = 355
  Height = 185
  Anchors = [akLeft, akTop, akRight, akBottom]
  Caption = 'Dumpdb'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 48
    Width = 6
    Height = 13
    Caption = '0'
  end
  object lbErro: TLabel
    Left = 16
    Top = 96
    Width = 43
    Height = 13
    Caption = 'No errors'
  end
  object Label2: TLabel
    Left = 16
    Top = 64
    Width = 32
    Height = 13
    Caption = 'Label2'
  end
  object Label3: TLabel
    Left = 280
    Top = 64
    Width = 32
    Height = 13
    Caption = 'Label3'
  end
  object Label4: TLabel
    Left = 280
    Top = 80
    Width = 32
    Height = 13
    Caption = 'Label4'
  end
  object Label5: TLabel
    Left = 16
    Top = 80
    Width = 32
    Height = 13
    Caption = 'Label5'
  end
  object Label6: TLabel
    Left = 280
    Top = 96
    Width = 32
    Height = 13
    Caption = 'Label6'
  end
  object btDump: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Hint = 'Executa o Dump'
    Caption = 'Dump DB'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = btDumpClick
  end
  object Timer1: TTimer
    Interval = 1200000
    OnTimer = Timer1Timer
    Left = 88
    Top = 112
  end
  object Timer2: TTimer
    Interval = 1200
    OnTimer = Timer2Timer
    Left = 120
    Top = 112
  end
end
