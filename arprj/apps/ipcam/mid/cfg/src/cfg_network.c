#include "cfg_common.h"
#include "cfg_network.h"
#include <sys/time.h>

/*******This is not checked yet, 2021-06-07 ********/

//static void random_produce_mac(char *randomMac);

IPC_CFG_NETWORK_S g_stRunNetworkCfg;


CFG_MAP lanMap[] = {
    {"enable",   &(g_stRunNetworkCfg.lan.enable),     CFG_DATA_TYPE_S32,    "1",                 "rw", 0, 1,             NULL},
    {"netName",   &(g_stRunNetworkCfg.lan.netName),   CFG_DATA_TYPE_STRING, "eth0",              "rw", 1, MAX_STR_LEN_64, NULL},
    {"ipversion", &(g_stRunNetworkCfg.lan.ipVersion), CFG_DATA_TYPE_S32,    "0",                 "rw", 0, 1,              "0 v4, 1 v6"},
    {"mac",       &(g_stRunNetworkCfg.lan.mac),       CFG_DATA_TYPE_STRING, "28:E3:83:F6:40:12", "rw", 1, MAX_STR_LEN_20, NULL},
    {"dhcp",      &(g_stRunNetworkCfg.lan.dhcpIp),    CFG_DATA_TYPE_S32, "1",                "rw", 0, 1,              NULL},
    {"upnp",      &(g_stRunNetworkCfg.lan.upnpEnable),CFG_DATA_TYPE_S32,    "0",                 "rw", 0, 1,              NULL},
    {"ip",        &(g_stRunNetworkCfg.lan.ip),        CFG_DATA_TYPE_STRING, "192.168.10.25",     "rw", 1, MAX_STR_LEN_16, NULL},
    {"netmask",   &(g_stRunNetworkCfg.lan.netmask),   CFG_DATA_TYPE_STRING, "255.255.255.0",     "rw", 1, MAX_STR_LEN_16, NULL},
    {"gateway",   &(g_stRunNetworkCfg.lan.gateway),   CFG_DATA_TYPE_STRING, "192.168.10.1",      "rw", 1, MAX_STR_LEN_16, NULL},
    {"multicast", &(g_stRunNetworkCfg.lan.multicast), CFG_DATA_TYPE_STRING, "192.168.10.255",    "rw", 1, MAX_STR_LEN_16, NULL},
    {"autoDns",   &(g_stRunNetworkCfg.lan.autoDns),   CFG_DATA_TYPE_S32,    "1",                 "rw", 0, 1,              NULL},
    {"dns1",      &(g_stRunNetworkCfg.lan.dns1),      CFG_DATA_TYPE_STRING, "114.114.114.114",   "rw", 1, MAX_STR_LEN_16, NULL},
    {"dns2",      &(g_stRunNetworkCfg.lan.dns2),      CFG_DATA_TYPE_STRING, "8.8.8.8",           "rw", 1, MAX_STR_LEN_16, NULL},
    {NULL,},
};

CFG_MAP emailMap[] = {
    {"enable",    	 &(g_stRunNetworkCfg.email.enable),    CFG_DATA_TYPE_S32,     "1",                "rw", 0, 1,      NULL},
	{"attachPicture",&(g_stRunNetworkCfg.email.attachPicture),	CFG_DATA_TYPE_S32,	  "1",				  "rw", 0, 1,	   NULL},
    {"smtpServer",   &(g_stRunNetworkCfg.email.smtpServer),     CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64, NULL},
	{"smtpPort",	 &(g_stRunNetworkCfg.email.smtpPort), 		CFG_DATA_TYPE_S32,	  "25",    "rw", 1, 65535, NULL},
	{"pop3Server",	 &(g_stRunNetworkCfg.email.pop3Server), 	CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64, NULL},
	{"pop3Port",	 &(g_stRunNetworkCfg.email.pop3Port),		CFG_DATA_TYPE_S32,	  "110",   "rw", 1, 65535, NULL},
    {"cryptionType", &(g_stRunNetworkCfg.email.encryptionType), CFG_DATA_TYPE_S32,     "0",                "rw",  0, 2,	   NULL},
    {"user",       	 &(g_stRunNetworkCfg.email.eMailUser),      CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64,    NULL},
    {"password",	 &(g_stRunNetworkCfg.email.eMailPass),		CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64,	   NULL},
	{"fromAddrList", &(g_stRunNetworkCfg.email.fromAddr),		CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64,    NULL},

	{"toAddrList0", &(g_stRunNetworkCfg.email.toAddrList0),		CFG_DATA_TYPE_STRING,  "", 	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"toAddrList1", &(g_stRunNetworkCfg.email.toAddrList1), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"toAddrList2", &(g_stRunNetworkCfg.email.toAddrList2), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"toAddrList3", &(g_stRunNetworkCfg.email.toAddrList3), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"ccAddrList0", &(g_stRunNetworkCfg.email.ccAddrList0), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"ccAddrList1", &(g_stRunNetworkCfg.email.ccAddrList1), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"ccAddrList2", &(g_stRunNetworkCfg.email.ccAddrList2), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"ccAddrList3", &(g_stRunNetworkCfg.email.ccAddrList3), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"bccAddrList0",&(g_stRunNetworkCfg.email.bccAddrList0), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"bccAddrList1",&(g_stRunNetworkCfg.email.bccAddrList1), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"bccAddrList2",&(g_stRunNetworkCfg.email.bccAddrList2), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
	{"bccAddrList3",&(g_stRunNetworkCfg.email.bccAddrList3), 	CFG_DATA_TYPE_STRING,  "",	   "rw", 1, MAX_STR_LEN_64,    NULL},
    {NULL,},
};

CFG_MAP ftpMap[] = {
    {"enable", &(g_stRunNetworkCfg.ftp.enable), CFG_DATA_TYPE_S32,     "0",       "rw", 0, 1,               NULL},
    {"address",   &(g_stRunNetworkCfg.ftp.address),   CFG_DATA_TYPE_STRING,  "",  "rw", 1, MAX_STR_LEN_128, NULL},
    {"port",      &(g_stRunNetworkCfg.ftp.port),      CFG_DATA_TYPE_S32,     "",      "rw", 0, 65536,           NULL},
    {"userName",  &(g_stRunNetworkCfg.ftp.userName),  CFG_DATA_TYPE_STRING,  "",    "rw", 1, MAX_STR_LEN_64,  NULL},
    {"password",  &(g_stRunNetworkCfg.ftp.password),  CFG_DATA_TYPE_STRING,  "",    "rw", 1, MAX_STR_LEN_64,  NULL},
    {"datapath",  &(g_stRunNetworkCfg.ftp.datapath),  CFG_DATA_TYPE_STRING,  "MDSnapshot",   "rw", 1, MAX_STR_LEN_128,  NULL},
    {"filename",  &(g_stRunNetworkCfg.ftp.filename),  CFG_DATA_TYPE_STRING,  "image.jpg",   "rw", 1, MAX_STR_LEN_128,  NULL},
    {NULL,},
};

CFG_MAP portMap[] = {
    {"httpPort", &(g_stRunNetworkCfg.port.httpPort), CFG_DATA_TYPE_S32,  "80",   "rw", 0, 65536, NULL},
    {"rtspPort", &(g_stRunNetworkCfg.port.rtspPort), CFG_DATA_TYPE_S32,  "554",  "rw", 0, 65536, NULL},
    {NULL,},
};

CFG_MAP wirelessMap[] ={
	{"enable",	  &g_stRunNetworkCfg.wireless.enable,    CFG_DATA_TYPE_S32, "1", "rw", 0, 1,  NULL},
	{"mode",	  &g_stRunNetworkCfg.wireless.mode,	     CFG_DATA_TYPE_S32, "0", "rw", 0, 1,  NULL},
	{"channel",	  &g_stRunNetworkCfg.wireless.channel,   CFG_DATA_TYPE_S32, "0", "rw", 0, 16,  NULL},
	{"wpaMode",   &g_stRunNetworkCfg.wireless.wpaMode,   CFG_DATA_TYPE_S32, "0", "rw", 0, 1,  NULL},
	{"dhcp",      &g_stRunNetworkCfg.wireless.dhcp,      CFG_DATA_TYPE_S32, "0", "rw", 0, 1,  NULL},
    {"standard",  &g_stRunNetworkCfg.wireless.standard,  CFG_DATA_TYPE_STRING, "802.11bgn", "rw", 0, MAX_STR_LEN_16,  NULL},
    {"ssid",      &g_stRunNetworkCfg.wireless.ssid,      CFG_DATA_TYPE_STRING, "artosyn_ipc", "rw", 0, MAX_STR_LEN_16,  NULL},
    {"password",  &g_stRunNetworkCfg.wireless.password,  CFG_DATA_TYPE_STRING, "123456789", "rw", 0, MAX_STR_LEN_16,  NULL},
	{NULL,}
};

void IPC_CFG_NETWORK_Print()
{
    printf("*************** Network **************\n");

    printf("lan:\n");
    CfgPrintMap(lanMap);
    printf("\n");

    printf("wifi:\n");
    CfgPrintMap(wirelessMap);
    printf("\n");

    printf("wifi:\n");
    CfgPrintMap(emailMap);
    printf("\n");

    printf("ftp:\n");
    CfgPrintMap(ftpMap);
    printf("\n");

    printf("port:\n");
    CfgPrintMap(portMap);
    printf("\n");

    printf("*************** Network **************\n\n");
}

int IPC_CFG_NETWORK_Save()
{
    cJSON *root;
	//cJSON *wifiLink = NULL;
    char *out;

    root = cJSON_CreateObject();

    CfgAddCjson(root, "lan", lanMap);
    CfgAddCjson(root, "email", emailMap);
    CfgAddCjson(root, "ftp", ftpMap);
    CfgAddCjson(root, "port", portMap);
    CfgAddCjson(root, "wireless", wirelessMap);

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(NETWORK_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", NETWORK_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

int IPC_CFG_NETWORK_LoadDefault()
{
    CfgLoadDefValue(lanMap);
    CfgLoadDefValue(emailMap);
    CfgLoadDefValue(ftpMap);
    CfgLoadDefValue(portMap);
	CfgLoadDefValue(wirelessMap);

    return 0;
}


int IPC_CFG_NETWORK_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    data = CfgReadFromFile(NETWORK_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", NETWORK_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    CfgParseCjson(json, "lan", lanMap);
    CfgParseCjson(json, "email", emailMap);
    CfgParseCjson(json, "ftp", ftpMap);
    CfgParseCjson(json, "port", portMap);
	CfgParseCjson(json, "wireless", wirelessMap);

	cJSON_Delete(json);

	free(data);
    return 0;

err:
    IPC_CFG_NETWORK_LoadDefault();
    IPC_CFG_NETWORK_Save();
    return 0;
}

#if 0
static void random_produce_mac(char *randomMac)
{
	char HEXCHAR[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char fmtMACAddr[]={'B','E',':','2','7',':','0','0',':','0','0',':','0','0',':','0','0',0};
	int i = 0, n = 0;
	unsigned long int seed = 0;
	seed = (unsigned long int)HEXCHAR+(unsigned long int)fmtMACAddr+(unsigned long int)&i +(unsigned long int)&n;
	PRINT_INFO("seed: %lu\n", seed);
	srand(seed);
	for(i = 6; i < strlen(fmtMACAddr); i++)
	{
		n = (rand()% 16);
		if(((i+1)% 3) !=0)
		{
			fmtMACAddr[i] = HEXCHAR[n];
		}
	}

	strcpy(randomMac, fmtMACAddr);
	PRINT_INFO("Random mac: %s\n",randomMac);
}
#endif

IPC_CFG_NETWORK_S * IPC_CFG_NETWORK_GetParam()
{
    return &g_stRunNetworkCfg;
}

