# M5 - ESP32 experiments with M5Stack devices

## Introduction

This is my collection of experiments using ESP32 devices from M5Stack, e.g. the [AtomMatrix](https://docs.m5stack.com/#/en/core/atom_matrix).

Some highlights:

-  [AtomMatrixVernie](AtomMatrixVernie) : controlling a Lego Boost robot (e.g. Vernie) wireless with the AtomMatrix and its accelerometer

-  [Wiimote](Wiimote) : using a Nintendo Wii Remote/Nunchuk as wireless input device for ESP32 (e.g. to control a robot)

Some examples use forked libraries with modifications/enhancements, that you can find in my other repositories.


## PlatformIO

I use [PlatformIO](https://platformio.org/platformio-ide) and [VS code](https://code.visualstudio.com/) with the Arduino framework under Windows 10. 
All examples are part of a single PlatformIO workspace. 
It should also be possible to open the individual examples/folders directly in the Arduino IDE.

### ini file

My platformio.ini files support the Arduino-style folder structure. Here is an example for the AtomMatrix:

```
[platformio]
src_dir = .

[env:atom-matrix]
platform = espressif32
board = m5stick-c
framework = arduino
;build_flags = -fpermissive
monitor_speed = 115200
```

### ENV variables

Additionally, I use some environment variables for global folders, instead of defining them in every ini file:

| ENV variable | path |
| --- | --- |
| PLATFORMIO_LIB_EXTRA_DIRS | e.g. c:\pio\extra_libs |
| PLATFORMIO_BUILD_DIR | e.g. c:\temp\pio_build |
| PLATFORMIO_LIBDEPS_DIR | e.g. c:\temp\pio_libs|

Alternatively, I could set those directories directly in the platformio.ini file:
```
lib_extra_dirs = c:\pio\extra_libs
build_dir      = c:\temp\pio_build
libdeps_dir    = c:\temp\pio_libs
```
