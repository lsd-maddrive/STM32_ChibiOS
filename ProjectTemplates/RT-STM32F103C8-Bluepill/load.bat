:: Full memory erase
st-link_cli -ME
:: Load ch.bin file from /build folder to memory 
st-link_cli -P %~dp0/build/ch.bin 0x08000000
:: Reset program to start execution
st-link_cli -Rst

:pause
sleep 2
