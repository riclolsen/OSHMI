object fmSDE: TfmSDE
  Left = 222
  Top = 299
  Width = 400
  Height = 144
  Caption = 'SOE'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lbErro: TLabel
    Left = 47
    Top = 38
    Width = 48
    Height = 13
    Caption = 'Sem Erros'
  end
  object lbRecCnt: TLabel
    Left = 47
    Top = 3
    Width = 6
    Height = 13
    Caption = '0'
  end
  object lbDescarte: TLabel
    Left = 84
    Top = 59
    Width = 6
    Height = 13
    Caption = '0'
  end
  object lbEvento: TLabel
    Left = 55
    Top = 20
    Width = 42
    Height = 13
    Caption = 'lbEvento'
  end
  object Label1: TLabel
    Left = 10
    Top = 2
    Width = 27
    Height = 13
    Caption = 'Total:'
  end
  object Label2: TLabel
    Left = 10
    Top = 20
    Width = 37
    Height = 13
    Caption = 'Evento:'
  end
  object Label3: TLabel
    Left = 11
    Top = 37
    Width = 27
    Height = 13
    Caption = 'Erros:'
  end
  object Label4: TLabel
    Left = 10
    Top = 59
    Width = 63
    Height = 13
    Caption = 'Descartados:'
  end
  object lbRec: TLabel
    Left = 185
    Top = 3
    Width = 28
    Height = 13
    Caption = 'lbRec'
  end
  object Label11: TLabel
    Left = 184
    Top = 16
    Width = 38
    Height = 13
    Caption = 'Label11'
  end
  object Label12: TLabel
    Left = 184
    Top = 32
    Width = 38
    Height = 13
    Caption = 'Label12'
  end
  object btEmpty: TButton
    Left = 280
    Top = 0
    Width = 81
    Height = 17
    Caption = 'Empty table'
    TabOrder = 0
    OnClick = btEmptyClick
  end
  object btReindex: TButton
    Left = 280
    Top = 16
    Width = 81
    Height = 17
    Caption = 'Reindex'
    TabOrder = 1
    OnClick = btReindexClick
  end
  object btPack: TButton
    Left = 280
    Top = 32
    Width = 81
    Height = 17
    Caption = 'Pack'
    TabOrder = 2
    OnClick = btPackClick
  end
  object btApagaAntigos: TButton
    Left = 280
    Top = 48
    Width = 81
    Height = 17
    Caption = 'Delete Old'
    TabOrder = 3
    OnClick = btApagaAntigosClick
  end
  object Timer1: TTimer
    Interval = 500
    OnTimer = Timer1Timer
    Left = 144
    Top = 72
  end
end
