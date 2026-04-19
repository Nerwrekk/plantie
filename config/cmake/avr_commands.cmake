# utility commands for programming the mcu
# They variables work because they exist in the main file "CMakeLists" and get carried over

add_custom_target(list_mcu COMMAND avrdude -p ?)

add_custom_target(read_lock_bits avrdude -c ${PROGRAMMER} -p ${MCU} -U lock:r:-:h)

# Upload the firmware with avrdude
# Example for flash: avrdude -c usbasp -p m644 -U flash:w:"C:\dev\Atmega644PlayGround\Atmega644PlayGround\Debug\Atmega644PlayGround.hex":a 
add_custom_target(flash avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${CMAKE_BINARY_DIR}/bin/plantie.hex DEPENDS hex)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom avrdude -c ${PROGRAMMER} -p ${MCU}  -U eeprom:w:${CMAKE_BINARY_DIR}/bin/plantie.eep DEPENDS eeprom)

# fuses
add_custom_target(write_fuses avrdude -c ${PROGRAMMER} -p ${MCU} -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m)
add_custom_target(read_fuses avrdude -c ${PROGRAMMER} -p ${MCU} -U hfuse:r:-:h -U lfuse:r:-:h -U efuse:r:-:h )
add_custom_target(chip_erase avrdude -c ${PROGRAMMER} -p ${MCU} -e ) #useful in case you make a mistake like setting lock bits you were not suppose too
