# VxWorks Raspberry Pi Thermostat

## Overview

A hardware thermostat and HVAC controller built for my Raspberry Pi 4B running VxWorks 7. It reads temperature (and humidity) from a DHT22 sensor, accepts button input through interrupts, drives a segmented display, and controls a relay that would control fan, cooling, and heating modes on an HVAC system (will test soon!!!).

I was finally brave enough to face C++, the old boy who scared me when I was a young one. A real time OS, tasks and priorities, semaphores, mutexes, interrupts for the buttons, hardware timings to measure out the pulses of the DHT sensor, and most of my GPIO pins used. Oh yeah, and it's in Kernel space (DKM). It's got it all.

## Features

- Custom DHT22 driver using direct GPIO access, high-resolution pulse measurement, and checksum validation
- Off, Fan, AC, and Heat operating modes with an adjustable temperature setpoint
- Interrupt-driven buttons
- Four-digit seven-segment display through a 74HC595 shift register
- Relay control with AC compressor restart protection
- Development cleanup routine for stopping tasks and releasing hardware resources

## Design

The application is divided into small modules for sensor input, button handling, display output, relay control, shared state, and startup. A central `AppContext` contains the system state and VxWorks synchronization objects.

Three tasks perform the main work:

- The **controller task** reads the DHT22, updates the current temperature, applies the selected HVAC mode, and manages compressor lockout.
- The **button task** wakes when a GPIO interrupt gives a binary semaphore, then handles user input.
- The **display task** snapshots the shared state and displays temp and mode (unless you are inputting temp or mode, then it shows that).

## Hardware

- Raspberry Pi 4B
- DHT22 / AM2302 temperature and humidity sensor
- Four-digit seven-segment display
- 74HC595 shift register
- Four-channel relay module
- Three push buttons with external pull-down resistors

## Building

Source the Wind River SDK environment, configure the DKM toolchain, and build:

```bash
source /path/to/wrsdk-vxworks7-raspberrypi4b/sdkenv.sh

cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE="$WIND_SDK_HOME/vxsdk/sysroot/mk/dkm.toolchain.cmake"
cmake --build build
```

The resulting module is written to:

```text
build/dkm.out
```

## Running

Load the module onto the VxWorks target and call the exported startup function from the shell:

```text
-> startTherm
```

The app initializes its GPIO resources and sync objects before spawning the controller, button, and display tasks.


To stop all tasks and free resources:
```text
-> freeAll
```
