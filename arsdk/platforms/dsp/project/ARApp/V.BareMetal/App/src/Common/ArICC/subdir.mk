################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/src/Common/ArICC/IccCore.c \
../App/src/Common/ArICC/IccFileOperation.c \
../App/src/Common/ArICC/IccPeer.c \
../App/src/Common/ArICC/IccPort.c \
../App/src/Common/ArICC/IccShm.c \
../App/src/Common/ArICC/IccSpinlock.c 

OBJS += \
./App/src/Common/ArICC/IccCore.o \
./App/src/Common/ArICC/IccFileOperation.o \
./App/src/Common/ArICC/IccPeer.o \
./App/src/Common/ArICC/IccPort.o \
./App/src/Common/ArICC/IccShm.o \
./App/src/Common/ArICC/IccSpinlock.o 

C_DEPS += \
./App/src/Common/ArICC/IccCore.d \
./App/src/Common/ArICC/IccFileOperation.d \
./App/src/Common/ArICC/IccPeer.d \
./App/src/Common/ArICC/IccPort.d \
./App/src/Common/ArICC/IccShm.d \
./App/src/Common/ArICC/IccSpinlock.d 


# Each subdirectory must supply rules for building sources it contributes
App/src/Common/ArICC/%.o: ../App/src/Common/ArICC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"X:\0000\dsp\project\ARApp\App\include" -I"X:\0000\dsp\project\ARApp\App\include\ceva_cv" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\include" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArICC" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArLog" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArMalloc" -I"X:\0000\dsp\project\ARApp\App\src\Common\cJSON" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include\xm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\SysCom" -I"X:\0000\dsp\project\ARApp\App\src\Framework" -I"X:\0000\dsp\project\ARApp\App\src\Kernels" -D_LIBCPP_INLINE_VISIBILITY="" -D_FREERTOS_VERSION_ON_=0 -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


