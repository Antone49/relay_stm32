#!/bin/bash

openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init" -c "reset halt" -c "flash write_image erase bin/relay.bin 0x08000000" -c "reset" -c shutdown