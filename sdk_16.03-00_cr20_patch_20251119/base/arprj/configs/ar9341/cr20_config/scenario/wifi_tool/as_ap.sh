
#!/bin/sh
if [ ! -d "/var/hostapd/" ]; then
    mkdir -p /var/hostapd
fi

if [ ! -d "/var/lib/misc/" ]; then
    mkdir -p /var/lib/misc                  # 创建dhcpd leases file路径，该路径是在bosybox开启udhcpd时配置的CONFIG_DHCPD_LEASES_FILE路径
fi

if [ ! -f "/var/lib/misc/udhcpd.leases" ]; then
    touch /var/lib/misc/udhcpd.leases       # 创建dhcpd leases file，该文件是在bosybox开启udhcpd时配置的CONFIG_DHCPD_LEASES_FILE文件
fi

ifconfig wlan0 up
sleep 1

ifconfig wlan0 192.168.0.1                  # 启动wlan0，并配置ip，该ip需要和udhcpd.conf文件配置的ip地址池保持同一网段
udhcpd -f ./udhcpd.conf &                   # 启动dhcp服务，提供给连接的设备分配ip的服务
sleep 1

./run/hostapd ./hostapd.conf -B              # 启动hostapd服务，提供热点

#./hostapd_cli -p /var/hostapd help         # hostapd_cli工具运行时通过socket与hostapd通信，需要-p指定socket文件路径，即hostapd.conf中配置的ctrl_interface路径