import serial
import time
import argparse

import socket

sock = socket.socket()
sock.connect(('9.9.9.153', 80))

test_size_bytes = 1024 * 10
chunk_size		= 1
chunk_count		= int(test_size_bytes / chunk_size)



chunk_idx = 0
received_bytes = 0

send_pkg = bytearray([0xff]) + \
			chunk_size.to_bytes(4, byteorder='little') + \
			chunk_count.to_bytes(4, byteorder='little')


sock.send(send_pkg)

start_time = time.time()

while received_bytes < test_size_bytes:
	result = sock.recv(test_size_bytes)
	received_bytes += len(result)

	print('New received %d / %d' % (received_bytes, test_size_bytes))

# chunk_idx += 1

# print('Send idx %d' % (chunk_idx))


end_time = time.time()

sock.close()

if received_bytes != test_size_bytes:
	print('Result len %d vs expected %d' % (len(result), test_size_bytes))
	exit(1)

# Baudrate / 10 / 1024 ~ KB/s
print('Chunk size %d / count %d' % (chunk_size, chunk_count))
print('Measured %s KB/s' % (test_size_bytes / (end_time - start_time) / 1024.))
