# Define target, only one type can be choosed: program, static library or dynamic library.
#------------------------------------------
ifeq ($(PARAM_FILE), )
     PARAM_FILE:=../../../../arsdk/Makefile.param
     include $(PARAM_FILE)
endif

PROGRAM   = test_mpp_audio

# Define files
#------------------------------------------

SRCDIRS   = ./ \

# Define flags
#------------------------------------------
LDFLAGS   += -lpthread -lrt -ldl -lm
LDFLAGS   += -lmpi_ai -lmpi_ao -lmpi_vo
#LDFLAGS   += -laac_codec -lhal_adec -lhal_aenc -lhal_vb -lhal_sys -lhal_dbglog -losal -lutils
CFLAGS    += -I ../common
LOCAL_LIB =


#include ../Makefile.param
# Include make rule file
#------------------------------------------
#include ../../usr_lib_rules.mk
include ../Makefile.param

$(PROGRAM): $(PROGRAM)_ audio_service audio_client audio_svr audio_svr2 audio_test


$(PROGRAM)_: sample_audio.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Additional targets for audio service and client
audio_service: audio_service.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

audio_client: audio_client.c
	$(CC) $(CFLAGS) -o $@ $<

SRCSW = main.c audio_player.c wave_parser.c audio_hw_if.c audio_svr.c

audio_svr2: $(SRCSW)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

audio_svr: audio_service_refactored.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

audio_test: sample_audio_refactored.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f audio_service audio_client sample_audio.o audio_service.o audio_client.o audio_service.d audio_client.d sample_audio.d $(PROGRAM)_ audio_svr audio_svr2 audio_test

.PHONY: $(PROGRAM) clean

