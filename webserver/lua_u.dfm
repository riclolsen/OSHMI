object fmLua: TfmLua
  Left = 142
  Top = 351
  Width = 686
  Height = 560
  Caption = 'Lua Script'
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
  object lbOut: TLabel
    Left = 0
    Top = 0
    Width = 35
    Height = 13
    Caption = 'Output:'
  end
  object lbSrc: TLabel
    Left = 0
    Top = 208
    Width = 37
    Height = 13
    Caption = 'Source:'
  end
  object Label1: TLabel
    Left = 176
    Top = 200
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 320
    Top = 200
    Width = 32
    Height = 13
    Caption = 'Label2'
  end
  object Label3: TLabel
    Left = 392
    Top = 200
    Width = 32
    Height = 13
    Caption = 'Label3'
  end
  object mmSrc: TMemo
    Left = 0
    Top = 224
    Width = 672
    Height = 301
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 0
    OnDblClick = mmSrcDblClick
  end
  object mmErr: TMemo
    Left = 0
    Top = 16
    Width = 671
    Height = 169
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 1
    OnDblClick = mmErrDblClick
  end
  object btExec: TButton
    Left = 88
    Top = 192
    Width = 75
    Height = 25
    Caption = 'Exec'
    TabOrder = 2
    OnClick = btExecClick
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = Timer1Timer
    Left = 528
    Top = 192
  end
end
