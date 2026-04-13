# 项目工程化重构计划

## TL;DR

> **目标**: 将 ESP32 固件项目从原型代码全面重构为分层架构 — 配置集中化、依赖注入、UI解耦、错误处理、测试基础设施
> 
> **交付物**:
> - settings.h 配置集中管理
> - ui_presenter UI展示层解耦
> - 依赖注入替代全局单例
> - Unity 测试框架 + CI 集成
> - 所有超长函数拆分、Bug 修复、命名统一
> 
> **工作量**: XL（约 37 个任务，5 个阶段）
> **并行执行**: YES - 5 个 Wave
> **关键路径**: Phase 0(测试基础) → Phase 1(Bug修复) → Phase 2(配置集中化) → Phase 3(架构解耦) → Phase 4(工程化)

---

## Context

### 原始请求
用户要求"帮我把项目工程化，专业化，架构化"。

### 访谈摘要
- 通过 3 个 explore 代理全面分析了代码库结构、代码质量、模块耦合
- 确认了全局裸指针单例、UI-设备耦合、无测试、硬编码凭证等核心问题
- 通过 Metis 咨询识别了 SIOF 风险、I2C 总线共享约束、`#elifdef` 兼容性等关键隐患

### Metis 审查（已解决的缺口）
- **SIOF Bug**: `new KKPortDevice()` 在文件作用域构造，与 `TwoWire wire1(1)` 存在静态初始化顺序风险 → Phase 3 通过 DI 解决
- **I2C 总线共享**: bq40z80/SW6306/IP2366 共享单个 TwoWire → DI 必须传递共享引用
- **`#elifdef` C++23 语法**: main.cpp 可能不兼容 ESP32 GCC → Phase 1 修复
- **OTA 验证被 runSelfTest bug 阻塞**: 必须首先修复 → Phase 1 Task 1
- **Phase 重排序**: 原计划 Phase 4=测试，应改为 Phase 0=测试基础设施（TDD 前提）
- **extern "C" 签名冻结**: 13 个 C API 函数签名不可变更
- **二进制大小增长限制**: 每阶段 ≤ 5%

---

## Work Objectives

### 核心目标
建立清晰的分层架构（配置层→驱动层→设备抽象层→业务逻辑层→UI展示层），消除全局状态，引入工程化工具链。

### 具体交付物
- `.clang-format` 代码格式化配置
- `main/settings.h` 集中配置头文件
- `main/data_types.h` POD 数据结构体
- `main/ui_presenter.h/cpp` UI 展示层
- `main/monitor_api.h` 重构后的 C API（签名不变）
- `test/` 目录下的 Unity 单元测试
- 所有 Bug 修复和超长函数拆分

### 完成定义
- [ ] `pio run -e esp32_D0WDQ6` 编译通过
- [ ] `pio run -e esp32_D0WDQ6_201` 编译通过
- [ ] `pio run -e emulator_64bits` 编译通过
- [ ] 所有 13 个 extern "C" 函数签名未变
- [ ] monitor_api.cpp 中零 `lv_label_*` 直接调用
- [ ] 全局裸指针 `new` 仅出现在 setup() 中
- [ ] 硬编码 magic number 数量为零

### Must Have
- 所有 Bug 修复（runSelfTest、extern C、#elifdef、scheduler 注释）
- 配置集中化（settings.h）
- 依赖注入（消除全局裸指针）
- UI Presenter 层（解耦 LVGL）
- 编译通过所有 env

### Must NOT Have（护栏）
- ❌ 不引入 RTOS、线程、事件总线、状态机、Observer 模式
- ❌ 不重写 ui.c/ui_schome.c（LVGL 生成代码冻结）
- ❌ 不新增外部依赖库
- ❌ 不修改 TaskScheduler.h 内部实现
- ❌ 不在空占位目录（devices/、core/、services/、proto/）中创建文件
- ❌ 不重命名文件而不更新 platformio.ini build_src_filter
- ❌ 不创建新的 LVGL 对象（ui_presenter 只读不创建）
- ❌ 不在 main loop 中引入 new/malloc 动态分配
- ❌ 二进制大小每阶段增长不超过 5%

---

## Verification Strategy

> **零人工干预** — 所有验证由代理执行。

### 测试决策
- **基础设施存在**: NO
- **自动化测试**: YES（Phase 0 建立 Unity 框架，随后 TDD）
- **框架**: Unity（PlatformIO 原生）

### QA 策略
每个任务包含代理执行的 QA 场景。证据保存在 `.sisyphus/evidence/`。
- **编译验证**: `pio run -e esp32_D0WDQ6` 每个任务执行
- **代码检查**: grep/ast_grep 验证特定模式
- **单元测试**: Unity 测试在 emulator 环境运行

---

## Execution Strategy

### 并行执行 Wave

```
Wave 0 (Phase 0 - 测试基础设施，立即可开始):
├── Task 1: 验证 Unity + emulator 兼容性 [quick]
├── Task 2: 配置 Unity 测试框架 [quick]
└── Task 3: 创建 extern "C" 桥接特征测试 [unspecified-high]

Wave 1 (Phase 1 - Bug 修复，Wave 0 之后):
├── Task 4: 修复 runSelfTest() 返回值 [quick]
├── Task 5: 修复 #elifdef 预处理器兼容性 [quick]
├── Task 6: 修复 auto_power_off scheduler 间隔 [quick]
├── Task 7: 修复 SW6306.h extern "C" 拼写 [quick]
└── Task 8: 移除 SPIFFS→LittleFS 宏别名 [quick]

Wave 2 (Phase 2 - 配置集中化，Wave 1 之后):
├── Task 9: 创建 settings.h 配置头文件 [unspecified-high]
├── Task 10: 更新 ota.cpp 使用配置宏 [quick]
├── Task 11: 更新 KKPortDevice.cpp 使用配置宏 [quick]
├── Task 12: 更新 hal/app_hal.cpp 和 main.cpp 配置引用 [quick]
├── Task 13: 移动生成代码到 generated/ 并更新 build_src_filter [unspecified-high]
├── Task 14: 清理重复文件和空目录 [quick]

Wave 3 (Phase 3 - 架构解耦，Wave 2 之后):
├── Task 15: 创建 data_types.h POD 数据结构体 [quick]
├── Task 16: 重构 KKPortDevice — 驱动实例改为类成员 [deep]
├── Task 17: DI: 移除全局 device 单例，main 中构造注入 [deep]
├── Task 18: 创建 ui_presenter 层 [visual-engineering]
├── Task 19: 重构 monitor_api — 拆分 updateUI 使用 presenter [deep]
├── Task 20: 拆分 main.cpp setup() 为子函数 [unspecified-high]
├── Task 21: 拆分 ota.cpp setup_ota() 为子函数 [unspecified-high]
├── Task 22: 拆分 KKPortDevice::getPortState() 辅助方法 [quick]
├── Task 23: 移除 monitor_api.h extern device 全局暴露 [quick]
├── Task 24: 移除 cell_helper.h 对 lvgl.h 的依赖 [quick]

Wave 4 (Phase 4 - 工程化完善，Wave 3 之后):
├── Task 25: BQ40Z80 添加 hasError/getLastError [quick]
├── Task 26: IP2366 添加 hasError/getLastError [quick]
├── Task 27: SW6306 添加 hasError/getLastError [quick]
├── Task 28: 驱动层 I2C 失败日志增强 [quick]
├── Task 29: 添加 .clang-format 配置文件 [quick]
├── Task 30: 创建单元测试 test_cell_helper + test_utils [unspecified-high]
├── Task 31: 统一 C API 函数命名 [unspecified-high]
├── Task 32: 更新 utils.h static→inline [quick]
├── Task 33: 运行 clang-format 格式化全项目 [quick]
├── Task 34: 全 env 编译验证 [quick]

Wave FINAL (所有任务完成后 — 4 个并行审查):
├── F1: 计划合规审计 (oracle)
├── F2: 代码质量审查 (unspecified-high)
├── F3: 实际 QA 验证 (unspecified-high)
└── F4: 范围保真检查 (deep)
→ 呈现结果 → 获得用户确认

关键路径: Task 1 → Task 4 → Task 9 → Task 17 → Task 19 → Task 34 → F1-F4
并行加速: ~60%
最大并发: 5 (Wave 2)
```

### 依赖矩阵

| 任务 | 依赖 | 阻塞 | Wave |
|------|------|------|------|
| 1-3 | - | 4-8 | 0 |
| 4-8 | 1-3 | 9-14 | 1 |
| 9 | 4-8 | 10-12 | 2 |
| 10-12 | 9 | 16 | 2 |
| 13-14 | 4-8 | - | 2 |
| 15 | 9 | 16,17,18 | 3 |
| 16 | 15 | 17,22 | 3 |
| 17 | 16 | 19,20,23 | 3 |
| 18 | 15 | 19 | 3 |
| 19 | 17,18 | 31 | 3 |
| 20-21 | 17 | - | 3 |
| 22 | 16 | - | 3 |
| 23-24 | 17,19 | - | 3 |
| 25-28 | 19 | - | 4 |
| 29,33 | 19 | - | 4 |
| 30 | 29 | 34 | 4 |
| 31 | 19 | 34 | 4 |
| 32 | - | 34 | 4 |
| 34 | 30,31 | F1-F4 | 4 |

### Agent Dispatch Summary

| Wave | Tasks | Agents |
|------|-------|--------|
| 0 | 3 | quick, unspecified-high |
| 1 | 5 | quick × 5 |
| 2 | 6 | unspecified-high, quick × 5 |
| 3 | 10 | quick × 5, deep × 3, visual-engineering, unspecified-high |
| 4 | 10 | quick × 8, unspecified-high × 2 |
| FINAL | 4 | oracle, unspecified-high × 2, deep |

---

## TODOs

- [x] 1. 验证 Unity + emulator 兼容性

  **What to do**:
  - 在 emulator_64bits 环境运行 `pio test`，确认 Unity 测试框架可用
  - 创建最小测试文件 `test/test_main/main_test.cpp` 验证框架运行
  - 如果 Unity 不可用，评估替代方案（如直接在 emulator main 中添加 assert）

  **Must NOT do**: 不修改任何生产代码

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 0, 与 Task 2, 3 并行
  - **Blocks**: Task 4-8
  - **Blocked By**: None

  **References**:
  - `platformio.ini` — 确认 emulator_64bits env 配置
  - `test/` — 当前仅有 README，无测试

  **Acceptance Criteria**:
  - [ ] `pio test -e emulator_64bits` 成功运行（或确认 Unity 不可用并提供替代方案）

  **QA Scenarios**:
  ```
  Scenario: Unity test framework runs in emulator
    Tool: Bash
    Steps:
      1. 创建 test/test_main/main_test.cpp 包含一个简单的 TEST(assert_true)
      2. 运行 pio test -e emulator_64bits
      3. 检查退出码为 0
    Expected Result: 测试通过，Unity 输出 "1 Tests, 0 Failures"
    Evidence: .sisyphus/evidence/task-1-unity-verify.txt
  ```

  **Commit**: YES (groups with Phase 0)
  - Message: `test: add Unity framework config and placeholder test`

- [x] 2. 配置 Unity 测试框架

  **What to do**:
  - 在 platformio.ini 中添加测试环境配置（如需要）
  - 创建测试目录结构 test/test_main/
  - 添加 platformio.ini 的 `[env:native]` 或配置 emulator_64bits 支持测试
  - 创建 test/test_main/main_test.cpp 基础测试文件

  **Must NOT do**: 不修改生产代码

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 0, 与 Task 1, 3 并行
  - **Blocks**: Task 30
  - **Blocked By**: None

  **References**:
  - `platformio.ini` — 理解 env 继承和 build 配置

  **Acceptance Criteria**:
  - [ ] test/ 目录包含至少一个可执行的测试文件
  - [ ] 测试框架配置正确

  **QA Scenarios**:
  ```
  Scenario: Test directory has valid structure
    Tool: Bash
    Steps:
      1. ls test/ — 确认目录结构
      2. 运行 pio test -e emulator_64bits（如果支持）
    Expected Result: 测试运行并报告结果
    Evidence: .sisyphus/evidence/task-2-test-config.txt
  ```

  **Commit**: YES (groups with Phase 0)

- [x] 3. 创建 extern "C" 桥接特征测试

  **What to do**:
  - 创建 test/test_bridge/ 测试目录
  - 为 monitor_api.h 中 13 个 extern "C" 函数编写特征测试
  - 测试内容：每个函数返回值类型是否正确（不测试实际硬件读取，仅测试接口签名）
  - 需要创建 Mock IPortDevice 实现

  **Must NOT do**: 不修改 monitor_api.h 的函数签名

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: []

  **Parallelization**: Wave 0, 与 Task 1, 2 并行
  - **Blocks**: Task 19
  - **Blocked By**: None

  **References**:
  - `main/monitor_api.h` — 13 个 extern "C" 函数声明
  - `main/PortDevices.h` — IPortDevice 接口定义（创建 Mock 需要）

  **Acceptance Criteria**:
  - [ ] test/test_bridge/ 目录存在
  - [ ] 每个 extern "C" 函数有对应的特征测试

  **QA Scenarios**:
  ```
  Scenario: Bridge tests cover all extern C functions
    Tool: Bash
    Steps:
      1. grep "extern.*C" main/monitor_api.h | wc -l — 统计函数数量
      2. grep "TEST(" test/test_bridge/ — 确认每个函数有测试
    Expected Result: 测试数量 ≥ extern "C" 函数数量
    Evidence: .sisyphus/evidence/task-3-bridge-tests.txt
  ```

  **Commit**: YES (groups with Phase 0)

- [x] 4. 修复 runSelfTest() 返回值

  **What to do**:
  - 修改 main/main.cpp:40 将 `return false;` 改为 `return true;`
  - 这是 OTA 验证阻塞 Bug — 当前所有 OTA 升级都会触发回滚

  **Must NOT do**: 不修改其他任何逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 1, 与 Task 5-8 并行
  - **Blocks**: 后续所有任务（OTA 验证必需）
  - **Blocked By**: Task 1-3

  **References**:
  - `main/main.cpp:35-41` — runSelfTest() 函数
  - `main/main.cpp:43-73` — checkPendingAndValidate() 调用 runSelfTest()

  **Acceptance Criteria**:
  - [ ] `grep -n "return" main/main.cpp` 显示 runSelfTest() 返回 true
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: runSelfTest returns true
    Tool: Bash
    Steps:
      1. grep -A5 "runSelfTest" main/main.cpp
      2. 确认函数体包含 "return true"
    Expected Result: runSelfTest() 返回 true
    Evidence: .sisyphus/evidence/task-4-runselftest-fix.txt

  Scenario: Build passes after fix
    Tool: Bash
    Steps:
      1. pio run -e esp32_D0WDQ6
    Expected Result: SUCCESS
    Evidence: .sisyphus/evidence/task-4-build.txt
  ```

  **Commit**: YES (Phase 1)
  - Message: `fix: runSelfTest() returns true to prevent OTA rollback`

- [x] 5. 修复 #elifdef 预处理器兼容性

  **What to do**:
  - 检查 main.cpp 中 `#elifdef ESP32_169` 是否为 C++23 语法
  - 如果是，替换为 `#elif defined(ESP32_169)` 以兼容旧版 GCC
  - 验证 esp32_D0WDQ6 和 esp32_D0WDQ6_201 env 均编译通过

  **Must NOT do**: 不改变代码分支逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 1, 与 Task 4, 6-8 并行
  - **Blocked By**: Task 1-3

  **References**:
  - `main/main.cpp:98-106` — #ifdef/#elifdef 条件编译块

  **Acceptance Criteria**:
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过
  - [ ] `pio run -e esp32_D0WDQ6_201` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Both envs compile
    Tool: Bash
    Steps:
      1. pio run -e esp32_D0WDQ6
      2. pio run -e esp32_D0WDQ6_201
    Expected Result: Both SUCCESS
    Evidence: .sisyphus/evidence/task-5-elifdef-fix.txt
  ```

  **Commit**: YES (Phase 1)

- [x] 6. 修复 auto_power_off scheduler 间隔

  **What to do**:
  - 确认 auto_power_off 的预期执行间隔（注释说 2 秒，参数为 1ms 单位）
  - 将 scheduler.addTask(auto_power_off, 1) 的间隔修正为合理值
  - 更新注释使其与代码一致

  **Must NOT do**: 不改变 auto_power_off 的逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 1, 与 Task 4-5, 7-8 并行
  - **Blocked By**: Task 1-3

  **References**:
  - `main/main.cpp:145` — scheduler.addTask(auto_power_off, 1)
  - `main/base.cpp` — auto_power_off() 实现理解间隔需求

  **Acceptance Criteria**:
  - [ ] scheduler.addTask 间隔参数与注释一致
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Interval matches comment
    Tool: Bash
    Steps:
      1. grep -A1 "auto_power_off" main/main.cpp
      2. 确认间隔值与注释一致
    Expected Result: 间隔值合理（如 2000 对应 2 秒）
    Evidence: .sisyphus/evidence/task-6-interval-fix.txt
  ```

  **Commit**: YES (Phase 1)

- [x] 7. 修复 SW6306.h extern "C" 拼写

  **What to do**:
  - 修改 main/drivers/sw6306.h:8 将 `extern C {` 改为 `extern "C" {`

  **Must NOT do**: 不修改 sw6306.h 内部其他代码

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 1, 与 Task 4-6, 8 并行
  - **Blocked By**: Task 1-3

  **References**:
  - `main/drivers/sw6306.h:8` — extern C 拼写错误

  **Acceptance Criteria**:
  - [ ] grep "extern" main/drivers/sw6306.h 显示 `extern "C"`
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: extern C spelling corrected
    Tool: Bash
    Steps:
      1. grep -n "extern" main/drivers/sw6306.h
    Expected Result: 显示 extern "C" 而非 extern C
    Evidence: .sisyphus/evidence/task-7-extern-fix.txt
  ```

  **Commit**: YES (Phase 1)

- [x] 8. 移除 SPIFFS→LittleFS 宏别名

  **What to do**:
  - 移除 main.cpp:25 `#define SPIFFS LittleFS`
  - 将所有使用 SPIFFS 的地方统一改为 LittleFS
  - 搜索项目中所有 SPIFFS 引用

  **Must NOT do**: 不改变文件系统操作逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 1, 与 Task 4-7 并行
  - **Blocked By**: Task 1-3

  **References**:
  - `main/main.cpp:25` — #define SPIFFS LittleFS
  - `main/main.cpp:109` — SPIFFS.begin(true)

  **Acceptance Criteria**:
  - [ ] `grep -rn "SPIFFS" main/` 返回零结果
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: No SPIFFS references remain
    Tool: Bash
    Steps:
      1. grep -rn "SPIFFS" main/ 
    Expected Result: 零匹配
    Evidence: .sisyphus/evidence/task-8-spiffs-removal.txt
  ```

  **Commit**: YES (Phase 1)

- [ ] 9. 创建 settings.h 配置头文件

  **What to do**:
  - 创建 main/settings.h，集中定义所有硬编码常量
  - 包含：GPIO 引脚（PIN_POWER_CTRL=12, TEMP_PIN 等）、I2C（BQ_I2C_SDA=26, BQ_I2C_SCL=25, BQ40Z80_ADDR, IP2366_ADDR, SW6306_ADDR, IP2366_INT_PIN=14）、WiFi（WIFI_SSID, WIFI_PASSWORD, AP_IP）、显示（LV_BUFFER_LINES=30）、功率（POWER_THRESHOLD, POWER_WINDOW_MS, CELL_COUNT=6）
  - 每个宏使用 `#ifndef` 保护，允许 PlatformIO build_flags 覆盖
  - 添加 static_assert 或注释确保值与原始硬编码一致

  **Must NOT do**: 不改变任何行为逻辑

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: []

  **Parallelization**: Wave 2，此任务优先完成
  - **Blocks**: Task 10, 11, 12
  - **Blocked By**: Task 4-8

  **References**:
  - `main/main.cpp:104-105` — pinMode(12, OUTPUT)
  - `main/KKPortDevice.cpp:18` — wire1.begin(26, 25)
  - `main/KKPortDevice.cpp:20-21` — SW6306(0x3C), IP2366(14)
  - `main/ota.cpp:13-14` — ssid/password 硬编码
  - `main/ota.cpp:51-53` — IPAddress 固定 IP
  - `main/base.cpp:17-18` — POWER_THRESHOLD, POWER_WINDOW_MS
  - `hal/esp32/app_hal.cpp:22` — lvBufferSize = screenWidth * 30
  - `platformio.ini:63-65` — BQ40Z80_ADDR, IP2366_ADDR, SW6306_ADDR 已有 build_flags

  **Acceptance Criteria**:
  - [ ] main/settings.h 文件存在
  - [ ] 包含所有列出类别的配置宏
  - [ ] 每个宏有 `#ifndef` 保护
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过（暂时未被引用也不影响）

  **QA Scenarios**:
  ```
  Scenario: settings.h covers all config categories
    Tool: Bash
    Steps:
      1. grep -c "#ifndef" main/settings.h — 确认多个宏保护
      2. grep "WIFI_SSID\|PIN_\|I2C_\|POWER_" main/settings.h — 覆盖关键类别
    Expected Result: 每个类别至少有一个宏定义
    Evidence: .sisyphus/evidence/task-9-settings-h.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 10. 更新 ota.cpp 使用配置宏

  **What to do**:
  - `#include <settings.h>` 到 ota.cpp
  - 替换 ssid/password 为 WIFI_SSID/WIFI_PASSWORD
  - 替换 IPAddress 为配置宏构建
  - 确认编译通过

  **Must NOT do**: 不改变 OTA 逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 2, 依赖 Task 9
  - **Blocked By**: Task 9

  **References**:
  - `main/ota.cpp:13-14` — ssid/password 硬编码
  - `main/ota.cpp:51-53` — IPAddress 固定

  **Acceptance Criteria**:
  - [ ] `grep -rn "12345678" main/` 返回零结果
  - [ ] `grep -rn "disk_raid_power" main/ota.cpp` 返回零直接字符串（仅通过宏引用）

  **QA Scenarios**:
  ```
  Scenario: No hardcoded credentials
    Tool: Bash
    Steps:
      1. grep -rn "12345678\|192.*168.*44" main/ota.cpp
    Expected Result: 零匹配
    Evidence: .sisyphus/evidence/task-10-ota-config.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 11. 更新 KKPortDevice.cpp 使用配置宏

  **What to do**:
  - `#include <settings.h>` 到 KKPortDevice.cpp
  - 替换 wire1.begin(26,25) → wire1.begin(BQ_I2C_SDA, BQ_I2C_SCL)
  - 替换 SW6306(0x3C) → SW6306(SW6306_ADDR)
  - 替换 IP2366(14) → IP2366(IP2366_INT_PIN)

  **Must NOT do**: 不改变 I2C 通信逻辑

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 2, 依赖 Task 9
  - **Blocked By**: Task 9

  **References**:
  - `main/KKPortDevice.cpp:18` — wire1.begin(26, 25)
  - `main/KKPortDevice.cpp:20-21` — SW6306(0x3C), IP2366(14)

  **Acceptance Criteria**:
  - [ ] `grep "26.*25\|0x3C" main/KKPortDevice.cpp` 中无原始硬编码值

  **QA Scenarios**:
  ```
  Scenario: Hardcoded values replaced
    Tool: Bash
    Steps:
      1. grep -n "wire1.begin\|SW6306(" main/KKPortDevice.cpp
    Expected Result: 使用宏名而非硬编码数字
    Evidence: .sisyphus/evidence/task-11-kkport-config.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 12. 更新 hal/app_hal.cpp 和 main.cpp 配置引用

  **What to do**:
  - hal/esp32/app_hal.cpp: 替换 lvBufferSize magic 30 → LV_BUFFER_LINES
  - main.cpp: 替换 pinMode(12) → PIN_POWER_CTRL
  - main.cpp: 移除不必要的驱动 include（bq40z80.h, ip2366.h, SW6306.h, i2c_utils.h）
  - `#include <settings.h>` 到相关文件

  **Must NOT do**: 不改变行为

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 2, 依赖 Task 9
  - **Blocked By**: Task 9

  **References**:
  - `hal/esp32/app_hal.cpp:22` — lvBufferSize = screenWidth * 30
  - `main/main.cpp:104` — pinMode(12)
  - `main/main.cpp:14-19` — 不必要的驱动 include

  **Acceptance Criteria**:
  - [ ] hal/app_hal.cpp 无 magic number 30
  - [ ] main.cpp 无直接引脚数字 12
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: No magic numbers in hal and main
    Tool: Bash
    Steps:
      1. grep -n "\* 30" hal/esp32/app_hal.cpp
      2. grep -n "pinMode(12" main/main.cpp
    Expected Result: 两个 grep 均返回零匹配
    Evidence: .sisyphus/evidence/task-12-hal-main-config.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 13. 移动生成代码到 generated/ 并更新 build_src_filter

  **What to do**:
  - 创建 main/generated/ 目录
  - 移动 ui_font_mibol10.c, ui_font_mibol12.c, ui_font_mibol16.c, ui_font_mibol40.c
  - 移动 ui_img_mainboard_png.c, ui_img_ic_bat_png.c
  - 更新 platformio.ini build_src_filter 包含 generated/ 子目录
  - 验证编译通过（生成的 .c 文件通过 -I main 路径找到其头文件）

  **Must NOT do**: 不修改生成代码内容

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: []

  **Parallelization**: Wave 2, 与 Task 9-12 独立
  - **Blocked By**: Task 4-8

  **References**:
  - `main/ui_font_*.c` — 字体文件（4 个，共 ~310KB）
  - `main/ui_img_*.c` — 图片文件（2 个）
  - `platformio.ini:80-82` — build_src_filter 当前值

  **Acceptance Criteria**:
  - [ ] `ls main/generated/` 包含 6 个文件
  - [ ] `ls main/ui_font_*.c` 返回空
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Generated files moved and build passes
    Tool: Bash
    Steps:
      1. ls main/generated/
      2. 确认 6 个文件存在
      3. pio run -e esp32_D0WDQ6
    Expected Result: 编译成功
    Evidence: .sisyphus/evidence/task-13-generated-move.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 14. 清理重复文件和空目录

  **What to do**:
  - 删除 main/ui_comp_hook.c（保留 main/components/ui_comp_hook.c）
  - 移除 main/drivers/1.47inch_Touch_LCD.ino
  - 对空目录（main/core/, main/devices/, main/services/, main/proto/, main/utils/）添加 README.md 说明为预留占位

  **Must NOT do**: 不删除任何被编译引用的文件

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 2, 与 Task 9-13 独立
  - **Blocked By**: Task 4-8

  **References**:
  - `main/ui_comp_hook.c` — 重复文件
  - `main/components/ui_comp_hook.c` — 保留的版本
  - `main/drivers/1.47inch_Touch_LCD.ino` — 示例文件

  **Acceptance Criteria**:
  - [ ] main/ui_comp_hook.c 不存在
  - [ ] main/drivers/1.47inch_Touch_LCD.ino 不存在
  - [ ] 每个"空"目录包含 README.md
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Duplicate and unused files removed
    Tool: Bash
    Steps:
      1. Test-Path main/ui_comp_hook.c — 应为 false
      2. Test-Path main/drivers/1.47inch_Touch_LCD.ino — 应为 false
      3. pio run -e esp32_D0WDQ6
    Expected Result: 文件已删除，编译通过
    Evidence: .sisyphus/evidence/task-14-cleanup.txt
  ```

  **Commit**: YES (Phase 2)

- [ ] 15. 创建 data_types.h POD 数据结构体

  **What to do**:
  - 创建 main/data_types.h
  - 定义 BatteryData { percent, voltage, current, power, wh, temp, cellVoltage[6], cellCount }
  - 定义 PortData { voltage, current, power, isCharging, isDischarging }
  - 定义 SystemData { BatteryData battery, PortData portC1, PortData portC2, float boardTemp, float totalInPower, float totalOutPower }
  - 所有字段为 float/uint8_t/int16_t 等基础类型，零 LVGL 依赖

  **Must NOT do**: 不包含任何 LVGL 头文件

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 3，此任务优先
  - **Blocks**: Task 16, 17, 18
  - **Blocked By**: Task 9

  **References**:
  - `main/monitor_api.cpp:69-185` — updateUI() 中使用的数据字段（voltage, current, power, percent, temp, cellVoltages, port states）
  - `main/PortDevices.h` — PortType/PortState/PortStatus 已有定义可参考

  **Acceptance Criteria**:
  - [ ] main/data_types.h 文件存在
  - [ ] `grep "lvgl\|lv_" main/data_types.h` 返回零结果
  - [ ] 包含 BatteryData, PortData, SystemData 三个结构体

  **QA Scenarios**:
  ```
  Scenario: Data types are LVGL-free
    Tool: Bash
    Steps:
      1. grep -c "lvgl\|lv_\|LV_" main/data_types.h
    Expected Result: 0
    Evidence: .sisyphus/evidence/task-15-data-types.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 16. 重构 KKPortDevice — 驱动实例改为类成员

  **What to do**:
  - 将文件作用域的 TwoWire wire1(1)、BQ40Z80* bq、SW6306* sw、IP2366* ip2366 改为 KKPortDevice 私有成员
  - 构造函数接收 TwoWire* 参数
  - init() 中创建驱动实例赋值给成员指针
  - 析构函数中 delete 驱动指针
  - 确保单个 TwoWire 实例（从外部注入）

  **Must NOT do**: 不创建额外的 TwoWire 实例；不改变 getPortState 等公共 API

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 15
  - **Blocks**: Task 17, 22
  - **Blocked By**: Task 15

  **References**:
  - `main/KKPortDevice.cpp:12-15` — 文件作用域全局变量（wire1, bq, sw, ip2366）
  - `main/KKPortDevice.h` — 当前类声明
  - `main/KKPortDevice.cpp:16-24` — init() 方法

  **Acceptance Criteria**:
  - [ ] KKPortDevice.cpp 文件作用域无裸指针全局变量
  - [ ] `grep "^TwoWire\|^BQ40Z80\*\|^SW6306\*\|^IP2366\*" main/KKPortDevice.cpp` 仅在类成员声明中出现
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: No file-scope driver globals
    Tool: Bash
    Steps:
      1. grep -n "^TwoWire\|^BQ40Z80\*\|^SW6306\*\|^IP2366\*" main/KKPortDevice.cpp
    Expected Result: 无文件作用域全局（所有都在类内）
    Evidence: .sisyphus/evidence/task-16-kkport-members.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 17. DI: 移除全局 device 单例，main 中构造注入

  **What to do**:
  - monitor_api.cpp: 将 `IPortDevice* device = new KKPortDevice()` 改为 `static IPortDevice* device = nullptr;`
  - monitor_api.h: 添加 `void monitor_api_set_device(IPortDevice* dev);` 和 `IPortDevice* monitor_api_get_device();`
  - main.cpp setup(): 构造 `KKPortDevice* dev = new KKPortDevice(...)` 并调用 `monitor_api_set_device(dev)`
  - base.cpp 中 auto_power_off 通过 monitor_api_get_device() 或 bq_get_power() 获取数据（不直接访问 device）

  **Must NOT do**: 不修改 extern "C" 函数签名；确保 device 在使用前已设置

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 16
  - **Blocks**: Task 19, 20, 23
  - **Blocked By**: Task 16

  **References**:
  - `main/monitor_api.cpp:12` — 当前全局 `device = new KKPortDevice()`
  - `main/monitor_api.h:33` — `extern IPortDevice* device`
  - `main/main.cpp:138-139` — device->init() 和 updateUI()
  - `main/base.cpp:20` — auto_power_off 调用 bq_get_power()

  **Acceptance Criteria**:
  - [ ] `grep -c "new KKPortDevice" main/*.cpp` 返回 1（仅在 setup() 中）
  - [ ] monitor_api.cpp 文件作用域仅有 `static IPortDevice* device = nullptr;`
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Device constructed in setup only
    Tool: Bash
    Steps:
      1. grep -n "new KKPortDevice" main/*.cpp
    Expected Result: 仅在 main.cpp setup() 中出现一次
    Evidence: .sisyphus/evidence/task-17-di-inject.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 18. 创建 ui_presenter 层

  **What to do**:
  - 创建 main/ui_presenter.h 声明 `void ui_present_all(const SystemData& data);`
  - 创建 main/ui_presenter.cpp 实现：
    - 包含 lvgl.h 和 ui_schome.h（获取 UI 控件指针）
    - 从 data_types.h 接收 SystemData 结构体
    - 调用 lv_label_set_text_fmt/lv_obj_set_style 更新 UI 控件
    - 处理充放电状态颜色切换逻辑
    - 不创建任何新的 LVGL 对象
  - 将 monitor_api.cpp 中所有 lv_label_*/lv_obj_set_style_* 调用迁移到 ui_presenter

  **Must NOT do**: 不修改 ui_schome.c/h；不创建新的 LVGL 对象

  **Recommended Agent Profile**:
  - **Category**: `visual-engineering`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 15
  - **Blocks**: Task 19
  - **Blocked By**: Task 15

  **References**:
  - `main/monitor_api.cpp:125-184` — 当前 LVGL 调用（lv_label_set_text_fmt, lv_obj_set_style_text_color 等）
  - `main/ui_schome.h` — UI 控件指针声明（ui_percent, ui_power, ui_voltage 等）
  - `main/data_types.h` — SystemData 结构体定义（Task 15 创建）

  **Acceptance Criteria**:
  - [ ] main/ui_presenter.h 和 main/ui_presenter.cpp 存在
  - [ ] `grep -c "lv_.*_create" main/ui_presenter.cpp` 返回 0（不创建 LVGL 对象）
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: Presenter reads data, updates UI, creates nothing
    Tool: Bash
    Steps:
      1. grep -c "lv_.*_create" main/ui_presenter.cpp — 应为 0
      2. grep "ui_present_all" main/ui_presenter.h — 确认入口函数存在
    Expected Result: 入口函数存在，零 LVGL 创建调用
    Evidence: .sisyphus/evidence/task-18-presenter.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 19. 重构 monitor_api — 拆分 updateUI 使用 presenter

  **What to do**:
  - monitor_api.cpp updateUI() 拆分为：
    - collectBatteryData(BatteryData&) — 从 device 读取电池数据
    - collectPortData(PortData&, PortType) — 从 device 读取端口数据
    - updateUI() — 调用 collect + ui_present_all()
  - 移除 monitor_api.cpp 中所有 `#include <lvgl.h>` 和 LVGL 直接调用
  - extern "C" 函数签名不变，但内部改为调用 collect 函数

  **Must NOT do**: 不改变 13 个 extern "C" 函数签名

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 17, 18
  - **Blocks**: Task 31
  - **Blocked By**: Task 17, 18

  **References**:
  - `main/monitor_api.cpp:69-185` — 当前 updateUI()（~116 行）
  - `main/monitor_api.h` — extern "C" 函数声明（签名不可变）
  - `main/data_types.h` — POD 结构体
  - `main/ui_presenter.h` — presenter 入口

  **Acceptance Criteria**:
  - [ ] `grep -c "lv_label" main/monitor_api.cpp` 返回 0
  - [ ] `grep "lvgl.h" main/monitor_api.cpp` 返回空
  - [ ] updateUI() 函数体 < 30 行
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: monitor_api is LVGL-free
    Tool: Bash
    Steps:
      1. grep -c "lv_label\|lv_obj_set\|lvgl.h" main/monitor_api.cpp
    Expected Result: 0
    Evidence: .sisyphus/evidence/task-19-monitor-decouple.txt

  Scenario: extern C signatures unchanged
    Tool: Bash
    Steps:
      1. 提取 monitor_api.h extern "C" 块中函数签名
      2. 与基准对比（13 个函数）
    Expected Result: 所有签名一致
    Evidence: .sisyphus/evidence/task-19-signatures.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 20. 拆分 main.cpp setup() 为子函数

  **What to do**:
  - 提取 init_serial()、init_storage()、init_hal()、init_ui()、init_device()、init_services()、register_tasks()
  - setup() 仅调用这些子函数
  - 每个子函数 < 30 行

  **Must NOT do**: 不改变初始化顺序和逻辑

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 17
  - **Blocked By**: Task 17

  **References**:
  - `main/main.cpp:81-179` — 当前 setup()（~98 行）

  **Acceptance Criteria**:
  - [ ] setup() 函数体 < 30 行
  - [ ] 每个子函数有明确职责和名称
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: setup() is concise
    Tool: Bash
    Steps:
      1. 统计 setup() 函数行数
    Expected Result: < 30 行
    Evidence: .sisyphus/evidence/task-20-setup-split.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 21. 拆分 ota.cpp setup_ota() 为子函数

  **What to do**:
  - 提取 init_wifi_ap()、setup_web_server()、setup_ota_routes()
  - setup_ota() 仅调用这些子函数

  **Must NOT do**: 不改变 OTA 逻辑

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 17
  - **Blocked By**: Task 10（配置宏引用）

  **References**:
  - `main/ota.cpp:54-150` — 当前 setup_ota()（~97 行）

  **Acceptance Criteria**:
  - [ ] setup_ota() 函数体 < 30 行
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **QA Scenarios**:
  ```
  Scenario: setup_ota() is concise
    Tool: Bash
    Steps:
      1. 统计 setup_ota() 行数
    Expected Result: < 30 行
    Evidence: .sisyphus/evidence/task-21-ota-split.txt
  ```

  **Commit**: YES (Phase 3)

- [ ] 22. 拆分 KKPortDevice::getPortState() 辅助方法

  **What to do**:
  - 提取私有方法 buildPortStatus(bool isCharging, bool isDischarging, float voltage, float current) → PortStatus
  - getPortState() 调用 buildPortStatus() 消除 C1/C2 重复代码

  **Must NOT do**: 不改变公共 API

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 16
  - **Blocked By**: Task 16

  **References**:
  - `main/KKPortDevice.cpp:38-92` — getPortState() 两个端口重复逻辑

  **Acceptance Criteria**:
  - [ ] getPortState() 函数体 < 30 行
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **Commit**: YES (Phase 3)

- [ ] 23. 移除 monitor_api.h extern device 全局暴露

  **What to do**:
  - 从 monitor_api.h 中移除 `extern IPortDevice* device;`
  - 改为仅通过 monitor_api_get_device()/monitor_api_set_device() 访问
  - 搜索所有使用 `device` 全局变量的地方并替换为函数调用

  **Must NOT do**: 不破坏 extern "C" 块

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 3, 依赖 Task 17, 19
  - **Blocked By**: Task 17, 19

  **References**:
  - `main/monitor_api.h:33` — `extern IPortDevice* device`
  - `main/main.cpp:138` — `device->init()`

  **Acceptance Criteria**:
  - [ ] `grep "extern IPortDevice" main/monitor_api.h` 返回空
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **Commit**: YES (Phase 3)

- [ ] 24. 移除 cell_helper.h 对 lvgl.h 的依赖

  **What to do**:
  - cell_helper.h 当前包含 lvgl.h 用于 lv_obj_t* 类型
  - 改为前向声明 `typedef struct _lv_obj_t lv_obj_t;` 或将 LVGL 依赖移到 cell_helper.cpp

  **Must NOT do**: 不改变 cell_helper 的公共 API

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 3, 与其他 Wave 3 任务并行
  - **Blocked By**: Task 17

  **References**:
  - `main/cell_helper.h:7` — `#include <lvgl.h>`
  - `main/cell_helper.h:11-12` — `extern lv_obj_t* ui_cell_containers[6]`

  **Acceptance Criteria**:
  - [ ] `grep "lvgl.h" main/cell_helper.h` 返回空
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **Commit**: YES (Phase 3)

- [ ] 25. BQ40Z80 添加 hasError/getLastError

  **What to do**:
  - bq40z80.h 添加 `bool hasError() const; int getLastError() const;`
  - bq40z80.cpp 添加 `_hasError` 和 `_lastError` 私有成员
  - I2C 通信失败时设置错误状态
  - 成功通信时清除错误状态

  **Must NOT do**: 不改变 read_voltage/read_current 等函数签名

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: []

  **Parallelization**: Wave 4, 与 Task 26, 27 并行
  - **Blocked By**: Task 19

  **References**:
  - `main/bq40z80.h` — 当前类声明
  - `main/bq40z80.cpp` — read_word() 中的 state_flag 错误处理

  **Acceptance Criteria**:
  - [ ] BQ40Z80 类包含 hasError() 和 getLastError() 方法
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **Commit**: YES (Phase 4)

- [ ] 26. IP2366 添加 hasError/getLastError

  **What to do**: 同 Task 25 模式，为 IP2366 添加错误查询接口

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **References**: `main/ip2366.h`, `main/ip2366.cpp`

  **Acceptance Criteria**: IP2366 类包含 hasError()/getLastError()

  **Commit**: YES (Phase 4)

- [ ] 27. SW6306 添加 hasError/getLastError

  **What to do**: 同 Task 25 模式，为 SW6306 添加错误查询接口

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **References**: `main/SW6306.h`, `main/SW6306.cpp`

  **Acceptance Criteria**: SW6306 类包含 hasError()/getLastError()

  **Commit**: YES (Phase 4)

- [ ] 28. 驱动层 I2C 失败日志增强

  **What to do**:
  - 在 BQ40Z80/IP2366/SW6306 的 I2C 通信失败点添加 mylog 日志
  - 格式: "I2C Error: [device] [operation] failed"
  - 使用已有的 mylog.printf

  **Must NOT do**: 不改变通信逻辑

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **References**: 各驱动 .cpp 文件中的 I2C 操作

  **Acceptance Criteria**: 每个驱动至少有一处错误日志

  **Commit**: YES (Phase 4)

- [ ] 29. 添加 .clang-format 配置文件

  **What to do**:
  - 在项目根目录创建 .clang-format
  - 基于 LLVM 风格，缩进 4 空格，中文注释友好
  - 设置 ColumnLimit: 120（嵌入式项目偏好较长行）

  **Must NOT do**: 不运行格式化（仅添加配置）

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **Acceptance Criteria**: .clang-format 文件存在且格式有效

  **Commit**: YES (Phase 4)

- [ ] 30. 创建单元测试 test_cell_helper + test_utils

  **What to do**:
  - test/test_cell_helper.cpp: 测试 lv_label_set_text_float 格式化逻辑（需模拟环境）
  - test/test_utils.cpp: 测试 extract_bits 函数
  - 配置 platformio.ini 测试环境（如 Task 2 未完成则在此完成）

  **Recommended Agent Profile**: `unspecified-high` | **Parallelization**: Wave 4

  **References**: `main/cell_helper.cpp`, `main/utils.cpp`

  **Acceptance Criteria**: `pio test` 运行且测试通过

  **Commit**: YES (Phase 4)

- [ ] 31. 统一 C API 函数命名

  **What to do**:
  - monitor_api.h: bq_get_percent → battery_get_percent
  - bq_get_voltage → battery_get_voltage
  - bq_get_current → battery_get_current
  - bq_get_power → battery_get_power
  - bg_get_temp → battery_get_temp
  - bg_get_remaining_energy_wh → battery_get_remaining_energy_wh
  - bq_get_cell_voltage → battery_get_cell_voltage
  - get2366Voltage → port_c2_get_voltage
  - get2366Current → port_c2_get_current
  - get2366Power → port_c2_get_power
  - is2366Charging → port_c2_is_charging
  - is2366DisCharging → port_c2_is_discharging
  - 更新所有调用点（base.cpp, ui_schome.c 等）
  - 保留旧名称作为废弃别名（#define 旧名 新名），确保渐进迁移

  **Must NOT do**: 不修改函数内部实现

  **Recommended Agent Profile**: `unspecified-high` | **Parallelization**: Wave 4

  **References**:
  - `main/monitor_api.h` — 13 个 extern "C" 函数
  - `main/base.cpp:20` — bq_get_power() 调用

  **Acceptance Criteria**:
  - [ ] 所有公共函数名使用一致前缀
  - [ ] `pio run -e esp32_D0WDQ6` 编译通过

  **Commit**: YES (Phase 4)

- [ ] 32. 更新 utils.h static→inline

  **What to do**:
  - main/utils.h 中 `static uint8_t extract_bits(...)` 改为 `inline uint8_t extract_bits(...)`
  - 或将函数体移到 utils.cpp，头文件仅保留声明

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **References**: `main/utils.h:8`

  **Acceptance Criteria**: utils.h 中无 `static` 函数定义

  **Commit**: YES (Phase 4)

- [ ] 33. 运行 clang-format 格式化全项目

  **What to do**:
  - 对 main/*.cpp, main/*.h, main/*.c 运行 clang-format
  - 排除 main/drivers/sw6306.c（第三方代码）
  - 排除 main/generated/（生成代码）

  **Must NOT do**: 不格式化第三方和生成代码

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4

  **Acceptance Criteria**: `clang-format --dry-run --Werror main/*.cpp` 通过

  **Commit**: YES (Phase 4)

- [ ] 34. 全 env 编译验证

  **What to do**:
  - 运行 `pio run -e esp32_D0WDQ6`
  - 运行 `pio run -e esp32_D0WDQ6_201`
  - 运行 `pio run -e emulator_64bits`
  - 记录每个 env 的二进制大小
  - 对比重构前后大小变化（应 ≤ 5% 每阶段）

  **Recommended Agent Profile**: `quick` | **Parallelization**: Wave 4 (last)

  **Acceptance Criteria**: 所有 env 编译通过

  **Commit**: NO (验证步骤)

---

## Final Verification Wave

> 4 个审查代理并行运行。全部通过后呈现结果并获得用户确认。

- [ ] F1. **计划合规审计** — `oracle`
  逐条验证每个 "Must Have"：检查实现存在（读文件、grep、编译）。每个 "Must NOT Have"：搜索代码库中禁止模式。对比交付物与计划。检查 .sisyphus/evidence/ 证据文件。
  输出: `Must Have [N/N] | Must NOT Have [N/N] | Tasks [N/N] | VERDICT: APPROVE/REJECT`

- [ ] F2. **代码质量审查** — `unspecified-high`
  `pio run` 所有 env。审查所有变更文件：`as any`、`@ts-ignore`（不适用C++但检查类似模式）、空 catch、console.log、注释掉的代码、未使用 include。AI slop: 过度注释、过度抽象、通用命名。
  输出: `Build [PASS/FAIL] | Files [N clean/N issues] | VERDICT`

- [ ] F3. **实际 QA 验证** — `unspecified-high`
  从干净状态开始。执行每个任务的 QA 场景 — 遵循确切步骤、捕获证据。测试跨任务集成。测试边界情况：空状态、无效输入。保存到 `.sisyphus/evidence/final-qa/`。
  输出: `Scenarios [N/N pass] | Integration [N/N] | VERDICT`

- [ ] F4. **范围保真检查** — `deep`
  每个任务：对比 "What to do" 与实际 diff。验证 1:1 — 计划中的都实现了（无遗漏），实现中的都没超出计划（无蔓延）。检查 "Must NOT do" 合规。标记未说明的变更。
  输出: `Tasks [N/N compliant] | Unaccounted [CLEAN/N files] | VERDICT`

---

## Commit Strategy

每个 Phase 完成后创建一个验证提交：

- **Phase 0**: `test: add Unity framework and characterization tests`
- **Phase 1**: `fix: resolve critical bugs (runSelfTest, preprocessor, extern C)`
- **Phase 2**: `refactor: centralize config to settings.h and reorganize files`
- **Phase 3**: `refactor: decouple architecture with DI and UI presenter`
- **Phase 4**: `chore: add error handling, tests, formatting, and naming`

---

## Success Criteria

### 验证命令
```bash
pio run -e esp32_D0WDQ6          # 预期: SUCCESS
pio run -e esp32_D0WDQ6_201      # 预期: SUCCESS
pio run -e emulator_64bits        # 预期: SUCCESS
grep -c "lv_label" main/monitor_api.cpp   # 预期: 0
grep -c "new KKPortDevice" main/*.cpp     # 预期: 1 (在 setup() 内)
grep -rn "12345678" main/                  # 预期: 0 (密码已移至配置)
```

### 最终清单
- [ ] 所有 "Must Have" 存在
- [ ] 所有 "Must NOT Have" 不存在
- [ ] 所有 env 编译通过
- [ ] extern "C" 13 个函数签名未变
- [ ] 单个 TwoWire 实例（数量为 1）
