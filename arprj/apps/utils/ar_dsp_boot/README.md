# ar_dsp_boot

`ar_dsp_boot` 用于验证并启动 DSP（CEVA）固件，并可设置 DSP 运行频率。

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动构建；或在本目录执行：

- `make`
- `make clean`

## 运行

### 基本用法（来自源码打印）

```sh
./ar_dsp_boot 0 /mnt/ceva0.bin [freq_mhz]
```

- `dsp_id`: 目前源码只接受 `0`（否则直接打印 usage 并返回）
- `ceva_bin`: 固件路径，例如 `/mnt/ceva0.bin`
- `freq_mhz`（可选）: 频率，默认 `500`，范围校验 `100~1000`

程序流程：

- 校验 bin 头（`sirius_verify_ceva()`）
- `AR_MPI_DSP_PowerOn` + `AR_MPI_DSP_LoadBin` + `AR_MPI_DSP_EnableCore`
- `AR_MPI_DSP_Set_Status_Phyaddr(0, 0x32400000)`
- `AR_MPI_DSP_SetFrequency(0, freq)`

## 示例

```sh
./ar_dsp_boot 0 /mnt/ceva0.bin 500
```

## 注意事项

- 依赖 DSP/CEVA 相关驱动、MMZ、MPI DSP API 可用。
- `AR_MPI_DSP_Set_Status_Phyaddr` 固定使用 `0x32400000`，需要与 DSP 固件侧约定一致。
