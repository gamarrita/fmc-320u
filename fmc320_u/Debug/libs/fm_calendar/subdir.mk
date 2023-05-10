################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/githubs/fmc-320u/libs/fm_calendar/fm_calendar.c 

OBJS += \
./libs/fm_calendar/fm_calendar.o 

C_DEPS += \
./libs/fm_calendar/fm_calendar.d 


# Each subdirectory must supply rules for building sources it contributes
libs/fm_calendar/fm_calendar.o: D:/githubs/fmc-320u/libs/fm_calendar/fm_calendar.c libs/fm_calendar/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/threadx/utility/low_power -I"D:/githubs/fmc-320u/libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-fm_calendar

clean-libs-2f-fm_calendar:
	-$(RM) ./libs/fm_calendar/fm_calendar.cyclo ./libs/fm_calendar/fm_calendar.d ./libs/fm_calendar/fm_calendar.o ./libs/fm_calendar/fm_calendar.su

.PHONY: clean-libs-2f-fm_calendar

