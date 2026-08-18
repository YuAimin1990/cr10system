#!/bin/sh
echo "run web service ......."

WORK_HOME=/usrdata
export LD_LIBRARY_PATH=/usrdata/local/web_service/lib:$LD_LIBRARY_PATH
cp -r $WORK_HOME/local/web_service/web /tmp/
cp $WORK_HOME/local/web_service/scripts/shutdown.sh /tmp/
sync

ifconfig lo up
sleep 5 #systemd wait binder server

echo "start rtsp to websocket proxy"
$WORK_HOME/local/web_service/bin/rtsp_ws_proxy -c 0 &

echo "start upgrade service"
$WORK_HOME/local/web_service/bin/upgrade_service&
#wait to avoid conf file readwrite conflict
sleep 1

echo "start systemd"
$WORK_HOME/local/web_service/bin/systemd &

echo "start web_service"
mkdir -p /tmp/web/lighttpd/log/
/tmp/web/lighttpd/sbin/lighttpd -m /tmp/web/lighttpd/lib -f /tmp/web/lighttpd/config/lighttpd_normal.conf
exit
