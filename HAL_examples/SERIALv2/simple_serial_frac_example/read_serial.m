clear s
s = serialport('COM3', 115200);
while 1
    read(s, 10, 'single')
end