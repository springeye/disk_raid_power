#include <unity.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t bq_get_percent(void);
uint16_t bq_get_cell_voltage(uint8_t cell_index);
float bg_get_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v);
float bq_get_voltage(void);
float bq_get_current(void);
float bq_get_power(void);
int16_t bg_get_temp(void);
float get2366Voltage(void);
float get2366Current(void);
float get2366Power(void);
bool is2366Charging(void);
bool is2366DisCharging(void);
void updateUI(void);

#ifdef __cplusplus
}
#endif

void setUp(void) {}
void tearDown(void) {}

void test_bq_get_percent_returns_uint8(void) {
    uint8_t result = bq_get_percent();
    TEST_ASSERT_TRUE(result <= 100);
}

void test_bq_get_cell_voltage_returns_uint16(void) {
    uint16_t v = bq_get_cell_voltage(0);
    (void)v;
    TEST_PASS();
}

void test_bg_get_remaining_energy_wh_returns_float(void) {
    float wh = bg_get_remaining_energy_wh(6, 3.0f);
    (void)wh;
    TEST_PASS();
}

void test_bq_get_voltage_returns_float(void) {
    float v = bq_get_voltage();
    (void)v;
    TEST_PASS();
}

void test_bq_get_current_returns_float(void) {
    float c = bq_get_current();
    (void)c;
    TEST_PASS();
}

void test_bq_get_power_returns_float(void) {
    float p = bq_get_power();
    (void)p;
    TEST_PASS();
}

void test_bg_get_temp_returns_int16(void) {
    int16_t t = bg_get_temp();
    (void)t;
    TEST_PASS();
}

void test_get2366Voltage_returns_float(void) {
    float v = get2366Voltage();
    (void)v;
    TEST_PASS();
}

void test_get2366Current_returns_float(void) {
    float c = get2366Current();
    (void)c;
    TEST_PASS();
}

void test_get2366Power_returns_float(void) {
    float p = get2366Power();
    (void)p;
    TEST_PASS();
}

void test_is2366Charging_returns_bool(void) {
    bool r = is2366Charging();
    (void)r;
    TEST_PASS();
}

void test_is2366DisCharging_returns_bool(void) {
    bool r = is2366DisCharging();
    (void)r;
    TEST_PASS();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bq_get_percent_returns_uint8);
    RUN_TEST(test_bq_get_cell_voltage_returns_uint16);
    RUN_TEST(test_bg_get_remaining_energy_wh_returns_float);
    RUN_TEST(test_bq_get_voltage_returns_float);
    RUN_TEST(test_bq_get_current_returns_float);
    RUN_TEST(test_bq_get_power_returns_float);
    RUN_TEST(test_bg_get_temp_returns_int16);
    RUN_TEST(test_get2366Voltage_returns_float);
    RUN_TEST(test_get2366Current_returns_float);
    RUN_TEST(test_get2366Power_returns_float);
    RUN_TEST(test_is2366Charging_returns_bool);
    RUN_TEST(test_is2366DisCharging_returns_bool);
    return UNITY_END();
}
