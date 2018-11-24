import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 1843200, timeout=20)
# ser.flushInput()
# ser.flushOutput()

start_time = time.time()

test_size_bytes = 1024 * 10

# ser.write(str.encode('write\n\r'))
ser.write(test_size_bytes.to_bytes(4, byteorder='little'))
# sleep(.1)

result = ser.read(test_size_bytes)

if len(result) != test_size_bytes:
	print('Result len: %d' % (len(result)))
	print('Timeout/Error')
	exit(1)


end_time = time.time()

# Baudrate / 10 / 1024 ~ KB/s
print('%s KB/s' % (test_size_bytes / (end_time - start_time) / 1024.))

# ser.write(str.encode('a'))

ser.close()


