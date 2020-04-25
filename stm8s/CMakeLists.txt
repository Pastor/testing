cmake_minimum_required(VERSION 3.11)
project(stm8s C)

#cmake -DCMAKE_C_COMPILER=sdcc -DCMAKE_SYSTEM_NAME=Generic
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(F_CPU 2000000)
set(MCU stm8s103f3)
set(ARCH stm8)

add_definitions(-DF_CPU=${F_CPU}UL)
set(STM8_CFLAGS
        -m${ARCH}
        -p${MCU}
        --std-sdcc11
        --stack-auto
        --noinduction
        --use-non-free
        --verbose
        -I/usr/local/Cellar/sdcc/3.9.0/share/sdcc/include)
set(STM8_LDFLAGS -m${ARCH} -l${ARCH} --out-fmt-ihx --model-medium)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#
set(CMAKE_C_FLAGS_INIT ${STM8_CFLAGS})
set(CMAKE_EXE_LINKER_FLAGS_INIT ${STM8_LDFLAGS})
#
add_compile_options(${STM8_CFLAGS})
add_link_options(${STM8_LDFLAGS})
file(GLOB SRC_FILES "lib/*.c")
file(GLOB HDR_FILES "lib/*.h")

add_executable(${PROJECT_NAME}_RAM examples/RAM_EXEC/main.c examples/RAM_EXEC/ram_blink.c examples/RAM_EXEC/ram_putchar.c ${SRC_FILES})
add_custom_target(flash_${PROJECT_NAME}_RAM stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_RAM)
add_executable(${PROJECT_NAME}_UART examples/UART/main.c ${SRC_FILES})
add_executable(${PROJECT_NAME}_SPI_LCD examples/SPI_LCD/main.c examples/SPI_LCD/LCD.c ${SRC_FILES})
add_executable(${PROJECT_NAME}_Flash examples/Flash/main.c ${SRC_FILES})
add_executable(${PROJECT_NAME}_EEPROM examples/Flash/main.c ${SRC_FILES})
add_executable(${PROJECT_NAME}_ADC examples/ADC/main.c ${SRC_FILES})
add_executable(${PROJECT_NAME}_IWDG examples/IWDG/main.c ${SRC_FILES})
#add_executable(${PROJECT_NAME}_F18A examples/F18A/f18_emu.c ${SRC_FILES})
add_custom_target(flash_${PROJECT_NAME}_UART stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_UART)
add_custom_target(flash_${PROJECT_NAME}_SPI_LCD stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_SPI_LCD)
add_custom_target(flash_${PROJECT_NAME}_Flash stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_Flash)
add_custom_target(flash_${PROJECT_NAME}_EEPROM stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_EEPROM)
add_custom_target(flash_${PROJECT_NAME}_ADC stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_ADC)
add_custom_target(flash_${PROJECT_NAME}_IWDG stm8flash -c stlinkv2 -p ${MCU} -w ${PROJECT_NAME}_IWDG)

#set_target_properties(${PROJECT_NAME}_UART PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)
#add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})
#add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
#add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)
#add_custom_target(upload C:/WinAVR-20100110/bin/avrdude  -c ${PROG_TYPE} -p ${MCU} -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)
#add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)
#add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )
#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")