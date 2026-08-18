#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/audio_service.sock"
#define MAX_RESPONSE_LEN 256

int send_command_to_service(const char* command)
{
    int sock_fd;
    struct sockaddr_un server_addr;
    char response[MAX_RESPONSE_LEN];
    ssize_t bytes_sent, bytes_received;
    
    // Create socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        printf("Error: Cannot connect to audio service. Is the service running?\n");
        close(sock_fd);
        return -1;
    }
    
    // Send command
    bytes_sent = write(sock_fd, command, strlen(command));
    if (bytes_sent == -1) {
        perror("write");
        close(sock_fd);
        return -1;
    }
    
    // Read response
    bytes_received = read(sock_fd, response, sizeof(response) - 1);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Service response: %s", response);
    } else if (bytes_received == -1) {
        perror("read");
    }
    
    close(sock_fd);
    return 0;
}

void print_usage(const char* program_name)
{
    printf("Usage:\n");
    printf("  %s play <filename> <volume>  - Play audio file at specified volume (0-31)\n", program_name);
    printf("  %s stop                      - Stop current playback\n", program_name);
    printf("\nExamples:\n");
    printf("  %s play /path/to/audio.wav 10\n", program_name);
    printf("  %s stop\n", program_name);
}

int main(int argc, char* argv[])
{
    char command[512];
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "play") == 0) {
        if (argc != 4) {
            printf("Error: play command requires filename and volume\n");
            print_usage(argv[0]);
            return 1;
        }
        
        int volume = atoi(argv[3]);
        if (volume < 0 || volume > 31) {
            printf("Error: volume must be between 0 and 31\n");
            return 1;
        }
        
        snprintf(command, sizeof(command), "play %s %d\n", argv[2], volume);
        
    } else if (strcmp(argv[1], "stop") == 0) {
        if (argc != 2) {
            printf("Error: stop command takes no additional arguments\n");
            print_usage(argv[0]);
            return 1;
        }
        
        snprintf(command, sizeof(command), "stop\n");
        
    } else {
        printf("Error: unknown command '%s'\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    
    return send_command_to_service(command);
}
