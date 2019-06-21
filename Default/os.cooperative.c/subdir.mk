################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os.cooperative.c/workq.c 

OBJS += \
./os.cooperative.c/workq.o 

C_DEPS += \
./os.cooperative.c/workq.d 


# Each subdirectory must supply rules for building sources it contributes
os.cooperative.c/%.o: ../os.cooperative.c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -pthread -I"../" -I"../os.cooperative.c" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


