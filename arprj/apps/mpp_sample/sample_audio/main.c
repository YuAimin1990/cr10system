#include "audio_service.h"
#include "audio_hw_if.h"
#include "utils.h"

#include <signal.h>
#include <stdlib.h>

// Global handle for the signal handler to access
static audio_service_handle g_service_handle = NULL;

static void signal_handler(int signum) {
    LOG_INFO("Caught signal %d, requesting service shutdown.", signum);
    if (g_service_handle) {
        audio_service_stop(g_service_handle);
    }
    // To handle forceful exit if shutdown takes too long
    static int quit_count = 0;
    quit_count++;
    if (quit_count > 2) {
        LOG_WARN("Forcing exit now!");
        exit(1);
    }
}

static int setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Don't restart syscalls
    
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        LOG_ERROR("Failed to set up signal handlers.");
        return -1;
    }
    return 0;
}

int main(void) {
    LOG_INFO("Audio Service starting...");

    if (setup_signal_handlers() != 0) {
        return EXIT_FAILURE;
    }

    if (hw_if_system_init() != 0) {
        LOG_ERROR("Failed to initialize hardware system.");
        return EXIT_FAILURE;
    }
    
    g_service_handle = audio_service_create();
    if (!g_service_handle) {
        LOG_ERROR("Failed to create the audio service.");
        hw_if_system_deinit();
        return EXIT_FAILURE;
    }
    
    // This is a blocking call
    audio_service_run(g_service_handle);
    
    LOG_INFO("Cleaning up resources...");
    audio_service_destroy(g_service_handle);
    hw_if_system_deinit();
    
    LOG_INFO("Audio Service stopped cleanly.");
    return EXIT_SUCCESS;
}