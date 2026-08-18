################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/src/ArArmCaculateResize.c \
../App/src/ArArmCalcGauss3x3.c \
../App/src/ArArmRegistNPUCallback.c \
../App/src/ArArmUnRegistNPUCallback.c \
../App/src/ArCnnOperatorCommon.c \
../App/src/ArCnnOperatorCustomLrn.c \
../App/src/ArCnnOperatorDeformable.c \
../App/src/ArCnnOperatorGetConf.c \
../App/src/ArCnnOperatorLrn.c \
../App/src/ArCnnOperatorMaxUnPool.c \
../App/src/ArCnnOperatorReciprocal.c \
../App/src/ArCnnOperatorReshape.c \
../App/src/ArCnnOperatorSevCallback.c \
../App/src/ArCnnOperatorUpSample.c \
../App/src/ArCnnParseCustomLrn.c \
../App/src/ArCnnParseDeformable.c \
../App/src/ArCnnParseLrn.c \
../App/src/ArCnnParseMaxUnPool.c \
../App/src/ArCnnParseMaxUpSample.c \
../App/src/ArCnnParseSevCallback.c \
../App/src/ArDmaCallFunc.c \
../App/src/ArDmaTest.c \
../App/src/ArDspArtSimulator.c \
../App/src/ArDspPlatform.c \
../App/src/ArDspRuntimeCalc.c \
../App/src/ArDspStatusInfo.c \
../App/src/ArDspUnitSimulator.c \
../App/src/ArNpuPlatform.c \
../App/src/ArTasksSchedule.c \
../App/src/main.c 

OBJS += \
./App/src/ArArmCaculateResize.o \
./App/src/ArArmCalcGauss3x3.o \
./App/src/ArArmRegistNPUCallback.o \
./App/src/ArArmUnRegistNPUCallback.o \
./App/src/ArCnnOperatorCommon.o \
./App/src/ArCnnOperatorCustomLrn.o \
./App/src/ArCnnOperatorDeformable.o \
./App/src/ArCnnOperatorGetConf.o \
./App/src/ArCnnOperatorLrn.o \
./App/src/ArCnnOperatorMaxUnPool.o \
./App/src/ArCnnOperatorReciprocal.o \
./App/src/ArCnnOperatorReshape.o \
./App/src/ArCnnOperatorSevCallback.o \
./App/src/ArCnnOperatorUpSample.o \
./App/src/ArCnnParseCustomLrn.o \
./App/src/ArCnnParseDeformable.o \
./App/src/ArCnnParseLrn.o \
./App/src/ArCnnParseMaxUnPool.o \
./App/src/ArCnnParseMaxUpSample.o \
./App/src/ArCnnParseSevCallback.o \
./App/src/ArDmaCallFunc.o \
./App/src/ArDmaTest.o \
./App/src/ArDspArtSimulator.o \
./App/src/ArDspPlatform.o \
./App/src/ArDspRuntimeCalc.o \
./App/src/ArDspStatusInfo.o \
./App/src/ArDspUnitSimulator.o \
./App/src/ArNpuPlatform.o \
./App/src/ArTasksSchedule.o \
./App/src/main.o 

C_DEPS += \
./App/src/ArArmCaculateResize.d \
./App/src/ArArmCalcGauss3x3.d \
./App/src/ArArmRegistNPUCallback.d \
./App/src/ArArmUnRegistNPUCallback.d \
./App/src/ArCnnOperatorCommon.d \
./App/src/ArCnnOperatorCustomLrn.d \
./App/src/ArCnnOperatorDeformable.d \
./App/src/ArCnnOperatorGetConf.d \
./App/src/ArCnnOperatorLrn.d \
./App/src/ArCnnOperatorMaxUnPool.d \
./App/src/ArCnnOperatorReciprocal.d \
./App/src/ArCnnOperatorReshape.d \
./App/src/ArCnnOperatorSevCallback.d \
./App/src/ArCnnOperatorUpSample.d \
./App/src/ArCnnParseCustomLrn.d \
./App/src/ArCnnParseDeformable.d \
./App/src/ArCnnParseLrn.d \
./App/src/ArCnnParseMaxUnPool.d \
./App/src/ArCnnParseMaxUpSample.d \
./App/src/ArCnnParseSevCallback.d \
./App/src/ArDmaCallFunc.d \
./App/src/ArDmaTest.d \
./App/src/ArDspArtSimulator.d \
./App/src/ArDspPlatform.d \
./App/src/ArDspRuntimeCalc.d \
./App/src/ArDspStatusInfo.d \
./App/src/ArDspUnitSimulator.d \
./App/src/ArNpuPlatform.d \
./App/src/ArTasksSchedule.d \
./App/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
App/src/%.o: ../App/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\include" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\include\xm6" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\src" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\FreeRTOS_10.3.1\Source\include" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -D_LIBCPP_INLINE_VISIBILITY="" -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


