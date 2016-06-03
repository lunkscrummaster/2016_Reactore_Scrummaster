################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries/LiquidCrystal/src/subdir.mk
-include libraries/LiquidCrystal/examples/setCursor/subdir.mk
-include libraries/LiquidCrystal/examples/TextDirection/subdir.mk
-include libraries/LiquidCrystal/examples/SerialDisplay/subdir.mk
-include libraries/LiquidCrystal/examples/Scroll/subdir.mk
-include libraries/LiquidCrystal/examples/HelloWorld/subdir.mk
-include libraries/LiquidCrystal/examples/Display/subdir.mk
-include libraries/LiquidCrystal/examples/CustomCharacter/subdir.mk
-include libraries/LiquidCrystal/examples/Cursor/subdir.mk
-include libraries/LiquidCrystal/examples/Blink/subdir.mk
-include libraries/LiquidCrystal/examples/Autoscroll/subdir.mk
-include core/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
arduino.ar \

ELF += \
2016_Reactore_Scrummaster.elf \

EEP += \
2016_Reactore_Scrummaster.eep \

FLASH_IMAGE += \
2016_Reactore_Scrummaster.hex \


# All Target
all: 2016_Reactore_Scrummaster

# Tool invocations
2016_Reactore_Scrummaster: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-size" -A "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.elf"
	@echo 'Finished building target: $@'
	@echo ' '

arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-ar" rcs     arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

2016_Reactore_Scrummaster.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-gcc" -Os -Wl,--gc-sections,--relax -mmcu=atmega2560  -o "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.elf"   $(LINK_OBJ) $(AR)    "-L/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE" -lm
	@echo 'Finished building: $@'
	@echo ' '

2016_Reactore_Scrummaster.eep: $(ELF)
	@echo 'Create eeprom image'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-objcopy" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0  "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.elf" "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.eep"
	@echo 'Finished building: $@'
	@echo ' '

2016_Reactore_Scrummaster.hex: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-objcopy" -O ihex -R .eeprom  "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.elf" "/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/MacOS/workspace/2016_Reactore_Scrummaster/Debug_AVaRICE/2016_Reactore_Scrummaster.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(EEP)$(FLASH_IMAGE)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS) 2016_Reactore_Scrummaster
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
