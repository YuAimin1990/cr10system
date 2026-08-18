#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/byteorder.h>
#include "common.h"
#include "uuid.h"

#if 0
static int inc = 0;
#endif

int uuid_str_to_bin(char *uuid_str, unsigned char *uuid_bin, int str_format)
{
	uint16_t tmp16;
	uint32_t tmp32;
	uint64_t tmp64;

	if (!uuid_str_valid(uuid_str))
		return -1;

	if (str_format == UUID_STR_FORMAT_STD) {
		tmp32 = __cpu_to_be32(strtoul(uuid_str, NULL, 16));
		memcpy(uuid_bin, &tmp32, 4);

		tmp16 = __cpu_to_be16(strtoul(uuid_str + 9, NULL, 16));
		memcpy(uuid_bin + 4, &tmp16, 2);

		tmp16 = __cpu_to_be16(strtoul(uuid_str + 14, NULL, 16));
		memcpy(uuid_bin + 6, &tmp16, 2);
	} else {
		tmp32 = __cpu_to_le32(strtoul(uuid_str, NULL, 16));
		memcpy(uuid_bin, &tmp32, 4);

		tmp16 = __cpu_to_le16(strtoul(uuid_str + 9, NULL, 16));
		memcpy(uuid_bin + 4, &tmp16, 2);

		tmp16 = __cpu_to_le16(strtoul(uuid_str + 14, NULL, 16));
		memcpy(uuid_bin + 6, &tmp16, 2);
	}

	tmp16 = __cpu_to_be16(strtoul(uuid_str + 19, NULL, 16));
	memcpy(uuid_bin + 8, &tmp16, 2);

	tmp64 = __cpu_to_be64(strtoull(uuid_str + 24, NULL, 16));
	memcpy(uuid_bin + 10, (char *)&tmp64 + 2, 6);

	return 0;
}

int uuid_str_valid(const char *uuid)
{
	int i, valid;

	if (uuid == NULL)
		return 0;

	for (i = 0, valid = 1; uuid[i] && valid; i++) {
		switch (i) {
		case 8: case 13: case 18: case 23:
			valid = (uuid[i] == '-');
			break;
		default:
			valid = isxdigit(uuid[i]);
			break;
		}
	}

	if (i != UUID_STR_LEN || !valid)
		return 0;

	return 1;
}

#if 0 
void gen_rand_uuid_str(char *uuid_str, int str_format)
{
	srand(time(NULL) + inc);

	snprintf(uuid_str, 64, "%08X-%04X-%04X-%04X-%04X%04X%04X",
		rand() & 0xffffffff,
		rand() & 0xffff, 
		rand() & 0xffff,
		rand() & 0xffff,
		rand() & 0xffff, rand() & 0xffff, rand() & 0xffff);

	inc++;
}
#else
void gen_rand_uuid_str(char *uuid_str, int str_format)
{
	uuid_t uuid;

	uuid_generate(uuid);
	uuid_unparse(uuid, uuid_str);
}
#endif
