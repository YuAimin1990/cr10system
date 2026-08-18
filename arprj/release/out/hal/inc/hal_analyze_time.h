#ifndef _AR_VIN_ANALYZE_H
#define _AR_VIN_ANALYZE_H

//#define TIME_START(uid, key_id, ...) _TIME_START(uid, key_id, ##__VA_ARGS__)

//#define TIME_END(uid, key_id, ...) _TIME_END(uid, key_id, ##__VA_ARGS__)

void TIME_START(int uid, char *key_id, ...);
void TIME_END(int uid, char *key_id, ...);

int init_vin_systemd_server(int inital, int module_id);
int deinit_vin_systemd_server();

typedef enum _UID
{
	UID0 = 0x0,
	UID1,
	UID2,
	UID3, 
	UID4,
	UID5,
	UID_MAX

}E_UID;

typedef enum _UKEY
{
	UIN_KEY = -1,
	UOUT_KEY,	
	UKEY_MAX

}E_UKEY;

typedef enum _MODULE_ID
{
	VI_CORE = 0,
	VI_MPI,	
	VI_SAMPLE,
}E_MODULE_ID;

#endif
