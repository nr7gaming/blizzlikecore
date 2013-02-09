@ECHO OFF
@title BlizzLikeAuth
CLS
ECHO Initializing Realm (Logon-Server)...
:1
start "BlizzLike Realm" /B /MIN /WAIT authserver.exe -c authserver.conf
if %errorlevel% == 0 goto end
goto 1
:end