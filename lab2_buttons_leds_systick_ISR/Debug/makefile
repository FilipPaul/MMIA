################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
lab2_buttons_leds_systick_ISR.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
lab2_buttons_leds_systick_ISR.list \

OBJCOPY_BIN += \
lab2_buttons_leds_systick_ISR.bin \


# All Target
all: lab2_buttons_leds_systick_ISR.elf secondary-outputs

# Tool invocations
lab2_buttons_leds_systick_ISR.elf: $(OBJS) $(USER_OBJS) C:\Users\Filip\Desktop\MMIA\lab2_buttons_leds_systick_ISR\STM32F030R8TX_FLASH.ld
	arm-none-eabi-gcc -o "lab2_buttons_leds_systick_ISR.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\Filip\Desktop\MMIA\lab2_buttons_leds_systick_ISR\STM32F030R8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="lab2_buttons_leds_systick_ISR.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

lab2_buttons_leds_systick_ISR.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "lab2_buttons_leds_systick_ISR.list"
	@echo 'Finished building: $@'
	@echo ' '

lab2_buttons_leds_systick_ISR.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "lab2_buttons_leds_systick_ISR.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
