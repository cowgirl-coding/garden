# Quantizer

## Author

April Lee

## Description

This is a quantizer that allows the user to specify a scale that incoming CV 
intput on will be quantized to. The output CV will only be updated when a gate 
trigger is received.

## Controls

| Pin Name | Pin Location | Function | Comment |
| --- | --- | --- | --- |
| gate_in_1 | B10 | Quantizer gate | Sets the pitch of the oscillator |
| CV_5 | C6 | 1V/Octave Input | Input signal to be quantized |
| CV_OUT_1 | C10 | Output CV | The quantized CV output |
