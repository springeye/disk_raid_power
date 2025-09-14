#ifdef __cplusplus
extern "C" {
#endif

// 获取电池剩余百分比
uint8_t bq_get_percent(void);
// 获取某个电芯电压
uint16_t bq_get_cell_voltage(uint8_t cell_index);

#ifdef __cplusplus
}
#endif

