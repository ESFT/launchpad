################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/Programs/ti/ccsv6/tools/compiler/arm_5.1.9

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./main.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./peripherals/accel250.obj" \
"./peripherals/altimeter.obj" \
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
"../tm4c123gh6pm.cmd" \
-l"libc.a" \
-l"D:/Code/git_repo/launchpad/workspace/esft_data_to_sd/../../TivaWare_C_Series-2.1.0.12573\driverlib\ccs\Release\driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include peripherals/subdir_vars.mk
-include subdir_rules.mk
-include peripherals/subdir_rules.mk
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
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
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
esft_data_to_sd.out \

EXE_OUTPUTS__QUOTED += \
"esft_data_to_sd.out" \

BIN_OUTPUTS += \
esft_data_to_sd.hex \

BIN_OUTPUTS__QUOTED += \
"esft_data_to_sd.hex" \


# All Target
all: esft_data_to_sd.out

dependents:
	-cd D:/Code/git_repo/launchpad/tivaware_c_series-2.1.0.12573/driverlib/ccs/Release && $(MAKE) clean all

# Tool invocations
esft_data_to_sd.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) D:/Code/git_repo/launchpad/tivaware_c_series-2.1.0.12573/driverlib/ccs/Release/driverlib.lib
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"D:/Programs/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C123GH6PM --display_error_number --diag_warning=225 --diag_wrap=off -z -m"esft_data_to_sd.map" --heap_size=0 --stack_size=512 -i"D:/Code/git_repo/launchpad/TivaWare_C_Series-2.1.0.12573/" -i"D:/Programs/ti/ccsv6/tools/compiler/arm_5.1.9/lib" -i"D:/Programs/ti/ccsv6/tools/compiler/arm_5.1.9/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="esft_data_to_sd_linkInfo.xml" --rom_model -o "esft_data_to_sd.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

esft_data_to_sd.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"D:/Programs/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armhex"  -o "esft_data_to_sd.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.pp" "tm4c123gh6pm_startup_ccs.pp" "peripherals\accel250.pp" "peripherals\altimeter.pp" "peripherals\flashstore.pp" "peripherals\gpio.pp" "peripherals\gps.pp" "peripherals\gyro.pp" "peripherals\i2c.pp" "peripherals\misc.pp" "peripherals\ssi.pp" "peripherals\status.pp" "peripherals\uart.pp" "peripherals\uartstdio.pp" 
	-$(RM) "main.obj" "tm4c123gh6pm_startup_ccs.obj" "peripherals\accel250.obj" "peripherals\altimeter.obj" "peripherals\flashstore.obj" "peripherals\gpio.obj" "peripherals\gps.obj" "peripherals\gyro.obj" "peripherals\i2c.obj" "peripherals\misc.obj" "peripherals\ssi.obj" "peripherals\status.obj" "peripherals\uart.obj" "peripherals\uartstdio.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
D:/Code/git_repo/launchpad/tivaware_c_series-2.1.0.12573/driverlib/ccs/Release/driverlib.lib:

-include ../makefile.targets

