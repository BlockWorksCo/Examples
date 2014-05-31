

set AVRDUDE="C:\Program Files (x86)\Arduino\hardware/tools/avr/bin/avrdude"
set AVRCONF="C:\Program Files (x86)\Arduino\hardware/tools/avr/etc/avrdude.conf"

rem %AVRDUDE% -cusbasp -p m328p -Pusb -e -u -U lock:w:0x3f:m -U efuse:w:0x05:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m



rem
rem via usbasp
rem
rem %AVRDUDE% -C%AVRCONF% -v -v -v -v -patmega328p -cusbasp -Pusb -Uflash:w:%1:i

rem
rem Via bootloader
rem
%AVRDUDE% -C%AVRCONF% -patmega328p -carduino -P\\.\COM3 -b19200 -D -Uflash:w:%1:i
