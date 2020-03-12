# Nordic BLE examples

This repository contains example applications for the Nordic [nRF52840](https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52840). They can be tested with the [nRF52840 DK](https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK).

To build the examples you need to install the install the GNU Embedded Toolchain for ARM in version 9-2019-q4-major. The toolchain can be downloaded [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). The installed toolchain needs to be in the `PATH` for CMake to pick it up.

You also need to download the nRF5 SDK version 16.0.0 and the S140 SoftDevice version 7.0.1 and place them in a checkout of this repository. You can execute the following commands from the root of a checkout to do so:

```sh
wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK160098a08e2.zip -O external/sdk.zip
mkdir external/nRF5_SDK
unzip external/sdk.zip -d external/nRF5_SDK

wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SoftDevices/S140/s140nrf52701.zip -O external/s140.zip
mkdir external/s140
unzip external/s140.zip -d external/s140
```

To flash the examples to a development kit you also need to install the [nRF Command Line Tools](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Command-Line-Tools) and ensure that the `nrfjprog` executable is inside the `PATH`.

Afterwards you can build the software with CMake:

```sh
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-files/nrf5-toolchain-file.cmake -G Ninja ..
ninja
```

For each sample application there are two build targets to flash it to the board: `flash-<app>-full` can be used to flash the application and the SoftDevice. `flash-<app>` flashes only the application:

```sh
ninja flash-advertising-full
```
