################################################################################
# Automatically-generated file. Do not edit!
################################################################################


ifeq ($(OS), Windows_NT)
    SHELL:=cmd
endif


VPATH += E:\dsp_0923\dsp\dsp_sdk\ARApp\App\lib;C:\CEVA-ToolBox\V18\XM6\cevatools\lib\clang\7.1.0\cevaxm6-unknown-unknown-elf\rtlv1.2.0-fp4\lib;C:\CEVA-ToolBox\V18\XM6\cevatools\lib\clang\7.1.0\cevaxm6-unknown-unknown-elf\rtlv1.2.0-fp4\lib

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include FreeRTOS_10.3.1/Source/portable/MemMang/subdir.mk
-include FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/subdir.mk
-include FreeRTOS_10.3.1/Source/subdir.mk
-include App/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
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

# All Target
all: ARApp.elf

# Tool invocations
ARApp.elf: $(OBJS) ../ARApp.ld $(USER_OBJS) $(LIBS)
	@echo 'Building target: $@'
	@echo 'Invoking: CEVA-ELF Linker'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/ceva-elf-ld -Map=ARApp.map -T ..\ARApp.ld --no-relax --no-gc-sections -L"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\lib" -L"C:\CEVA-ToolBox\V18\XM6\cevatools\lib\clang\7.1.0\cevaxm6-unknown-unknown-elf\rtlv1.2.0-fp4\lib" -defsym __internal_data_size=512k -defsym __internal_code_size=64k -L"C:/CEVA-ToolBox/V18/XM6/cevatools/lib/clang/7.1.0/cevaxm6-unknown-unknown-elf/rtlv1.2.0-fp4/lib" $(OBJS) $(USER_OBJS) --start-group $(LIBS) --end-group -o  "ARApp.elf"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) ARApp.elf
	-@echo ' '

post-build:
	-@echo 'ceva.bin'
	-../../ARToolsWindows/xm6-elf-bintool/Debug/xm6elftool.exe ARApp.elf
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
