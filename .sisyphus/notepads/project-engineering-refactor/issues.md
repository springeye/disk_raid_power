# Issues - project-engineering-refactor

## 已知 Bug（待修复）
- runSelfTest() 返回 false 而非 true → OTA 升级始终触发回滚
- #elifdef 是 C++23 语法，ESP32 GCC 可能不兼容
- SW6306.h:8 `extern C {` 应为 `extern "C" {`
- #define SPIFFS LittleFS 掩盖 API 差异

## SIOF 风险
- new KKPortDevice() 在文件作用域，与 TwoWire wire1(1) 存在静态初始化顺序风险
- Phase 3 DI 任务解决此问题
