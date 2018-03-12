object fmSimTR: TfmSimTR
  Left = 277
  Top = 191
  Width = 668
  Height = 706
  Caption = 'Sim_TR - Version 1.02'
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
    Left = 8
    Top = 0
    Width = 80
    Height = 13
    Caption = 'Arquivo Sim_TR:'
  end
  object lbMsg: TLabel
    Left = 56
    Top = 120
    Width = 28
    Height = 13
    Caption = 'lbMsg'
  end
  object lbLine: TLabel
    Left = 56
    Top = 96
    Width = 9
    Height = 16
    Caption = '0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 360
    Top = 0
    Width = 44
    Height = 13
    Caption = 'Destinos:'
  end
  object lbNomeArq: TLabel
    Left = 8
    Top = 16
    Width = 52
    Height = 13
    Caption = 'lbNomeArq'
  end
  object Label2: TLabel
    Left = 8
    Top = 96
    Width = 29
    Height = 13
    Caption = 'Linha:'
  end
  object Label4: TLabel
    Left = 8
    Top = 120
    Width = 31
    Height = 13
    Caption = 'Ponto:'
  end
  object Label5: TLabel
    Left = 8
    Top = 72
    Width = 36
    Height = 13
    Caption = 'Estado:'
  end
  object lbEstado: TLabel
    Left = 56
    Top = 72
    Width = 41
    Height = 13
    Caption = 'lbEstado'
  end
  object btFileOpen: TButton
    Left = 8
    Top = 32
    Width = 97
    Height = 25
    Caption = 'Choose &File'
    TabOrder = 0
    OnClick = btFileOpenClick
  end
  object btPlay: TBitBtn
    Left = 8
    Top = 176
    Width = 75
    Height = 25
    Caption = '> &Play'
    TabOrder = 1
    OnClick = btPlayClick
  end
  object btPause: TBitBtn
    Left = 88
    Top = 176
    Width = 75
    Height = 25
    Caption = '|| Pa&use'
    TabOrder = 2
    OnClick = btPauseClick
  end
  object btFim: TBitBtn
    Left = 168
    Top = 176
    Width = 75
    Height = 25
    Caption = 'X &End'
    TabOrder = 3
    OnClick = btFimClick
  end
  object lbSim: TListBox
    Left = 0
    Top = 216
    Width = 658
    Height = 457
    Anchors = [akLeft, akTop, akRight, akBottom]
    Enabled = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    MultiSelect = True
    ParentFont = False
    TabOrder = 4
  end
  object sgDestinos: TStringGrid
    Left = 360
    Top = 16
    Width = 298
    Height = 185
    Anchors = [akLeft, akTop, akRight]
    ColCount = 3
    FixedCols = 0
    RowCount = 20
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    ScrollBars = ssVertical
    TabOrder = 5
    ColWidths = (
      96
      114
      60)
    RowHeights = (
      20
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24
      24)
  end
  object OpenDialog1: TOpenDialog
    Left = 256
    Top = 65528
  end
  object IdUDPClient1: TIdUDPClient
    BroadcastEnabled = True
    Port = 65280
    Left = 192
    Top = 65528
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 224
    Top = 65528
  end
end
