@echo off

set PATH=D:\gcc-arm\bin;D:\stlink-1.3.0-win64\bin;%PATH%

cargo build --release
cargo objcopy --bin stm32f103c8t6 --target thumbv7m-none-eabi --release -- -O binary stm32f103c8t6.bin
st-flash erase
st-flash write stm32f1.bin 0x8000000