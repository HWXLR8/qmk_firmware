#!/bin/bash

sudo avrdude -p m32u4 -c avr109 -P /dev/ttyACM0  -U flash:w:"$1"
