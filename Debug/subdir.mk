################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../alarm.c \
../alarm2.c \
../ecg_test.c \
../radio.c \
../radio_test.c 

OBJS += \
./alarm.o \
./alarm2.o \
./ecg_test.o \
./radio.o \
./radio_test.o 

C_DEPS += \
./alarm.d \
./alarm2.d \
./ecg_test.d \
./radio.d \
./radio_test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


