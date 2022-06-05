# ASL Project Group 40 - SS2022
#Project FAST10 Corner Detection

## Folder Structure

`src` contains the algorithm implementation and optimizations
`scrips` contains the python scripts to make the plots
`output` contains the performance measurements
`plots` contains the visual plots
`uica` contains the assembly of the different checks and the analysis of the tool

##Build Instructions

The project build system is cmake, libcvd has the following dependencies:

```shell
sudo apt-get install libjpeg-dev libpng-dev libtiff-dev libx11-dev libavformat-dev libavdevice-dev libavcodec-dev libavutil-dev libswresample-dev libglu-dev libdc1394-22-dev
```

Then run  

```shell

mkdir build
cd build
cmake ..
make
```

The build system produce the following executables: fast10, test-vtune.

To run the tests run :

``` shell
./fast10
```
