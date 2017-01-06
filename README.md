# Zephyr Firmware

## Introduction

## Requirements

## Flashing

```bash
$ export ZEPHYR_GCC_VARIANT=zephyr
$ export ZEPHYR_SDK_INSTALL_DIR=/home/monkeyman/Documents/zephyr-sdk

$ source zephyr-env.sh
```

![dfu-util -l](/README/dfu_util_list.png?raw=true "dfu-util -l")



### Bluetooth Firmware (ble_core)

```bash
$ make -C nvisionit/ble_core BOARD=arduino_101_ble
$ sudo dfu-util -a ble_core -D nvisionit/ble_core/outdir/arduino_101_ble/zephyr.bin
```

### Messaging App (x86_app)
```bash
$ make -C nvisionit/x86_app/<appname> BOARD=arduino_101
$ sudo -E dfu-util -a x86_app -D nvisionit/x86_app/base/outdir/arduino_101/zephyr.bin
```

### Sensor App (sensor_core)
```bash
$ make -C nvisionit/sensor_core/<appname> BOARD=arduino_101_sss
$ sudo -E dfu-util -a sensor_core -D nvisionit/sensor_core/base/outdir/arduino_101_sss/zephyr.bin
```
