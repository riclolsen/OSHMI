object fmHist: TfmHist
  Left = 212
  Top = 177
  Width = 194
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
    Left = 16
    Top = 8
    Width = 32
    Height = 13
    Caption = 'Label3'
  end
  object Label4: TLabel
    Left = 16
    Top = 32
    Width = 32
    Height = 13
    Caption = 'Label4'
  end
  object Label6: TLabel
    Left = 16
    Top = 56
    Width = 32
    Height = 13
    Caption = 'Label6'
  end
  object lbErro: TLabel
    Left = 16
    Top = 80
    Width = 27
    Height = 13
    Caption = 'lbErro'
  end
  object btApagaAntigos: TButton
    Left = 48
    Top = 104
    Width = 97
    Height = 25
    Caption = 'Clear Old Values'
    TabOrder = 0
    OnClick = btApagaAntigosClick
  end
  object Timer1: TTimer
    Interval = 300
    OnTimer = Timer1Timer
    Left = 152
    Top = 16
  end
end
