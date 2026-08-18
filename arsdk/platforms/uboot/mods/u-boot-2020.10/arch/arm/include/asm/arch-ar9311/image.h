#ifndef __AR9311_IMAGE_H
#define __AR9311_IMAGE_H

struct sirius_image_header
{
	unsigned int magic;
	unsigned int version;
	unsigned int img_type;
	unsigned int img_load_addr; //actual entry_point after decryption
	unsigned int img_len;
	unsigned int algo;
	unsigned int hash_size;
	unsigned int sig_len;
	unsigned int checksum;
	unsigned int reserved[7];
};

typedef struct spl_header
{
	unsigned int magic; 		//'A','R',0x93,0x11
	unsigned short img_type;	//image type.
	unsigned short header_len;  //header length
	unsigned int header_checksum;//header checksum
	unsigned int img_version;	//image version
	unsigned int flag;			//boot flag
	unsigned int boot_info;     //rom code use this field to stroe boot device and partition.
	unsigned int spl_load_addr;
	unsigned int spl_len;
	unsigned int troot_load_addr;
	unsigned int troot_len;
	unsigned int signature_load_addr;
	unsigned int signature_len;
	unsigned int spl_dtb_offset;
	unsigned long checksum;		//payload checksum
	unsigned int reserved; //pad to 64 byte
	unsigned char patch[0];	//reserved for spl use
} __attribute__ ((packed)) spl_header;

#endif
