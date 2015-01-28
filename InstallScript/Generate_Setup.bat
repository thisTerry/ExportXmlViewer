
@echo off

rem 增加环境变量
set PATH=%PATH%;D:\Program Files\NSIS\Bin\;
makensis.exe Script_ExportXmlViewer.nsi

pause