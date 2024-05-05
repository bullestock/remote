#!/bin/bash
esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=no_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x1000 build/bootloader/bootloader.bin 0x10000 build/controller.bin 0x8000 build/partition_table/partition-table.bin
