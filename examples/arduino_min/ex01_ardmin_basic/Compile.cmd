@echo off
SETLOCAL DisableDelayedExpansion EnableExtensions
rem *******************************
rem Frank Bösing 11/2018
rem Windows Batch to compile Arduino sketches

rem Usage:
rem compile.cmd 0 : compile sketch
rem compile.cmd 1 : compile & upload sketch
rem compile.cmd 2 : rebuild & upload sketch
rem - Attention: Place compile.cmd in Sketch folder!
rem
rem Edit these paths:

set arduino=C:\arduino-1.8.12
set TyTools=C:\Program Files\TyQt
set libs=C:\Users\kurte\Documents\Arduino\libraries
set tools=D:\GitHub\Tset

rem *******************************
rem Set Teensy-specific variables here:
rem


REM defragster was here 

set model=teensy40
set speed=600
set opt=o2std
set usb=serial
cd.

rem set keys=de-de
set keys=en-us

rem *******************************
rem Don't edit below this line
rem *******************************

for %%i in (*.ino) do set sketchname=%%i
if "%sketchname%"=="" (
  echo No Arduino Sketch found!
  exit 1
)

set myfolder=.\
set ino="%myfolder%%sketchname%"
set temp1="%temp%\\arduino_build_%sketchname%"
set temp2="%temp%\\arduino_cache_%sketchname%"
set fqbn=teensy:avr:%model%:usb=%usb%,speed=%speed%,opt=%opt%,keys=%keys%

if "%1"=="2" (
  echo Temp: %temp1%
  echo Temp: %temp2%
  del /s /q %temp1%>NUL
  del /s /q %temp2%>NUL
  echo Temporary files deleted.
)

if not exist %temp1% mkdir %temp1%
if not exist %temp2% mkdir %temp2%

echo Building Sketch: %ino%
"%arduino%\arduino-builder" -verbose=1 -warnings=more -compile -logger=human -hardware "%arduino%\hardware" -hardware "%LOCALAPPDATA%\Arduino15\packages" -tools "%arduino%\tools-builder" -tools "%arduino%\hardware\tools\avr" -tools "%LOCALAPPDATA%\Arduino15\packages" -built-in-libraries "%arduino%\libraries" -libraries "%libs%" -fqbn=%fqbn% -build-path %temp1% -build-cache "%temp2%"  %ino%

rem Comment line below to build prior to TeensyDuino 1.50
if "%model%"=="teensy31" set model=teensy32
if not "%1"=="0" (
  if "%errorlevel%"=="0" "%TyTools%\TyCommanderC.exe" upload --autostart --wait --multi "%temp1%\%sketchname%.%model%.hex"
  REM "%arduino%\hardware\tools\arm\bin\arm-none-eabi-gcc-nm.exe" -n "%temp1%\%sketchname%.elf" | "%tools%\imxrt_size.exe"
)

if "%1x"=="x%1" PAUSE
if not "%1x"=="x%1" exit %errorlevel%
