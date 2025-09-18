#ifdef __cplusplus
extern "C" {
#endif

// 获取电池剩余百分比
uint8_t bq_get_percent(void);
// 获取某个电芯电压
uint16_t bq_get_cell_voltage(uint8_t cell_index);
float read_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v);
float read_voltage();
float get2366Voltage();
float get2366Current();
float get2366Power();
bool is2366Charging();
bool is2366DisCharging();

#ifdef __cplusplus
}
#endif

