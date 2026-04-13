- 新建 `main/data_types.h` 时保持为纯数据头文件，仅依赖 `<cstdint>`，避免引入 LVGL、Arduino、PortDevices 造成循环依赖。
- `BatteryData`、`PortData`、`SystemData` 的字段应直接对齐 `monitor_api.cpp::updateUI()` 的实际数据来源，便于后续 UI/状态聚合。

## [Task 16] KKPortDevice 驱动成员化
- KKPortDevice 构造函数现在接收 TwoWire* 参数
- monitor_api.cpp 临时用 static TwoWire s_wire(1) 保持编译（Task 17 会彻底 DI）
- 析构函数 delete _bq/_sw/_ip2366，不 delete _wire（外部拥有）
## [Task 18] ui_presenter 层创建
- ui_present_all 参数为 const SystemData*（指针而非引用，兼容 C 调用）
- 不创建任何 LVGL 对象（无 lv_*_create 调用）
- 包含 lvgl.h + ui_schome.h + data_types.h + cell_helper.h
- update_cells() 仍在 presenter 末尾调用（来自 cell_helper）

## [Task 17] DI 完整注入
- monitor_api_set_device()/get_device() 是 C++ 函数（不在 extern "C" 块内）
- KKPortDevice 和 TwoWire 现在只在 main.cpp setup() 中构造
- static TwoWire s_wire(1) 声明为 static 保证对象不被栈销毁
- monitor_api.cpp 中 device 是 static IPortDevice* device = nullptr（不再自己构造）
- base.cpp 的 auto_power_off 通过 bq_get_power() extern "C" 访问设备，无需修改
- main.cpp 需新增 #include <Wire.h> 和 #include <KKPortDevice.h>（具体类型移至此处）
- monitor_api.cpp 删除 #include <KKPortDevice.h>（不再直接依赖具体实现）
