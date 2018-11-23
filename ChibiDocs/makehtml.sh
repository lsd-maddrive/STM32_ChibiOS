#!/bin/bash

if [ ! -n "$CHIBIOS_ROOT" ]; then
    	echo "CHIBIOS_ROOT is not set"
	exit 1
fi

rm html/*
doxygen Doxyfile_html
rm html/*.md5
rm html/*.map

tar czf docs.tar.gz index.html html
