/*
 * Artosyn program to generate spi/emmc program image
 *
 *
 * Upgrade image layout:
 *
 * |--Header--|--Hash--|--Signature--|--Version--|--Romcode--|--bootloader--|--Uboot env--|--Partitions[]--|--Segment info[]--|--Segment data[]--|
 */
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ar_flash.h"

#define UPGRADE_STATUS_FILE "/factory/upgrade_status"

static struct option long_options[] = {
	{"enterclean", no_argument, NULL, 'c'},
	{"reboot", no_argument, NULL, 'r'},
	{"help", no_argument, NULL, 'h'},
	{"verbose", no_argument, NULL, 'v'},
	{"sdkversion", no_argument, NULL, 's'},
	{"force", no_argument, NULL, 'f'},
	{"mmap", no_argument, NULL, 'm'},
	{"product", no_argument, NULL, 'p'},
	{NULL, 0, NULL, 0}
};

static void usage(void)
{
	fprintf(stderr,
		"Usage: artosyn_upgrade [OPTION] [image-file]\n"
		"Upgrade the image\n"
		"\n"
		" -c, --enterclean     reboot the system and enter clean system, no image-file\n"
		" -r, --reboot         reboot system\n"
		" -h, --help           print this help\n"
		" -v, --verbose        display more detail information, such as upgrade progress,\n"
		"                      and write upgrade status to /factory/upgrade_status_file if factory is a mountpoint\n"
		" -s, --sdkversion     display sdk version\n"
		" -f, --force          force upgrade if DDR change\n"
		" -m, --mmap           read file by mmap\n"
		" -p, --product        set upgrade mode: product phase, version rollback not allowed\n"
		" -M, --Miniloader     upgrade miniloader\n"
		" -u, --update_gpt     update emmc gpt, resize the last partition\n"
		"\n"
		"example:\n"
		" artosyn_upgrade image-file       normal upgrade\n"
		" artosyn_upgrade -v image-file    normal upgrade and display the upgrade progress\n"
		" artosyn_upgrade -c               enter clean system\n"
		" artosyn_upgrade -r image-file    upgrade and and reboot system, you can use\n"
		"                                  it if you are already in clean system\n"
		"\n");
}

static int upgrade_callback(int state, int percent)
{
	struct stat st;
	int ret, is_not_mnt = 1;
	char log[64] = {0};

	switch (state) {
	case STAT_VERIFY_IMAGE:
	case STAT_START:
	case STAT_IN_PROGRESS:
	case STAT_DONE:
		printf("%s %%%d\n", state == STAT_VERIFY_IMAGE ? "image verify" : "image upgrade", percent);
		break;
	case STAT_ENTER_CLEAN_SYSTEM:
		printf("enter clean system...\n");
		break;
	case STAT_ENTER_CLEAN_SYSTEM_FAILED:
		printf("ERROR: enter clean system failed\n");
		break;
	default:
		break;
	}

	/* factory is a mountpoint? */
	ret = stat("/factory", &st);
	if (!ret && S_ISDIR(st.st_mode)) {
		dev_t st_dev = st.st_dev;
		ino_t st_ino = st.st_ino;
		if (stat("/", &st) == 0)
			is_not_mnt = (st_dev == st.st_dev) && (st_ino != st.st_ino);
	}

	if (!is_not_mnt) {
		int fd;

		fd = open(UPGRADE_STATUS_FILE, O_CREAT | O_RDWR | O_SYNC);
		if (fd == -1)
			goto out;

		switch (state) {
		case STAT_VERIFY_IMAGE:
			sprintf(log, "status: verify image, percent: %%%d\n", percent);
			break;
		case STAT_START:
			sprintf(log, "status: start,          percent: %%%d\n", percent);
			break;
		case STAT_IN_PROGRESS:
			sprintf(log, "status: in progress,    percent: %%%d\n", percent);
			break;
		case STAT_DONE:
			sprintf(log, "status: done,           percent: %%%d\n", percent);
			break;
		case STAT_ENTER_CLEAN_SYSTEM:
			sprintf(log, "status: enter clean system           \n");
			break;
		case STAT_ENTER_CLEAN_SYSTEM_FAILED:
			sprintf(log, "status: enter clean system failed    \n");
			break;
		default:
			sprintf(log, "status: unknown                      \n");
			break;
		}
		ret = write(fd, log, strlen(log));
		if (ret == -1) {
			close(fd);
			goto out;
		}

		close(fd);
	}

out:
	return 0;
}

static int vsystem(const char *command)
{
	int status, ret = 0;
	pid_t pid;

	pid = vfork();
	if (pid < 0) {
		printf("vfork failed\n");
		return -1;
	} else if (pid == 0) {
		ret = execl("/bin/sh", "sh", "-c", command, (char *)0);
		if (ret < 0) {
			printf("execl failed, errno = %d\n", errno);
			return -1;
		}
	}
	ret = waitpid(pid, &status, 0);
	if (ret != pid) {
		printf("waitpid failed, ret = %d\n", ret);
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status)) {
			printf("exit status = %d\n", WEXITSTATUS(status));
			return -1;
		}
	} else {
		printf("abort exceptly\n");
		return -1;
	}

	return 0;
}

/*
 * miniloader head
*/
struct img_header
{
	uint32_t magic;
	uint32_t version;
	uint32_t image_type;
	uint32_t spl_addr;
	uint32_t spl_len;
	uint32_t troot_fw_addr;
	uint32_t troot_fw_len;
	uint32_t sig_addr;
	uint32_t sig_len;
	uint32_t checksum;
	uint32_t reserved[6];
} __attribute__((packed));



int boot_checksum(int cs, unsigned char *img, unsigned int size)
{
	int cs_calc = 0;
	int i = 0;

	while(i < size)
		cs_calc += img[i++];

	if(cs_calc == cs)
		return 0;
	return -1;
}


int upgrade_miniloader(char *filename)
{
	int fd0, fd1, ret;
	size_t size;
	struct img_header *hdr;
	void *img = NULL;
	struct stat st;
	off_t offs;

	ret = stat(filename, &st);
	if(ret < 0) {
		printf("error %d, %d, %d\n", __LINE__, ret, errno);
		return -1;
	}

	fd0 = open(filename, O_RDONLY);
	if (fd0 < 0) {
		printf("open failed\n");
		return -1;
	}

	fd1 = open("/dev/mtd0", O_RDWR);
	if (fd1 < 0) {
		printf("open flash fail\n");
		goto check_write_fail;
	}

	img = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd0, 0);
	if (img == MAP_FAILED) {
		printf("error %d, %d, %d\n", __LINE__, ret, errno);
		return -1;
	}

#define BOOT_UP_MAGIC		(0x4152544F)
#define MINILOADER_OFFSET	0x10000

	hdr = (struct img_header *)img;

	if (hdr->magic != BOOT_UP_MAGIC){
		printf("img magic fail\n");
		goto check_write_fail;
	}

	ret = boot_checksum(hdr->checksum, img + sizeof(struct img_header), hdr->spl_len);
	if(ret < 0) {
		printf("Checksum img fail\n");
		goto check_write_fail;
	}

	if ((offs = lseek(fd1, MINILOADER_OFFSET, SEEK_SET)) != MINILOADER_OFFSET) {
		printf("lseek fail\n", ret, st.st_size);
		goto check_write_fail;
	}

	if ((ret = vsystem("/etc/flash_erase /dev/mtd0 0 0")) < 0) {
		printf("flash_erase ret =%d\n", ret);
		goto check_write_fail;
	}

	if ((ret = write(fd1, img, st.st_size)) != st.st_size) {
		printf("open flash fail ret =%d file size=%d\n", ret, st.st_size);
		goto check_write_fail;
	}

	munmap(img, st.st_size);
	close(fd1);
	close(fd0);

	return 0;

check_write_fail:
	munmap(img, st.st_size);

error_mmap:
	close(fd1);

error_read_header:
	close(fd0);
	return -1;
}


int main(int argc, char *argv[])
{
	int ret, fd, c;
	char *img;
	struct stat st;
	int verbose = 0;
	int reboot = 0;
	int enterclean = 0;
	int hasimage = 0;
	int getsdkver = 0;
	int mmap_mode = 0;
	int mini_loader = 0;
	int update_gpt = 0;
	char buf[128] = {0};

	printf("Artosyn Upgrade, Version: %s %s\n", __DATE__, __TIME__);

	if(argc < 2) {
		usage();
		exit(0);
	}

	while ((c = getopt_long(argc, argv, ":crhvsfmpMu", long_options, NULL)) !=
	       EOF) {
		switch (c) {
		case 'c':
			enterclean = 1;
			break;
		case 'r':
			reboot = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		case 's':
			getsdkver = 1;
			break;
		case 'f':
			artosyn_set_force_upgrade(true);
			break;
		case 'm':
			mmap_mode = 1;
			break;
		case 'p':
			ar_upgrade_product(true);
			break;
		case 'M':
			mini_loader = 1;
			break;
		case 'u':
			update_gpt = 1;
			break;
		case 'h':
		default:
			/* ignore unknown options */
			usage();
			exit(0);
			break;
		}
	}

	/* non-option has only one, image-file */
	if (argc > optind + 1) {
		usage();
		exit(0);
	}

	/* no image-file */
	if (argc == optind) {
		if (update_gpt) {
			ret = ar_emmc_last_partition_resize();
			if (ret)
				exit(1);
			exit(0);
		}

		if (enterclean) {
			if (verbose)
				ret = ar_enter_clean_system(upgrade_callback);
			else
				ret = ar_enter_clean_system(NULL);
			if (ret)
				exit(1);
			exit(0);
		}

		if (reboot) {
			ret = access("/tmp/ar_wdt_service", F_OK);
			if (ret) {
				printf("/tmp/ar_wdt_service is not exsit\n");
				exit(1);
			}

			ret = vsystem("/tmp/ar_wdt_service -t 0");
			if (ret) {
				printf("reboot by watchdog failed\n");
				exit(1);
			}
			exit(0);
		}
	} else {
		/* -c and image-file could not be co-exsit */
		if (enterclean)	{
			usage();
			exit(0);
		}
		hasimage = 1;
	}

	if (!hasimage) {
		usage();
		exit(0);
	}

	if (mini_loader)
		return upgrade_miniloader(argv[optind]);

	/* if upgrade, kill firstly */
	if (!getsdkver) {
		ret = ar_upgrade_prepare();
		if (ret) {
			printf("OTA-PREPARE failed\n");
			artosyn_set_force_upgrade(false);
			exit(1);
		}
	}

	ret = stat(argv[optind], &st);
	if(ret < 0) {
		printf("error %d, %d, %d\n", __LINE__, ret, errno);
		artosyn_set_force_upgrade(false);
		exit(1);
	}

	fd = open(argv[optind], O_RDONLY);
	if (fd < 0) {
		printf("open failed\n");
		artosyn_set_force_upgrade(false);
		exit(1);
	}

	if (mmap_mode) {
		img = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (img == MAP_FAILED) {
			printf("error %d, %d, %d\n", __LINE__, ret, errno);
			ret = -1;
			goto error_close;
		}
	} else {
#define RDLEN 	0x10000
		img = malloc(RDLEN);
		if (!img) {
			printf("malloc failed\n");
			ret = -1;
			goto error_close;
		}
		ret = read(fd, img, RDLEN);
		if (ret < 0) {
			printf("read failed\n");
			goto error_exit;
		}
	}

	if (getsdkver) {
		ret = ar_flash_get_sdkversion(img, 0, buf);
		if(ret < 0) {
			printf("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_exit;
		}
		printf("SDK Version: %s\n", buf);
		goto error_exit;
	}

	if (verbose)
		ret = ar_upgrade_ex(argv[optind], upgrade_callback);
	else
		ret = ar_upgrade_ex(argv[optind], NULL);
	if(ret < 0) {
		printf("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error_exit;
	}

	if (reboot) {
		ret = access("/tmp/ar_wdt_service", F_OK);
		if (ret) {
			printf("/tmp/ar_wdt_service is not exsit\n");
			goto error_exit;
		}

		ret = vsystem("/tmp/ar_wdt_service -t 0");
		if (ret) {
			printf("reboot by watchdog failed\n");
			goto error_exit;
		}
	}

error_exit:
	if (mmap_mode)
		munmap(img, st.st_size);
	else
		free(img);
error_close:
	artosyn_set_force_upgrade(false);
	ar_upgrade_product(false);
	close(fd);
	return ret;
}
