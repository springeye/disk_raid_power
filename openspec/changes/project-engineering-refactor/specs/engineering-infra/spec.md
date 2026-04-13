## ADDED Requirements

### Requirement: Clang-format configuration
项目根目录 SHALL 包含 `.clang-format` 配置文件，定义统一的代码格式化规则。

#### Scenario: Format applied to all source files
- **WHEN** 开发者对项目运行 clang-format
- **THEN** 所有 .cpp/.h/.c 文件 SHALL 被格式化为一致的代码风格（缩进、大括号位置、空格、换行）

### Requirement: CI static analysis step
CI 工作流 SHALL 包含编译检查步骤，在每次提交时验证所有 PlatformIO env 能成功编译。

#### Scenario: PR triggers build check
- **WHEN** 向仓库提交 Pull Request
- **THEN** CI SHALL 对所有定义的 env（esp32_D0WDQ6、emulator_64bits 等）执行 `pio run` 并报告编译结果

### Requirement: Unit test framework setup
项目 SHALL 配置 Unity 测试框架，在 test/ 目录下支持编写和运行单元测试。

#### Scenario: Run unit tests in emulator
- **WHEN** 执行 `pio test -e emulator_64bits`
- **THEN** 所有 test/ 目录下的测试用例 SHALL 被编译执行并报告通过/失败结果

#### Scenario: Test pure logic functions
- **WHEN** 编写针对 cell_helper、utils 等纯逻辑模块的测试
- **THEN** 测试 SHALL 能在 SDL2 模拟环境中运行，无需实际硬件

### Requirement: Consistent naming convention
项目 SHALL 制定并文档化统一的命名规范：公共 API 使用 camelCase，内部函数使用 snake_case，常量使用 UPPER_SNAKE_CASE，类型名使用 PascalCase。

#### Scenario: Function naming follows convention
- **WHEN** 审查所有头文件中的函数声明
- **THEN** 公共 API 函数（如 IPortDevice 的方法）SHALL 使用 camelCase，内部辅助函数 SHALL 使用 snake_case
