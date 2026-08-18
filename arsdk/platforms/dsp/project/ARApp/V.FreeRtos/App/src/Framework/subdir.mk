################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/src/Framework/ArDmaDtcmFunc.c \
../App/src/Framework/ArDspArtSimulator.c \
../App/src/Framework/ArDspPlatform.c \
../App/src/Framework/ArDspStatusInfo.c \
../App/src/Framework/ArDspUnitSimulator.c \
../App/src/Framework/ArNpuPlatform.c \
../App/src/Framework/ArTasksSchedule.c 

OBJS += \
./App/src/Framework/ArDmaDtcmFunc.o \
./App/src/Framework/ArDspArtSimulator.o \
./App/src/Framework/ArDspPlatform.o \
./App/src/Framework/ArDspStatusInfo.o \
./App/src/Framework/ArDspUnitSimulator.o \
./App/src/Framework/ArNpuPlatform.o \
./App/src/Framework/ArTasksSchedule.o 

C_DEPS += \
./App/src/Framework/ArDmaDtcmFunc.d \
./App/src/Framework/ArDspArtSimulator.d \
./App/src/Framework/ArDspPlatform.d \
./App/src/Framework/ArDspStatusInfo.d \
./App/src/Framework/ArDspUnitSimulator.d \
./App/src/Framework/ArNpuPlatform.d \
./App/src/Framework/ArTasksSchedule.d 


# Each subdirectory must supply rules for building sources it contributes
App/src/Framework/%.o: ../App/src/Framework/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"X:\0000\dsp\project\ARApp\App\include" -I"X:\0000\dsp\project\ARApp\App\include\ceva_cv" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\include" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArICC" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArLog" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArMalloc" -I"X:\0000\dsp\project\ARApp\App\src\Common\cJSON" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include\xm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\SysCom" -I"X:\0000\dsp\project\ARApp\App\src\Framework" -I"X:\0000\dsp\project\ARApp\App\src\Kernels" -D_LIBCPP_INLINE_VISIBILITY="" -D_FREERTOS_VERSION_ON_=1 -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


