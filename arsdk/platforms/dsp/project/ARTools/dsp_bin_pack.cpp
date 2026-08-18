#include "dsp_bin_pack_unpack.h"
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

static mode_t file_mode = S_IRUSR |
                          S_IWUSR |
                          S_IXUSR |
                          S_IRGRP |
                          S_IWGRP |
                          S_IXGRP |
                          S_IROTH |
                          S_IWOTH |
                          S_IXOTH;

dsp_bin::dsp_bin(int file_len,
                 addr_t addr_0,
                 addr_t addr_1,
                 addr_t addr_2,
                 addr_t addr_3,
                 addr_t addr_4) : all_in_one_bin_len(file_len)
{
    int i = 0;

    cout << "target file len: " << all_in_one_bin_len << endl;
    all_in_noe_bin = (char *)malloc(all_in_one_bin_len);
    cout << hex << all_in_noe_bin << endl;
    if (nullptr == all_in_noe_bin)
    {
        cout << "new error" << __func__ << __LINE__ << endl;

        return;
    }
    cout << all_in_one_bin_len << endl;
    cout << "all in one bin addr " << (void *)all_in_noe_bin << endl;
    for (i = 0; i < MAX_BIN_TYPE; i++)
    {
        max_file_len = max(max_file_len, bin_len[i]);
        cout << "max file len" << max_file_len << endl;
    }
    read_file_buff = (char *)malloc(max_file_len);
    cout << "read file buff addr " << (void *)read_file_buff << endl;
    if (nullptr == read_file_buff)
    {
        cout << "new error" << __func__ << __LINE__ << endl;

        return;
    }
    memset(read_file_buff, 0x0, max_file_len);
    cout << "sdss" << endl;
    img_header.start_addr[C0_BIN] = addr_0;
    img_header.start_addr[D0_BIN] = addr_1;
    img_header.start_addr[C1_BIN] = addr_2;
    img_header.start_addr[D1_BIN] = addr_3;
    img_header.start_addr[D12_BIN] = addr_4;
}

dsp_bin::~dsp_bin()
{
    free(all_in_noe_bin);
    free(read_file_buff);
}

int dsp_bin::dsp_pack()
{
    int bin_fd = -1;
    int i = 0;
    int read_sz = 0, write_sz = 0;
    off_t file_offset = 0;
    int output_put_bin_fd = -1;

    remove(output_bin_path);
    output_put_bin_fd = open(output_bin_path, O_RDWR | O_TRUNC | O_CREAT, file_mode);
    if (output_put_bin_fd < 0)
    {
        cout << "open file error"
             << " err" << strerror(errno)
             << "  func:" << __func__ << "  line:" << __LINE__ << endl;

        return open_file_err;
    }
    for (i = 0; i < MAX_BIN_TYPE; i++)
    {
        read_sz = 0;
     
        bin_fd = open(bin_path[i], O_RDWR);
        if (bin_fd < 0)
        {
            cout << __func__ << ": " << __LINE__ << " File no exist: " << bin_path[i] << endl;
            img_header.offset[i] = file_offset;
            img_header.bin_len[i] = 0;
            continue;
        }

        cout << "read file" << bin_path[i] << endl;
        memset(read_file_buff, 0x0, bin_len[i]);
        read_sz = read(bin_fd, read_file_buff, bin_len[i]);
        cout << "read sz" << read_sz << endl;
        lseek(output_put_bin_fd, file_offset, SEEK_SET);
#if 1
    //if(i != 0)
    {
    	if(bin_path[i] == CO_FILE_NAME)
    	{
    		read_sz = bin_len[i];
    	}    
        write_sz = write(output_put_bin_fd, read_file_buff, read_sz);
        // write_sz = write(output_put_bin_fd, read_file_buff, bin_len[i]);
        cout << "file offset" << file_offset << "  write sz: " << write_sz << endl;
        img_header.bin_len[i] = write_sz;
        //img_header.bin_len[i] = bin_len[i];
        img_header.offset[i] = file_offset;
        file_offset = file_offset + write_sz;
        //file_offset = file_offset + bin_len[i];
    }
#else

    else
    {
        write_sz = write(output_put_bin_fd, read_file_buff, bin_len[i]);
        cout << "file offset" << file_offset << "  write sz: " << write_sz << endl;
        img_header.bin_len[i] = bin_len[i];
        img_header.offset[i] = file_offset;
        file_offset = file_offset + bin_len[i];
    }
#endif
    }
    chmod(output_bin_path, file_mode);
    close(output_put_bin_fd);

    return 0;
}

int dsp_bin::add_header_to_img()
{
    int_32_t output_put_bin_fd = -1, read_sz = -1, write_sz = -1;
    char *bin_buff = NULL;
    struct stat file_stat;
    int file_len = 0;

    memset(&file_stat, 0x0, sizeof(file_stat));
    img_header.header_len = sizeof(img_header);
    cout << img_header.header_len << endl;
    stat(output_bin_path, &file_stat);
    file_len = file_stat.st_size;
    cout << "bin file len: " << file_len << endl;
    bin_buff = (char *)malloc(file_len + img_header.header_len);
    if (nullptr == bin_buff)
    {
        cout << "new error" << __func__ << __LINE__ << endl;

        return alloc_err;
    }
    memset(bin_buff, 0x0, file_len + img_header.header_len);
    output_put_bin_fd = open(output_bin_path, O_RDWR, file_mode);
    if (output_put_bin_fd < 0)
    {
        cout << "open file error "
             << " err" << strerror(errno)
             << "  func:" << __func__ << "  line: " << __LINE__ << endl;
        free(bin_buff);

        return open_file_err;
    }
    read_sz = read(output_put_bin_fd, bin_buff + img_header.header_len, file_len);
    if (read_sz < 0)
    {
        cout << "read file error " << __func__ << __LINE__ << endl;
        free(bin_buff);

        return read_file_err;
    }
    remove(output_bin_path);
    output_put_bin_fd = open(output_bin_path, O_RDWR | O_TRUNC | O_CREAT, file_mode);
    if (output_put_bin_fd < 0)
    {
        cout << "open file error"
             << " err" << strerror(errno)
             << "  func:" << __func__ << "  line: " << __LINE__ << endl;
        free(bin_buff);

        return open_file_err;
    }
    memcpy(bin_buff, &img_header, img_header.header_len);
    write_sz = write(output_put_bin_fd, bin_buff, img_header.header_len + file_len);
    if (write_sz < 0)
    {
        cout << "write file error " << __func__ << __LINE__ << endl;
        free(bin_buff);

        return write_file_err;
    }
    close(output_put_bin_fd);
    free(bin_buff);

    return 0;
}
