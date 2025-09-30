#ifndef LV_FS_ARDUINO_H
#define LV_FS_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>
#include <SPIFFS.h>

    /**
     * 初始化 SPIFFS -> LVGL 文件系统驱动
     * 驱动号固定为 'S'，因此访问路径为 "S:/xxx.png"
     */
    void lv_fs_arduino_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_FS_ARDUINO_H */
