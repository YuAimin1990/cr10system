#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
//#include <unistd.h>
#include "stdafx.h"
#include "dsp_bin_pack.h"

using namespace std;
#define MAX(a,b) ((a)>(b)?(a):(b))

dsp_bin::dsp_bin(int file_len,
	             char * ptcm,
				 char * dtcm,
				 char * extc,
				 char * extd,
				 char * out,
                 addr_t addr_0,
                 addr_t addr_1,
                 addr_t addr_2,
                 addr_t addr_3,
                 addr_t addr_4) : all_in_one_bin_len(file_len)
{
    int i = 0;

	bin_len[0] = CO_FILE_LEN;
	bin_len[1] = DO_FILE_LEN;
	bin_len[2] = C1_FILE_LEN;
	bin_len[3] = D1_FILE_LEN;
	bin_len[4] = D12_FILE_LEN;

    //cout << "target file len: " << all_in_one_bin_len << endl;
	printf("target file len %d.\n", file_len);

    all_in_noe_bin = (char *)malloc(all_in_one_bin_len);
    //cout << hex << all_in_noe_bin << endl;
    if (nullptr == all_in_noe_bin)
    {
        cout << "new error" << __FUNCTION__ << __LINE__ << endl;

        return;
    }
    //cout << all_in_one_bin_len << endl;
    //cout << "all in one bin addr " << (void *)all_in_noe_bin << endl;
    for (i = 0; i < MAX_BIN_TYPE; i++)
    {
        max_file_len = MAX(max_file_len, bin_len[i]);
        //cout << "max file len" << max_file_len << endl;
    }
    read_file_buff = (char *)malloc(max_file_len);
    //cout << "read file buff addr " << (void *)read_file_buff << endl;
    if (nullptr == read_file_buff)
    {
        cout << "new error" << __FUNCTION__ << __LINE__ << endl;

        return;
    }
    memset(read_file_buff, 0x0, max_file_len);
    //cout << "sdss" << endl;
    img_header.start_addr[C0_BIN] = addr_0;
    img_header.start_addr[D0_BIN] = addr_1;
    img_header.start_addr[C1_BIN] = addr_2;
    img_header.start_addr[D1_BIN] = addr_3;
    img_header.start_addr[D12_BIN] = addr_4;
	strcpy(bin_path[0], ptcm);
	strcpy(bin_path[1], dtcm);
	strcpy(bin_path[2], extc);
	strcpy(bin_path[3], extd);
	strcpy(output_bin_path, out);
}

dsp_bin::~dsp_bin()
{
    free(all_in_noe_bin);
    free(read_file_buff);
}

int dsp_bin::dsp_pack()
{
    int i = 0;
    int read_sz = 0, write_sz = 0;
    off_t file_offset = 0;
    FILE * fpOut = NULL;
	FILE * fpBin = NULL;

    remove(output_bin_path);
    fpOut = fopen(output_bin_path, "wb+");
    if (!fpOut)
    {
        cout << "open file error"
             << " err" << strerror(errno)
             << "  func:" << __FUNCTION__ << "  line:" << __LINE__ << endl;

        return open_file_err;
    }
    for (i = 0; i < MAX_BIN_TYPE; i++)
    {
        read_sz = 0;
     
        fpBin = fopen(bin_path[i], "rb");
        if (!fpBin)
        {
			cout << __FUNCTION__ << ": " << __LINE__ << " File no exist: " << bin_path[i] << endl;
            img_header.offset[i] = file_offset;
            img_header.bin_len[i] = 0;
            continue;
        }

        //cout << "read file" << bin_path[i] << endl;
        memset(read_file_buff, 0x0, bin_len[i]);
        read_sz = fread(read_file_buff, 1, bin_len[i], fpBin);
        //cout << "read sz" << read_sz << endl;
		fseek(fpOut, file_offset, SEEK_SET);        

		printf("bin_path[i] = %s;read_sz=%d\n", bin_path[i], read_sz);
		//for PTCM, there's a size align requirement by PDMA, so we copy the whole size
    	if(i == 0)
    	{
    		read_sz = bin_len[i];
    	}    
		write_sz = fwrite(read_file_buff, 1, read_sz, fpOut);
        
        // cout << "file offset" << file_offset << "  write sz: " << write_sz << endl;
        img_header.bin_len[i] = write_sz;
        //img_header.bin_len[i] = bin_len[i];
        img_header.offset[i] = file_offset;
        file_offset = file_offset + write_sz;
        //file_offset = file_offset + bin_len[i];
		fclose(fpBin);
    }
    //chmod(output_bin_path, file_mode);
    fclose(fpOut);

    return 0;
}

int dsp_bin::add_header_to_img()
{
    int_32_t read_sz = -1, write_sz = -1;
    char *bin_buff = NULL;
    //struct stat file_stat;
    int file_len = 0;
	FILE * fpOut = NULL;

	fpOut = fopen(output_bin_path, "rb+");
	if (!fpOut)
	{
		cout << "open file error "
			<< " err" << strerror(errno)
			<< "  func:" << __FUNCTION__ << "  line: " << __LINE__ << endl;
	
		return open_file_err;
	}
	    
    img_header.header_len = sizeof(img_header);
    cout << img_header.header_len << endl;

	fseek(fpOut,0,SEEK_END);
	file_len = ftell(fpOut);
	rewind(fpOut);
    //cout << "bin file len: " << file_len << endl;

    bin_buff = (char *)malloc(file_len + img_header.header_len);
    if (nullptr == bin_buff)
    {
		cout << "new error" << __FUNCTION__ << __LINE__ << endl;

        return alloc_err;
    }
    memset(bin_buff, 0x0, file_len + img_header.header_len);

    read_sz = fread(bin_buff + img_header.header_len, 1, file_len, fpOut);
	if (read_sz < 0)
    {
		cout << "read file error " << __FUNCTION__ << __LINE__ << endl;
        free(bin_buff);

        return read_file_err;
    }
    remove(output_bin_path);
    
	fpOut = fopen(output_bin_path, "wb+");
	//output_put_bin_fd = open(output_bin_path, O_RDWR | O_TRUNC | O_CREAT, file_mode);
    if (!fpOut)
    {
        cout << "open file error"
             << " err" << strerror(errno)
			 << "  func:" << __FUNCTION__ << "  line: " << __LINE__ << endl;
        free(bin_buff);

        return open_file_err;
    }
    memcpy(bin_buff, &img_header, img_header.header_len);
	write_sz = fwrite(bin_buff, 1, img_header.header_len + file_len, fpOut);
    if (write_sz < 0)
    {
		cout << "write file error " << __FUNCTION__ << __LINE__ << endl;
        free(bin_buff);

        return write_file_err;
    }
    fclose(fpOut);
    free(bin_buff);

    return 0;
}
