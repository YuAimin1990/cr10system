#!/bin/csh

coffutil -c -split -b vv CDNNCallFuncDemo.a
sorthex vv0C.out vv0C.out.hex -pad=00
sorthex vv1C.out vv1C.out.hex -pad=00
sorthex vv0D.out vv0D.out.hex -pad=00
sorthex vv1D.out vv1D.out.hex -pad=00
hex2rom vv0C.out.hex vv0C.rom
hex2rom vv1C.out.hex vv1C.rom
hex2rom vv1D.out.hex vv1D.rom
