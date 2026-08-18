#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

// Opaque pointer to the internal state
typedef struct audio_player_s* audio_player_handle;

/**
 * @brief Creates an audio player instance.
 * @return Handle to the player, or NULL on failure.
 */
audio_player_handle audio_player_create(void);

/**
 * @brief Destroys the audio player instance and cleans up resources.
 * @param handle Handle to the player.
 */
void audio_player_destroy(audio_player_handle handle);

/**
 * @brief Starts playing an audio file. If a file is already playing, it will be stopped first.
 * @param handle Handle to the player.
 * @param filename Path to the audio file.
 * @param volume Initial volume (device-specific range).
 * @return 0 on success, -1 on failure.
 */
int audio_player_play(audio_player_handle handle, const char *filename, int volume);

/**
 * @brief Stops the current playback.
 * @param handle Handle to the player.
 * @return 0 on success.
 */
int audio_player_stop(audio_player_handle handle);

/**
 * @brief Checks if the player is currently playing.
 * @param handle Handle to the player.
 * @return 1 if playing, 0 otherwise.
 */
int audio_player_is_playing(audio_player_handle handle);

/**
 * @brief Sets the playback volume.
 * @param handle Handle to the player.
 * @param volume The new volume.
 * @return 0 on success, -1 on failure.
 */
int audio_player_set_volume(audio_player_handle handle, int volume);

#endif // AUDIO_PLAYER_H