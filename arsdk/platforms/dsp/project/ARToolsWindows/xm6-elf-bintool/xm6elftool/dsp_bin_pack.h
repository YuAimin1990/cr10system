#ifndef DSP_BIN_PACK_UNPACK_H
#define DSP_BIN_PACK_UNPACK_H
//#ifdef __cplusplus
//extern "C" {
//#endif
#include <iostream>
#include <string>

using namespace std;

#define CO_FILE_NAME ("./C0.bin")
#define DO_FILE_NAME ("./D0.bin")
#define C1_FILE_NAME ("./C1.bin")
#define D1_FILE_NAME ("./D1.bin")
#define D12_FILE_NAME ("./D12.bin")

#define OUTPUT_FILE_PATH ("./ceva.bin")

#define CO_FILE_LEN (64 * 1024)
#define DO_FILE_LEN (512 * 1024)
#define C1_FILE_LEN (0x400000)
#define D1_FILE_LEN (0x2000000)
#define D12_FILE_LEN (0x2000000)

typedef int int_32_t;
typedef unsigned int uint_32_t;
typedef unsigned int offset_t;
typedef unsigned int addr_t;

class dsp_bin
{
  private:
    typedef enum
    {
        C0_BIN = 0,
        D0_BIN,
        C1_BIN,
        D1_BIN,
        D12_BIN,
        MAX_BIN_TYPE
    } dsp_bin_type_e;
    typedef enum
    {
        input_file_err = -1,
        open_file_err = -2,
        alloc_err = -3,
        read_file_err = -4,
        write_file_err = -5
    } dsp_bin_error_e;

#pragma pack(8)
    struct img_header_st
    {
        int_32_t header_len;
        offset_t offset[MAX_BIN_TYPE];
        addr_t start_addr[MAX_BIN_TYPE];
        int_32_t bin_len[MAX_BIN_TYPE];
	   //int_32_t reserved[3];
    } img_header;
#pragma pack()

    char bin_path[MAX_BIN_TYPE][FILENAME_MAX];
    unsigned int bin_len[MAX_BIN_TYPE];
    unsigned int all_in_one_bin_len = 0;
    char *all_in_noe_bin;
    char *read_file_buff;
    unsigned int max_file_len = 0;
	char output_bin_path[FILENAME_MAX];

  public:
	  dsp_bin(int,
              char * ptcm,
              char * dtcm,
			  char * extc,
			  char * extd,
			  char * out,
            addr_t,
            addr_t,
            addr_t,
            addr_t,
            addr_t);
    int dsp_pack();
    int add_header_to_img();
    ~dsp_bin();
};

//#ifdef __cplusplus
//}
//#endif
#endif /* DSP_BIN_PACK_UNPACK_H */
