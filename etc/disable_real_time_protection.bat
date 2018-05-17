rem Disable Windows Realtime Protection for better performance.
rem Must be exe as admin rights!
PowerShell Set-MpPreference -DisableRealtimeMonitoring 1
