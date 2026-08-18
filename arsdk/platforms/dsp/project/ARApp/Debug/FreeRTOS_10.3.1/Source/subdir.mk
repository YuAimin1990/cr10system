################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS_10.3.1/Source/croutine.c \
../FreeRTOS_10.3.1/Source/event_groups.c \
../FreeRTOS_10.3.1/Source/list.c \
../FreeRTOS_10.3.1/Source/queue.c \
../FreeRTOS_10.3.1/Source/stream_buffer.c \
../FreeRTOS_10.3.1/Source/tasks.c \
../FreeRTOS_10.3.1/Source/timers.c 

OBJS += \
./FreeRTOS_10.3.1/Source/croutine.o \
./FreeRTOS_10.3.1/Source/event_groups.o \
./FreeRTOS_10.3.1/Source/list.o \
./FreeRTOS_10.3.1/Source/queue.o \
./FreeRTOS_10.3.1/Source/stream_buffer.o \
./FreeRTOS_10.3.1/Source/tasks.o \
./FreeRTOS_10.3.1/Source/timers.o 

C_DEPS += \
./FreeRTOS_10.3.1/Source/croutine.d \
./FreeRTOS_10.3.1/Source/event_groups.d \
./FreeRTOS_10.3.1/Source/list.d \
./FreeRTOS_10.3.1/Source/queue.d \
./FreeRTOS_10.3.1/Source/stream_buffer.d \
./FreeRTOS_10.3.1/Source/tasks.d \
./FreeRTOS_10.3.1/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS_10.3.1/Source/%.o: ../FreeRTOS_10.3.1/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: CEVA-ELF C Compiler'
	C:\CEVA-ToolBox\V18\XM6\toolbox\../cevatools/bin/clang -c -fmessage-length=0 -Os -g -Wall -pedantic -Wa,-rstr-check-args="-quick -verbose" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\include" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\include\xm6" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\App\src" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\FreeRTOS_10.3.1\Source\include" -I"E:\dsp_0923\dsp\dsp_sdk\ARApp\FreeRTOS_10.3.1\Source\portable\CEVA-DSP\cevaxm6" -D_LIBCPP_INLINE_VISIBILITY="" -Dfree=ar_free -Dmalloc=ar_malloc -D_LIBCPP_EXTERN_TEMPLATE_INLINE_VISIBILITY="" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" --target=cevaxm6-elf -mcpu=cevaxm6v1.3.0 -mnonlinear=2 -mfp=4 -mvfp=2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


