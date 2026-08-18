#!/bin/sh
echo "run yolov2 16bit..."

arcnndemo /usrdata/yolov2_1080p_16bit/0.json

diff /usrdata/yolov2_1080p_16bit/img-pad_001001.jpg_output.bin /usrdata/yolov2_1080p_16bit/output_16351.bin
if [ $? = 0 ]; then
	echo "+++Yolov2 16bit success!+++"
else
	echo "+++Yolov2 16bit failed!+++"
fi

echo "run yolov2 8bit..."

arcnndemo /usrdata/yolov2_1080p_8bit/0.json

diff /usrdata/yolov2_1080p_8bit/img-pad_001001.jpg_output.bin /usrdata/yolov2_1080p_8bit/output_4426.bin
if [ $? = 0 ]; then
	echo "+++Yolov2 8bit success!+++"
else
	echo "+++Yolov2 8bit failed!+++"
fi

