#ifndef DISK_RAID_POWER_UI_PRESENTER_H
#define DISK_RAID_POWER_UI_PRESENTER_H

#include "data_types.h"

// 用 C linkage 暴露给 ui_schome.c 等 C 代码可调用（按需）
#ifdef __cplusplus
extern "C" {
#endif

// 用 SystemData 更新所有 UI 控件
void ui_present_all(const SystemData* data);

#ifdef __cplusplus
}
#endif

#endif // DISK_RAID_POWER_UI_PRESENTER_H
