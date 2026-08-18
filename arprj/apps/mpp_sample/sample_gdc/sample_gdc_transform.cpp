#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "hal_sys.h"
#include "mpi_gdc_api.h"
#include "mpi_sysctl.h"
MPI_SYSCTL_HANDLE g_phandle = NULL;


using namespace std;
using namespace cv;

struct usrpoint
{
   float x;
   float y;
};
static struct usrpoint usrcdr[18*31] = 
{
{   0.50,   0.25},{  64.50,   0.25},{ 127.75,   0.25},{ 192.00,   0.25},{ 255.00,   0.25},{ 319.25,   0.25},{ 382.50,   0.25},{ 446.75,   0.25},{ 509.75,   0.25},{ 574.00,   0.25},{ 637.25,   0.25},{ 701.50,   0.25},{ 764.50,   0.25},{ 828.75,   0.25},{ 891.75,   0.25},{ 955.00,   0.25},{1018.00,   0.25},{1082.25,   0.25},{1145.50,   0.25},{1209.75,   0.25},{1272.75,   0.25},{1337.00,   0.25},{1400.25,   0.25},{1464.50,   0.25},{1527.50,   0.25},{1591.75,   0.25},{1655.00,   0.25},{1719.00,   0.25},{1782.25,   0.25},{1846.50,   0.25},{1909.50,   0.25}, 
{ -11.50,  71.50},{  52.75,  71.50},{ 116.00,  71.50},{ 180.25,  71.50},{ 243.50,  71.50},{ 308.00,  71.50},{ 371.25,  71.25},{ 435.50,  71.25},{ 498.75,  71.25},{ 563.00,  71.25},{ 626.25,  71.25},{ 690.50,  71.25},{ 753.50,  71.25},{ 817.75,  71.25},{ 881.00,  71.25},{ 944.00,  71.25},{1007.00,  71.25},{1071.25,  71.25},{1134.25,  71.25},{1198.25,  71.00},{1261.25,  71.00},{1325.50,  71.00},{1388.50,  71.00},{1452.50,  71.00},{1515.50,  71.00},{1579.50,  71.00},{1642.25,  71.00},{1706.25,  71.00},{1769.25,  71.00},{1833.25,  71.00},{1896.00,  71.00}, 
{ -24.00, 142.25},{  40.50, 142.00},{ 104.00, 142.00},{ 168.50, 142.00},{ 232.00, 142.00},{ 296.50, 141.75},{ 359.75, 141.75},{ 424.25, 141.75},{ 487.50, 141.75},{ 552.00, 141.50},{ 615.00, 141.50},{ 679.50, 141.50},{ 742.50, 141.50},{ 806.75, 141.25},{ 870.00, 141.25},{ 933.00, 141.25},{ 996.00, 141.25},{1060.00, 141.25},{1123.00, 141.00},{1187.00, 141.00},{1250.00, 141.00},{1314.00, 141.00},{1376.75, 140.75},{1440.75, 140.75},{1503.50, 140.75},{1567.25, 140.75},{1630.00, 140.50},{1693.75, 140.50},{1756.50, 140.50},{1820.00, 140.50},{1882.75, 140.25}, 
{ -36.00, 214.00},{  28.50, 213.75},{  92.25, 213.75},{ 157.00, 213.50},{ 220.50, 213.50},{ 285.25, 213.25},{ 348.75, 213.25},{ 413.25, 213.25},{ 476.50, 213.00},{ 541.00, 213.00},{ 604.25, 212.75},{ 668.75, 212.75},{ 732.00, 212.50},{ 796.25, 212.50},{ 859.25, 212.25},{ 922.25, 212.25},{ 985.25, 212.00},{1049.50, 212.00},{1112.25, 211.75},{1176.25, 211.75},{1239.00, 211.50},{1303.00, 211.50},{1365.50, 211.50},{1429.50, 211.25},{1492.00, 211.25},{1555.75, 211.00},{1618.25, 211.00},{1681.75, 210.75},{1744.25, 210.75},{1807.50, 210.50},{1870.00, 210.50}, 
{ -48.50, 284.75},{  16.25, 284.50},{  80.25, 284.50},{ 145.25, 284.25},{ 209.00, 284.00},{ 273.75, 283.75},{ 337.50, 283.75},{ 402.00, 283.50},{ 465.50, 283.25},{ 530.25, 283.00},{ 593.50, 283.00},{ 658.00, 282.75},{ 721.25, 282.50},{ 785.50, 282.25},{ 848.50, 282.25},{ 911.75, 282.00},{ 974.75, 281.75},{1038.75, 281.50},{1101.50, 281.50},{1165.50, 281.25},{1228.25, 281.00},{1292.00, 280.75},{1354.50, 280.75},{1418.25, 280.50},{1480.75, 280.25},{1544.25, 280.00},{1606.50, 280.00},{1669.75, 279.75},{1732.00, 279.50},{1795.25, 279.25},{1857.25, 279.25}, 
{ -59.00, 355.75},{   6.00, 355.50},{  70.25, 355.25},{ 135.25, 355.00},{ 199.25, 354.75},{ 264.25, 354.50},{ 328.00, 354.25},{ 393.00, 354.00},{ 456.50, 353.50},{ 521.25, 353.25},{ 584.75, 353.00},{ 649.00, 352.75},{ 712.50, 352.50},{ 776.75, 352.25},{ 840.00, 352.00},{ 903.00, 351.75},{ 966.00, 351.50},{1030.00, 351.25},{1092.75, 351.00},{1156.75, 350.75},{1219.25, 350.50},{1283.00, 350.25},{1345.50, 349.75},{1409.00, 349.50},{1471.50, 349.25},{1534.75, 349.00},{1597.00, 348.75},{1660.25, 348.50},{1722.25, 348.25},{1785.25, 348.00},{1847.25, 347.75}, 
{ -68.00, 424.75},{  -2.50, 424.50},{  61.50, 424.00},{ 126.75, 423.75},{ 191.00, 423.25},{ 256.00, 423.00},{ 320.00, 422.75},{ 385.00, 422.25},{ 448.75, 422.00},{ 513.50, 421.50},{ 577.00, 421.25},{ 641.50, 421.00},{ 705.00, 420.50},{ 769.25, 420.25},{ 832.50, 419.75},{ 895.50, 419.50},{ 958.50, 419.25},{1022.75, 418.75},{1085.50, 418.50},{1149.25, 418.00},{1212.00, 417.75},{1275.50, 417.50},{1338.00, 417.00},{1401.50, 416.75},{1463.75, 416.50},{1527.00, 416.00},{1589.25, 415.75},{1652.25, 415.25},{1714.25, 415.00},{1777.25, 414.75},{1839.00, 414.25}, 
{ -77.00, 495.25},{ -11.25, 494.75},{  53.00, 494.50},{ 118.50, 494.00},{ 182.75, 493.50},{ 248.25, 493.00},{ 312.25, 492.50},{ 377.25, 492.25},{ 441.25, 491.75},{ 506.00, 491.25},{ 569.75, 490.75},{ 634.25, 490.25},{ 697.75, 490.00},{ 762.25, 489.50},{ 825.50, 489.00},{ 888.50, 488.50},{ 951.50, 488.00},{1015.50, 487.75},{1078.50, 487.25},{1142.25, 486.75},{1205.00, 486.25},{1268.50, 485.75},{1331.00, 485.50},{1394.25, 485.00},{1456.50, 484.50},{1519.75, 484.00},{1581.75, 483.75},{1644.75, 483.25},{1706.50, 482.75},{1769.25, 482.25},{1831.00, 482.00}, 
{ -86.00, 565.00},{ -20.00, 564.50},{  44.25, 563.75},{ 110.00, 563.25},{ 174.50, 562.75},{ 240.00, 562.25},{ 304.25, 561.50},{ 369.50, 561.00},{ 433.50, 560.50},{ 498.50, 560.00},{ 562.25, 559.25},{ 627.00, 558.75},{ 690.50, 558.25},{ 755.00, 557.75},{ 818.25, 557.00},{ 881.50, 556.50},{ 944.50, 556.00},{1008.50, 555.50},{1071.25, 554.75},{1135.25, 554.25},{1197.75, 553.75},{1261.25, 553.25},{1323.75, 552.75},{1387.00, 552.00},{1449.25, 551.50},{1512.50, 551.00},{1574.25, 550.50},{1637.25, 550.00},{1699.00, 549.25},{1761.50, 548.75},{1823.00, 548.25}, 
{ -95.00, 634.75},{ -28.75, 634.00},{  35.75, 633.25},{ 101.75, 632.75},{ 166.50, 632.00},{ 232.00, 631.25},{ 296.50, 630.50},{ 361.75, 630.00},{ 426.00, 629.25},{ 491.00, 628.50},{ 555.00, 627.75},{ 619.75, 627.25},{ 683.50, 626.50},{ 748.00, 625.75},{ 811.25, 625.00},{ 874.50, 624.50},{ 937.75, 623.75},{1001.75, 623.00},{1064.75, 622.50},{1128.50, 621.75},{1191.00, 621.00},{1254.50, 620.50},{1317.00, 619.75},{1380.25, 619.00},{1442.50, 618.50},{1505.50, 617.75},{1567.25, 617.00},{1630.25, 616.50},{1691.75, 615.75},{1754.25, 615.00},{1815.75, 614.50}, 
{-102.50, 703.50},{ -36.00, 702.50},{  28.75, 701.75},{  95.00, 701.00},{ 159.75, 700.25},{ 225.50, 699.25},{ 290.00, 698.50},{ 355.50, 697.75},{ 419.75, 697.00},{ 485.00, 696.00},{ 549.00, 695.25},{ 614.00, 694.50},{ 677.75, 693.75},{ 742.50, 693.00},{ 805.75, 692.00},{ 869.00, 691.25},{ 932.25, 690.50},{ 996.25, 689.75},{1059.25, 689.00},{1123.00, 688.25},{1185.75, 687.25},{1249.25, 686.50},{1311.50, 685.75},{1375.00, 685.00},{1437.00, 684.25},{1500.00, 683.50},{1561.75, 682.75},{1624.50, 682.00},{1686.00, 681.00},{1748.50, 680.25},{1809.75, 679.50}, 
{-104.50, 769.25},{ -38.00, 768.25},{  27.00, 767.50},{  93.00, 766.50},{ 158.00, 765.75},{ 224.00, 764.75},{ 288.50, 764.00},{ 354.00, 763.00},{ 418.50, 762.25},{ 483.75, 761.25},{ 547.75, 760.50},{ 612.75, 759.50},{ 676.50, 758.75},{ 741.25, 757.75},{ 804.75, 757.00},{ 868.00, 756.00},{ 931.25, 755.25},{ 995.50, 754.25},{1058.25, 753.50},{1122.25, 752.75},{1184.75, 751.75},{1248.50, 751.00},{1310.75, 750.00},{1374.25, 749.25},{1436.25, 748.50},{1499.25, 747.50},{1561.00, 746.75},{1624.00, 745.75},{1685.50, 745.00},{1748.00, 744.25},{1809.25, 743.25}, 
{-106.50, 834.00},{ -40.00, 833.00},{  25.00, 832.00},{  91.25, 831.25},{ 156.25, 830.25},{ 222.25, 829.25},{ 287.00, 828.25},{ 352.50, 827.25},{ 417.00, 826.50},{ 482.25, 825.50},{ 546.50, 824.50},{ 611.50, 823.50},{ 675.25, 822.75},{ 740.00, 821.75},{ 803.50, 820.75},{ 867.00, 819.75},{ 930.25, 819.00},{ 994.50, 818.00},{1057.25, 817.00},{1121.25, 816.25},{1184.00, 815.25},{1247.50, 814.25},{1310.00, 813.50},{1373.25, 812.50},{1435.50, 811.50},{1498.50, 810.75},{1560.25, 809.75},{1623.25, 808.75},{1684.75, 808.00},{1747.25, 807.00},{1808.50, 806.25}, 
{-108.50, 900.00},{ -42.00, 899.00},{  23.25, 898.00},{  89.50, 897.00},{ 154.75, 896.00},{ 220.75, 895.00},{ 285.50, 894.00},{ 351.25, 893.00},{ 415.75, 891.75},{ 481.00, 890.75},{ 545.25, 889.75},{ 610.25, 888.75},{ 674.25, 887.75},{ 739.00, 886.75},{ 802.75, 885.75},{ 866.00, 884.75},{ 929.25, 883.75},{ 993.50, 882.75},{1056.50, 881.75},{1120.50, 880.75},{1183.25, 879.75},{1247.00, 878.75},{1309.25, 877.75},{1372.75, 876.75},{1434.75, 876.00},{1498.00, 875.00},{1559.75, 874.00},{1622.50, 873.00},{1684.25, 872.00},{1746.75, 871.00},{1808.00, 870.00}, 
{-110.50, 965.25},{ -43.75, 964.00},{  21.50, 963.00},{  87.75, 961.75},{ 153.00, 960.75},{ 219.00, 959.50},{ 284.00, 958.50},{ 349.75, 957.50},{ 414.25, 956.25},{ 479.75, 955.25},{ 544.00, 954.25},{ 609.25, 953.00},{ 673.00, 952.00},{ 738.00, 950.75},{ 801.50, 949.75},{ 865.00, 948.75},{ 928.50, 947.75},{ 992.75, 946.50},{1055.75, 945.50},{1119.75, 944.50},{1182.50, 943.25},{1246.25, 942.25},{1308.50, 941.25},{1372.00, 940.25},{1434.25, 939.00},{1497.25, 938.00},{1559.25, 937.00},{1622.00, 936.00},{1683.50, 935.00},{1746.00, 933.75},{1807.50, 932.75}, 
{-112.75,1031.25},{ -46.00,1030.25},{  19.50,1029.00},{  86.00,1027.75},{ 151.25,1026.50},{ 217.25,1025.50},{ 282.25,1024.25},{ 348.25,1023.00},{ 412.75,1021.75},{ 478.25,1020.75},{ 542.50,1019.50},{ 607.75,1018.25},{ 671.75,1017.25},{ 736.75,1016.00},{ 800.50,1014.75},{ 864.00,1013.75},{ 927.25,1012.50},{ 991.75,1011.50},{1054.75,1010.25},{1118.75,1009.00},{1181.50,1008.00},{1245.25,1006.75},{1307.75,1005.75},{1371.25,1004.50},{1433.25,1003.50},{1496.50,1002.25},{1558.50,1001.25},{1621.25,1000.00},{1682.75, 999.00},{1745.25, 997.75},{1806.75, 996.75}, 
{-108.00,1092.50},{ -41.25,1091.25},{  24.00,1090.25},{  90.50,1089.00},{ 155.75,1087.75},{ 221.75,1086.50},{ 286.75,1085.50},{ 352.50,1084.25},{ 417.25,1083.00},{ 482.75,1082.00},{ 547.00,1080.75},{ 612.25,1079.50},{ 676.25,1078.50},{ 741.25,1077.25},{ 804.75,1076.00},{ 868.25,1075.00},{ 931.75,1073.75},{ 996.00,1072.75},{1059.25,1071.50},{1123.25,1070.50},{1186.25,1069.25},{1250.00,1068.00},{1312.50,1067.00},{1376.00,1065.75},{1438.25,1064.75},{1501.50,1063.50},{1563.50,1062.50},{1626.25,1061.25},{1688.00,1060.25},{1750.75,1059.25},{1812.25,1058.00}, 
{-103.00,1154.75},{ -36.25,1153.75},{  29.00,1152.50},{  95.25,1151.25},{ 160.50,1150.25},{ 226.50,1149.00},{ 291.25,1147.75},{ 357.00,1146.75},{ 421.75,1145.50},{ 487.25,1144.50},{ 551.50,1143.25},{ 616.75,1142.00},{ 680.75,1141.00},{ 745.50,1139.75},{ 809.25,1138.75},{ 872.75,1137.50},{ 936.25,1136.50},{1000.75,1135.25},{1063.75,1134.25},{1128.00,1133.00},{1190.75,1131.75},{1254.75,1130.75},{1317.25,1129.75},{1380.75,1128.50},{1443.25,1127.50},{1506.50,1126.25},{1568.50,1125.25},{1631.50,1124.00},{1693.50,1123.00},{1756.25,1121.75},{1817.75,1120.75}}; 
                



#define GDC_LUT_MATRIX_ROW 65
#define GDC_LUT_MATRIX_COL 65
#define SAMPLE_GDC_STRIDE_ALIGNE_TO(size,num) ( (num)*( ( (size)+(num)-1)/(num) ) )

static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 i=0;
	for(i=0;i<s32Argc;i++)
	{
         if(!strcmp(ps8Name,ps8Argv[i]))
         {
            printf("Find Pra %s @ %d ",ps8Name,i);
            return i;
         }
	}
	return 0;
}

static AR_S32 SAMPLE_GDC_MMZ_Buffer_Request(AR_GDC_TRANSFORM_S *pstParam)
{
	AR_S32 s32Ret = -1;
	if(!pstParam)
	{
		printf("pstParam is NULL\r\n");
		return -1;
	}

    /*******************SRC Y******************************/
	int u32YSrcSize = pstParam->stInBuffer.format.luma_stride*pstParam->stInBuffer.format.height; //SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	AR_U64 u64YSrcPa = 0;
	void * pvYSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, "YSrc", NULL, u32YSrcSize);
	if(s32Ret)
	{
		printf(" get src y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYSrcVa, 0, u32YSrcSize);

	pstParam->stInBuffer.pannel[0].addr_virt= (AR_UINTPTR)pvYSrcVa;
	pstParam->stInBuffer.pannel[0].addr_phy= (AR_UINTPTR)u64YSrcPa;
        printf("in y %llx\r\n",pstParam->stInBuffer.pannel[0].addr_phy);
	/*******************SRC U******************************/
	//AR_UCHAR *pu8USrcVa = ar_ion_malloc(u32YSrcSize/2);
	//AR_UCHAR *pu8USrcPa = ar_ion_virt_to_phy(pu8USrcVa);
	int u32USrcSize = pstParam->stInBuffer.format.chroma_stride*pstParam->stInBuffer.format.height/2; //SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	AR_U64 u64USrcPa = 0;
	void * pvUSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, "USrc", NULL, u32USrcSize);
	if(s32Ret)
	{
		printf(" get src u addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvUSrcVa,0,u32USrcSize);
	pstParam->stInBuffer.pannel[1].addr_virt = (AR_UINTPTR)pvUSrcVa;
	pstParam->stInBuffer.pannel[1].addr_phy= (AR_UINTPTR)u64USrcPa;
       printf("in u %llx\r\n",pstParam->stInBuffer.pannel[1].addr_phy);

	/*******************SRC V******************************/
	//AR_UCHAR *pu8VSrcVa = ar_ion_malloc(u32YSrcSize/2);
	//AR_UCHAR *pu8VSrcPa = ar_ion_virt_to_phy(pu8VSrcVa);
	AR_U64 u64VSrcPa = 0;
	void * pvVSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, "VSrc", NULL, u32USrcSize);
	if(s32Ret)
	{
		printf(" get src v addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvVSrcVa,0,u32USrcSize);
	pstParam->stInBuffer.pannel[2].addr_virt = (AR_UINTPTR)pvVSrcVa;
	pstParam->stInBuffer.pannel[2].addr_phy= (AR_UINTPTR)u64VSrcPa;
        printf("in v %llx\r\n",pstParam->stInBuffer.pannel[2].addr_phy);

	/*******************DST Y******************************/
	//AR_U32 u32YDstSize = SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	int halg = SAMPLE_GDC_STRIDE_ALIGNE_TO(pstParam->stOutBuffer.format.height,64);
	int u32YDstSize = pstParam->stOutBuffer.format.luma_stride*halg;
	//AR_UCHAR *pu8YDstVa = ar_ion_malloc(u32YDstSize);
	//AR_UCHAR *pu8YDstPa = ar_ion_virt_to_phy(pu8YDstVa);
	AR_U64 u64YDstPa = 0;
	void * pvYDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YDstPa, &pvYDstVa, "YDst", NULL, u32YDstSize);
	if(s32Ret)
	{
		printf(" get dst y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYDstVa,0,u32YDstSize);
	pstParam->stOutBuffer.pannel[0].addr_virt = (AR_UINTPTR)pvYDstVa;
	pstParam->stOutBuffer.pannel[0].addr_phy= (AR_UINTPTR)u64YDstPa;
printf("out y %llx\r\n",pstParam->stOutBuffer.pannel[0].addr_phy);

	/*******************DST U******************************/
	//AR_UCHAR *pu8UDstVa = ar_ion_malloc(u32YDstSize/2);
	//AR_UCHAR *pu8UDstPa = ar_ion_virt_to_phy(pu8UDstVa);
	int huvalg = SAMPLE_GDC_STRIDE_ALIGNE_TO(pstParam->stOutBuffer.format.height/2,32);
	int u32UDstSize = pstParam->stOutBuffer.format.chroma_stride*huvalg;
	AR_U64 u64UDstPa = 0;
	void * pvUDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64UDstPa, &pvUDstVa, "UDst", NULL, u32UDstSize);
	if(s32Ret)
	{
		printf(" get dst u addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvUDstVa,0,u32UDstSize);
	pstParam->stOutBuffer.pannel[1].addr_virt = (AR_UINTPTR)pvUDstVa;
	pstParam->stOutBuffer.pannel[1].addr_phy= (AR_UINTPTR)u64UDstPa;
printf("out u %llx\r\n",pstParam->stOutBuffer.pannel[1].addr_phy);

	/*******************DST V******************************/
	//AR_UCHAR *pu8VDstVa = ar_ion_malloc(u32YDstSize/2);
	//AR_UCHAR *pu8VDstPa = ar_ion_virt_to_phy(pu8VDstVa);
	AR_U64 u64VDstPa = 0;
	void * pvVDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64VDstPa, &pvVDstVa, "VDst", NULL, u32UDstSize);
	if(s32Ret)
	{
		printf(" get dst_v addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvVDstVa,0,u32UDstSize);
	pstParam->stOutBuffer.pannel[2].addr_virt = (AR_UINTPTR)pvVDstVa;
	pstParam->stOutBuffer.pannel[2].addr_phy= (AR_UINTPTR)u64VDstPa;
printf("out v %llx\r\n",pstParam->stOutBuffer.pannel[2].addr_phy);

        printf("IN Y/U/V Addr %llx %llx %llx \r\n",pstParam->stInBuffer.pannel[0].addr_phy,pstParam->stInBuffer.pannel[1].addr_phy,pstParam->stInBuffer.pannel[2].addr_phy);
	printf("OUT Y/U/V Addr %llx %llx %llx \r\n",pstParam->stOutBuffer.pannel[0].addr_phy,pstParam->stOutBuffer.pannel[1].addr_phy,pstParam->stOutBuffer.pannel[2].addr_phy);

	return 0;

}

static AR_S32 SAMPLE_GDC_MMZ_Buffer_Release(AR_GDC_TRANSFORM_S *pstParam)
{
    if(!pstParam)
    {
    	printf("pstParam is NULL\r\n");
		return -1;
    }

	if(pstParam->stInBuffer.pannel[0].addr_virt)
	   //ar_ion_free(pstParam->stInBuffer.pannel[0].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstParam->stInBuffer.pannel[0].addr_phy, (AR_VOID *)pstParam->stInBuffer.pannel[0].addr_virt);

	if(pstParam->stInBuffer.pannel[1].addr_virt)
	   //ar_ion_free(pstParam->stInBuffer.pannel[1].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstParam->stInBuffer.pannel[1].addr_phy, (AR_VOID *)pstParam->stInBuffer.pannel[1].addr_virt);

	if(pstParam->stInBuffer.pannel[2].addr_virt)
   	   //ar_ion_free(pstParam->stInBuffer.pannel[2].addr_virt);
   	   ar_hal_sys_mmz_free((AR_U64)pstParam->stInBuffer.pannel[2].addr_phy, (AR_VOID *)pstParam->stInBuffer.pannel[2].addr_virt);

	if(pstParam->stOutBuffer.pannel[0].addr_virt)
	   //ar_ion_free(pstParam->stOutBuffer.pannel[0].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstParam->stOutBuffer.pannel[0].addr_phy, (AR_VOID *)pstParam->stOutBuffer.pannel[0].addr_virt);

	if(pstParam->stOutBuffer.pannel[1].addr_virt)
	   //ar_ion_free(pstParam->stOutBuffer.pannel[1].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstParam->stOutBuffer.pannel[1].addr_phy, (AR_VOID *)pstParam->stOutBuffer.pannel[1].addr_virt);

	if(pstParam->stOutBuffer.pannel[2].addr_virt)
   	   //ar_ion_free(pstParam->stOutBuffer.pannel[2].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstParam->stOutBuffer.pannel[2].addr_phy, (AR_VOID *)pstParam->stOutBuffer.pannel[2].addr_virt);


	return 0;
}



static AR_S32 SAMPLE_GDC_Load_Src_Img(AR_GDC_TRANSFORM_S *pstParam, AR_CHAR*s8SrcFileName)
{
    if((!pstParam) || (!s8SrcFileName))
    {
    	printf("pstParam || s8SrcFileName is NULL\r\n");
		return -1;
    }

	 FILE* pSrcFileFp =0;
	 pSrcFileFp = fopen(s8SrcFileName, "r");
	 if(!pSrcFileFp)
	 {
		printf("open file fail!!\n");
		return -1;
	 }

	 AR_S32 s32SrcHeight = pstParam->stInBuffer.format.height;
	 AR_S32 s32SrcLumaStride = pstParam->stInBuffer.format.luma_stride;
	 AR_S32 s32SrcChromaStride = pstParam->stInBuffer.format.chroma_stride;

	 AR_UCHAR *pu8YSrcVa = (AR_UCHAR *)pstParam->stInBuffer.pannel[0].addr_virt;
	 AR_UCHAR *pu8USrcVa = (AR_UCHAR *)pstParam->stInBuffer.pannel[1].addr_virt;
	 AR_UCHAR *pu8VSrcVa = (AR_UCHAR *)pstParam->stInBuffer.pannel[2].addr_virt;


    AR_S32 s32Ret =-1;
	AR_S32 s32DataLenSrc=0;
	AR_S32 s32PlanarSrc=0;

	for(s32PlanarSrc = 0; s32PlanarSrc < 3; s32PlanarSrc++)
	{
	   if(s32PlanarSrc == 0)
	   {
			s32DataLenSrc = s32SrcLumaStride*s32SrcHeight;
			s32Ret = fread(pu8YSrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src y = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);
	   }
	   else if(s32PlanarSrc == 1)
	   {
			s32DataLenSrc = s32SrcChromaStride*s32SrcHeight/2;
			s32Ret = fread(pu8USrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src u = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);

	   }
	   else
	   {
			s32DataLenSrc = s32SrcChromaStride*s32SrcHeight/2;
			s32Ret = fread(pu8VSrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src v = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);

	   }
    }

	fclose(pSrcFileFp);
    pSrcFileFp=0;

	return 0;

}



static AR_S32 SAMPLE_GDC_Store_Dst_Img(AR_GDC_TRANSFORM_S *pstParam, AR_CHAR*s8DstFileName)
{
    if((!pstParam) || (!s8DstFileName))
    {
    	printf("pstParam || s8DstFileName is NULL\r\n");
		return -1;
    }

	FILE* pDstFileFp =0;
	pDstFileFp = fopen(s8DstFileName, "w");
	if(!pDstFileFp)
	{
		printf("open file fail!!\n");
		return -1;
	}


	AR_S32 s32DstHeight = pstParam->stOutBuffer.format.height;
	AR_S32 s32DstLumaStride = pstParam->stOutBuffer.format.luma_stride;
	AR_S32 s32DstChromaStride = pstParam->stOutBuffer.format.chroma_stride;

	AR_UCHAR *pu8YDstVa = (AR_UCHAR *)pstParam->stOutBuffer.pannel[0].addr_virt;
	AR_UCHAR *pu8UDstVa = (AR_UCHAR *)pstParam->stOutBuffer.pannel[1].addr_virt;
	AR_UCHAR *pu8VDstVa = (AR_UCHAR *)pstParam->stOutBuffer.pannel[2].addr_virt;

	AR_S32 s32Ret =-1;
	AR_S32 s32DataLenDst=0;
	AR_S32 s32PlanarDst=0;
	for(s32PlanarDst = 0; s32PlanarDst < 3; s32PlanarDst++)
	{
	   if(s32PlanarDst == 0)
	   {
			s32DataLenDst = s32DstLumaStride*s32DstHeight;
			s32Ret = fwrite(pu8YDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
			//printf("write dst y = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }
	   else if(s32PlanarDst == 1)
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8UDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
			//printf("write dst u = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }
	   else
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8VDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
				printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
				return -1;
			}
			//printf("write dst v = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }

    }

	fclose(pDstFileFp);
    pDstFileFp=0;
	//printf("dst file store finished\r\n");

	return 0;

}


static AR_S32 SAMPLE_GDC_MMZ_Buffer_Request_General(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut)
{
	AR_S32 s32Ret = -1;
	if((!pstImgIn)||(!pstImgOut))
	{
		printf("pstImgIn/pstImgOut is NULL\r\n");
		return -1;
	}

    /*******************SRC Y******************************/
	int u32YSrcSize = pstImgIn->astChannels[0].u32Stride*pstImgIn->u32Height; //SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	AR_U64 u64YSrcPa = 0;
	void * pvYSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, "YSrc", NULL, u32YSrcSize);
	if(s32Ret)
	{
		printf(" get src y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYSrcVa, 0, u32YSrcSize);

	pstImgIn->astChannels[0].uptrAddrVirt= (AR_UINTPTR)pvYSrcVa;
	pstImgIn->astChannels[0].u32AddrPhy= (AR_U32)(u64YSrcPa & 0xffffffff);
        printf("in y %llx\r\n",pstImgIn->astChannels[0].u32AddrPhy);
	/*******************SRC U******************************/
	//AR_UCHAR *pu8USrcVa = ar_ion_malloc(u32YSrcSize/2);
	//AR_UCHAR *pu8USrcPa = ar_ion_virt_to_phy(pu8USrcVa);
	int u32USrcSize = pstImgIn->astChannels[1].u32Stride*pstImgIn->u32Height/2; //SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	AR_U64 u64USrcPa = 0;
	void * pvUSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, "USrc", NULL, u32USrcSize);
	if(s32Ret)
	{
		printf(" get src u addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvUSrcVa,0,u32USrcSize);
	pstImgIn->astChannels[1].uptrAddrVirt= (AR_UINTPTR)pvUSrcVa;
	pstImgIn->astChannels[1].u32AddrPhy= (AR_U32)(u64USrcPa & 0xffffffff);
       printf("in u %llx\r\n",pstImgIn->astChannels[1].u32AddrPhy);

	/*******************SRC V******************************/
	//AR_UCHAR *pu8VSrcVa = ar_ion_malloc(u32YSrcSize/2);
	//AR_UCHAR *pu8VSrcPa = ar_ion_virt_to_phy(pu8VSrcVa);
	AR_U64 u64VSrcPa = 0;
	void * pvVSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, "VSrc", NULL, u32USrcSize);
	if(s32Ret)
	{
		printf(" get src v addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvVSrcVa,0,u32USrcSize);
	pstImgIn->astChannels[2].uptrAddrVirt= (AR_UINTPTR)pvVSrcVa;
	pstImgIn->astChannels[2].u32AddrPhy= (AR_U32)(u64VSrcPa & 0xffffffff);
        printf("in v %llx\r\n",pstImgIn->astChannels[2].u32AddrPhy);

	/*******************DST Y******************************/
	//AR_U32 u32YDstSize = SAMPLE_GDC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	int halg = SAMPLE_GDC_STRIDE_ALIGNE_TO(pstImgOut->u32Height,64);
	int u32YDstSize = pstImgOut->astChannels[0].u32Stride*halg;
	//AR_UCHAR *pu8YDstVa = ar_ion_malloc(u32YDstSize);
	//AR_UCHAR *pu8YDstPa = ar_ion_virt_to_phy(pu8YDstVa);
	AR_U64 u64YDstPa = 0;
	void * pvYDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YDstPa, &pvYDstVa, "YDst", NULL, u32YDstSize);
	if(s32Ret)
	{
		printf(" get dst y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYDstVa,0,u32YDstSize);
	pstImgOut->astChannels[0].uptrAddrVirt= (AR_UINTPTR)pvYDstVa;
	pstImgOut->astChannels[0].u32AddrPhy= (AR_U32)(u64YDstPa & 0xffffffff);
printf("out y %llx\r\n",pstImgOut->astChannels[0].u32AddrPhy);

	/*******************DST U******************************/
	//AR_UCHAR *pu8UDstVa = ar_ion_malloc(u32YDstSize/2);
	//AR_UCHAR *pu8UDstPa = ar_ion_virt_to_phy(pu8UDstVa);
	int huvalg = SAMPLE_GDC_STRIDE_ALIGNE_TO(pstImgOut->u32Height/2,32);
	int u32UDstSize = pstImgOut->astChannels[1].u32Stride*huvalg;
	AR_U64 u64UDstPa = 0;
	void * pvUDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64UDstPa, &pvUDstVa, "UDst", NULL, u32UDstSize);
	if(s32Ret)
	{
		printf(" get dst u addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvUDstVa,0,u32UDstSize);
	pstImgOut->astChannels[1].uptrAddrVirt= (AR_UINTPTR)pvUDstVa;
	pstImgOut->astChannels[1].u32AddrPhy= (AR_U32)(u64UDstPa & 0xffffffff);
printf("out u %llx\r\n",pstImgOut->astChannels[1].u32AddrPhy);

	/*******************DST V******************************/
	//AR_UCHAR *pu8VDstVa = ar_ion_malloc(u32YDstSize/2);
	//AR_UCHAR *pu8VDstPa = ar_ion_virt_to_phy(pu8VDstVa);
	AR_U64 u64VDstPa = 0;
	void * pvVDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64VDstPa, &pvVDstVa, "VDst", NULL, u32UDstSize);
	if(s32Ret)
	{
		printf(" get dst_v addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvVDstVa,0,u32UDstSize);
	pstImgOut->astChannels[2].uptrAddrVirt= (AR_UINTPTR)pvVDstVa;
	pstImgOut->astChannels[2].u32AddrPhy= (AR_U32)(u64VDstPa & 0xffffffff);
printf("out v %llx\r\n",pstImgOut->astChannels[2].u32AddrPhy);

        printf("IN Y/U/V Addr %llx %llx %llx \r\n",pstImgIn->astChannels[0].u32AddrPhy,pstImgIn->astChannels[1].u32AddrPhy,pstImgIn->astChannels[2].u32AddrPhy);
	printf("OUT Y/U/V Addr %llx %llx %llx \r\n",pstImgOut->astChannels[0].u32AddrPhy,pstImgOut->astChannels[1].u32AddrPhy,pstImgOut->astChannels[2].u32AddrPhy);

	return 0;

}

static AR_S32 SAMPLE_GDC_MMZ_Buffer_Release_General(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut)
{
    if((!pstImgIn)||(!pstImgOut))
	{
		printf("pstImgIn/pstImgOut is NULL\r\n");
		return -1;
	}

	if(pstImgIn->astChannels[0].uptrAddrVirt)
	   //ar_ion_free(pstParam->stInBuffer.pannel[0].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstImgIn->astChannels[0].u32AddrPhy, (AR_VOID *)pstImgIn->astChannels[0].uptrAddrVirt);

	if(pstImgIn->astChannels[1].uptrAddrVirt)
	   //ar_ion_free(pstParam->stInBuffer.pannel[1].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstImgIn->astChannels[1].u32AddrPhy, (AR_VOID *)pstImgIn->astChannels[1].uptrAddrVirt);

	if(pstImgIn->astChannels[2].uptrAddrVirt)
   	   //ar_ion_free(pstParam->stInBuffer.pannel[2].addr_virt);
   	   ar_hal_sys_mmz_free((AR_U64)pstImgIn->astChannels[2].u32AddrPhy, (AR_VOID *)pstImgIn->astChannels[2].uptrAddrVirt);

	if(pstImgOut->astChannels[0].uptrAddrVirt)
	   //ar_ion_free(pstParam->stOutBuffer.pannel[0].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstImgOut->astChannels[0].u32AddrPhy, (AR_VOID *)pstImgOut->astChannels[0].uptrAddrVirt);

	if(pstImgOut->astChannels[1].uptrAddrVirt)
	   //ar_ion_free(pstParam->stOutBuffer.pannel[1].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstImgOut->astChannels[1].u32AddrPhy, (AR_VOID *)pstImgOut->astChannels[1].uptrAddrVirt);

	if(pstImgOut->astChannels[2].uptrAddrVirt)
   	   //ar_ion_free(pstParam->stOutBuffer.pannel[2].addr_virt);
	   ar_hal_sys_mmz_free((AR_U64)pstImgOut->astChannels[2].u32AddrPhy, (AR_VOID *)pstImgOut->astChannels[2].uptrAddrVirt);


	return 0;
}



static AR_S32 SAMPLE_GDC_Load_Src_Img_General(AR_IMG_S *pstImgIn, AR_CHAR*s8SrcFileName)
{
    if((!pstImgIn)|| (!s8SrcFileName))
    {
    	printf("pstImgIn || s8SrcFileName is NULL\r\n");
		return -1;
    }

	 FILE* pSrcFileFp =0;
	 pSrcFileFp = fopen(s8SrcFileName, "r");
	 if(!pSrcFileFp)
	 {
		printf("open file fail!!\n");
		return -1;
	 }

	 AR_S32 s32SrcHeight = pstImgIn->u32Height;
	 AR_S32 s32SrcLumaStride = pstImgIn->astChannels[0].u32Stride;
	 AR_S32 s32SrcChromaStride = pstImgIn->astChannels[1].u32Stride;

	 AR_UCHAR *pu8YSrcVa = (AR_UCHAR *)pstImgIn->astChannels[0].uptrAddrVirt;
	 AR_UCHAR *pu8USrcVa = (AR_UCHAR *)pstImgIn->astChannels[1].uptrAddrVirt;
	 AR_UCHAR *pu8VSrcVa = (AR_UCHAR *)pstImgIn->astChannels[2].uptrAddrVirt;


    AR_S32 s32Ret =-1;
	AR_S32 s32DataLenSrc=0;
	AR_S32 s32PlanarSrc=0;

	for(s32PlanarSrc = 0; s32PlanarSrc < 3; s32PlanarSrc++)
	{
	   if(s32PlanarSrc == 0)
	   {
			s32DataLenSrc = s32SrcLumaStride*s32SrcHeight;
			s32Ret = fread(pu8YSrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src y = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);
	   }
	   else if(s32PlanarSrc == 1)
	   {
			s32DataLenSrc = s32SrcChromaStride*s32SrcHeight/2;
			s32Ret = fread(pu8USrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src u = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);

	   }
	   else
	   {
			s32DataLenSrc = s32SrcChromaStride*s32SrcHeight/2;
			s32Ret = fread(pu8VSrcVa,1,s32DataLenSrc,pSrcFileFp);
			printf("LOAD planar:%d data_len:%d  ret_len:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret);
			if(s32Ret != s32DataLenSrc)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarSrc, s32DataLenSrc, s32Ret, __LINE__);
			   return -1;
			}
			//printf("read src v = %d datalen =%d\r\n",s32Ret,s32DataLenSrc);

	   }
    }

	fclose(pSrcFileFp);
    pSrcFileFp=0;

	return 0;

}



static AR_S32 SAMPLE_GDC_Store_Dst_Img_General(AR_IMG_S *pstImgOut, AR_CHAR*s8DstFileName)
{
    if((!pstImgOut) || (!s8DstFileName))
    {
    	printf("pstImgOut || s8DstFileName is NULL\r\n");
		return -1;
    }

	FILE* pDstFileFp =0;
	pDstFileFp = fopen(s8DstFileName, "w");
	if(!pDstFileFp)
	{
		printf("open file fail!!\n");
		return -1;
	}


	AR_S32 s32DstHeight = pstImgOut->u32Height;
	AR_S32 s32DstLumaStride = pstImgOut->astChannels[0].u32Stride;
	AR_S32 s32DstChromaStride = pstImgOut->astChannels[1].u32Stride;

	AR_UCHAR *pu8YDstVa = (AR_UCHAR *)pstImgOut->astChannels[0].uptrAddrVirt;
	AR_UCHAR *pu8UDstVa = (AR_UCHAR *)pstImgOut->astChannels[1].uptrAddrVirt;
	AR_UCHAR *pu8VDstVa = (AR_UCHAR *)pstImgOut->astChannels[2].uptrAddrVirt;

	AR_S32 s32Ret =-1;
	AR_S32 s32DataLenDst=0;
	AR_S32 s32PlanarDst=0;
	for(s32PlanarDst = 0; s32PlanarDst < 3; s32PlanarDst++)
	{
	   if(s32PlanarDst == 0)
	   {
			s32DataLenDst = s32DstLumaStride*s32DstHeight;
			s32Ret = fwrite(pu8YDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
			//printf("write dst y = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }
	   else if(s32PlanarDst == 1)
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8UDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
			//printf("write dst u = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }
	   else
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8VDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
				printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
				return -1;
			}
			//printf("write dst v = %d datalen =%d\r\n",s32Ret,s32DataLenDst);
	   }

    }

	fclose(pDstFileFp);
    pDstFileFp=0;
	//printf("dst file store finished\r\n");

	return 0;

}

static AR_S32 Sample_GDC_GenerateLut(AR_S32 s32OutW,AR_S32 s32OutH,AR_GDC_USER_ATTR_S *pstUsrAttr,Mat & Minv)
{
	if(!pstUsrAttr)
	{
	   printf("pstUsrAttr is NULL \r\n");
	   return -1;
	}

	AR_S32 s32Xaxis[GDC_LUT_MATRIX_ROW][GDC_LUT_MATRIX_COL] ={0};
	AR_S32 s32Yaxis[GDC_LUT_MATRIX_ROW][GDC_LUT_MATRIX_COL] ={0};
	
	AR_S32 s32BlkHnum = (s32OutW + 63) / 64;
	AR_S32 s32BlkVnum = (s32OutH + 63) / 64;
	AR_S32 s32AxisHnum = s32BlkHnum + 1;
	AR_S32 s32AxisVnum = s32BlkVnum + 1;
	
	AR_DOUBLE Minv00 = (AR_DOUBLE)Minv.at<double>(0,0);
	AR_DOUBLE Minv01 = (AR_DOUBLE)Minv.at<double>(0,1);
	AR_DOUBLE Minv02 = (AR_DOUBLE)Minv.at<double>(0,2);
	AR_DOUBLE Minv10 = (AR_DOUBLE)Minv.at<double>(1,0);
	AR_DOUBLE Minv11 = (AR_DOUBLE)Minv.at<double>(1,1);
	AR_DOUBLE Minv12 = (AR_DOUBLE)Minv.at<double>(1,2);

	for (int y = 0; y < s32AxisVnum; y++){
		for (int x = 0; x < s32AxisHnum; x++){
		   int srcX = 64*x;
		   int srcY = 64*y;
		   s32Xaxis[y][x] = ((AR_S32)(Minv00*srcX + Minv01*srcY + Minv02))<<2;
		   s32Yaxis[y][x] = ((AR_S32)(Minv10*srcX + Minv11*srcY + Minv12))<<2;
		}
	}


	for (int y = 0; y < s32AxisVnum-1; y++){
		for (int x = 0; x < s32AxisHnum-1; x++){
		   if( (abs(s32Xaxis[y][x]-s32Xaxis[y][x+1]) > 96*4)||
		       (abs(s32Xaxis[y][x]-s32Xaxis[y+1][x]) > 96*4)||
		       (abs(s32Xaxis[y][x]-s32Xaxis[y+1][x+1])>96*4) )
		   {
				printf("Adjacent Xaxis > 96 \r\n");
				printf("x/y[%d/%d] \r\n",x,y);
				printf("[%d][%d][%d][%d] \r\n",s32Xaxis[y][x],s32Xaxis[y][x+1],s32Xaxis[y+1][x],s32Xaxis[y+1][x+1]);
			    return -1;
		   }

		   if( (abs(s32Yaxis[y][x]-s32Yaxis[y][x+1]) > 96*4)||
		       (abs(s32Yaxis[y][x]-s32Yaxis[y+1][x]) > 96*4)||
		       (abs(s32Yaxis[y][x]-s32Yaxis[y+1][x+1])>96*4) )
		   {
				printf("Adjacent Yaxis > 96 \r\n");
				printf("x/y[%d/%d] \r\n",x,y);
				printf("[%d][%d][%d][%d] \r\n",s32Yaxis[y][x],s32Yaxis[y][x+1],s32Yaxis[y+1][x],s32Yaxis[y+1][x+1]);
				return -1;
		   }
		}
	}

	AR_S32 s32AxisStride = ((s32AxisHnum + 3) / 4)*4;
	pstUsrAttr->u32LutLen = s32AxisStride*s32AxisVnum*sizeof(AR_U32);
	pstUsrAttr->u64LutVirtAddr = (AR_U64)malloc(pstUsrAttr->u32LutLen);
	memset((void*)pstUsrAttr->u64LutVirtAddr, 0 ,pstUsrAttr->u32LutLen);
	AR_U32 *pu32Lut = (AR_U32 *)pstUsrAttr->u64LutVirtAddr;

	for (int i = 0; i < s32AxisVnum; i++){
		for (int j = 0; j < s32AxisStride; j++){
		   int k=i*s32AxisStride+j;
		   if(j<s32AxisHnum)
		   {
			 pu32Lut[k]=(s32Xaxis[i][j] & 0xffff) + ((s32Yaxis[i][j] & 0xffff) << 16);
		   }
		}
	}
	return 0;
}

void Inbuffer_Padding_Inside(AR_GDC_TRANSFORM_S *pstParam, AR_S32 PadLine)
{
        struct timeval tm;
        gettimeofday(&tm, NULL);
        AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;
		
		AR_UCHAR *pu8YSrcVaH = (AR_UCHAR *)pstParam->stInBuffer.pannel[0].addr_virt;
		AR_UCHAR *pu8USrcVaH = (AR_UCHAR *)pstParam->stInBuffer.pannel[1].addr_virt;
		AR_UCHAR *pu8VSrcVaH = (AR_UCHAR *)pstParam->stInBuffer.pannel[2].addr_virt;

		AR_S32 InYStride = pstParam->stInBuffer.format.luma_stride;
		AR_S32 InUVStride = pstParam->stInBuffer.format.chroma_stride;
		AR_S32 InYW = pstParam->stInBuffer.format.width;
		AR_S32 InYH = pstParam->stInBuffer.format.height;

		memset(pu8YSrcVaH,0 ,InYStride*PadLine);
		pu8YSrcVaH += InYStride*(InYH-PadLine);
		memset(pu8YSrcVaH,0 ,InYStride*PadLine);
		
		memset(pu8USrcVaH,0 ,InUVStride*PadLine/2);
		pu8USrcVaH += InUVStride*(InYH/2-PadLine/2);
		memset(pu8USrcVaH,0 ,InUVStride*PadLine/2);
		
		memset(pu8VSrcVaH,0 ,InUVStride*PadLine/2);
		pu8VSrcVaH += InUVStride*(InYH/2-PadLine/2);
		memset(pu8VSrcVaH,0 ,InUVStride*PadLine/2);


		AR_UCHAR *pu8YSrcVaV = (AR_UCHAR *)pstParam->stInBuffer.pannel[0].addr_virt;
		AR_UCHAR *pu8USrcVaV = (AR_UCHAR *)pstParam->stInBuffer.pannel[1].addr_virt;
		AR_UCHAR *pu8VSrcVaV = (AR_UCHAR *)pstParam->stInBuffer.pannel[2].addr_virt;

        for(int i=0;i<InYH;i++)
        { 
        	 pu8YSrcVaV += InYStride; 
			 for(int j=0;j<PadLine;j++)
			 {
			 	*(AR_UCHAR*)(pu8YSrcVaV+j) = 0;
			 	*(AR_UCHAR*)(pu8YSrcVaV + (InYW-1)-j) = 0;
			 }
		}

		for(int i=0;i<InYH/2;i++)
        {   
        	 pu8USrcVaV += InUVStride;
			 pu8VSrcVaV += InUVStride; 
			 for(int j=0;j<PadLine/2;j++)
			 {
			 	*(AR_UCHAR*)(pu8USrcVaV+j) = 0;
			 	*(AR_UCHAR*)(pu8USrcVaV + (InYW/2-1)-j) = 0;
				*(AR_UCHAR*)(pu8VSrcVaV+j) = 0;
			 	*(AR_UCHAR*)(pu8VSrcVaV + (InYW/2-1)-j) = 0;
			 }					
		}

		gettimeofday(&tm, NULL);
		AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;

		AR_FLOAT dTime = (u64Toc - u64Tic)*1.0/1000;
        printf("%s, line %d takes %f ms\n", __FUNCTION__, __LINE__, dTime);

}

void TEST_GDC_USER_MODE_ROTX_Matrix(AR_FLOAT dAngle,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_ROT [%f] Matrix start\r\n",dAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);
	
	AR_S32	PadLine=2; 


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;
	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;


    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
		return;
	}

	AR_GDC_USER_ATTR_S usrAttr={0};
	AR_S32 s32OutWidth = stParam.stOutBuffer.format.width;
	AR_S32 s32OutHeight = stParam.stOutBuffer.format.height;

	cv::Point2f center(s32SrcWidth/2, s32SrcHeight/2);
	Mat M = cv::getRotationMatrix2D(center,dAngle,1.0);

	Mat Mq(3,3,CV_64FC1);
    for(int m =0 ;m <2; m++){
		for(int n =0 ;n<3; n++){
			Mq.at<double>(m,n) = M.at<double>(m,n);
		}
	}
	Mq.at<double>(2,0) = 0.0;
	Mq.at<double>(2,1) = 0.0;
	Mq.at<double>(2,2) = 1.0;
	cout<<Mq <<endl;

	Mat Minv = Mq.inv();
	cout<<Minv <<endl;

	s32Ret = Sample_GDC_GenerateLut(s32OutWidth,s32OutHeight,&usrAttr,Minv);
	if(s32Ret < 0){
		printf("Sample_GDC_GenerateLut fail\r\n");
		goto exit_handler;
	}
	stParam.stParams[0].enType = AR_GDC_USER_DEF_E;
	stParam.stParams[0].pAttr = &usrAttr;

	s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

	Inbuffer_Padding_Inside(&stParam,PadLine);
	stParam.stOutBuffer.format.width -= PadLine;
	stParam.stOutBuffer.format.height -= PadLine;

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}
	stParam.stOutBuffer.format.width += PadLine;
	stParam.stOutBuffer.format.height += PadLine;

	sprintf(s8DstFileName, "./USRMODE_ROT%f_Matrix_Img%sSize%dx%dIS%dOS%d.yuv",dAngle,s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);


exit_handler:

    if(usrAttr.u64LutVirtAddr){
		free((void*)usrAttr.u64LutVirtAddr);
    }

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_USR_MODE_ROT [%f] end\r\n",dAngle);
}

void TEST_GDC_USER_MODE_ROTX_Matrix_Without_Padding(AR_FLOAT dAngle,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_ROT [%f] Matrix start\r\n",dAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);
	
	//AR_S32	PadLine=2; 


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;
	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;


    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
		return;
	}

	AR_GDC_USER_ATTR_S usrAttr={0};
	AR_S32 s32OutWidth = stParam.stOutBuffer.format.width;
	AR_S32 s32OutHeight = stParam.stOutBuffer.format.height;

	cv::Point2f center(s32SrcWidth/2, s32SrcHeight/2);
	Mat M = cv::getRotationMatrix2D(center,dAngle,1.0);

	Mat Mq(3,3,CV_64FC1);
    for(int m =0 ;m <2; m++){
		for(int n =0 ;n<3; n++){
			Mq.at<double>(m,n) = M.at<double>(m,n);
		}
	}
	Mq.at<double>(2,0) = 0.0;
	Mq.at<double>(2,1) = 0.0;
	Mq.at<double>(2,2) = 1.0;
	cout<<Mq <<endl;

	Mat Minv = Mq.inv();
	cout<<Minv <<endl;

	s32Ret = Sample_GDC_GenerateLut(s32OutWidth,s32OutHeight,&usrAttr,Minv);
	if(s32Ret < 0){
		printf("Sample_GDC_GenerateLut fail\r\n");
		goto exit_handler;
	}
	stParam.stParams[0].enType = AR_GDC_USER_DEF_E;
	stParam.stParams[0].pAttr = &usrAttr;

	s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

	//Inbuffer_Padding_Inside(&stParam,PadLine);
	//stParam.stOutBuffer.format.width -= PadLine;
	//stParam.stOutBuffer.format.height -= PadLine;

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}
	//stParam.stOutBuffer.format.width += PadLine;
	//stParam.stOutBuffer.format.height += PadLine;

	sprintf(s8DstFileName, "./USRMODE_ROT%f_Matrix_Img%sSize%dx%dIS%dOS%d.yuv",dAngle,s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);


exit_handler:

    if(usrAttr.u64LutVirtAddr){
		free((void*)usrAttr.u64LutVirtAddr);
    }

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_USR_MODE_ROT [%f] end\r\n",dAngle);
}



void TEST_GDC_USER_MODE_ROTX_With_Coordinate()
{
    printf("TEST_GDC_USER_MODE_ROTX_With_Coordinate start\r\n");
	AR_S32 s32SrcWidth = 1920;
	AR_S32 s32SrcHeight = 1088;
	AR_S32 s32InStrideAlgn = 64;
	AR_S32 s32OutStrideAlgn = 64;
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, "coord.yuv");
	
	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;
	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;

    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32InStrideAlgn);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32InStrideAlgn/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32OutStrideAlgn);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32OutStrideAlgn/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
		return;
	}

	AR_GDC_USER_ATTR_S usrAttr={0};

	AR_S32 s32BlkHnum = (stParam.stOutBuffer.format.width + 63) / 64;
	AR_S32 s32BlkVnum = (stParam.stOutBuffer.format.height + 63) / 64;
	AR_S32 s32AxisHnum = s32BlkHnum + 1;
	AR_S32 s32AxisVnum = s32BlkVnum + 1;
	AR_S32 s32AxisStride = ((s32AxisHnum + 3) / 4)*4;

	printf("s32AxisHnum:%d s32AxisVnum:%d s32AxisStride:%d \r\n",s32AxisHnum,s32AxisVnum,s32AxisStride);

	usrAttr.u32LutLen = s32AxisStride*s32AxisVnum;
	usrAttr.u64LutVirtAddr = (AR_U64)malloc(usrAttr.u32LutLen*sizeof(AR_U32));
	memset((void*)usrAttr.u64LutVirtAddr, 0 ,usrAttr.u32LutLen*sizeof(AR_U32));
    AR_U32 *pu32Lut = (AR_U32 *)usrAttr.u64LutVirtAddr;
	for (int i = 0; i < s32AxisVnum; i++){
		 for (int j = 0; j < s32AxisStride; j++){
			   int k=i*s32AxisStride+j;
			   if(j<s32AxisHnum){
			   	
				 AR_U32 u32TmpData = ((int)(4*usrcdr[i*s32AxisHnum+j].x) & 0xffff) + (((int)(4*usrcdr[i*s32AxisHnum+j].y) & 0xffff) << 16);
				 pu32Lut[k]=u32TmpData;
			   }
			   else
			   {
				 pu32Lut[k]=0;
			   }
		 }
	}
	
	stParam.stParams[0].enType = AR_GDC_USER_DEF_E;
	stParam.stParams[0].pAttr = &usrAttr;

	s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	sprintf(s8DstFileName, "./USRMODE_Coord_ImgSize%dx%dIS%dOS%d.yuv",s32SrcWidth,s32SrcHeight,s32InStrideAlgn,s32OutStrideAlgn);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);


exit_handler:

    if(usrAttr.u64LutVirtAddr){
		free((void*)usrAttr.u64LutVirtAddr);
    }

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_USER_MODE_ROTX_With_Coordinate end\r\n");
}

void TEST_GDC_USER_MODE_ROT90_Matrix(AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
	AR_DOUBLE dAngle = 90.0;
    printf("TEST_GDC_ROT [%f] Matrix start\r\n",dAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;
	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;

	if((fabs(dAngle-90.0)<=1e-6)||(fabs(dAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}


    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
		return;
	}

	AR_GDC_USER_ATTR_S usrAttr={0};
	AR_S32 s32OutWidth = stParam.stOutBuffer.format.width;
	AR_S32 s32OutHeight = stParam.stOutBuffer.format.height;

    AR_S32 WHMax = s32SrcWidth > s32SrcHeight? s32SrcWidth:s32SrcHeight;
    cv::Point2f center(WHMax/2, WHMax/2);
	Mat M = cv::getRotationMatrix2D(center,dAngle,1.0);

	Mat Mq(3,3,CV_64FC1);
    for(int m =0 ;m <2; m++){
		for(int n =0 ;n<3; n++){
			Mq.at<double>(m,n) = M.at<double>(m,n);
		}
	}
	Mq.at<double>(2,0) = 0.0;
	Mq.at<double>(2,1) = 0.0;
	Mq.at<double>(2,2) = 1.0;
	cout<<Mq <<endl;

	Mat Minv = Mq.inv();
	cout<<Minv <<endl;

	#if 0
	AR_DOUBLE M[2][3] = {{0,0,0},{0,0,0}};
	M[0][0] = 0.0;
	M[0][1] = -1.0;
	M[0][2] = stParam.stInBuffer.format.width*1.0;
	M[1][0] = 1.0;
	M[1][1] = 0.0;
	M[1][2] = 0.0;
	#endif
	s32Ret = Sample_GDC_GenerateLut(s32OutWidth,s32OutHeight,&usrAttr,Minv);
	if(s32Ret < 0){
		printf("Sample_GDC_GenerateLut fail\r\n");
		goto exit_handler;
	}
	stParam.stParams[0].enType = AR_GDC_USER_DEF_E;
	stParam.stParams[0].pAttr = &usrAttr;

	
    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	sprintf(s8DstFileName, "./USRMODE_ROT90_Matrix_Img%sSize%dx%dIS%dOS%d.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);


exit_handler:

    if(usrAttr.u64LutVirtAddr){
		free((void*)usrAttr.u64LutVirtAddr);
    }

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_USR_MODE_ROT [%f] end\r\n",dAngle);
}


void TEST_GDC_USER_MODE_ROT90(AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
	AR_FLOAT fAngle = 90.0;
    printf("TEST_GDC_ROT [%f] start\r\n",fAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;
	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;

	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
		return;
	}

	AR_GDC_USER_ATTR_S usrAttr={0};

	AR_S32 s32BlkHnum = (stParam.stOutBuffer.format.width + 63) / 64;
	AR_S32 s32BlkVnum = (stParam.stOutBuffer.format.height + 63) / 64;
	AR_S32 s32AxisHnum = s32BlkHnum + 1;
	AR_S32 s32AxisVnum = s32BlkVnum + 1;
	AR_S32 s32AxisStride = ((s32AxisHnum + 3) / 4)*4;

	usrAttr.u32LutLen = s32AxisStride*s32AxisVnum;
	usrAttr.u64LutVirtAddr = (AR_U64)malloc(usrAttr.u32LutLen*sizeof(AR_U32));
	memset((void*)usrAttr.u64LutVirtAddr, 0 ,usrAttr.u32LutLen*sizeof(AR_U32));
    AR_U32 *pu32Lut = (AR_U32 *)usrAttr.u64LutVirtAddr;
	for (int i = 0; i < s32AxisVnum; i++){
		 for (int j = 0; j < s32AxisStride; j++){
			   int k=i*s32AxisStride+j;
			   if(j<s32AxisHnum){

				 AR_U32 u32TmpData = ((256*(s32BlkVnum-i)) & 0xffff) + (((j*256) & 0xffff) << 16);
				 pu32Lut[k]=u32TmpData;
			   }
			   else
			   {
				 pu32Lut[k]=0;
			   }
		 }
	}

	stParam.stParams[0].enType = AR_GDC_USER_DEF_E;
	stParam.stParams[0].pAttr = &usrAttr;

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}


	sprintf(s8DstFileName, "./USRMODE_ROT90_Img%sSize%dx%dIS%dOS%d.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);


exit_handler:

    if(usrAttr.u64LutVirtAddr){
		free((void*)usrAttr.u64LutVirtAddr);
    }

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_USR_MODE_ROT [%f] end\r\n",fAngle);
}


void TEST_GDC_ROT(AR_FLOAT fAngle,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_ROT [%f] start\r\n",fAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);

	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}



	stParam.stParams[0].enType = AR_GDC_ROTATE_E;
	AR_GDC_ROTATE_ATTR_S rotAttr;
	memset(&rotAttr,0,sizeof(rotAttr));
	rotAttr.fAngle = fAngle;
	stParam.stParams[0].pAttr = &rotAttr;


	AR_FLOAT fPTime = 0.0;

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}


    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}


	s32Ret= AR_MPI_GDC_GetTime(&fPTime);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_GetTime fail\r\n");
		goto exit_handler;

	}
	printf("ioctrl gdc taskes %f ms\r\n",fPTime);
	
    if(stParam.stParams[0].enType==AR_GDC_ROTATE_E)
    {
      AR_GDC_ROTATE_ATTR_S* pstRot =(AR_GDC_ROTATE_ATTR_S*)stParam.stParams[0].pAttr;
      if(fabs(pstRot->fAngle-90.0)<=1e-6)
      {
          strcat(s8OpName,"Rot90");
	  }
	  else if(fabs(pstRot->fAngle-180.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot180");
	  }
	  else if(fabs(pstRot->fAngle-270.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot270");
	  }
	  else if(fabs(pstRot->fAngle-0.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot0");
	  }
    }
	
	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_ROT [%f] end\r\n",fAngle);
}


void TEST_GDC_ROT_General(AR_FLOAT fAngle,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_ROT_General [%f] start\r\n",fAngle);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);

	AR_IMG_S  stImgIn ={0};
	AR_IMG_S  stImgOut ={0};
	AR_GDC_TRANSFORM_ARRAY_S stParam;
	memset(stParam.stParams,0,sizeof(stParam.stParams));

	stImgIn.u32Width = s32SrcWidth;
	stImgIn.u32Height = s32SrcHeight;

	stImgOut.u32Width = stImgIn.u32Width;
	stImgOut.u32Height = stImgIn.u32Height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
		stImgOut.u32Width = stImgIn.u32Height;
		stImgOut.u32Height = stImgIn.u32Width;
	}
    stImgIn.astChannels[0].u32Stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stImgIn.u32Width, s32AlgnUnit);
	stImgIn.astChannels[1].u32Stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stImgIn.u32Width/2, s32AlgnUnit/2);
	stImgOut.astChannels[0].u32Stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stImgOut.u32Width, s32AlgnUnit);
	stImgOut.astChannels[1].u32Stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stImgOut.u32Width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stImgIn.astChannels[0].u32Stride,stImgIn.astChannels[1].u32Stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stImgOut.astChannels[0].u32Stride,stImgOut.astChannels[1].u32Stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request_General(&stImgIn, &stImgOut);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}

	stParam.stParams[0].enType = AR_GDC_ROTATE_E;
	AR_GDC_ROTATE_ATTR_S rotAttr;
	memset(&rotAttr,0,sizeof(rotAttr));
	rotAttr.fAngle = fAngle;
	stParam.stParams[0].pAttr = &rotAttr;


	AR_FLOAT fPTime = 0.0;
    s32Ret = SAMPLE_GDC_Load_Src_Img_General(&stImgIn, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

	s32Ret= AR_MPI_GDC_Transform_General(&stImgIn, &stImgOut, &stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform_General fail\r\n");
		goto exit_handler;

	}

	s32Ret= AR_MPI_GDC_GetTime(&fPTime);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_GetTime fail\r\n");
		goto exit_handler;

	}
	printf("ioctrl gdc taskes %f ms\r\n",fPTime);
	
    if(stParam.stParams[0].enType==AR_GDC_ROTATE_E)
    {
      AR_GDC_ROTATE_ATTR_S* pstRot =(AR_GDC_ROTATE_ATTR_S*)stParam.stParams[0].pAttr;
      if(fabs(pstRot->fAngle-90.0)<=1e-6)
      {
          strcat(s8OpName,"Rot90");
	  }
	  else if(fabs(pstRot->fAngle-180.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot180");
	  }
	  else if(fabs(pstRot->fAngle-270.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot270");
	  }
	  else if(fabs(pstRot->fAngle-0.0)<=1e-6)
	  {
		  strcat(s8OpName,"Rot0");
	  }
    }
	
	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%sGeneral.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img_General(&stImgOut, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stImgIn.astChannels[0].u32AddrPhy,stImgIn.astChannels[1].u32AddrPhy,stImgIn.astChannels[2].u32AddrPhy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stImgOut.astChannels[0].u32AddrPhy,stImgOut.astChannels[1].u32AddrPhy,stImgOut.astChannels[2].u32AddrPhy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release_General(&stImgIn, &stImgOut);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_ROT_General [%f] end\r\n",fAngle);
}


void TEST_GDC_Flip(AR_BOOL bEnable,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_Flip [%d] start\r\n",bEnable);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);

    AR_FLOAT fAngle =0.0;
	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}

	stParam.stParams[0].enType = AR_GDC_FLIP_E;
	AR_GDC_FLIP_ATTR_S flipAttr;
	memset(&flipAttr,0,sizeof(flipAttr));
	flipAttr.bEnable= bEnable;
	stParam.stParams[0].pAttr = &flipAttr;


	AR_FLOAT fPTime =0.0;

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

	
    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}


	s32Ret= AR_MPI_GDC_GetTime(&fPTime);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_GetTime fail\r\n");
		goto exit_handler;

	}
	printf("ioctrl gdc taskes %f ms\r\n",fPTime);

	if(stParam.stParams[0].enType==AR_GDC_FLIP_E)
	{
       AR_GDC_FLIP_ATTR_S* pstFlip = (AR_GDC_FLIP_ATTR_S*)stParam.stParams[0].pAttr;
	   if(pstFlip->bEnable==AR_TRUE)
	   {
		   strcat(s8OpName,"Flip");
	   }
	}

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_Flip [%d] end\r\n",bEnable);
}

void TEST_GDC_MIRROR(AR_BOOL bEnable,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_MIRROR [%d] start\r\n",bEnable);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);

    AR_FLOAT fAngle =0.0;
	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}



	stParam.stParams[0].enType = AR_GDC_MIRROR_E;
	AR_GDC_MIRROR_ATTR_S mirrorAttr;
	memset(&mirrorAttr,0,sizeof(mirrorAttr));
	mirrorAttr.bEnable= bEnable;
	stParam.stParams[0].pAttr = &mirrorAttr;

    AR_FLOAT fPTime = 0.0;
	
    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	s32Ret= AR_MPI_GDC_GetTime(&fPTime);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_GetTime fail\r\n");
		goto exit_handler;

	}
	printf("ioctrl gdc taskes %f ms\r\n",fPTime);

	if(stParam.stParams[0].enType==AR_GDC_MIRROR_E)
	{
       AR_GDC_MIRROR_ATTR_S* pstMirror = (AR_GDC_MIRROR_ATTR_S*)stParam.stParams[0].pAttr;
	   if(pstMirror->bEnable==AR_TRUE)
	   {
		   strcat(s8OpName,"Mirror");
	   }
	}

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_MIRROR [%d] end\r\n",bEnable);
}


void TEST_GDC_LDC(AR_CHAR* s8LdcFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_LDC start\r\n");
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8LdcFileName);

    AR_FLOAT fAngle =0.0;
	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}

	stParam.stParams[0].enType = AR_GDC_LDC_E;
	AR_GDC_LDC_ATTR_S ldcAttr;
	memset(&ldcAttr,0,sizeof(ldcAttr));
	ldcAttr.ldc_k0 = -0.4501 ;
    ldcAttr.ldc_k1 =	0.2584 ;
    ldcAttr.ldc_k2 = -0.0927 ;
    ldcAttr.k[0]=1367.9;
    ldcAttr.k[1]=0;
    ldcAttr.k[2]=962.29;
    ldcAttr.k[3]=0;
    ldcAttr.k[4]=1367.7;
    ldcAttr.k[5]=513.1752;
    ldcAttr.k[6]=0;
    ldcAttr.k[7]=0;
    ldcAttr.k[8]=1;
	stParam.stParams[0].pAttr = &ldcAttr;

	AR_FLOAT fPTime =0.0;

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}


    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	s32Ret= AR_MPI_GDC_GetTime(&fPTime);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_GetTime fail\r\n");
		goto exit_handler;

	}
	printf("ioctrl gdc taskes %f ms\r\n",fPTime);

	if(stParam.stParams[0].enType==AR_GDC_LDC_E)
    {
  	   AR_GDC_LDC_ATTR_S*pstLdc = (AR_GDC_LDC_ATTR_S*)stParam.stParams[0].pAttr;
	   if(pstLdc)
	   {
		   strcat(s8OpName,"Ldc");
	   }
    }

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",s8LdcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_LDC end\r\n");
}


void TEST_GDC_COMPLEX_ROT_MIRROR(AR_BOOL bRotMirOrder, AR_FLOAT fAngle, AR_BOOL bMirrorEnable,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
	printf("TEST_GDC_COMPLEX_ROT_MIRROR bRotMirOrder[%d] Angle[%f] Mirror[%d] start\r\n",bRotMirOrder,fAngle,bMirrorEnable);
	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);

	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

/*********************Request Ion Buffer***********************/

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}


    if(bRotMirOrder==AR_TRUE)
    {
		stParam.stParams[0].enType = AR_GDC_ROTATE_E;
		AR_GDC_ROTATE_ATTR_S rotAttr;
		memset(&rotAttr,0,sizeof(rotAttr));
		rotAttr.fAngle = fAngle;
		stParam.stParams[0].pAttr = &rotAttr;

		if(bMirrorEnable==AR_TRUE){
			stParam.stParams[1].enType = AR_GDC_MIRROR_E;
			AR_GDC_MIRROR_ATTR_S mirrorAttr;
			memset(&mirrorAttr,0,sizeof(mirrorAttr));
			mirrorAttr.bEnable= bMirrorEnable;
			stParam.stParams[1].pAttr = &mirrorAttr;
		}
	}
	else
	{
		if(bMirrorEnable==AR_TRUE){
			stParam.stParams[0].enType = AR_GDC_MIRROR_E;
			AR_GDC_MIRROR_ATTR_S mirrorAttr;
			memset(&mirrorAttr,0,sizeof(mirrorAttr));
			mirrorAttr.bEnable= bMirrorEnable;
			stParam.stParams[0].pAttr = &mirrorAttr;
		}
		stParam.stParams[1].enType = AR_GDC_ROTATE_E;
		AR_GDC_ROTATE_ATTR_S rotAttr={0};
		rotAttr.fAngle = fAngle;
		stParam.stParams[1].pAttr = &rotAttr;

	}

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	for(int n=0;n <AR_GDC_OP_NUM_MAX;n++){
		  if(stParam.stParams[n].enType==AR_GDC_ROTATE_E){
		      AR_GDC_ROTATE_ATTR_S* pstRot =(AR_GDC_ROTATE_ATTR_S*)stParam.stParams[n].pAttr;
              if(fabs(pstRot->fAngle-90.0)<=1e-6){
                  strcat(s8OpName,"Rot90");
			  }
			  else if(fabs(pstRot->fAngle-180.0)<=1e-6){
				  strcat(s8OpName,"Rot180");
			  }
			  else if(fabs(pstRot->fAngle-270.0)<=1e-6){
				  strcat(s8OpName,"Rot270");
			  }
			  else{
				  strcat(s8OpName,"Rot0");
			  }
		  }
		  else if(stParam.stParams[n].enType==AR_GDC_MIRROR_E){
		  	   AR_GDC_MIRROR_ATTR_S* pstMirror= (AR_GDC_MIRROR_ATTR_S*)stParam.stParams[n].pAttr;
			   if(pstMirror->bEnable==AR_TRUE){
				   strcat(s8OpName,"Mirror");
			   }
		  }
	}

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",
		     s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);


	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0)
	{
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
    printf("TEST_GDC_COMPLEX_ROT_MIRROR bRotMirOrder[%d] Angle[%f] Mirror[%d] end\r\n",bRotMirOrder,fAngle,bMirrorEnable);

}

void TEST_GDC_COMPLEX_ROT_FLIP(AR_BOOL bRotFlipOrder, AR_FLOAT fAngle, AR_BOOL bFlipEnable,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
	printf("TEST_GDC_COMPLEX_ROT_FLIP bRotFlipOrder[%d] Angle[%f] Mirror[%d] start\r\n",bRotFlipOrder,fAngle,bFlipEnable);

	AR_CHAR s8SrcFileName[64] = {};
	AR_CHAR s8OpName[128] = {};
	AR_CHAR s8DstFileName[128] = {};
	sprintf(s8SrcFileName, s8RotFileName);


	AR_GDC_TRANSFORM_S stParam={0};
	stParam.stInBuffer.format.width = s32SrcWidth;
	stParam.stInBuffer.format.height = s32SrcHeight;

	stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
	stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
	if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
	{
	   stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
	   stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
	}
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
	stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);

	stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
	stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);

    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
	printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);

/*********************Request Ion Buffer***********************/

	AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
	}


    if(bRotFlipOrder==AR_TRUE)
    {
		stParam.stParams[0].enType = AR_GDC_ROTATE_E;
		AR_GDC_ROTATE_ATTR_S rotAttr={0};
		rotAttr.fAngle = fAngle;
		stParam.stParams[0].pAttr = &rotAttr;

		if(bFlipEnable==AR_TRUE){
			stParam.stParams[1].enType = AR_GDC_FLIP_E;
			AR_GDC_FLIP_ATTR_S flipAttr;
			memset(&flipAttr,0,sizeof(flipAttr));
			flipAttr.bEnable= bFlipEnable;
			stParam.stParams[1].pAttr = &flipAttr;
		}
	}
	else
	{
		if(bFlipEnable==AR_TRUE){
			stParam.stParams[0].enType = AR_GDC_FLIP_E;
			AR_GDC_FLIP_ATTR_S flipAttr;
			memset(&flipAttr,0,sizeof(flipAttr));
			flipAttr.bEnable= bFlipEnable;
			stParam.stParams[0].pAttr = &flipAttr;
		}
		stParam.stParams[1].enType = AR_GDC_ROTATE_E;
		AR_GDC_ROTATE_ATTR_S rotAttr={0};
		rotAttr.fAngle = fAngle;
		stParam.stParams[1].pAttr = &rotAttr;

	}

    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
		printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}

    s32Ret= AR_MPI_GDC_Transform(&stParam);
	if(s32Ret < 0)
	{
		printf("AR_MPI_GDC_Transform fail\r\n");
		goto exit_handler;

	}

	for(int n=0;n <AR_GDC_OP_NUM_MAX;n++){
		  if(stParam.stParams[n].enType==AR_GDC_ROTATE_E){
		      AR_GDC_ROTATE_ATTR_S* pstRot =(AR_GDC_ROTATE_ATTR_S*)stParam.stParams[n].pAttr;
              if(fabs(pstRot->fAngle-90.0)<=1e-6){
                  strcat(s8OpName,"Rot90");
			  }
			  else if(fabs(pstRot->fAngle-180.0)<=1e-6){
				  strcat(s8OpName,"Rot180");
			  }
			  else if(fabs(pstRot->fAngle-270.0)<=1e-6){
				  strcat(s8OpName,"Rot270");
			  }
			  else{
				  strcat(s8OpName,"Rot0");
			  }
		  }
		  else if(stParam.stParams[n].enType==AR_GDC_FLIP_E){
               AR_GDC_FLIP_ATTR_S* pstFlip = (AR_GDC_FLIP_ATTR_S*)stParam.stParams[n].pAttr;
			   if(pstFlip->bEnable==AR_TRUE){
				   strcat(s8OpName,"Flip");
			   }
		  }
		  else if(stParam.stParams[n].enType==AR_GDC_MIRROR_E){
		  	   AR_GDC_MIRROR_ATTR_S* pstMirror= (AR_GDC_MIRROR_ATTR_S*)stParam.stParams[n].pAttr;
			   if(pstMirror->bEnable==AR_TRUE){
				   strcat(s8OpName,"Mirror");
			   }
		  }
	}

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",
		     s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);


	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0)
	{
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}

	printf("TEST_GDC_COMPLEX_ROT_FLIP bRotFlipOrder[%d] Angle[%f] Mirror[%d] end\r\n",bRotFlipOrder,fAngle,bFlipEnable);
}

static AR_U32 gu32QuitFlag = 0;
AR_S32 s32GdcDone = 0;

void AR_SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
    SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        //while(!s32GdcDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_GDC_Suspend();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("gdc suspend failed, ret:%d!\n", ret);
        else
            printf("gdc suspend success!\n");
        
        flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        //while(!s32GdcDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_GDC_Resume();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("gdc resume failed, ret:%d!\n", ret);
        else
            printf("gdc resume success!\n");
        
        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");

    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}

void sighandler(int sig)
{
    printf("Get signal %d\r\n", sig);

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    
    if (SIGINT == sig || SIGTERM == sig) {
        printf("termination abnormally, signo:%d!\n", sig);
        gu32QuitFlag = 1;
    }  
}


void TEST_GDC_MIRROR_Suspend_Resume(AR_BOOL bEnable,AR_CHAR* s8RotFileName,AR_S32 s32SrcWidth,AR_S32 s32SrcHeight,AR_S32 s32AlgnUnit)
{
    printf("TEST_GDC_MIRROR [%d] start\r\n",bEnable);
    AR_CHAR s8SrcFileName[64] = {};
    AR_CHAR s8OpName[128] = {};
    AR_CHAR s8DstFileName[128] = {};
    sprintf(s8SrcFileName, s8RotFileName);
    
    AR_FLOAT fAngle =0.0;
    AR_GDC_TRANSFORM_S stParam={0};
    stParam.stInBuffer.format.width = s32SrcWidth;
    stParam.stInBuffer.format.height = s32SrcHeight;
    
    stParam.stOutBuffer.format.width = stParam.stInBuffer.format.width;
    stParam.stOutBuffer.format.height = stParam.stInBuffer.format.height;
    if((fabs(fAngle-90.0)<=1e-6)||(fabs(fAngle-270.0)<=1e-6))
    {
        stParam.stOutBuffer.format.width = stParam.stInBuffer.format.height;
        stParam.stOutBuffer.format.height = stParam.stInBuffer.format.width;
    }
    stParam.stInBuffer.format.luma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width, s32AlgnUnit);
    stParam.stInBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stInBuffer.format.width/2, s32AlgnUnit/2);
    stParam.stOutBuffer.format.luma_stride = SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width, s32AlgnUnit);
    stParam.stOutBuffer.format.chroma_stride= SAMPLE_GDC_STRIDE_ALIGNE_TO(stParam.stOutBuffer.format.width/2, s32AlgnUnit/2);
    
    printf("SRC LumaStride/ChromaStride %d%d\r\n",stParam.stInBuffer.format.luma_stride,stParam.stInBuffer.format.chroma_stride);
    printf("DST LumaStride/ChromaStride %d%d\r\n",stParam.stOutBuffer.format.luma_stride,stParam.stOutBuffer.format.chroma_stride);
    
    AR_S32 s32Ret = SAMPLE_GDC_MMZ_Buffer_Request(&stParam);
    if(s32Ret < 0){
        printf("SAMPLE_GDC_MMZ_Buffer_Request fail\r\n");
    }
	
    stParam.stParams[0].enType = AR_GDC_MIRROR_E;
    AR_GDC_MIRROR_ATTR_S mirrorAttr;
    memset(&mirrorAttr,0,sizeof(mirrorAttr));
    mirrorAttr.bEnable= bEnable;
    stParam.stParams[0].pAttr = &mirrorAttr;
    
    AR_FLOAT fPTime = 0.0;
    AR_S32 s32CntP = 0;
    s32Ret = SAMPLE_GDC_Load_Src_Img(&stParam, s8SrcFileName);
    if(s32Ret < 0){
        printf("SAMPLE_GDC_Load_Src_Img fail\r\n");
        goto exit_handler;
    }
	
    while(!gu32QuitFlag){
        //usleep(1000*1000);
        //s32GdcDone = 0;
        s32Ret= AR_MPI_GDC_Transform(&stParam);
        //s32GdcDone = 1;
        if(s32Ret < 0){
			if(s32CntP++ > 10000){
				s32CntP = 0;
                printf("gdc maybe suspend!\n");
			}
        }
    }

    if(s32Ret < 0)
    {
        printf("AR_MPI_GDC_Transform fail\r\n");
        goto exit_handler;
    }

    s32Ret= AR_MPI_GDC_GetTime(&fPTime);
    if(s32Ret < 0)
    {
        printf("AR_MPI_GDC_GetTime fail\r\n");
        goto exit_handler;
    }
    printf("ioctrl gdc taskes %f ms\r\n",fPTime);

    if(stParam.stParams[0].enType==AR_GDC_MIRROR_E)
    {
        AR_GDC_MIRROR_ATTR_S* pstMirror = (AR_GDC_MIRROR_ATTR_S*)stParam.stParams[0].pAttr;
        if(pstMirror->bEnable==AR_TRUE)
        {
            strcat(s8OpName,"Mirror");
        }
    }

	sprintf(s8DstFileName, "./Img%sSize%dx%dIS%dOS%d%s.yuv",s8RotFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit,s32AlgnUnit,s8OpName);

	s32Ret = SAMPLE_GDC_Store_Dst_Img(&stParam, s8DstFileName);
	if(s32Ret < 0){
		printf("SAMPLE_EIS_Store_Dst_Img fail\r\n");
	}
	printf("write %s finished.\r\n",s8DstFileName);

	printf("SRC Y/U/V Addr %llx %llx %llx \r\n",stParam.stInBuffer.pannel[0].addr_phy,stParam.stInBuffer.pannel[1].addr_phy,stParam.stInBuffer.pannel[2].addr_phy);
	printf("DST Y/U/V Addr %llx %llx %llx \r\n",stParam.stOutBuffer.pannel[0].addr_phy,stParam.stOutBuffer.pannel[1].addr_phy,stParam.stOutBuffer.pannel[2].addr_phy);

exit_handler:

	/*********************Release Ion Buffer***********************/
	s32Ret = SAMPLE_GDC_MMZ_Buffer_Release(&stParam);
	if(s32Ret < 0){
		printf("SAMPLE_GDC_MMZ_Buffer_Release fail\r\n");
	}
	printf("TEST_GDC_MIRROR [%d] end\r\n",bEnable);
}

AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
	AR_S32 s32Ret = 0;
    //For EIS Algo Debug 
    //TEST_GDC_USER_MODE_ROTX_With_Coordinate();

	AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-srcfile: rot/mirror/flip/ldc file name\n");
        printf("-w: file width default 1920\n");
        printf("-h: file height default 1080\n");
		printf("-algn: file stride algn unit default 64\n");
		printf("FastBoot : if enable fastboot, default no need\n");
		printf("only support yuv420\n");
		return 0;
    }


	AR_CHAR s8SrcFileName[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-srcfile",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8SrcFileName,argv[s32Index+1]);
		printf("s8SrcFileName=%s \r\n",s8SrcFileName);
	}
	else{
		printf(" NO s8RotFileName defined\r\n");
		return -1;
	}
	
	AR_S32 s32SrcWidth = 1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-w",argc,argv);
	if(s32Index>0)
	{
		s32SrcWidth=atoi(argv[s32Index+1]);
		printf("s32SrcWidth=%d \r\n",s32SrcWidth);
		if(s32SrcWidth >4096 || s32SrcWidth <0)
		{
			printf("s32SrcWidth error \r\n");
			return -1;
		}	
	}

	AR_S32 s32SrcHeight = 1080;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-h",argc,argv);
	if(s32Index>0)
	{
		s32SrcHeight=atoi(argv[s32Index+1]);
		printf("s32SrcHeight=%d \r\n",s32SrcHeight);
		if(s32SrcHeight >2160 || s32SrcHeight <0)
		{
			printf("s32SrcHeight error \r\n");
			return -1;
		}
	}

	AR_S32 s32AlgnUnit = 64;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-s",argc,argv);
	if(s32Index>0)
	{
		s32AlgnUnit=atoi(argv[s32Index+1]);
		printf("s32AlgnUnit=%d \r\n",s32AlgnUnit);
		if(s32AlgnUnit >4096 || s32AlgnUnit <0)
		{
			printf("s32AlgnUnit error \r\n");
			return -1;
		}
	}

	AR_S32 s32FastBoot = 0;

    s32Index=SAMPLE_Find_Pra_Index_By_Name("FastBoot",argc,argv);
    if(s32Index>0){
        s32FastBoot = 1;
    }
    
    printf("fast boot is enable %d\n", s32FastBoot);
    /*** register in app to sysctl ***/
    if(s32FastBoot){	
        g_phandle = AR_MPI_SYSCTL_Register((AR_CHAR *)argv[0], 0, &AR_SAMPLE_fast_boot_func);
        if(!g_phandle){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("register sysctl %s success!\n", argv[0]);
    }
    
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    if(s32FastBoot){
        TEST_GDC_MIRROR_Suspend_Resume(AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    }
    else{
        //0.ROT
        TEST_GDC_USER_MODE_ROTX_Matrix(28.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
        TEST_GDC_USER_MODE_ROTX_Matrix(45.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_USER_MODE_ROTX_Matrix(110.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_USER_MODE_ROTX_Matrix(135.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_USER_MODE_ROTX_Matrix(90.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    
    	TEST_GDC_USER_MODE_ROTX_Matrix_Without_Padding(45.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	
        TEST_GDC_USER_MODE_ROT90_Matrix(s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
        TEST_GDC_USER_MODE_ROT90(s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    
        TEST_GDC_ROT(0.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT(90.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT(180.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT(270.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    
    	//1. Mirror/Flip
    	TEST_GDC_Flip(AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_MIRROR(AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    
        //2. LDC
    	TEST_GDC_LDC(s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
     
        //3. Complex FUNC (LDC/EIS Not Support NOW)
    
        //3.1 ROT + FLIP
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_TRUE, 0.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_TRUE, 90.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_TRUE, 180.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_TRUE, 270.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
        //3.2  FLIP + ROT
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_FALSE, 0.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_FALSE, 90.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_FALSE, 180.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_FLIP(AR_FALSE, 270.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	//3.3 ROT + MIRROR
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_TRUE, 0.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_TRUE, 90.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_TRUE, 180.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_TRUE, 270.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
        //3.4  MIRROR + ROT
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_FALSE, 0.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_FALSE, 90.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_FALSE, 180.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_COMPLEX_ROT_MIRROR(AR_FALSE, 270.0, AR_TRUE,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    
    
    #if 1
    	TEST_GDC_ROT_General(0.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT_General(90.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT_General(180.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    	TEST_GDC_ROT_General(270.0,s8SrcFileName,s32SrcWidth,s32SrcHeight,s32AlgnUnit);
    #endif
	}

    /*** unregister in app to sysctl ***/
    if(s32FastBoot){
        s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
        if(s32Ret < 0){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("unregister sysctl %s success!\n", argv[0]);
    }
    return 0;
}
