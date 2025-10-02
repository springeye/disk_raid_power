#ifndef ESP32_CONTROL_H
#define ESP32_CONTROL_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <Update.h>

// 蓝牙服务UUID定义
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class ESP32Control {
public:
    // 初始化函数
    static void begin(const char* deviceName = "ESP32_Device");
    
    // 主循环处理
    static void loop();
    
    // 获取WiFi连接状态
    static bool isWiFiConnected();
    //是否有设备连接
    static bool isClintConnected();
    static void sendData();
    
    // 获取设备数据（示例）
    static void getDeviceData(char* out, size_t out_size);

    // 设置设备参数（示例）
    static void setDeviceParameter(int value);

private:
    // 蓝牙相关变量
    static BLEServer* pServer;
    static BLECharacteristic* pCharacteristic;
    static bool deviceConnected;
    static bool oldDeviceConnected;
    
    // 设备状态变量
    static int deviceParameter;
    static char deviceStatus[32];

    // OTA相关成员
    static BLECharacteristic* pOtaDataCharacteristic;
    static bool otaReceiving;
    static size_t otaTotalSize;
    static size_t otaReceivedSize;

    // 蓝牙回调类
    class MyServerCallbacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer);
        void onDisconnect(BLEServer* pServer);
    };
    
    // 特征回调类
    class MyCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic);
    };

    // OTA数据特征值回调类
    class OtaDataCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pChar);
    };

    // 处理蓝牙接收的数据
    static void handleBluetoothData(const char* data);

    // 处理WiFi配置
    static void handleWiFiConfig(const char* ssid, const char* password);

    // 处理OTA更新
    static void handleOTAUpdate(const char* data);

    // 处理控制命令
    static void handleControlCommand(const char* command);

    // 发送蓝牙响应
    static void sendResponse(const char* response);

    // 设置WiFi连接
    static void setupWiFi(const char* ssid, const char* password);
};

#endif // ESP32_CONTROL_H
