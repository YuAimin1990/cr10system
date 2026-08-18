# Project Brief
> - CHIP: AR9321
> - DDR: DDR4 32bit 1GB 2400MHz
> - FLASH: emmc
---

## SDK
> - Version: ar9341-ar-sdk-0.16.03-00-release-20231106

---
## Build Image
cd arprj  
./build_cr10.sh

---
## Blank burning
### tool
UsbUpgradeTool93-V1.1.5.exe（工具获取请联系Artosyn工程师）
### file
/arprj/release/prebuilds/uboot/uboot.nonsec.img  
/arprj/release/prebuilds/uboot/troot_spl.nonsec.img  
/arprj/release/prebuilds/image/artosyn-upgrade-ar9341.img
### method
设备上电按回车键进入uboot,输入artosyn_hid_upgrade 0 进入烧录模式  

---
## Code and test method

### wifi
#### code path
/arsdk/platforms/kmodule/ssv6x5x  
#### test

- 站点模式(STA):   
cd /var  
修改：wpa_supplicant.conf 中的`ssid` 和 `psk`（需要连接WIFI的id 和 passward）  
依次执行下列脚本  
as_start.sh  
as_station_ap.sh  
成功标志：ifconfig -a 分配到ip就成功了  

- 接入点模式(AP)：  
cd /var  
修改：hostapd.conf 中的`ssid` 和 `wpa_passphrase`  
依次执行下列脚本 
as_start.sh  
as_ap.sh  
成功标志：可以连接热点，连接成功就OK了，`ssid`和`wpa_passphrase`不改也可。
<!-- STA AP 不支持同时使用 -->

---
### sensor & LCD
#### code path
/arprj/apps/mpp_sample/vio/sample_vio_main.c -> SAMPLE_VIO_Only_use_default_attr()
#### test
##### sensor0:
test_mpp_vio -index 83 -sensor 0 -vo 1 &
##### sensor1:
test_mpp_vio -index 83 -sensor 1 -vo 1 &  
成功标志：屏幕显示sensor图像

---
### touch
#### code path
/arsdk/platforms/kmodule/GT1X
#### test
insmod /mod/GT1X.ko  
insmod /mod/ar_framebuffer.ko width=720 height=1440 format=0 （width height 为touch 面板宽高）  
cat /dev/input/event0 | hexdump -c  
成功标志：读到坐标值

---
### audio
#### code path
/arprj/apps/mpp_sample/sample_audio/sample_audio.c
#### test
##### speaker:
test_mpp_audio -O -o /usrdata/test.wav -m 1 -s 22050 -V 2  
成功标志：声音播放正常
##### mic:
test_mpp_audio -I -i /usrdata/ -D 0 -c 0 -m 1 -s 48000 -b 1 -v 28  
成功标志：本地生成三个wav文件，播放文件声音正常

---
