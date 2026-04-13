# Decisions - project-engineering-refactor

## 架构决策
- 依赖注入：KKPortDevice 在 main setup() 中构造，通过 monitor_api_set_device() 注入
- UI 解耦：ui_presenter 只读 UI 控件，不创建 LVGL 对象
- 配置集中：settings.h 集中定义，#ifndef 保护允许 build_flags 覆盖
- 生成代码：移到 main/generated/ 子目录，更新 build_src_filter
