object fmAjustes: TfmAjustes
  Left = 194
  Top = 123
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Ajustes'
  ClientHeight = 195
  ClientWidth = 276
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 24
    Width = 39
    Height = 13
    Caption = 'NPonto:'
  end
  object Label2: TLabel
    Left = 8
    Top = 56
    Width = 41
    Height = 13
    Caption = 'KConv1:'
  end
  object Label3: TLabel
    Left = 8
    Top = 88
    Width = 41
    Height = 13
    Caption = 'KConv2:'
  end
  object Label4: TLabel
    Left = 8
    Top = 120
    Width = 25
    Height = 13
    Caption = 'Conf:'
  end
  object lbErro: TLabel
    Left = 8
    Top = 176
    Width = 6
    Height = 13
    Caption = '--'
  end
  object edNPonto: TEdit
    Left = 64
    Top = 16
    Width = 121
    Height = 21
    TabOrder = 0
  end
  object edKConv1: TEdit
    Left = 64
    Top = 48
    Width = 121
    Height = 21
    TabOrder = 1
  end
  object edKConv2: TEdit
    Left = 64
    Top = 80
    Width = 121
    Height = 21
    TabOrder = 2
  end
  object btAjusta: TButton
    Left = 64
    Top = 144
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    OnClick = btAjustaClick
  end
  object edConf: TEdit
    Left = 64
    Top = 112
    Width = 121
    Height = 21
    TabOrder = 4
  end
  object btCancel: TButton
    Left = 152
    Top = 144
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 5
    OnClick = btCancelClick
  end
end
