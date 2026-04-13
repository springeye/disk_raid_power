## ADDED Requirements

### Requirement: Generated code separation
LVGL 生成的 UI 代码、字体文件和图片资源 SHALL 从 main/ 根目录移动到 main/generated/ 子目录。

#### Scenario: Font files in generated directory
- **WHEN** 查看 main/ 目录结构
- **THEN** ui_font_*.c 文件 SHALL 位于 main/generated/ 目录下，而非 main/ 根目录

#### Scenario: Image files in generated directory
- **WHEN** 查看 main/ 目录结构
- **THEN** ui_img_*.c 文件 SHALL 位于 main/generated/ 目录下

#### Scenario: Build includes generated directory
- **WHEN** PlatformIO 编译项目
- **THEN** build_src_filter SHALL 包含 generated/ 子目录，确保生成代码被正确编译

### Requirement: Duplicate file cleanup
项目 SHALL NOT 包含重复或冗余文件。

#### Scenario: Remove duplicate ui_comp_hook.c
- **WHEN** 搜索项目中 ui_comp_hook.c 的副本
- **THEN** SHALL 只存在一份 ui_comp_hook.c 文件（保留 main/components/ 下的版本或合并为一份）

#### Scenario: Remove unused files
- **WHEN** 搜索 main/drivers/ 目录
- **THEN** .ino 示例文件和未使用的驱动文件 SHALL 被移除或归档

### Requirement: Clear module directory structure
main/ 目录 SHALL 按功能模块组织子目录，每个子目录职责明确。

#### Scenario: Drivers in dedicated directory
- **WHEN** 添加新的硬件驱动文件
- **THEN** 驱动 .cpp/.h 文件 SHALL 放置在 main/drivers/ 目录下

#### Scenario: Empty placeholder directories removed or documented
- **WHEN** 查看 main/ 下的空目录（core/、devices/、services/、proto/、utils/）
- **THEN** 空目录 SHALL 被移除，或包含 README.md 说明其预留用途
