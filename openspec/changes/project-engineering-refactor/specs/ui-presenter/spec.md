## ADDED Requirements

### Requirement: UI Presenter layer separation
系统 SHALL 提供 ui_presenter 模块作为数据到 LVGL UI 的映射层，monitor_api SHALL NOT 直接调用任何 LVGL API。

#### Scenario: monitor_api without LVGL dependency
- **WHEN** 查看 monitor_api.cpp 的 #include 列表
- **THEN** SHALL NOT 包含 "lvgl.h" 或任何 LVGL 相关头文件

#### Scenario: ui_presenter updates LVGL widgets
- **WHEN** 设备数据更新后需要刷新 UI
- **THEN** 数据 SHALL 通过 POD 结构体传递给 ui_presenter，由 ui_presenter 调用 LVGL API 更新控件

### Requirement: Data model struct for UI updates
系统 SHALL 定义纯数据结构体（BatteryData、PortData 等），用于在数据层和 UI 层之间传递数据。

#### Scenario: BatteryData struct
- **WHEN** 电池数据（电压、电流、功率、百分比、温度、单体电压）需要传递给 UI
- **THEN** SHALL 通过 BatteryData 结构体打包传递，结构体中不包含任何 LVGL 类型

#### Scenario: PortData struct
- **WHEN** 端口状态（电压、电流、功率、充放电状态）需要传递给 UI
- **THEN** SHALL 通过 PortData 结构体打包传递

### Requirement: Single update entry point for UI
ui_presenter SHALL 提供 `ui_present_all(const BatteryData&, const PortData&, const PortData&)` 统一入口函数。

#### Scenario: Present all data at once
- **WHEN** updateUI() 被调度执行
- **THEN** monitor_api SHALL 收集所有设备数据到结构体中，调用 ui_presenter 的统一入口函数一次性更新所有 UI 元素
