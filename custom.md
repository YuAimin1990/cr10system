
## start logo

准备好 start_logo2.png, 720(W)x1440(H) 大小(左上角将显示在LCD的右上角，显示是相当于顺时针旋转90度)，将其转换为 start_logo2.yuv

```
ffmpeg -i start_logo2.png -pix_fmt yuv420p -y base/arprj/configs/ar9341/zam210_config/scenario/start_logo2.yuv
```

然后修改 base/arprj/configs/ar9341/zam210_config/plat/flash_layout_emmc.ini 中：

```
[partition8]
name=logo
path=configs/ar9341/zam210_config/scenario/start_logo2.yuv
size=0x400000
upgrade=y
```

在既有的设备上，也可以用如下命令直接修改 flash：

```
dd if=start_logo2.yuv of=/dev/mmcblk0p9 bs=4K
```

## flash 空间划分

修改 base/arprj/configs/ar9341/zam210_config/plat/flash_layout_emmc.ini。注意:

- `[flash]` 规定的容量不能大于实际芯片的容量，否则烧写映像会失败;
- 以下个分区总计不能超过 `[flash]` 规定的容量

