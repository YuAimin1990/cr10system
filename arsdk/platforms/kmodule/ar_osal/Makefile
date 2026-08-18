#kernel module definitions
TARGET_MODULE:=ar_osal
src = $(kernel_module_dir)/$(TARGET_MODULE)
EXTRA_CFLAGS += $(AR_DFLAGS)
EXTRA_CFLAGS += -I$(src)
EXTRA_CFLAGS += -I$(src)/himedia
EXTRA_CFLAGS += -I$(src)/include
$(TARGET_MODULE)-objs := osal_fileops.o osal_vmalloc.o osal_addr.o osal.o osal_atomic.o osal_barrier.o osal_cache.o osal_debug.o osal_device.o osal_interrupt.o osal_math.o osal_mutex.o osal_proc.o osal_schedule.o osal_semaphore.o osal_spinlock.o osal_string.o osal_task.o osal_timer.o osal_wait.o osal_workqueue.o osal_notifier.o osal_platform.o ./himedia/base.o ./himedia/himedia.o ./mmz/media-mem.o ./mmz/mmz-userdev.o ./mmz/hisi_allocator.o
obj-m := $(TARGET_MODULE).o

