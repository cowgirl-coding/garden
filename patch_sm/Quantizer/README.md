# Quantizer

## Author

April Lee

## Description

This is a quantizer that allows the user to specify a scale that incoming CV 
intput on `CV_5` will be quantized to. Quantized CV is output to `CV_OUT_1`.

## Build

Here's the command to build the program on the target device, after resetting 
it by holding `boot` and pressing `reset`.

`make clean; make; make program-dfu`
