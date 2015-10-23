################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./peripherals/accel250.obj" \
"./peripherals/altimeter.obj" \
"./peripherals/compass.obj" \
"./peripherals/flashstore.obj" \
"./peripherals/gpio.obj" \
"./peripherals/gps.obj" \
"./peripherals/gyro.obj" \
"./peripherals/i2c.obj" \
"./peripherals/misc.obj" \
"./peripherals/ssi.obj" \
"./peripherals/status.obj" \
"./peripherals/uart.obj" \
"./peripherals/uartstdio.obj" \
"./peripherals/tinygps/tinygps.obj" \
"./peripherals/fatfs/option/syscall.obj" \
"./peripherals/fatfs/option/unicode.obj" \
"./peripherals/fatfs/ff.obj" \
"./peripherals/fatfs/mmc-dk-tm4c123g.obj" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \
-l"P:/tm4c/workspace/../tivaware\driverlib\ccs\Debug\driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include peripherals/subdir_vars.mk
-include peripherals/tinygps/subdir_vars.mk
-include peripherals/fatfs/option/subdir_vars.mk
-include peripherals/fatfs/subdir_vars.mk
-include subdir_rules.mk
-include peripherals/subdir_rules.mk
-include peripherals/tinygps/subdir_rules.mk
-include peripherals/fatfs/option/subdir_rules.mk
-include peripherals/fatfs/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
esft_launchpad_package.out \

EXE_OUTPUTS__QUOTED += \
"esft_launchpad_package.out" \

BIN_OUTPUTS += \
esft_launchpad_package.bin \

BIN_OUTPUTS__QUOTED += \
"esft_launchpad_package.bin" \


# All Target
all: esft_launchpad_package.out

# Tool invocations
esft_launchpad_package.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --opt_for_speed=5 -g --gcc --define=ccs="ccs" --define=DEBUG --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number -z -m"esft_launchpad_package.map" --heap_size=0 --stack_size=4096 -i"P:/tm4c/tivaware/" -i"P:/tm4c/tivaware/utils/" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --display_error_number --diag_wrap=off --warn_sections --xml_link_info="esft_launchpad_package_linkInfo.xml" --rom_model -o "esft_launchpad_package.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

esft_launchpad_package.bin: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "esft_launchpad_package.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.pp" "tm4c123gh6pm_startup_ccs.pp" "peripherals\accel250.pp" "peripherals\altimeter.pp" "peripherals\compass.pp" "peripherals\flashstore.pp" "peripherals\gpio.pp" "peripherals\gps.pp" "peripherals\gyro.pp" "peripherals\i2c.pp" "peripherals\misc.pp" "peripherals\ssi.pp" "peripherals\status.pp" "peripherals\uart.pp" "peripherals\uartstdio.pp" "peripherals\tinygps\tinygps.pp" "peripherals\fatfs\option\syscall.pp" "peripherals\fatfs\option\unicode.pp" "peripherals\fatfs\ff.pp" "peripherals\fatfs\mmc-dk-tm4c123g.pp" 
	-$(RM) "main.obj" "tm4c123gh6pm_startup_ccs.obj" "peripherals\accel250.obj" "peripherals\altimeter.obj" "peripherals\compass.obj" "peripherals\flashstore.obj" "peripherals\gpio.obj" "peripherals\gps.obj" "peripherals\gyro.obj" "peripherals\i2c.obj" "peripherals\misc.obj" "peripherals\ssi.obj" "peripherals\status.obj" "peripherals\uart.obj" "peripherals\uartstdio.obj" "peripherals\tinygps\tinygps.obj" "peripherals\fatfs\option\syscall.obj" "peripherals\fatfs\option\unicode.obj" "peripherals\fatfs\ff.obj" "peripherals\fatfs\mmc-dk-tm4c123g.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv6/utils/tiobj2bin/tiobj2bin" "esft_launchpad_package.out" "esft_launchpad_package.bin" "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armofd" "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex" "C:/ti/ccsv6/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

