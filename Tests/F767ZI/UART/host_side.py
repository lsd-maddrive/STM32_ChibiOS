import serial
import time
import argparse

test_speed = 1843200

argparser = argparse.ArgumentParser(description='Testing script')
argparser.add_argument('-s', '--serial', help='Device name', default='/dev/ttyUSB0')

args = argparser.parse_args()
serial_name = args.serial

ser = serial.Serial(serial_name, test_speed, timeout=10)
ser.flushInput()
ser.flushOutput()


test_size_bytes = 1024 * 10
chunk_size		= 16
chunk_count		= int(test_size_bytes / chunk_size)

send_pkg = bytearray([0xff]) + \
			chunk_size.to_bytes(4, byteorder='little') + \
			chunk_count.to_bytes(4, byteorder='little')

ser.write(send_pkg)


start_time = time.time()

result = ser.read(test_size_bytes)

if len(result) != test_size_bytes:
	print('Result len %d vs expected %d' % (len(result), test_size_bytes))
	print('Timeout/Error')
	exit(1)


end_time = time.time()

# Baudrate / 10 / 1024 ~ KB/s
print('Chunk size %d / count %d' % (chunk_size, chunk_count))
print('Measured %s KB/s' % (test_size_bytes / (end_time - start_time) / 1024.))
print('Approximated UART %s KB/s' % (test_speed / 10. / 1024.))

ser.close()


