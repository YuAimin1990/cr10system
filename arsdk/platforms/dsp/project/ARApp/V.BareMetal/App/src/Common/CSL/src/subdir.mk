################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/src/Common/CSL/src/dma_common.c \
../App/src/Common/CSL/src/dma_driver_extended_desc.c \
../App/src/Common/CSL/src/dma_sync_fifo.c \
../App/src/Common/CSL/src/mss.c 

OBJS += \
./App/src/Common/CSL/src/dma_common.o \
./App/src/Common/CSL/src/dma_driver_extended_desc.o \
./App/src/Common/CSL/src/dma_sync_fifo.o \
./App/src/Common/CSL/src/mss.o 

C_DEPS += \
./App/src/Common/CSL/src/dma_common.d \
./App/src/Common/CSL/src/dma_driver_extended_desc.d \
./App/src/Common/CSL/src/dma_sync_fifo.d \
./App/src/Common/CSL/src/mss.d 


# Each subdirectory must supply rules for building sources it contributes
App/src/Common/CSL/src/%.o: ../App/src/Common/CSL/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"X:\0000\dsp\project\ARApp\App\include" -I"X:\0000\dsp\project\ARApp\App\include\ceva_cv" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\include" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArICC" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArLog" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArMalloc" -I"X:\0000\dsp\project\ARApp\App\src\Common\cJSON" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include\xm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\SysCom" -I"X:\0000\dsp\project\ARApp\App\src\Framework" -I"X:\0000\dsp\project\ARApp\App\src\Kernels" -D_LIBCPP_INLINE_VISIBILITY="" -D_FREERTOS_VERSION_ON_=0 -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


