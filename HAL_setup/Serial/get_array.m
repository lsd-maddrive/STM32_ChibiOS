delete(instrfind);
dat = serial('COM8', 'BaudRate', 115200);
dat.InputBufferSize = 4096;

fopen(dat)
set(dat, 'ByteOrder', 'littleEndian')

disp 'Ok!';

fwrite(dat, 2, 'uint8');

array = fread(dat, [3, 1], 'uint32')

fclose(dat);
