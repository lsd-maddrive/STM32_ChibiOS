#!/bin/bash

make -j4 && st-flash write build/ch.bin 0x8000000
