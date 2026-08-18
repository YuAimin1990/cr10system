#ifndef _VIDEO_DA_H_
#define _VIDEO_DA_H_
#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        enum_tp2803_vo_ahd1080p30 = 0,
        enum_tp2803_vo_ahd1080p25
    };

    typedef struct tp28xx_dev_attr_
    {
        uint32_t i2cbus;
        uint32_t uGpioGroup; // 3    GB3_3
        uint32_t uGpioPort;  // 1
        uint32_t uGpioId;    // 3
        uint32_t vomode;
    }tp28xx_dev_attr;

    int tp28xx_dvp_init();
    int tp28xx_init(tp28xx_dev_attr attr);
    void tp28xx_deinit();

#ifdef __cplusplus
}
#endif
#endif
