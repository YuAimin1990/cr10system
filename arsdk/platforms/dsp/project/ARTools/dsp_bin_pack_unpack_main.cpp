#include "dsp_bin_pack_unpack.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
    addr_t c0_bin_addr = 0;
    addr_t d0_bin_addr = 0;
    addr_t c1_bin_addr = 0;
    addr_t d1_bin_addr = 0;
    addr_t d12_bin_addr = 0;

    if(argv[1])
    {
        c0_bin_addr = (addr_t)strtoll(argv[1], NULL, 16);
    }
    if(argv[2])
    {
        d0_bin_addr = (addr_t)strtoll(argv[2], NULL, 16);
    }
    if(argv[3])
    {
        c1_bin_addr = (addr_t)strtoll(argv[3], NULL, 16);
    }
    if(argv[4])
    {
        d1_bin_addr = (addr_t)strtoll(argv[4], NULL, 16);
    }
    if(argv[5])
    {
        d12_bin_addr = (addr_t)strtoll(argv[5], NULL, 16);
    }

    cout << "input addr " << hex << c0_bin_addr << endl;
    cout << "input addr " << hex << d0_bin_addr << endl;
    cout << "input addr " << hex << c1_bin_addr << endl;
    cout << "input addr " << hex << d1_bin_addr << endl;
    cout << "input addr " << hex << d12_bin_addr << endl;

    dsp_bin dsp_bin1(1024 * 1024 * 10,
                     c0_bin_addr,
                     d0_bin_addr,
                     c1_bin_addr,
                     d1_bin_addr,
                     d12_bin_addr);
    dsp_bin1.dsp_pack();
    dsp_bin1.add_header_to_img();

    return 0;
}