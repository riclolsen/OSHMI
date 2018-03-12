object fmSimul: TfmSimul
  Left = 256
  Top = 367
  Width = 655
  Height = 465
  Caption = 'Simul'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 24
    Top = 8
    Width = 64
    Height = 13
    Caption = 'Point Number'
  end
  object Label2: TLabel
    Left = 24
    Top = 56
    Width = 27
    Height = 13
    Caption = 'Value'
  end
  object Label3: TLabel
    Left = 24
    Top = 104
    Width = 27
    Height = 13
    Caption = 'Qualif'
  end
  object lbFlg: TLabel
    Left = 226
    Top = 29
    Width = 3
    Height = 13
    Caption = '-'
  end
  object lbID: TLabel
    Left = 24
    Top = 248
    Width = 3
    Height = 13
    Caption = '-'
  end
  object lbDesc: TLabel
    Left = 24
    Top = 264
    Width = 3
    Height = 13
    Caption = '-'
  end
  object lbErr: TLabel
    Left = 24
    Top = 296
    Width = 9
    Height = 13
    Caption = '---'
  end
  object Label4: TLabel
    Left = 144
    Top = 123
    Width = 60
    Height = 13
    Caption = '<---------------->'
  end
  object lbEstado: TLabel
    Left = 360
    Top = 204
    Width = 41
    Height = 13
    Caption = 'lbEstado'
  end
  object lbQuest: TLabel
    Left = 359
    Top = 73
    Width = 36
    Height = 13
    Caption = 'lbQuest'
  end
  object lbTipo: TLabel
    Left = 360
    Top = 99
    Width = 29
    Height = 13
    Caption = 'lbTipo'
  end
  object lbImposto: TLabel
    Left = 360
    Top = 123
    Width = 45
    Height = 13
    Caption = 'lbImposto'
  end
  object lbOrigem: TLabel
    Left = 359
    Top = 153
    Width = 41
    Height = 13
    Caption = 'lbOrigem'
  end
  object lbFalha: TLabel
    Left = 358
    Top = 50
    Width = 34
    Height = 13
    Caption = 'lbFalha'
  end
  object Bevel1: TBevel
    Left = 215
    Top = 191
    Width = 385
    Height = 44
  end
  object Bevel2: TBevel
    Left = 215
    Top = 119
    Width = 385
    Height = 20
  end
  object Bevel3: TBevel
    Left = 215
    Top = 143
    Width = 385
    Height = 44
  end
  object Bevel5: TBevel
    Left = 215
    Top = 71
    Width = 385
    Height = 20
  end
  object Bevel7: TBevel
    Left = 215
    Top = 47
    Width = 385
    Height = 20
  end
  object Bevel8: TBevel
    Left = 215
    Top = 95
    Width = 385
    Height = 20
  end
  object Bevel4: TBevel
    Left = 8
    Top = 248
    Width = 265
    Height = 41
  end
  object lbModo: TLabel
    Left = 40
    Top = 376
    Width = 35
    Height = 13
    Caption = 'lbModo'
  end
  object edNPonto: TEdit
    Left = 24
    Top = 24
    Width = 97
    Height = 21
    MaxLength = 6
    TabOrder = 0
    OnEnter = edNPontoEnter
  end
  object edValor: TEdit
    Left = 24
    Top = 72
    Width = 97
    Height = 21
    Hint = 'Só interressa para ponto analógico.'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnEnter = edValorEnter
  end
  object edFlags: TEdit
    Left = 24
    Top = 120
    Width = 97
    Height = 21
    MaxLength = 4
    TabOrder = 2
    OnChange = edFlagsChange
    OnEnter = edFlagsEnter
  end
  object btEscreve: TButton
    Left = 37
    Top = 181
    Width = 75
    Height = 25
    Caption = 'Write state'
    TabOrder = 3
    OnClick = btEscreveClick
  end
  object btBusca: TButton
    Left = 37
    Top = 149
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 4
    OnClick = btBuscaClick
  end
  object cbSimul: TCheckBox
    Left = 32
    Top = 336
    Width = 201
    Height = 17
    Caption = 'Simulation'
    TabOrder = 5
    OnClick = cbSimulClick
  end
  object cbQBit6: TCheckBox
    Left = 224
    Top = 73
    Width = 97
    Height = 17
    Hint = '0=Normal  1=Falha do tag de tempo para eventos'
    Caption = 'Bit 6 - Quest'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = cbQBit7Click
  end
  object cbQBit5: TCheckBox
    Left = 224
    Top = 97
    Width = 97
    Height = 17
    Hint = '1=Analógico 0=Digital'
    Caption = 'Bit 5 - Type A/D'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnClick = cbQBit7Click
  end
  object cbQBit4: TCheckBox
    Left = 224
    Top = 121
    Width = 97
    Height = 17
    Hint = '1=Imposto 0=Normal'
    Caption = 'Bit 4 - Imposed'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    OnClick = cbQBit7Click
  end
  object cbQBit3: TCheckBox
    Left = 224
    Top = 144
    Width = 97
    Height = 17
    Caption = 'Bit 3 - Origin 2'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
    OnClick = cbQBit7Click
  end
  object cbQBit2: TCheckBox
    Left = 224
    Top = 168
    Width = 97
    Height = 17
    Caption = 'Bit 2 - Origin 1'
    TabOrder = 10
    OnClick = cbQBit7Click
  end
  object cbQBit1: TCheckBox
    Left = 224
    Top = 192
    Width = 113
    Height = 17
    Hint = '1=ON para digitais, bit 1 da casa decimal para analógicos'
    Caption = 'Bit 1 - State ON'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 11
    OnClick = cbQBit7Click
  end
  object cbQBit0: TCheckBox
    Left = 224
    Top = 216
    Width = 113
    Height = 17
    Hint = '1=OFF para digitais, bit 0 da casa decimal para analógicos'
    Caption = 'Bit 0 - State OFF'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 12
    OnClick = cbQBit7Click
  end
  object btEvento: TButton
    Left = 37
    Top = 213
    Width = 75
    Height = 25
    Caption = 'Write event'
    TabOrder = 13
    OnClick = btEventoClick
  end
  object cbQBit7: TCheckBox
    Left = 224
    Top = 49
    Width = 121
    Height = 17
    Hint = '1=Falha 0=Normal'
    Caption = 'Bit 7 - Value Fail / old'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 14
    OnClick = cbQBit7Click
  end
  object Timer1: TTimer
    Interval = 8000
    OnTimer = Timer1Timer
    Left = 216
    Top = 8
  end
end
