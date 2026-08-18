################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/src/Kernels/ArArmCalcGauss3x3.c \
../App/src/Kernels/ArArmCanny.c \
../App/src/Kernels/ArArmDmaDemo.c \
../App/src/Kernels/ArArmRegistNPUCallback.c \
../App/src/Kernels/ArArmUnRegistNPUCallback.c \
../App/src/Kernels/ArArmWarpaffine.c \
../App/src/Kernels/ArCnnOperatorCommon.c \
../App/src/Kernels/ArCnnOperatorCustomLrn.c \
../App/src/Kernels/ArCnnOperatorDeformable.c \
../App/src/Kernels/ArCnnOperatorGetConf.c \
../App/src/Kernels/ArCnnOperatorLrn.c \
../App/src/Kernels/ArCnnOperatorMaxUnPool.c \
../App/src/Kernels/ArCnnOperatorReciprocal.c \
../App/src/Kernels/ArCnnOperatorReshape.c \
../App/src/Kernels/ArCnnOperatorSevCallback.c \
../App/src/Kernels/ArCnnOperatorUpSample.c \
../App/src/Kernels/ArCnnParseCustomLrn.c \
../App/src/Kernels/ArCnnParseDeformable.c \
../App/src/Kernels/ArCnnParseLrn.c \
../App/src/Kernels/ArCnnParseMaxUnPool.c \
../App/src/Kernels/ArCnnParseMaxUpSample.c \
../App/src/Kernels/ArCnnParseSevCallback.c \
../App/src/Kernels/ArDspRuntimeCalc.c 

OBJS += \
./App/src/Kernels/ArArmCalcGauss3x3.o \
./App/src/Kernels/ArArmCanny.o \
./App/src/Kernels/ArArmDmaDemo.o \
./App/src/Kernels/ArArmRegistNPUCallback.o \
./App/src/Kernels/ArArmUnRegistNPUCallback.o \
./App/src/Kernels/ArArmWarpaffine.o \
./App/src/Kernels/ArCnnOperatorCommon.o \
./App/src/Kernels/ArCnnOperatorCustomLrn.o \
./App/src/Kernels/ArCnnOperatorDeformable.o \
./App/src/Kernels/ArCnnOperatorGetConf.o \
./App/src/Kernels/ArCnnOperatorLrn.o \
./App/src/Kernels/ArCnnOperatorMaxUnPool.o \
./App/src/Kernels/ArCnnOperatorReciprocal.o \
./App/src/Kernels/ArCnnOperatorReshape.o \
./App/src/Kernels/ArCnnOperatorSevCallback.o \
./App/src/Kernels/ArCnnOperatorUpSample.o \
./App/src/Kernels/ArCnnParseCustomLrn.o \
./App/src/Kernels/ArCnnParseDeformable.o \
./App/src/Kernels/ArCnnParseLrn.o \
./App/src/Kernels/ArCnnParseMaxUnPool.o \
./App/src/Kernels/ArCnnParseMaxUpSample.o \
./App/src/Kernels/ArCnnParseSevCallback.o \
./App/src/Kernels/ArDspRuntimeCalc.o 

C_DEPS += \
./App/src/Kernels/ArArmCalcGauss3x3.d \
./App/src/Kernels/ArArmCanny.d \
./App/src/Kernels/ArArmDmaDemo.d \
./App/src/Kernels/ArArmRegistNPUCallback.d \
./App/src/Kernels/ArArmUnRegistNPUCallback.d \
./App/src/Kernels/ArArmWarpaffine.d \
./App/src/Kernels/ArCnnOperatorCommon.d \
./App/src/Kernels/ArCnnOperatorCustomLrn.d \
./App/src/Kernels/ArCnnOperatorDeformable.d \
./App/src/Kernels/ArCnnOperatorGetConf.d \
./App/src/Kernels/ArCnnOperatorLrn.d \
./App/src/Kernels/ArCnnOperatorMaxUnPool.d \
./App/src/Kernels/ArCnnOperatorReciprocal.d \
./App/src/Kernels/ArCnnOperatorReshape.d \
./App/src/Kernels/ArCnnOperatorSevCallback.d \
./App/src/Kernels/ArCnnOperatorUpSample.d \
./App/src/Kernels/ArCnnParseCustomLrn.d \
./App/src/Kernels/ArCnnParseDeformable.d \
./App/src/Kernels/ArCnnParseLrn.d \
./App/src/Kernels/ArCnnParseMaxUnPool.d \
./App/src/Kernels/ArCnnParseMaxUpSample.d \
./App/src/Kernels/ArCnnParseSevCallback.d \
./App/src/Kernels/ArDspRuntimeCalc.d 


# Each subdirectory must supply rules for building sources it contributes
App/src/Kernels/%.o: ../App/src/Kernels/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"X:\0000\dsp\project\ARApp\App\include" -I"X:\0000\dsp\project\ARApp\App\include\ceva_cv" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\include" -I"X:\0000\dsp\project\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArICC" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArLog" -I"X:\0000\dsp\project\ARApp\App\src\Common\ArMalloc" -I"X:\0000\dsp\project\ARApp\App\src\Common\cJSON" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include" -I"X:\0000\dsp\project\ARApp\App\src\Common\CSL\include\xm6" -I"X:\0000\dsp\project\ARApp\App\src\Common\SysCom" -I"X:\0000\dsp\project\ARApp\App\src\Framework" -I"X:\0000\dsp\project\ARApp\App\src\Kernels" -D_LIBCPP_INLINE_VISIBILITY="" -D_FREERTOS_VERSION_ON_=0 -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


