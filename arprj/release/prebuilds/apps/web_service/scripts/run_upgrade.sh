#!/bin/sh

#/bin/udhcpc -b -i eth0 -s /etc/udhcpc.script -x lease:300 &
if [ ! -e /tmp/web/lighttpd/sbin/lighttpd ]; then
sleep 5
fi

/tmp/web/lighttpd/sbin/lighttpd -m /tmp/web/lighttpd/lib -f /tmp/web/lighttpd/config/lighttpd.conf &

upgrade_service -i