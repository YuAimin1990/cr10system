#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#define SOCKET_PATH "/tmp/audio_service.sock"
#define MAX_COMMAND_LEN 1024
#define MAX_FILENAME_LEN 512

typedef enum {
    CMD_PLAY,
    CMD_STOP,
    CMD_SET_VOLUME,
    CMD_UNKNOWN
} command_type_t;

typedef struct {
    command_type_t type;
    char filename[MAX_FILENAME_LEN];
    int volume;
} command_t;

#endif // IPC_PROTOCOL_H