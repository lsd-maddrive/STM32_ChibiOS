
set BIN="%~dp0/build/ch.bin"
set "BIN=%BIN:\=/%"

openocd -s "%CHIBISTUDIO%/tools/openocd/scripts" -f board/st_nucleo_f7.cfg -c "program %BIN% reset exit 0x08000000"

:pause
sleep 2
