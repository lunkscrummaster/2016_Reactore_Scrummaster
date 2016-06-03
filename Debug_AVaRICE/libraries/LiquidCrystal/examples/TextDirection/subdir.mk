################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.4/examples/TextDirection/TextDirection.ino 

INO_DEPS += \
./libraries/LiquidCrystal/examples/TextDirection/TextDirection.ino.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/LiquidCrystal/examples/TextDirection/TextDirection.o: /Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.4/examples/TextDirection/TextDirection.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/avr-gcc/4.8.1-arduino5/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10606 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR     -I"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.5/cores/arduino" -I"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino/hardware/avr/1.6.5/variants/mega" -I"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.4" -I"/Users/Prospect/Documents/eclipseArduino/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.4/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -w
	@echo 'Finished building: $<'
	@echo ' '


