## ADDED Requirements

### Requirement: Device creation in main entry point
设备实例 SHALL 在 main.cpp 的 setup() 中显式构造，而非在模块全局作用域中 new。

#### Scenario: KKPortDevice created in setup
- **WHEN** 固件启动并执行 setup()
- **THEN** KKPortDevice 实例 SHALL 在 setup() 中通过局部变量构造，并通过接口指针传递给需要的模块

#### Scenario: No global device pointer
- **WHEN** 搜索所有 .cpp 文件
- **THEN** SHALL NOT 存在文件作用域的 `IPortDevice* device = new ...` 全局构造语句

### Requirement: Device injection via setter function
monitor_api 模块 SHALL 提供 setter 函数接受外部注入的设备指针，而非自行创建设备实例。

#### Scenario: Inject device via setter
- **WHEN** main.cpp 构造完 KKPortDevice 后
- **THEN** SHALL 调用 `monitor_api_set_device(devicePtr)` 将设备注入 monitor_api，monitor_api 不再自行 new 设备

#### Scenario: Device pointer stored internally
- **WHEN** monitor_api_set_device 被调用
- **THEN** monitor_api 内部 SHALL 存储该指针并在后续 updateUI() 等函数中使用

### Requirement: Driver instances as class members
KKPortDevice 中的驱动实例（BQ40Z80、SW6306、IP2366）SHALL 作为类成员变量持有，而非文件作用域全局变量。

#### Scenario: BQ40Z80 as member
- **WHEN** 查看 KKPortDevice 类定义
- **THEN** BQ40Z80、SW6306、IP2366 实例 SHALL 为 KKPortDevice 的成员变量或 unique_ptr 成员

#### Scenario: TwoWire as member
- **WHEN** 查看 KKPortDevice 类定义
- **THEN** TwoWire 实例 SHALL 为 KKPortDevice 的成员变量，而非文件作用域全局对象
