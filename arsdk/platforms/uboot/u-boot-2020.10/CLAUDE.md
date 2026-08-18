# CLAUDE.md - U-Boot 2020.10 for Artosyn AR9341/AR9301 Processors

## Overview
This repository contains U-Boot 2020.10 customized for Artosyn AR9341, AR9301, and AR9311 ARM64 processors. The codebase includes vendor-specific extensions for security, DDR training, and custom image generation.

## Architecture
- **CPU**: ARM64 (ARMv8 architecture)
- **SOC**: Artosyn AR9301/AR9311/AR9341 series
- **Build System**: Standard U-Boot Kbuild with vendor extensions
- **Security**: Custom secure boot implementation with vendor signing

## Target Configurations

### Available Defconfigs
- `ar9341_emmc_defconfig` - AR9341 with eMMC storage
- `ar9341_nand_defconfig` - AR9341 with NAND flash  
- `ar9341_nor_defconfig` - AR9341 with NOR flash
- `ar9311_emmc_defconfig` - AR9311 with eMMC storage
- `ar9311_nand_defconfig` - AR9311 with NAND flash
- `ar9311_nor_defconfig` - AR9311 with NOR flash

### Key Configuration Features
- ARM64 architecture with AArch32 support
- SPL (Secondary Program Loader) framework
- Device Tree support (OF_CONTROL)
- MMC/SD card support
- USB gadget and host support
- Networking (GMAC, TFTP, DHCP)
- Custom vendor drivers

## Build Commands

### Standard Build Process
```bash
# Configure for target
make ar9341_emmc_defconfig

# Build U-Boot
make -j$(nproc)

# Build with verbose output
make V=1
```

### Output Files
- `u-boot.bin` - Main U-Boot binary
- `spl/u-boot-spl.bin` - SPL binary
- `u-boot.dtb` - Device tree blob
- `u-boot.img` - U-Boot image with header

## Vendor Image Generation

The build system includes custom vendor image generation for secure boot:

### Non-Secure Images
```bash
# Generate vendor common image
python2 gen_vendor_image.py \
    --target=vendor.common.img \
    --version=1 \
    --ddr4_1d_imem=board/artosyn/common/ddr_firmware/ddr4_pmu_train_imem.bin \
    --ddr4_1d_dmem=board/artosyn/common/ddr_firmware/ddr4_pmu_train_dmem.bin \
    --ddr4_2d_imem=board/artosyn/common/ddr_firmware/ddr4_2d_pmu_train_imem.bin \
    --ddr4_2d_dmem=board/artosyn/common/ddr_firmware/ddr4_2d_pmu_train_dmem.bin \
    --ddr4_diag_imem=board/artosyn/common/ddr_firmware/ddr4_diags_imem.bin \
    --ddr4_diag_dmem=board/artosyn/common/ddr_firmware/ddr4_diags_dmem.bin \
    --ddr3_1d_imem=board/artosyn/common/ddr_firmware/ddr3_pmu_train_imem.bin \
    --ddr3_1d_dmem=board/artosyn/common/ddr_firmware/ddr3_pmu_train_dmem.bin

# Sign vendor image
./sign -i vendor.common.img -o vendor.nonsec.img -t uboot

# Generate SPL image
python2 gen_packed_bootloader.py \
    --out_file=troot_spl.nonsec.img \
    --spl_file=spl/u-boot-spl.bin \
    --spl_nodtb_file=spl/u-boot-spl-nodtb.bin \
    --spl_addr=<entry_address> \
    --chip=<chip_type>

# Generate U-Boot image
v_entry_addr=$(readelf -h u-boot | grep "Entry" | cut -d : -f2 | sed 's/[[:space:]]//g')
./mkimage -A arm64 -C none -d u-boot.bin -a $v_entry_addr -e $v_entry_addr -O linux -T firmware -n U-boot uboot.common.img
./sign -i uboot.common.img -o uboot.nonsec.img -t uboot -a $v_entry_addr
```

### Secure Images (with SEC_BOOT)
```bash
# Generate secure vendor image
python2 gen_vendor_image_sec.py \
    --target=vendor.common.img \
    --version=1 \
    --ddr4_1d_imem=board/artosyn/common/ddr_firmware/ddr4_pmu_train_imem.bin \
    --ddr4_1d_dmem=board/artosyn/common/ddr_firmware/ddr4_pmu_train_dmem.bin \
    --ddr4_2d_imem=board/artosyn/common/ddr_firmware/ddr4_2d_pmu_train_imem.bin \
    --ddr4_2d_dmem=board/artosyn/common/ddr_firmware/ddr4_2d_pmu_train_dmem.bin \
    --ddr4_diag_imem=board/artosyn/common/ddr_firmware/ddr4_diags_imem.bin \
    --ddr4_diag_dmem=board/artosyn/common/ddr_firmware/ddr4_diags_dmem.bin \
    --ddr3_1d_imem=board/artosyn/common/ddr_firmware/ddr3_pmu_train_imem.bin \
    --ddr3_1d_dmem=board/artosyn/common/ddr_firmware/ddr3_pmu_train_dmem.bin
```

## Key Directories and Files

### Board Support
- `board/artosyn/` - Artosyn-specific board code
- `board/artosyn/common/` - Common board components
- `board/artosyn/common/ddr_firmware/` - DDR training firmware binaries

### Device Tree
- `arch/arm/dts/` - Device tree source files
- `arch/arm/dts/artosyn-ar9301.dts` - Main device tree

### Drivers
- `drivers/mmc/` - MMC/SD card drivers
- `drivers/net/` - Networking drivers
- `drivers/usb/` - USB host/gadget drivers
- `drivers/watchdog/` - Watchdog drivers

## Development Workflow

### 1. Configuration Management
```bash
# List available configurations
find configs/ -name "*defconfig" | grep ar93

# Modify configuration
make menuconfig

# Save configuration
make savedefconfig
cp defconfig configs/ar9341_emmc_defconfig
```

### 2. Device Tree Modifications
```bash
# Edit device tree
vi arch/arm/dts/artosyn-ar9301.dts

# Recompile device tree
make dtbs
```

### 3. Debugging
```bash
# Enable debug output
make menuconfig  # Enable CONFIG_DEBUG_UART

# View memory map
make u-boot.sym

# Disassemble
make u-boot.dis
```

### 4. Clean Build
```bash
# Full clean
make distclean

# Partial clean
make clean
```

## Security Features

### Secure Boot Components
- Vendor image signing with custom `sign` tool
- DDR training firmware protection
- Secure image generation pipeline
- Non-secure fallback mode support

### DDR Training Firmware
The build includes these DDR training components:
- DDR4 1D training (IMEM/DMEM)
- DDR4 2D training (IMEM/DMEM)  
- DDR4 diagnostics (IMEM/DMEM)
- DDR3 1D training (IMEM/DMEM)

## Environment Variables

### Build Configuration
```bash
export CROSS_COMPILE=aarch64-linux-gnu-
export ARCH=arm64
```

### Custom Build Options
```bash
# Enable security build
export SEC_BOOT=y

# Specify chip type
export CHIP=ar9341
```

## Testing

### Basic Functionality Test
```bash
# Build and check basic functionality
make ar9341_emmc_defconfig
make -j8
ls -la u-boot.bin spl/u-boot-spl.bin

# Check image sizes
ls -la u-boot.bin spl/u-boot-spl.bin
```

### Image Validation
```bash
# Verify U-Boot image
./mkimage -l u-boot.img

# Check SPL entry point
readelf -h spl/u-boot-spl
```

## Troubleshooting

### Common Issues
1. **Missing toolchain**: Ensure aarch64-linux-gnu cross compiler is installed
2. **Python version**: Some scripts require Python 2
3. **DDR firmware missing**: Check board/artosyn/common/ddr_firmware/ directory
4. **Sign tool missing**: The `sign` binary must be built or provided

### Build Errors
- If `gen_vendor_image.py` fails: Check Python dependencies and file paths
- If `sign` tool fails: Verify the tool exists and has execute permissions
- If DDR firmware missing: Check all required .bin files are present

## References

### U-Boot Documentation
- [U-Boot README](https://github.com/u-boot/u-boot)
- [U-Boot Documentation](http://www.denx.de/wiki/U-Boot/Documentation)

### ARM64 Architecture
- [ARM Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/)
- [ARM Trusted Firmware](https://www.trustedfirmware.org/)

### Artosyn Resources
- Check vendor documentation for AR9341/AR9301 specifics
- Reference hardware manuals for register definitions

---
*This CLAUDE.md file was generated for future Claude Code instances to quickly understand and work with this U-Boot codebase.*