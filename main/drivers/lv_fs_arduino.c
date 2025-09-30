#include "lv_fs_arduino.h"

typedef struct {
    File file;
} ArduinoFileWrap;

static void * arduino_fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t arduino_fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t arduino_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t arduino_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t arduino_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos);

void lv_fs_arduino_init(void) {
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);

    drv.letter = 'S';   // 驱动号 -> "S:/"
    drv.cache_size = 0;

    drv.open_cb = arduino_fs_open;
    drv.close_cb = arduino_fs_close;
    drv.read_cb = arduino_fs_read;
    drv.seek_cb = arduino_fs_seek;
    drv.tell_cb = arduino_fs_tell;

    lv_fs_drv_register(&drv);
}

static void * arduino_fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode) {
    LV_UNUSED(drv);

    const char * flags = (mode == LV_FS_MODE_RD) ? FILE_READ : FILE_WRITE;
    File f = SPIFFS.open(path, flags);
    if (!f) return NULL;

    ArduinoFileWrap * wrap = (ArduinoFileWrap*)lv_mem_alloc(sizeof(ArduinoFileWrap));
    wrap->file = f;
    return wrap;
}

static lv_fs_res_t arduino_fs_close(lv_fs_drv_t * drv, void * file_p) {
    LV_UNUSED(drv);
    ArduinoFileWrap * wrap = (ArduinoFileWrap*)file_p;
    wrap->file.close();
    lv_mem_free(wrap);
    return LV_FS_RES_OK;
}

static lv_fs_res_t arduino_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br) {
    LV_UNUSED(drv);
    ArduinoFileWrap * wrap = (ArduinoFileWrap*)file_p;
    *br = wrap->file.read((uint8_t*)buf, btr);
    return LV_FS_RES_OK;
}

static lv_fs_res_t arduino_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence) {
    LV_UNUSED(drv);
    ArduinoFileWrap * wrap = (ArduinoFileWrap*)file_p;

    SeekMode sm;
    switch (whence) {
        case LV_FS_SEEK_SET: sm = SeekSet; break;
        case LV_FS_SEEK_CUR: sm = SeekCur; break;
        case LV_FS_SEEK_END: sm = SeekEnd; break;
        default: return LV_FS_RES_INV_PARAM;
    }

    if (!wrap->file.seek(pos, sm)) return LV_FS_RES_FS_ERR;
    return LV_FS_RES_OK;
}

static lv_fs_res_t arduino_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos) {
    LV_UNUSED(drv);
    ArduinoFileWrap * wrap = (ArduinoFileWrap*)file_p;
    *pos = wrap->file.position();
    return LV_FS_RES_OK;
}
