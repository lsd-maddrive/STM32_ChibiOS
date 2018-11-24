import serial
import time

ser = serial.Serial('/dev/ttyACM1', 115200,timeout=5)
ser.flushInput()
ser.flushOutput()

start_time = time.time()

# ser.write(str.encode('write\n\r'))
ser.write(str.encode('a'))
# sleep(.1)

ser.read(1024 * 1024)

end_time = time.time()

print('%s KB/s' % (1000. / (end_time - start_time)))

# ser.write(str.encode('a'))

ser.close()


