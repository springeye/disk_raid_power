/* 最小化 LVGL 配置，仅保留基础控件和字体，最大化减小固件体积 */
#ifndef LV_CONF_H
#define LV_CONF_H


#define LV_CONF_VERSION 90000

/* 内存和颜色深度配置 */
#define LV_MEM_SIZE           (32U * 1024U)   /* 可根据实际需求调整 */
#define LV_COLOR_DEPTH        16

/* 基础控件，仅保留按钮和标签 */
#define LV_USE_BTN            1
#define LV_USE_LABEL          1
#define LV_USE_QRCODE         1  /* 启用二维码控件 */
#define LV_USE_SLIDER         1
#define LV_USE_LIST           0
#define LV_USE_CHART          0
#define LV_USE_TABLE          0
#define LV_USE_ARC            1
#define LV_USE_SWITCH         0
#define LV_USE_DROPDOWN       1
#define LV_USE_TEXTAREA       1
#define LV_USE_CANVAS         1
#define LV_USE_IMG            1
#define LV_USE_BAR            1
#define LV_USE_LED            0
#define LV_USE_LINE           1
#define LV_USE_ROLLER         1
#define LV_USE_TABVIEW        0
#define LV_USE_TILEVIEW       0
#define LV_USE_WIN            0
#define LV_USE_SPINBOX        1
#define LV_USE_SPINNER        0
#define LV_USE_MSGBOX         0
#define LV_USE_CALENDAR       0
#define LV_USE_KEYBOARD       1
#define LV_USE_METER          0
#define LV_USE_MENU           0
#define LV_USE_COLORWHEEL     0
#define LV_USE_IMGBTN         0
#define LV_USE_GRID           0
#define LV_USE_OBJ            1
#define LV_USE_OBJMASK        0

/* 字体，仅保留12号字体 */
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_44 1

/* 关闭所有主题和demo */
#define LV_USE_THEME_DEFAULT  1
#define LV_USE_THEME_BASIC    0
#define LV_USE_THEME_MONO     0
#define LV_USE_DEMO_WIDGETS   0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK 0
#define LV_USE_DEMO_STRESS    0
#define LV_USE_DEMO_MUSIC     0

/* 关闭日志和动画 */
#define LV_USE_LOG            0
#define LV_USE_ANIMATION      0

/* 关闭所有图片格式支持 */
#define LV_USE_PNG            0
#define LV_USE_BMP            0
#define LV_USE_SJPG           0
#define LV_USE_GIF            0

#endif /* LV_CONF_H */
