# AR9341 固件构建指南

本文档详细描述了基于AR9341芯片的固件构建流程和镜像布局。

## 1. 构建命令

```bash
cd arprj
./build_cr10.sh
```

或手动指定参数：

```bash
make BOOT_MEDIA=emmc BASE_BOARD=cr10 BUILD_TYPE=release APP=ipcam CHIP=ar9341 SEC_BOOT=n MPP_SERVICE=app all
```

### 1.1 分区表参数说明
- `BOOT_MEDIA`: 启动介质 (emmc/nand/nor)
- `BASE_BOARD`: 开发板型号 (cr10)
- `BUILD_TYPE`: 构建类型 (release/debug/debug1)
- `APP`: 应用程序 (ipcam)
- `CHIP`: 芯片型号 (ar9341)
- `SEC_BOOT`: 安全启动 (y/n)
- `MPP_SERVICE`: 媒体处理服务 (app)

## 2. 构建流程

### 2.1 构建目标

1.1. **product_config**
   - 验证构建参数
   - 为目标板(cr10)设置配置

1.2. **uboot**
   - 编译U-Boot引导加载程序

1.3. **kernel**
   - 编译Linux内核

1.4. **dtb**
   - 编译设备树

1.5. **rootfs**
   - 准备根文件系统

1.6. **kmodule**
   - 编译内核模块

1.7. **single_app**
   - 编译主应用程序(ipcam)

1.8. **strip**
   - 去除二进制文件中的调试符号以减小体积

1.9. **fs_img**
   - 创建文件系统镜像

1.10. **image**
    - 生成最终可启动镜像

## 3. eMMC 闪存布局

### 3.1 分区表

| 分区 | 大小 | 路径 | 可升级 | 说明 |
|------|-----|------|--------|------|
| bootloader | 256KB | release/prebuilds/uboot/troot_spl.img | 是 | 引导加载程序 |
| vendor | 512KB | release/prebuilds/uboot/vendor.img | 是 | 厂商相关 |
| factory | 512KB | - | 否 | 工厂分区 |
| env | 512KB | release/prebuilds/uboot/u-boot-env | 是 | U-Boot环境变量 |
| uboot | 1MB | release/prebuilds/uboot/uboot.img | 是 | 主U-Boot镜像 |
| kernel | 48MB | release/prebuilds/kernel/kernel.img | 是 | 内核镜像 |
| dtb | 512KB | release/prebuilds/kernel/artosyn-kernel.dtb | 是 | 设备树 |

## 4. 输出文件

### 4.1 主要镜像文件

构建完成后，在`release/out`目录下会生成以下文件：

- `emmc.img`: 可启动的eMMC镜像
- `artosyn-upgrade-ar9341.img`: 升级镜像
- (如果是NAND) `nand.csv`: NAND闪存配置

### 4.2 中间文件

## 5. 关键工具

### 5.1 镜像生成处理工具

1.1. **genimagetool-program**
   - 生成可启动程序镜像

1.2. **genimagetool-upgrade**
   - 生成升级镜像

1.3. **generate_ubi.sh**
   - 生成UBI文件系统(NAND闪存使用)

1.4. **generate_nand_configs_sofi.sh**
   - 生成NAND闪存配置

## 6. 目录结构

### 6.1 主要目录

- **release/prebuilds/**
  - 包含预编译的U-Boot、内核等组件
  - 是生成最终镜像的源文件所在

- **release/out/**
  - 最终生成的镜像文件输出目录

## 7. 升级说明

### 7.1 升级方式

1.1. 使用`artosyn-upgrade-ar9341.img`进行升级
1.2. 升级工具: UsbUpgradeTool93-V1.1.5.exe
1.3. 升级文件:
   - uboot.nonsec.img
   - troot_spl.nonsec.img
   - artosyn-upgrade-ar9341.img

## 8. 注意事项

### 8.1 环境要求

1.1. 修改配置后建议先执行`make clean`
1.2. 确保有足够的磁盘空间(建议至少20GB)
1.3. 构建环境要求:
   - 64位Linux系统
   - Python 2.7
   - 标准构建工具(make, gcc等)
   - 交叉编译工具链

### 8.2 常见问题

#### 构建失败
- 检查是否所有依赖都已安装
- 检查磁盘空间是否充足
- 查看详细的错误日志进行排查

#### 镜像刷写失败
### 镜像刷写失败
- 确认使用的烧录工具版本正确
- 检查硬件连接是否正常
- 确认目标设备支持所选启动介质

## 10. 版本信息

- SDK版本: ar9341-ar-sdk-0.16.03-00-release-20231106
- 最后更新: 2023-11-06
