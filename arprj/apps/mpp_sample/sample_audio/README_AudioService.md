# Audio Service - IPC音频播放服务

## 概述

这是一个基于IPC（进程间通信）的音频播放服务，修改自原始的`sample_audio.c`程序。该服务通过UNIX域套接字接收命令，支持播放和停止音频文件。

## 功能特性

1. **服务模式运行**: 作为后台服务运行，等待IPC命令
2. **IPC通信**: 使用UNIX域套接字 (`/tmp/audio_service.sock`) 进行通信
3. **支持的命令**:
   - `play <file_name> <volume>` - 以指定音量播放WAV文件
   - `stop` - 停止当前播放
4. **自动停止**: 如果收到新的播放命令，会自动停止当前播放
5. **无文件不播放**: 如果没有指定音频文件，则不会播放

## 编译

```bash
make all
```

这将生成三个可执行文件：
- `test_mpp_audio` - 原始的音频测试程序
- `audio_service` - 音频服务程序
- `audio_client` - 客户端工具

## 使用方法

### 1. 启动音频服务

```bash
./audio_service
```

服务启动后会显示：
```
Audio Service Starting...
Audio Service Ready. Listening for commands...
```

### 2. 使用客户端发送命令

#### 播放音频文件
```bash
./audio_client play /path/to/audio.wav 10
```
- `play` - 播放命令
- `/path/to/audio.wav` - 音频文件路径
- `10` - 音量 (0-31)

#### 停止播放
```bash
./audio_client stop
```

### 3. 直接使用套接字通信

也可以直接向套接字发送命令：

```bash
# 播放音频
echo "play /path/to/audio.wav 15" | socat - UNIX-CONNECT:/tmp/audio_service.sock

# 停止播放
echo "stop" | socat - UNIX-CONNECT:/tmp/audio_service.sock
```

## 命令格式

### play 命令
```
play <filename> <volume>
```
- `filename`: WAV音频文件的完整路径
- `volume`: 音量级别 (0-31)
  - 0: 静音
  - 31: 最大音量
  - 推荐值: 5-15

### stop 命令
```
stop
```
无需额外参数，立即停止当前播放。

## 支持的音频格式

- **WAV文件**: 支持标准WAV格式
- **PCM原始数据**: 支持原始PCM数据文件
- **采样率**: 48000 Hz (默认)
- **位宽**: 16位 (默认)
- **声道**: 立体声 (默认)

## 服务管理

### 停止服务
使用 Ctrl+C 或发送 SIGTERM 信号：
```bash
kill <service_pid>
```

### 查看服务状态
```bash
ps aux | grep audio_service
```

### 检查套接字
```bash
ls -la /tmp/audio_service.sock
```

## 错误处理

### 常见错误及解决方法

1. **"Cannot connect to audio service"**
   - 确保音频服务正在运行
   - 检查套接字文件是否存在

2. **"open xxx.wav failed"**
   - 检查音频文件路径是否正确
   - 确保文件存在且可读

3. **"Wav file samplerate not match"**
   - 音频文件参数与系统配置不匹配
   - 使用48kHz, 16位, 立体声的WAV文件

4. **音量无效**
   - 音量范围必须在0-31之间

## 系统要求

- AR9341 SDK环境
- MPP (Media Process Platform) 库
- pthread库支持
- UNIX域套接字支持

## 示例脚本

### 播放测试脚本
```bash
#!/bin/bash
# play_test.sh

# 启动服务 (后台运行)
./audio_service &
SERVICE_PID=$!

# 等待服务启动
sleep 2

# 播放音频文件
./audio_client play /path/to/test.wav 10
sleep 5

# 停止播放
./audio_client stop

# 关闭服务
kill $SERVICE_PID
```

## 技术细节

### IPC通信协议
- **传输方式**: UNIX域套接字 (SOCK_STREAM)
- **套接字路径**: `/tmp/audio_service.sock`
- **消息格式**: 文本命令，以换行符结尾
- **响应格式**: "OK" 或 "ERROR: <message>"

### 线程模型
- **主线程**: 系统初始化和服务管理
- **IPC线程**: 处理客户端连接和命令解析
- **播放线程**: 音频数据读取和播放

### 资源管理
- 自动清理VB (Video Buffer) 资源
- 正确关闭音频设备
- 线程同步和互斥保护

## 故障排除

### 调试模式
服务运行时会输出详细日志，包括：
- 接收到的命令
- 音频文件信息
- 播放状态变化
- 错误信息

### 日志示例
```
Audio Service Starting...
Audio Service Ready. Listening for commands...
Received command: play test.wav 10
**********************
*** audio wav info:
*** SoundMode   :1 [0:MONO,1:STEREO]
*** BitWidth    :16 
*** SampleRate  :48000 
*** DataLen     :1920000 
**********************
Started playback: test.wav at volume 10
```

## 注意事项

1. 服务运行时会占用音频设备，其他音频程序可能无法使用
2. 播放大文件时会循环播放，直到收到停止命令
3. 服务异常退出时，套接字文件可能需要手动删除
4. 建议在生产环境中添加守护进程管理
