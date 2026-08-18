#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVICE_H

// Opaque pointer
typedef struct audio_service_s* audio_service_handle;

audio_service_handle audio_service_create(void);
void audio_service_destroy(audio_service_handle handle);

/**
 * @brief Runs the audio service. This is a blocking call.
 *        It starts listening for IPC commands and will run until
 *        audio_service_stop() is called from another thread (e.g., signal handler).
 * @param handle Handle to the service.
 * @return 0 on normal exit, -1 on failure.
 */
int audio_service_run(audio_service_handle handle);

/**
 * @brief Requests the audio service to stop.
 * @param handle Handle to the service.
 */
void audio_service_stop(audio_service_handle handle);

#endif // AUDIO_SERVICE_H