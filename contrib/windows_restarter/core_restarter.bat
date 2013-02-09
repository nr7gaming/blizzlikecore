@ECHO OFF
@title BlizzLikeCore
CLS
ECHO Initializing Core (World-Server)...
:1
start "BlizzLike Core" /B /MIN /WAIT worldserver.exe -c worldserver.conf
if %errorlevel% == 0 goto end
goto 1
:end