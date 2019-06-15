% Close all previously opened serial connections
delete(instrfind);

% Create Serial Communication Object (SCO) on COM8 with BaudRate = 115200
dat = serial('COM8', 'BaudRate', 115200);
% Set Receive buffer size = 4 KB
dat.InputBufferSize = 4096;

% Open SCO channel (Start working)
fopen(dat)
% Setup byte order of communication to LE (https://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D1%80%D1%8F%D0%B4%D0%BE%D0%BA_%D0%B1%D0%B0%D0%B9%D1%82%D0%BE%D0%B2)
set(dat, 'ByteOrder', 'littleEndian')
% Print 'Ok!' to workspace to show that we successfully started
disp 'Ok!';

% Write one byte (uint8) with value '2' to SCO
fwrite(dat, 2, 'uint8');

% Wait for three ([3, 1] - array/vector column) uint32 numbers in LE (set previously)
array = fread(dat, [3, 1], 'uint32')

% Close SCO (Stop working)
fclose(dat);
