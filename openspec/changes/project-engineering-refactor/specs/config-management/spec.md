## ADDED Requirements

### Requirement: Centralized configuration header
系统 SHALL 提供 `settings.h` 头文件，集中定义所有硬件相关常量，包括 GPIO 引脚、I2C 地址、WiFi 配置、显示参数、功率阈值等。

#### Scenario: GPIO pin defined in settings.h
- **WHEN** 开发者在代码中需要使用 GPIO 引脚号
- **THEN** 该引脚号 SHALL 从 `settings.h` 中引用，而非在源文件中硬编码数字字面量

#### Scenario: I2C address defined in settings.h
- **WHEN** 驱动初始化需要 I2C 设备地址
- **THEN** 地址 SHALL 从 `settings.h` 中引用，且可通过 PlatformIO build_flags 覆盖

### Requirement: Build-time configuration override
所有配置常量 SHALL 支持通过 PlatformIO build_flags 在 env 级别覆盖，无需修改源文件。

#### Scenario: Override WiFi SSID via build_flags
- **WHEN** 在 platformio.ini 的 env 中定义 `-DWIFI_SSID=\"my_network\"`
- **THEN** 编译后的固件 SHALL 使用 "my_network" 作为 WiFi SSID，而非 settings.h 中的默认值

#### Scenario: Override GPIO pin for different hardware
- **WHEN** 为不同硬件板卡创建新的 PlatformIO env
- **THEN** 新 env SHALL 能通过 build_flags 覆盖 settings.h 中的引脚定义，无需修改任何 .cpp/.h 文件

### Requirement: No hardcoded credentials in source
WiFi SSID、密码、固定 IP 地址等凭证 SHALL NOT 出现在任何 .cpp/.h 源文件中。

#### Scenario: OTA credentials from config
- **WHEN** OTA 模块启动 WiFi AP
- **THEN** SSID 和密码 SHALL 从配置宏读取，而非源文件中的字符串字面量
