# Garden

## About

This is my collection of patches for devices in the [Electrosmith 
Daisy](https://electro-smith.com/collections/daisy)
platform. Please see the included README for each patch for detailed 
instructions for its operation.

## Building Patches

Patches are grouped by hardware device, just like the [Daisy 
Examples](https://github.com/electro-smith/DaisyExamples) repository. Patches 
can be built using the same procedure outlined in the documentation for that 
repo, except that since these examples are not local to that repo and you will 
need to link with `libDaisy` and `DaisySP`, you will need to specify the 
location of the DaisyExamples repository via a `DAISY_EXAMPLES_DIRECTORY` 
environment variable.

Example to build the Quanitizer patch for Patch Submodule:

``` sh
cd patch_sm/Quantizer
export DAISY_EXAMPLES_DIRECTORY=$HOME/Code/DaisyExamples
make clean; make; make program-dfu
```
