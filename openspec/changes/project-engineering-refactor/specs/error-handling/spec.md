## ADDED Requirements

### Requirement: Driver error state query interface
每个驱动类 SHALL 提供 `hasError()` 和 `getLastError()` 方法，允许上层查询最后一次操作的错误状态。

#### Scenario: BQ40Z80 read failure detection
- **WHEN** BQ40Z80 的 I2C 读取操作失败
- **THEN** 后续调用 `bq.hasError()` SHALL 返回 true，`bq.getLastError()` SHALL 返回非零错误码

#### Scenario: Successful read clears error
- **WHEN** BQ40Z80 的 I2C 读取操作成功完成
- **THEN** `bq.hasError()` SHALL 返回 false

### Requirement: Error logging on driver failure
驱动层 SHALL 在检测到通信失败时通过 mylog 记录错误信息，包含设备名和操作类型。

#### Scenario: I2C communication failure logged
- **WHEN** I2C 读写操作超时或返回 NACK
- **THEN** 驱动 SHALL 调用 mylog.printf 记录 "I2C Error: [device] [operation] failed" 格式的错误信息

### Requirement: Safe default values on read failure
驱动读取函数在失败时 SHALL 返回安全的默认值（0），并通过 hasError() 标记错误状态。

#### Scenario: Voltage read returns 0 on failure
- **WHEN** 电压读取失败且错误未清除
- **THEN** 返回值 SHALL 为 0，且 hasError() SHALL 为 true，调用方可区分"有效 0V"和"读取失败"
