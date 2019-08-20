
set BIN="%~dp0/build/ch.elf"
set "BIN=%BIN:\=/%"
 
openocd -s "%CHIBISTUDIO%/tools/openocd/scripts" -f board/st_nucleo_f1.cfg -c "stm32f1x.cpu configure -rtos auto;"

