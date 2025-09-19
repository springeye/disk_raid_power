#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Preferences.h>

class BLEManager {
private:
    // BLE 组件
    BLEServer* pServer;
    BLEService* pService;
    BLECharacteristic* pSsidCharacteristic;
    BLECharacteristic* pPassCharacteristic;
    BLECharacteristic* pStatusCharacteristic;
    BLECharacteristic* pDataCharacteristic;

    // WiFi 状态
    bool isWifiConfigured;
    bool isWifiConnected;

    // UUID 配置
    const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    const char* CHARACTERISTIC_UUID_SSID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
    const char* CHARACTERISTIC_UUID_PASS = "0175b355-1e38-40fb-a434-539352c366ac";
    const char* CHARACTERISTIC_UUID_STATUS = "8c85f05a-9357-417c-864d-3b1a8d5df232";
    const char* CHARACTERISTIC_UUID_DATA = "f27b53ad-c631-4c8f-a006-0e3190a6f815";
    const char* DEVICE_NAME = "ESP32_Config_Device";

    Preferences preferences;

    // WiFi 状态回调，用于通知 App
    void updateWifiStatus(const String& status);

    // 接收到 SSID 和密码后的回调处理
    class WifiConfigCallbacks: public BLECharacteristicCallbacks {
        BLEManager* manager;
    public:
        WifiConfigCallbacks(BLEManager* mgr);
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };

    void initWiFi();
    void setupBle();

public:
    BLEManager();
    void begin();
    void loop();
};
extern BLEManager bleManager;
#endif // BLE_MANAGER_H