#!/bin/bash

openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "gdb_port 3333"
