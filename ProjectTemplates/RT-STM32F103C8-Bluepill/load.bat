st-link_cli -ME
st-link_cli -P %~dp0/build/ch.bin 0x08000000
st-link_cli -Rst

:pause
sleep 2
