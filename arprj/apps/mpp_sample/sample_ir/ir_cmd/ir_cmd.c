#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>

#define IR_CMD_FILE_NAME        "/usrdata/ir_cmd.bin"

typedef struct {
    unsigned int cmd_cnt;
    char cmd[32][64];
} ir_cmd_t;

static int write_ir_cmd(const char* fname, ir_cmd_t *ir_cmd)
{
    FILE* fp = NULL;
    size_t w_size = 0;

    fp = fopen(fname, "wb");
    if (fp == NULL) {
        printf("ir_cmd: open %s failed\n", fname);
        return -1;
    }

    w_size = fwrite(ir_cmd, 1, sizeof(ir_cmd_t), fp);
    if (w_size != sizeof(ir_cmd_t)) {
        printf("ir_cmd: write ir cmd to %s failed\n", fname);
        return -1;
    }

    fclose(fp);
    return 0;
}

int get_ir_sample_pid(void)
{
    int pid = -1;
    FILE *fp = NULL;
    char buf[64] = {0};

    fp = popen("ps -ef | grep ir_sample | grep -v grep | awk '{print $1}'", "r");
    if (fp == NULL) {
        printf("popen failed");
        return -1;
    }

    fgets(buf, sizeof(buf), fp);
    pid = atoi(buf);
    fclose(fp);

    return pid;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    int pid = -1;
    int signum = SIGUSR1;
    ir_cmd_t *ir_cmd = NULL;
    int i = 0;

    //if (argc < 2) {
    //    printf("ir_cmd: no pid\n");
    //    return 0;
    //}

    pid = get_ir_sample_pid();
    if (pid <= 0) {
        printf("get ir sample pid failed");
        return 0;
    }

    ir_cmd = malloc(sizeof(ir_cmd_t));
    if (ir_cmd == NULL) {
        printf("ir_cmd: allocate ir_cmd failed\n");
        return 0;
    }

#if 1
    char buf[1024] = {0};
    char *pstr1 = NULL;
    char *pstr2 = NULL;

    system("stty erase ^H");

    while (1) {
        printf("\n==> ");

        while (fgets(buf, 1024, stdin) == NULL) {
            printf("fgets failed");
        }

        i = 0;
        pstr1 = strtok_r(buf, " \t\n\r", &pstr2);
        while (pstr1 != NULL) {
            strcpy(ir_cmd->cmd[i], pstr1);
            //printf("ir_cmd[%d]: %s %s\n", i, pstr1, ir_cmd->cmd[i]);
            pstr1 = strtok_r(NULL, " \t\n\r", &pstr2);
            i++;
        }

        if (i <= 0) {
            continue;
        }

        if (strcmp(ir_cmd->cmd[0], "quit") == 0) {
            break;
        }

        ir_cmd->cmd_cnt = i;
        ret = write_ir_cmd(IR_CMD_FILE_NAME, ir_cmd);
        if (ret) {
            printf("ir_cmd: write ir_cmd failed\n");
            continue;
        }

        kill(pid, signum);
    }
#else
    ir_cmd->cmd_cnt = 0;
    for (i = 1; i < argc; i++) {
        strcpy(ir_cmd->cmd[i-1], argv[i]);
        ir_cmd->cmd_cnt++;
    }

    ret = write_ir_cmd(IR_CMD_FILE_NAME, ir_cmd);
    if (ret) {
        printf("ir_cmd: write ir_cmd failed\n");
        return 0;
    }

    kill(pid, signum);
#endif

    free(ir_cmd);
    return 0;
}

