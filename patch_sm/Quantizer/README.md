# Quantizer

## Author

April Lee

## Description

This is a quantizer that allows the user to specify a scale that incoming CV 
intput on `CV_5` will be quantized to. Quantized CV is output to `CV_OUT_1`.
The output CV will only be updated when a gate trigger is present on 
`gate_in_1`.

## Build

Here's the command to build the program on the target device, after resetting 
it by holding `boot` and pressing `reset`.

`make clean; make; make program-dfu`

## Controls

| Pin Name | Pin Location | Function | Comment |
| --- | --- | --- | --- |
| gate_in_1 | B10 | Quantizer gate | Sets the pitch of the oscillator |
| CV_5 | C6 | 1V/Octave Input | Input signal to be quantized |
| CV_OUT_1 | C10 | Output CV | The quantized CV output |
