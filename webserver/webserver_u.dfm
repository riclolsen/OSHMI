object fmWebServ: TfmWebServ
  Left = 190
  Top = 390
  Width = 592
  Height = 547
  Caption = 'fmWebServ'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object lbOutroIHM: TLabel
    Left = 176
    Top = 8
    Width = 54
    Height = 13
    Caption = 'lbOutroIHM'
  end
  object btSinc: TButton
    Left = 488
    Top = 8
    Width = 65
    Height = 25
    Caption = 'Get Sync.'
    TabOrder = 0
    OnClick = btSincClick
  end
  object btResetServer: TButton
    Left = 400
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Reset Server'
    TabOrder = 1
    OnClick = btResetServerClick
  end
  object mmWeb: TMemo
    Left = 8
    Top = 72
    Width = 561
    Height = 433
    Anchors = [akLeft, akTop, akRight, akBottom]
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object cbLogar: TCheckBox
    Left = 8
    Top = 48
    Width = 97
    Height = 17
    Caption = 'Log'
    TabOrder = 3
  end
  object IdHTTPServer1: TIdHTTPServer
    Active = True
    Bindings = <>
    CommandHandlers = <>
    DefaultPort = 51908
    Greeting.NumericCode = 0
    MaxConnectionReply.NumericCode = 0
    OnException = IdHTTPServer1Exception
    ReplyExceptionCode = 0
    ReplyTexts = <>
    ReplyUnknownCommand.NumericCode = 0
    OnCommandGet = IdHTTPServer1CommandGet
    Left = 336
    Top = 16
  end
  object NMHTTP1: TNMHTTP
    Port = 51908
    TimeOut = 1
    ReportLevel = 0
    OnStatus = NMHTTP1Status
    Body = 'Default.htm'
    Header = 'Head.txt'
    InputFileMode = False
    OutputFileMode = False
    OnSuccess = NMHTTP1Success
    ProxyPort = 0
    Left = 416
    Top = 48
  end
  object Timer2: TTimer
    Interval = 10000
    OnTimer = Timer2Timer
    Left = 368
    Top = 48
  end
  object Timer3: TTimer
    OnTimer = Timer3Timer
    Left = 368
    Top = 80
  end
end
