#!/bin/sh


openocd -f interface/stlink-v2.cfg -f target/stm8s003.cfg -c "init" -c "reset halt"