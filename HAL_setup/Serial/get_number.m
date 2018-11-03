delete(instrfind);
dat = serial('COM8', 'BaudRate', 115200);
dat.InputBufferSize = 4096;

fopen(dat)
set(dat, 'ByteOrder', 'littleEndian')

disp 'Ok!';

fwrite(dat, 1, 'uint8');

number = fread(dat, 1, 'uint16')

fclose(dat);
