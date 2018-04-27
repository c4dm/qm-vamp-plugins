@ECHO OFF
SET WIXPATH="C:\Program Files (x86)\WiX Toolset v3.11\bin"
IF NOT EXIST %WIXPATH% (
    SET WIXPATH="C:\Program Files\WiX Toolset v3.11\bin"
)
DEL qm-vamp-plugins.msi
%WIXPATH%\candle.exe -v qm-vamp-plugins.wxs
%WIXPATH%\light.exe -ext WixUIExtension -v qm-vamp-plugins.wixobj
PAUSE
DEL qm-vamp-plugins.wixobj
DEL qm-vamp-plugins.wixpdb
