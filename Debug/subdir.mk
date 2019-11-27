################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../alarm.c \
../ecg.c \
../ecg_test.c \
../radio.c 

OBJS += \
./alarm.o \
./ecg.o \
./ecg_test.o \
./radio.o 

C_DEPS += \
./alarm.d \
./ecg.d \
./ecg_test.d \
./radio.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


