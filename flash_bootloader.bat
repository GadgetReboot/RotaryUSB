@echo off
IF %1.==. GOTO MissingComPort
.\avrdude -C .\avrdude.conf -c avrisp -p attiny85 -P %1 -b 19200 -Uflash:w:t85_default.hex:i -U lfuse:w:0xe1:m -U hfuse:w:0x5d:m -U efuse:w:0xfe:m
GOTO End

:MissingComPort
  ECHO. 
  ECHO Specify the COM port when running this batch file eg: .\flash_bootloader.bat COM1
  ECHO.  

:End