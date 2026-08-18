#include "audio_service.h"
#include "audio_player.h"
#include "ipc_protocol.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

struct audio_service_s {
    audio_player_handle player;
    volatile bool stop_requested;
    int server_fd;
};

static int parse_ipc_command(const char *buffer, command_t *cmd) {
    // A more robust parser than sscanf
    char cmd_str[32];
    char arg1[MAX_FILENAME_LEN];
    int arg2;

    if (sscanf(buffer, "%31s %511s %d", cmd_str, arg1, &arg2) == 3 && strcmp(cmd_str, "play") == 0) {
        cmd->type = CMD_PLAY;
        strncpy(cmd->filename, arg1, MAX_FILENAME_LEN - 1);
        cmd->volume = arg2;
    } else if (sscanf(buffer, "%31s %d", cmd_str, &arg2) == 2 && strcmp(cmd_str, "volume") == 0) {
        cmd->type = CMD_SET_VOLUME;
        cmd->volume = arg2;
    } else if (sscanf(buffer, "%31s", cmd_str) == 1 && strcmp(cmd_str, "stop") == 0) {
        cmd->type = CMD_STOP;
    } else {
        cmd->type = CMD_UNKNOWN;
        return -1;
    }
    return 0;
}

static void handle_client_connection(int client_fd, audio_player_handle player) {
    char buffer[MAX_COMMAND_LEN];
    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    
    command_t cmd;
    parse_ipc_command(buffer, &cmd);
    
    const char *response = "OK\n";
    switch (cmd.type) {
        case CMD_PLAY:
            LOG_INFO("IPC: Received PLAY command for '%s' at volume %d", cmd.filename, cmd.volume);
            if (audio_player_play(player, cmd.filename, cmd.volume) != 0) {
                response = "ERROR: Failed to start playback\n";
            }
            break;
        case CMD_STOP:
            LOG_INFO("IPC: Received STOP command");
            audio_player_stop(player);
            break;
        case CMD_SET_VOLUME:
            LOG_INFO("IPC: Received SET_VOLUME command to %d", cmd.volume);
            if (audio_player_set_volume(player, cmd.volume) != 0) {
                 response = "ERROR: Failed to set volume\n";
            }
            break;
        default:
            LOG_WARN("IPC: Received unknown command");
            response = "ERROR: Unknown command\n";
            break;
    }
    write(client_fd, response, strlen(response));
    close(client_fd);
}

audio_service_handle audio_service_create(void) {
    audio_service_handle handle = calloc(1, sizeof(struct audio_service_s));
    if (!handle) {
        LOG_ERROR("Failed to allocate service handle.");
        return NULL;
    }
    handle->player = audio_player_create();
    if (!handle->player) {
        free(handle);
        return NULL;
    }
    handle->server_fd = -1;
    return handle;
}

void audio_service_destroy(audio_service_handle handle) {
    if (!handle) return;
    if (handle->server_fd != -1) {
        close(handle->server_fd);
        unlink(SOCKET_PATH);
    }
    // audio_player_destroy 会调用 stop, 进而反初始化硬件
    audio_player_destroy(handle->player);
    free(handle);
}

void audio_service_stop(audio_service_handle handle) {
    if (handle) {
        handle->stop_requested = true;
    }
}

int audio_service_run(audio_service_handle handle) {
    struct sockaddr_un addr;
    
    unlink(SOCKET_PATH);
    handle->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (handle->server_fd < 0) {
        LOG_ERROR("socket() failed: %s", strerror(errno));
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    if (bind(handle->server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR("bind() failed: %s", strerror(errno));
        return -1;
    }
    
    chmod(SOCKET_PATH, 0666);
    
    if (listen(handle->server_fd, 5) < 0) {
        LOG_ERROR("listen() failed: %s", strerror(errno));
        return -1;
    }

    LOG_INFO("Audio service listening on %s", SOCKET_PATH);

    while (!handle->stop_requested) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(handle->server_fd, &read_fds);
        
        struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
        
        int ret = select(handle->server_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno == EINTR) continue; // Interrupted by signal
            LOG_ERROR("select() failed: %s", strerror(errno));
            break;
        }
        
        if (ret > 0 && FD_ISSET(handle->server_fd, &read_fds)) {
            int client_fd = accept(handle->server_fd, NULL, NULL);
            if (client_fd >= 0) {
                handle_client_connection(client_fd, handle->player);
            }
        }
    }
    
    LOG_INFO("Audio service shutting down.");
    return 0;
}