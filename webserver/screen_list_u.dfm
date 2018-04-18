object FormScreenList: TFormScreenList
  Left = 0
  Top = 366
  Hint = 'Choose a screen'
  HorzScrollBar.Visible = False
  VertScrollBar.Style = ssHotTrack
  VertScrollBar.Visible = False
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  BorderWidth = 1
  Caption = 'Screen List'
  ClientHeight = 728
  ClientWidth = 882
  Color = clWhite
  Font.Charset = ANSI_CHARSET
  Font.Color = clTeal
  Font.Height = -21
  Font.Name = 'Consolas'
  Font.Style = [fsBold, fsItalic]
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  PixelsPerInch = 96
  TextHeight = 24
end
