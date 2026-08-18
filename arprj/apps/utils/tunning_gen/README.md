# tunning_gen

`tunning_gen`（tuning generation）用于生成 ISP/VIN tuning 参数的二进制文件（`.bin`）。

核心思路：

- `inc/*.h` 中保存了各传感器/场景的 `STRU_ALGO_ISP_TUNING_T` 结构体初始化数据
- `header_to_bin.c` 把这些结构体编译成一个小程序并运行，按固定列表导出为 `./bin/*.bin`
- `convert.sh` 负责调用 host gcc 编译并执行生成过程

## 构建

在 `arprj/apps/utils` 顶层执行 `make` 会自动进入本目录；但本目录的主要生成动作通常通过 `Makefile` 里的 `tuning` 目标触发：

```sh
make tuning
```

## 生成流程

### Makefile 目标 `tuning`

`Makefile` 中：

- `gCUSTOM_TUNING ?= ./tuning_ext.c`
- `tuning` 目标会执行：
  - `./convert.sh $(OUTPUT_HAL_DIR)/inc $(OUTPUT_KMODULE_DIR)/inc ./inc $(gCUSTOM_TUNING)`

### convert.sh

脚本内容（简化）：

- 清理旧输出：`rm -rf ./bin/*.bin`
- 编译：`gcc header_to_bin.c -I<hal_inc> -I<kmodule_inc> -I<inc> -DCUSTOM_TUNING=<tuning_ext.c>`
- 执行 `./a.out` 生成 bin

## 输出

- 默认输出路径：`./bin/*.bin`
- 生成后会 `chmod 777 -R ./bin/*.bin`

## 自定义扩展 tuning

`header_to_bin.c` 中支持：

```c
#ifdef CUSTOM_TUNING
#include PATH_STR(PATH(CUSTOM_TUNING))
#endif
```

你可以：

- 修改 `tuning_ext.c` 中的 `dump_custom`/`dump_custom[]` 列表
- 或在调用时覆盖 `gCUSTOM_TUNING` 指向其它 `.c` 文件

例如：

```sh
make tuning gCUSTOM_TUNING=./my_tuning_ext.c
```

## 注意事项

- `convert.sh` 使用的是 host 的 `gcc`（不是交叉编译器）；这是因为它只是把 header 初始化数据转换成 bin，不需要在 target 上运行。
- `header_to_bin.c` 会 `#include` 大量 `inc/*.h`，这些文件体积大，编译可能耗时较久。
- `header_to_bin.c` 里使用了 `write/close` 但缺少 `unistd.h` include，会产生告警（目前不影响构建通过）。
