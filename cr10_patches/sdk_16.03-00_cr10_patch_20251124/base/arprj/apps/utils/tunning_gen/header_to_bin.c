#include <stdio.h>
#include <stdint.h>
#include "hal_type.h"
#include "hal_vin_tuning_def.h"
#include <sys/stat.h>
#include <fcntl.h>
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_rgb={
    #include "./inc/imx307_tuning_preview_rgb.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_ir={
    #include "./inc/imx307_tuning_preview_ir.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80={
   #include "./inc/imx307_tuning_preview_dwt_80.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80_hdr={
   #include "./inc/imx307_tuning_preview_dwt_80_hdr.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_night_dwt_80={
   #include "./inc/imx307_tuning_night_dwt_80.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80_l={
#include "./inc/imx307_tuning_preview_dwt_80_l.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80_r={
#include "./inc/imx307_tuning_preview_dwt_80_r.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80_hdr_l={
#include "./inc/imx307_tuning_preview_dwt_80_hdr_l.h"
};
static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_dwt_80_hdr_r={
#include "./inc/imx307_tuning_preview_dwt_80_hdr_r.h"
};
static STRU_ALGO_ISP_TUNING_T imx415_tuning_preview={
#include "./inc/imx415_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T gn2_tuning_preview={
#include "./inc/gn2_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T imx415_tuning_preview_hdr={
#include "./inc/imx415_tuning_preview_hdr.h"
};
static STRU_ALGO_ISP_TUNING_T imx464_tuning_preview={
#include "./inc/imx464_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T imx464_hdr_tuning_preview={
#include "./inc/imx464_hdr_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T imx347_tuning_preview={
#include "./inc/imx347_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T imx347_hdr_tuning_preview={
#include "./inc/imx347_hdr_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T sc910gs_tuning_preview={
#include "./inc/sc910gs_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T sc910gs_tuning_preview_hdr={
#include "./inc/sc910gs_tuning_preview_hdr.h"
};
static STRU_ALGO_ISP_TUNING_T common_infrared_tuning_preview={
#include "./inc/common_infrared_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T common_infrared_tuning_preview1={
#include "./inc/common_infrared_tuning_preview1.h"
};
static STRU_ALGO_ISP_TUNING_T common_infrared_tuning_preview_thermal={
#include "./inc/common_infrared_tuning_preview_thermal.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_timo256_tuning_preview={
#include "./inc/infrared_timo256_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_timo256_tuning_preview_isp0={
#include "./inc/infrared_timo256_tuning_preview_isp0.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_timo256_tuning_preview_isp1={
#include "./inc/infrared_timo256_tuning_preview_isp1.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_elf1_tuning_preview={
#include "./inc/infrared_elf1_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_elf1_tuning_preview_isp0={
#include "./inc/infrared_elf1_tuning_preview_isp0.h"
};
static STRU_ALGO_ISP_TUNING_T infrared_elf1_tuning_preview_isp1={
#include "./inc/infrared_elf1_tuning_preview_isp1.h"
};
static STRU_ALGO_ISP_TUNING_T os04a10_tuning_preview={
#include "./inc/os04a10_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T os04a10_tuning_preview_hdr={
#include "./inc/os04a10_tuning_preview_hdr.h"
};
static STRU_ALGO_ISP_TUNING_T os05a20_tuning_preview={
#include "./inc/os05a20_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T os05a20_tuning_preview_hdr={
#include "./inc/os05a20_tuning_preview_hdr.h"
};
static STRU_ALGO_ISP_TUNING_T gc2093_tuning_preview={
#include "./inc/gc2093_tuning_preview.h"
};
static STRU_ALGO_ISP_TUNING_T gc2093_tuning_preview_hdr={
#include "./inc/gc2093_tuning_preview_hdr.h"
};

static STRU_ALGO_ISP_TUNING_T sc530ai_tuning_preview={
#include "./inc/sc530ai_tuning_preview_2880x1616_30fps.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst417w_tuning_preview_thermal = {
#include "./inc/infrared_gst417w_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst417w_tuning_preview_isp1 = {
#include "./inc/infrared_gst417w_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst417w_tuning_preview_isp2 = {
#include "./inc/infrared_gst417w_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_h3812c1sg_tuning_preview_thermal = {
#include "./inc/infrared_h3812c1sg_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_h3812c1sg_tuning_preview_isp1 = {
#include "./inc/infrared_h3812c1sg_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_h3812c1sg_tuning_preview_isp2 = {
#include "./inc/infrared_h3812c1sg_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd6122c_tuning_preview_thermal = {
#include "./inc/infrared_rtd6122c_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd6122c_tuning_preview_isp1 = {
#include "./inc/infrared_rtd6122c_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd6122c_tuning_preview_isp2 = {
#include "./inc/infrared_rtd6122c_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd3172c_tuning_preview_thermal = {
#include "./inc/infrared_rtd3172c_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd3172c_tuning_preview_isp1 = {
#include "./inc/infrared_rtd3172c_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_rtd3172c_tuning_preview_isp2 = {
#include "./inc/infrared_rtd3172c_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst412c_tuning_preview_thermal = {
#include "./inc/infrared_gst412c_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst412c_tuning_preview_isp1 = {
#include "./inc/infrared_gst412c_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst412c_tuning_preview_isp2 = {
#include "./inc/infrared_gst412c_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst612c_tuning_preview_thermal = {
#include "./inc/infrared_gst612c_tuning_preview_thermal.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst612c_tuning_preview_isp1 = {
#include "./inc/infrared_gst612c_tuning_preview_isp1.h"
};

static STRU_ALGO_ISP_TUNING_T infrared_gst612c_tuning_preview_isp2 = {
#include "./inc/infrared_gst612c_tuning_preview_isp2.h"
};

static STRU_ALGO_ISP_TUNING_T sc2210_tuning_preview = {
#include "./inc/sc2210_tuning_preview.h"
};

static STRU_ALGO_ISP_TUNING_T imx307_tuning_preview_fam30 = {
#include "./inc/imx307_tuning_preview_fam30.h"
};

static STRU_ALGO_ISP_TUNING_T sc132gs_tuning_preview_fam30= {
#include "./inc/sc132gs_tuning_preview_fam30.h"
};

struct dump_tuning_file
{
   char *name;
   STRU_ALGO_ISP_TUNING_T  *p_tuning;
};
struct dump_tuning_file dump[]=
{
//1
    {
        "imx307_tuning_preview_rgb.bin",
        &imx307_tuning_preview_rgb,
    },
    {
        "imx307_tuning_preview_ir.bin",
        &imx307_tuning_preview_ir,
    },
    {
       "imx307_tuning_preview_dwt_80.bin",
        &imx307_tuning_preview_dwt_80,
    },
    {
       "imx307_tuning_night_dwt_80.bin",
        &imx307_tuning_night_dwt_80,
    },
    {
       "imx307_tuning_preview_dwt_80_hdr.bin",
        &imx307_tuning_preview_dwt_80_hdr,
    },
    {
        "imx307_tuning_preview_dwt_80_l.bin",
        &imx307_tuning_preview_dwt_80_l,
    },
    {
        "imx307_tuning_preview_dwt_80_r.bin",
        &imx307_tuning_preview_dwt_80_r,
    },
    {
        "imx307_tuning_preview_dwt_80_hdr_l.bin",
        &imx307_tuning_preview_dwt_80_hdr_l,
    },
    {
        "imx307_tuning_preview_dwt_80_hdr_r.bin",
        &imx307_tuning_preview_dwt_80_hdr_r,
    },
    {
        "gn2_tuning_preview.bin",
        &gn2_tuning_preview,
    },
    {
        "imx415_tuning_preview.bin",
        &imx415_tuning_preview,
    },
    {
        "imx415_tuning_preview_hdr.bin",
        &imx415_tuning_preview_hdr,
    },
    {
        "imx464_tuning_preview.bin",
        &imx464_tuning_preview,
    },
    {
        "imx464_hdr_tuning_preview.bin",
        &imx464_hdr_tuning_preview,
    },
    {
        "imx347_tuning_preview.bin",
        &imx347_tuning_preview,
    },
    {
        "imx347_hdr_tuning_preview.bin",
        &imx347_hdr_tuning_preview,
    },
    {
        "sc910gs_tuning_preview.bin",
        &sc910gs_tuning_preview,
    },
    {
        "sc910gs_tuning_preview_hdr.bin",
        &sc910gs_tuning_preview_hdr,
    },
    {
        "common_infrared_tuning_preview.bin",
        &common_infrared_tuning_preview,
    },
    {
        "common_infrared_tuning_preview1.bin",
        &common_infrared_tuning_preview1,
    },
    {
        "common_infrared_tuning_preview_thermal.bin",
        &common_infrared_tuning_preview_thermal,
    },
    {
        "infrared_timo256_tuning_preview.bin",
        &infrared_timo256_tuning_preview,
    },
    {
        "infrared_timo256_tuning_preview_isp0.bin",
        &infrared_timo256_tuning_preview_isp0,
    },
    {
        "infrared_timo256_tuning_preview_isp1.bin",
        &infrared_timo256_tuning_preview_isp1,
    },
    {
        "infrared_elf1_tuning_preview.bin",
        &infrared_elf1_tuning_preview,
    },
    {
        "infrared_elf1_tuning_preview_isp0.bin",
        &infrared_elf1_tuning_preview_isp0,
    },
    {
        "infrared_elf1_tuning_preview_isp1.bin",
        &infrared_elf1_tuning_preview_isp1,
    },
    {
        "os04a10_tuning_preview.bin",
        &os04a10_tuning_preview,
    },
    {
        "os04a10_tuning_preview_hdr.bin",
        &os04a10_tuning_preview_hdr,
    },
    {
        "os05a20_tuning_preview.bin",
        &os05a20_tuning_preview,
    },
    {
        "os05a20_tuning_preview_hdr.bin",
        &os05a20_tuning_preview_hdr,
    },
    {
        "gc2093_tuning_preview.bin",
        &gc2093_tuning_preview,
    },
    {
        "gc2093_tuning_preview_hdr.bin",
        &gc2093_tuning_preview_hdr,
    },
    {
        "sc530ai_tuning_preview.bin",
        &sc530ai_tuning_preview,
    },
    {
        "infrared_gst417w_tuning_preview_thermal.bin",
        &infrared_gst417w_tuning_preview_thermal,
    },
    {
        "infrared_gst417w_tuning_preview_isp1.bin",
        &infrared_gst417w_tuning_preview_isp1,
    },
    {
        "infrared_gst417w_tuning_preview_isp2.bin",
        &infrared_gst417w_tuning_preview_isp2,
    },
    {
        "infrared_h3812c1sg_tuning_preview_thermal.bin",
        &infrared_h3812c1sg_tuning_preview_thermal,
    },
    {
        "infrared_h3812c1sg_tuning_preview_isp1.bin",
        &infrared_h3812c1sg_tuning_preview_isp1,
    },
    {
        "infrared_h3812c1sg_tuning_preview_isp2.bin",
        &infrared_h3812c1sg_tuning_preview_isp2,
    },
    {
        "infrared_rtd6122c_tuning_preview_thermal.bin",
        &infrared_rtd6122c_tuning_preview_thermal,
    },
    {
        "infrared_rtd6122c_tuning_preview_isp1.bin",
        &infrared_rtd6122c_tuning_preview_isp1,
    },
    {
        "infrared_rtd6122c_tuning_preview_isp2.bin",
        &infrared_rtd6122c_tuning_preview_isp2,
    },
    {
        "infrared_rtd3172c_tuning_preview_thermal.bin",
        &infrared_rtd3172c_tuning_preview_thermal,
    },
    {
        "infrared_rtd3172c_tuning_preview_isp1.bin",
        &infrared_rtd3172c_tuning_preview_isp1,
    },
    {
        "infrared_rtd3172c_tuning_preview_isp2.bin",
        &infrared_rtd3172c_tuning_preview_isp2,
    },
    {
        "infrared_gst412c_tuning_preview_thermal.bin",
        &infrared_gst412c_tuning_preview_thermal,
    },
    {
        "infrared_gst412c_tuning_preview_isp1.bin",
        &infrared_gst412c_tuning_preview_isp1,
    },
    {
        "infrared_gst412c_tuning_preview_isp2.bin",
        &infrared_gst412c_tuning_preview_isp2,
    },
        {
        "infrared_gst612c_tuning_preview_thermal.bin",
        &infrared_gst612c_tuning_preview_thermal,
    },
    {
        "infrared_gst612c_tuning_preview_isp1.bin",
        &infrared_gst612c_tuning_preview_isp1,
    },
    {
        "infrared_gst612c_tuning_preview_isp2.bin",
        &infrared_gst612c_tuning_preview_isp2,
    },
    {
        "sc2210_tuning_preview.bin",
        &sc2210_tuning_preview,
    },
    {
        "imx307_tuning_preview_fam30.bin",
        &imx307_tuning_preview_fam30,
    },
    {
        "sc132gs_tuning_preview_fam30.bin",
        &sc132gs_tuning_preview_fam30,
    },

};
#define PATH(x) x
#define XSTR(x) #x
#define PATH_STR(x) XSTR(x)

#ifdef CUSTOM_TUNING
#include PATH_STR(PATH(CUSTOM_TUNING))
#endif
int main(int argc,void *argv[])
{
  int i=0;
  int size=sizeof(dump)/sizeof(struct dump_tuning_file);
  int fd=-1;
  char name[128];
  for(i=0;i<size;i++){
     sprintf(name,"./bin/%s",dump[i].name);
     fd=open(name,O_CREAT|O_RDWR, 0644);
     if(fd>=0){
         write(fd,dump[i].p_tuning,sizeof(STRU_ALGO_ISP_TUNING_T));
         close(fd);
     }
  }

  int cus_size=sizeof(dump_custom)/sizeof(struct dump_tuning_file);
  for(i=0;i<cus_size;i++){
     sprintf(name,"./bin/%s",dump_custom[i].name);
	 printf("cusom tuning %s done!\n",name);
     fd=open(name,O_CREAT|O_RDWR|O_TRUNC, 0644);
     if(fd>=0){
         write(fd,dump_custom[i].p_tuning,sizeof(STRU_ALGO_ISP_TUNING_T));
         close(fd);
     }
  }
  return 0;
}


