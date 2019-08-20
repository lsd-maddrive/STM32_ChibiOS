
set BIN="%~dp0/build/ch.bin"
set "BIN=%BIN:\=/%"

openocd -s "%CHIBISTUDIO%/tools/openocd/scripts" -f interface\stlink-v2.cfg -f target\stm32f1x.cfg -c "program %BIN% reset exit 0x08000000"

:pause
sleep 2
