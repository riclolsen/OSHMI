object fmHist: TfmHist
  Left = 226
  Top = 194
  Width = 330
  Height = 210
  Caption = 'Historical Recoding'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 64
    Top = 8
    Width = 32
    Height = 13
    Caption = 'Label3'
  end
  object Label4: TLabel
    Left = 88
    Top = 32
    Width = 32
    Height = 13
    Caption = 'Label4'
  end
  object Label6: TLabel
    Left = 48
    Top = 56
    Width = 32
    Height = 13
    Caption = 'Label6'
  end
  object lbErro: TLabel
    Left = 16
    Top = 104
    Width = 27
    Height = 13
    Caption = 'lbErro'
  end
  object Label1: TLabel
    Left = 16
    Top = 56
    Width = 25
    Height = 13
    Caption = 'Calls:'
  end
  object Label2: TLabel
    Left = 16
    Top = 8
    Width = 39
    Height = 13
    Caption = 'ListSize:'
  end
  object Label5: TLabel
    Left = 16
    Top = 32
    Width = 60
    Height = 13
    Caption = 'TimeLstRec:'
  end
  object Label7: TLabel
    Left = 16
    Top = 80
    Width = 49
    Height = 13
    Caption = 'CntWrites:'
  end
  object lbWrites: TLabel
    Left = 72
    Top = 80
    Width = 38
    Height = 13
    Caption = 'lbWrites'
  end
  object btApagaAntigos: TButton
    Left = 96
    Top = 120
    Width = 97
    Height = 25
    Caption = 'Clear Old Values'
    TabOrder = 0
    OnClick = btApagaAntigosClick
  end
  object Timer1: TTimer
    Interval = 450
    OnTimer = Timer1Timer
    Left = 232
    Top = 8
  end
end
