################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/xm6_port.asm 

C_SRCS += \
../FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/port.c 

OBJS += \
./FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/port.o \
./FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/xm6_port.o 

ASM_DEPS += \
./FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/xm6_port.d 

C_DEPS += \
./FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/port.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/%.o: ../FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"X:\0000\dsp\project\ARApp\App\include" -I"X:\0000\dsp\project\ARApp\App\include\ceva_cv" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\include" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArICC" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArLog" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArMalloc" -I"X:\0000\dsp\project\ARApp\App\src\Common\cJSON" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include\xm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\SysCom" -I"X:\0000\dsp\project\ARApp\App\src\Framework" -I"X:\0000\dsp\project\ARApp\App\src\Kernels" -D_LIBCPP_INLINE_VISIBILITY="" -D_FREERTOS_VERSION_ON_=1 -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/%.o: ../FreeRTOS_10.3.1/Source/portable/CEVA-DSP/cevaxm6/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF Assembler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -x assembler-with-cpp -Wa,-g -Wa,-rstr-check-args="-quick -verbose" -D_DMA_API_CHECK_=0 -Wa,-defsym -Wa,_DMA_API_CHECK_=0 -MMD -MP -MF"$(@:%.o=%.d)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


