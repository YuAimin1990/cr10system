# Sample Audio Refactored - Professional C Implementation

## Overview

This is a professional refactored version of the original `sample_audio.c` program. The refactored code follows C language best practices and modern software engineering principles while maintaining full compatibility with the AR MPP audio framework.

## Key Improvements

### 1. **Code Organization & Structure**
- **Modular Design**: Code is organized into logical modules with clear separation of concerns
- **Header Organization**: Proper header file organization with system headers first, then project headers
- **Function Organization**: Functions are logically grouped and properly documented

### 2. **Error Handling & Logging**
- **Consistent Error Handling**: Unified error handling using `CHECK_RESULT` macro
- **Professional Logging**: Structured logging with different levels (ERROR, WARN, INFO)
- **Graceful Degradation**: Proper error recovery and resource cleanup

### 3. **Memory Management**
- **Resource Cleanup**: Systematic cleanup of all allocated resources
- **RAII-style Management**: Clear initialization and cleanup patterns
- **Memory Safety**: Proper bounds checking and validation

### 4. **Thread Safety & Concurrency**
- **Volatile Flags**: Proper use of volatile for thread communication
- **Clean Thread Management**: Structured thread creation and termination
- **Resource Synchronization**: Proper handling of shared resources

### 5. **Code Quality & Maintainability**
- **Consistent Naming**: Professional naming conventions throughout
- **Documentation**: Comprehensive function documentation with Doxygen-style comments
- **Type Safety**: Strong typing with proper type definitions
- **Const Correctness**: Proper use of const where applicable

### 6. **Configuration Management**
- **Centralized Configuration**: All configuration in structured data types
- **Parameter Validation**: Comprehensive input validation
- **Default Values**: Sensible default configurations

## Architecture

### Data Structures

```c
typedef struct {
    /* Audio configuration */
    AUDIO_SAMPLE_RATE_E sample_rate;
    AUDIO_SOUND_MODE_E  sound_mode;
    AUDIO_BIT_WIDTH_E   bit_width;
    
    /* Device configuration */
    AUDIO_DEV ai_dev_id;
    AI_CHN    ai_channel;
    AUDIO_DEV ao_dev_id;
    AO_CHN    ao_channel;
    
    /* Volume settings */
    int32_t ai_volume;
    int32_t ao_volume;
    
    /* File paths and operation flags */
    // ... additional fields
} audio_config_t;

typedef struct {
    /* Thread control */
    volatile bool thread_run_flag;
    volatile bool ai_exit;
    volatile bool ao_exit;
    
    /* Thread handles and system resources */
    // ... additional fields
} audio_context_t;
```

### Module Structure

1. **Configuration Module**: Parameter parsing and validation
2. **System Module**: VB and SYS initialization/cleanup
3. **Audio Module**: AI/AO device management
4. **Thread Module**: Audio processing threads
5. **Utility Module**: Helper functions and utilities

## Usage

### Command Line Options

```bash
./sample_audio_refactored [OPTIONS]

Options:
  -I              Enable AI (Audio Input)
  -i <path>       AI output file path
  -O              Enable AO (Audio Output)
  -o <path>       AO input file path
  -s <rate>       Sample rate (8000, 16000, 32000, 48000)
  -D <id>         Device ID
  -c <id>         Channel ID
  -b <width>      Bit width (16, 24, 32)
  -m <mode>       Sound mode (0=mono, 1=stereo)
  -B              Enable AI bind to AO
  -S              Enable AI send frame to AO
  -v <volume>     AI volume (0-31)
  -V <volume>     AO volume (0-31)
  -t              Test AI acodec
  -T              Test AO acodec
  -h              Display help
```

### Examples

```bash
# Audio playback
./sample_audio_refactored -O -o /path/to/audio.wav -V 15

# Audio capture
./sample_audio_refactored -I -i /tmp/capture/ -v 20

# AI to AO passthrough
./sample_audio_refactored -I -O -S -v 20 -V 15

# AI bind to AO (hardware binding)
./sample_audio_refactored -I -O -B -v 20 -V 15
```

## Building

### Prerequisites
- AR MPP SDK properly installed
- GCC compiler with C99 support
- Make utility

### Compilation
```bash
# Using the provided Makefile
make sample_audio_refactored

# Manual compilation
gcc -std=c99 -Wall -Wextra -O2 \
    -I$(AR_SDK_PATH)/include \
    -L$(AR_SDK_PATH)/lib \
    sample_audio_refactored.c \
    -lmpi_audio -lmpi_vb -lmpi_sys -lmpi_sysctl \
    -lpthread -o sample_audio_refactored
```

## Thread Architecture

### Main Thread
- Parameter parsing and validation
- System initialization
- Thread management
- Signal handling
- Resource cleanup

### AI Capture Thread (`ai_capture_thread`)
- Captures audio from AI device
- Saves to WAV file with proper header
- Handles frame acquisition and release

### AO Playback Thread (`ao_playback_thread`)
- Reads WAV files
- Validates audio format
- Sends frames to AO device
- Supports looping playback

### AI-to-AO Thread (`ai_to_ao_thread`)
- Real-time audio passthrough
- Low-latency frame forwarding
- Proper frame lifecycle management

## Error Handling

### Error Categories
1. **System Errors**: VB/SYS initialization failures
2. **Audio Errors**: AI/AO device failures
3. **File Errors**: File I/O operations
4. **Parameter Errors**: Invalid configuration
5. **Resource Errors**: Memory/thread allocation

### Error Recovery
- Graceful degradation on non-critical errors
- Proper resource cleanup on failures
- Informative error messages with context
- Exit codes for automation

## Performance Considerations

### Optimizations
- Efficient memory management with VB pools
- Minimal memory copying in audio paths
- Proper thread scheduling and priorities
- Optimized buffer sizes for low latency

### Resource Usage
- Configurable buffer sizes
- Efficient thread synchronization
- Minimal system call overhead
- Proper resource pooling

## Debugging & Monitoring

### Logging Levels
- **ERROR**: Critical failures requiring attention
- **WARN**: Non-critical issues that may affect performance
- **INFO**: Normal operational information

### Debug Features
- Thread lifecycle logging
- Audio format validation
- Resource allocation tracking
- Performance metrics

## Compatibility

### Original Features
- All original functionality preserved
- Command-line compatibility maintained
- Same audio quality and performance
- Identical hardware interface

### Improvements
- Better error reporting
- More robust operation
- Enhanced logging
- Cleaner shutdown

## Testing

### Unit Testing
- Parameter validation tests
- Error handling verification
- Resource cleanup validation

### Integration Testing
- End-to-end audio pipeline testing
- Multi-threading stress tests
- Hardware compatibility verification

### Performance Testing
- Latency measurements
- Memory usage profiling
- CPU utilization analysis

## Future Enhancements

### Potential Improvements
1. **Configuration Files**: Support for config file input
2. **Plugin Architecture**: Modular audio processing plugins
3. **Network Streaming**: Network audio streaming capabilities
4. **Advanced Codecs**: Support for additional audio formats
5. **Real-time Monitoring**: Performance monitoring interface

### Maintenance
- Regular code reviews
- Performance profiling
- Security audits
- Documentation updates

## License

This refactored code maintains the same license as the original AR MPP sample code.

## Contributing

When contributing to this codebase:
1. Follow the established coding style
2. Add comprehensive documentation
3. Include proper error handling
4. Write unit tests for new features
5. Update this README for significant changes
