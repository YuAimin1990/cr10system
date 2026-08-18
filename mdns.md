# mDNS 实现文档

## 1. 编译 mdnsd

### 1.1 下载源码
```bash
git clone https://github.com/troglobit/mdnsd
cd mdnsd
```

### 1.2 配置交叉编译
```bash
export PATH="/opt/toolchain/7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin:$PATH"
export CROSS_COMPILE=aarch64-linux-gnu-
./configure --host=aarch64-linux-gnu --prefix=/path/to/install
```

### 1.3 编译安装
```bash
make
make install
```

## 2. mdnsd 使用说明

### 2.1 命令选项
```
Usage: mdnsd [-hnsv] [-i IFACE] [-l LEVEL] [-t TTL] [PATH]

Options:
    -h       显示帮助信息
    -i IFACE 指定网络接口，仅在该接口上发布服务
    -l LEVEL 设置日志级别: none, err, notice (默认), info, debug
    -n       在前台运行，不脱离控制终端
    -s       即使在前台运行也使用系统日志
    -t TTL   设置mDNS数据包的TTL，默认: 1（仅限本地链路）
    -v       显示程序版本

参数:
    PATH     mDNS-SD .service 文件路径，默认为: /etc/mdns.d

问题反馈: https://github.com/troglobit/mdnsd/issues
```

## 3. 集成到文件系统

### 3.1 安装文件
- 将编译生成的 mdnsd 二进制文件放入文件系统的 `/usr/sbin/` 目录
- 创建服务配置目录：`mkdir -p /etc/mdns.d`

### 3.2 创建启动脚本
创建 `/etc/init.d/mdnsd`：

```bash
#!/bin/sh /etc/rc.common

START=50
STOP=50

start() {
    echo "Starting mDNS daemon..."
    # -n: 前台运行
    # -i: 指定网络接口（可选）
    # /etc/mdns.d: 服务定义文件路径
    /usr/sbin/mdnsd -n -i eth0 /etc/mdns.d &
}

stop() {
    echo "Stopping mDNS daemon..."
    killall mdnsd
}
```

设置执行权限：
```bash
chmod +x /etc/init.d/mdnsd
```

## 4. 服务配置示例

### 4.1 SSH 服务示例
创建 `/etc/mdns.d/ssh.service`：
```
# mDNS-SD 广告 SSH 服务
type _ssh._tcp
port 22
```

### 4.2 HTTP 服务示例
创建 `/etc/mdns.d/http.service`：
```
# mDNS-SD 广告 HTTP 服务
type _http._tcp
port 80
```

## 5. 测试 mDNS 功能

### 5.1 启动服务
```bash
# 启动服务
/etc/init.d/mdnsd start

# 设置开机自启（如果支持）
/etc/init.d/mdnsd enable
```

### 5.2 在局域网内发现服务

#### 使用 mdns-scan 发现服务
```bash
# 安装工具
sudo apt install libavahi-compat-libdnssd-dev

# 发现所有 mDNS 服务
mdns-scan
```

#### 使用 avahi-browse 发现服务
```bash
# 安装工具
sudo apt install avahi-utils

# 发现所有 mDNS 服务
avahi-browse -at
```

#### 在 macOS 上发现服务
```bash
# 使用 dns-sd
dns-sd -B _services._dns-sd._udp
```

### 5.3 连接到发现的服务

发现服务后，您可以使用以下方式连接：

1. **对于 SSH 服务**：
   - 如果发现 `art_sirius._ssh._tcp.local`，使用：
     ```bash
     ssh art_sirius.local
     ```
   - 如果使用非标准端口，请使用 `-p` 参数指定端口

2. **对于 HTTP 服务**：
   - 如果发现 `art_sirius._http._tcp.local`，在浏览器中访问：
     ```
     http://art_sirius.local
     ```
   - 如果服务运行在非标准端口，请添加端口号：
     ```
     http://art_sirius.local:8080
     ```

> **注意**：
> - 确保您的系统支持 mDNS（在 Linux 上通常需要 `avahi-daemon` 或 `nss-mdns`）
> - 如果无法解析 `.local` 域名，请检查 `/etc/nsswitch.conf` 中是否包含 `mdns4_minimal` 和 `mdns4`
> - 某些网络环境可能需要启用 mDNS 支持

## 6. 故障排除

1. **服务未发现**
   - 确认防火墙允许 UDP 5353 端口的组播流量
   - 检查网络接口是否支持组播
   - 使用 `-l debug` 参数启动 mdnsd 查看详细日志

2. **服务启动失败**
   - 确认二进制文件有执行权限
   - 检查配置文件的路径和权限
   - 查看系统日志获取错误信息

3. **服务定义不生效**
   - 确认服务定义文件位于正确的目录
   - 检查服务定义文件的格式是否正确
   - 重启 mdnsd 服务使配置生效

## 7. 参考资源

- [mdnsd GitHub 仓库](https://github.com/troglobit/mdnsd)
- [mDNS 协议规范 (RFC 6762)](https://tools.ietf.org/html/rfc6762)
- [DNS-SD 服务发现 (RFC 6763)](https://tools.ietf.org/html/rfc6763)
avahi-browse -at  # Linux
dns-sd -B _http._tcp  # macOS

```

### 资源占用预估

- 二进制大小：约200KB
- 内存占用：约500KB
- CPU占用：空闲时接近0%，响应查询时有短暂峰值

### 注意事项

- 确保设备的主机名已正确设置（/etc/hostname）
- 如果使用防火墙，需要允许UDP 5353端口的入站流量
- 对于生产环境，建议添加监控和自动重启机制

