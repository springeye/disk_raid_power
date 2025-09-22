#include "ESP32Control.h"

// 静态成员变量初始化
BLEServer* ESP32Control::pServer = nullptr;
BLECharacteristic* ESP32Control::pCharacteristic = nullptr;
bool ESP32Control::deviceConnected = false;
bool ESP32Control::oldDeviceConnected = false;
int ESP32Control::deviceParameter = 0;
char ESP32Control::deviceStatus[32] = "Ready";

void ESP32Control::begin(const char* deviceName) {
    Serial.begin(115200);
    Serial.println("初始化ESP32控制模块...");
    
    // 初始化蓝牙
    BLEDevice::init(deviceName);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // 创建服务
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // 创建可读写的特征
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());
    
    // 启动服务
    pService->start();
    
    // 开始广播
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("蓝牙服务已启动，等待客户端连接...");
    Serial.print("设备名称: ");
    Serial.println(deviceName);
#if defined(ESP_PLATFORM)
    // 释放未用的 Classic BT 内存
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
#endif
    Serial.print("Free heap before WiFi: ");
    Serial.println(esp_get_free_heap_size());
}

void ESP32Control::loop() {
    // 处理蓝牙连接状态变化
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // 给蓝牙栈时间
        pServer->startAdvertising();
        Serial.println("设备断开连接，重新广播...");
        oldDeviceConnected = deviceConnected;
    }
    
    if (deviceConnected && !oldDeviceConnected) {
        Serial.println("设备已连接");
        oldDeviceConnected = deviceConnected;
        
        // 发送连接成功消息
        sendResponse("CONNECTED:ESP32_Ready");
    }
    
    // 可以在这里添加定期状态更新
    static unsigned long lastUpdate = 0;
    if (deviceConnected && millis() - lastUpdate > 5000) {
        lastUpdate = millis();
        char status[128];
        snprintf(status, sizeof(status), "STATUS:Param=%d,WiFi=%s,RSSI=%d", deviceParameter, isWiFiConnected() ? "Connected" : "Disconnected", WiFi.RSSI());
        sendResponse(status);
    }
}

bool ESP32Control::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void ESP32Control::getDeviceData(char* out, size_t out_size) {
    snprintf(out, out_size, "Device Parameter: %d, WiFi: %s, Free Memory: %u bytes", deviceParameter, isWiFiConnected() ? "Connected" : "Disconnected", (unsigned int)esp_get_free_heap_size());
}

void ESP32Control::setDeviceParameter(int value) {
    deviceParameter = value;
    Serial.print("设备参数已更新: ");
    Serial.println(value);
    if (deviceConnected) {
        char buf[32];
        snprintf(buf, sizeof(buf), "PARAM_UPDATED:%d", value);
        sendResponse(buf);
    }
}

void ESP32Control::MyServerCallbacks::onConnect(BLEServer* pServer) {
    ESP32Control::deviceConnected = true;
    Serial.println("客户端已连接");
}

void ESP32Control::MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    ESP32Control::deviceConnected = false;
    Serial.println("客户端已断开连接");
}

void ESP32Control::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
        Serial.print("收到数据: ");
        Serial.println(value.c_str());
        handleBluetoothData(value.c_str());
    }
}

void ESP32Control::handleBluetoothData(const char* data) {
    if (strncmp(data, "WIFI:", 5) == 0) {
        const char* sep = strchr(data + 5, ';');
        if (sep) {
            char ssid[64] = {0};
            char password[64] = {0};
            size_t ssid_len = sep - (data + 5);
            strncpy(ssid, data + 5, ssid_len > 63 ? 63 : ssid_len);
            strncpy(password, sep + 1, 63);
            handleWiFiConfig(ssid, password);
        }
    } else if (strncmp(data, "OTA:", 4) == 0) {
        handleOTAUpdate(data);
    } else if (strncmp(data, "CMD:", 4) == 0) {
        handleControlCommand(data);
    } else if (strcmp(data, "READ_DATA") == 0) {
        char buf[128];
        getDeviceData(buf, sizeof(buf));
        char resp[160];
        snprintf(resp, sizeof(resp), "DATA:%s", buf);
        sendResponse(resp);
    } else if (strncmp(data, "SET_PARAM:", 10) == 0) {
        int value = atoi(data + 10);
        setDeviceParameter(value);
    } else {
        Serial.print("未知命令: ");
        Serial.println(data);
        sendResponse("ERROR:Unknown command");
    }
}

void ESP32Control::handleWiFiConfig(const char* ssid, const char* password) {
    Serial.print("配置WiFi: ");
    Serial.print(ssid);
    Serial.print("/");
    Serial.println(password);
    sendResponse("WIFI:Connecting...");
    setupWiFi(ssid, password);
}

void ESP32Control::handleOTAUpdate(const char* data) {
    if (strncmp(data, "OTA:START:", 10) == 0) {
        size_t fileSize = atoi(data + 10);
        Serial.print("开始OTA更新，文件大小: ");
        Serial.println(fileSize);
        if (Update.begin(fileSize)) {
            sendResponse("OTA:READY");
        } else {
            sendResponse("OTA:FAIL:Begin failed");
        }
    } else if (strncmp(data, "OTA:DATA", 8) == 0) {
        const char* otaData = data + 8;
        size_t len = strlen(otaData);
        size_t written = Update.write((uint8_t*)otaData, len);
        if (written != len) {
            sendResponse("OTA:FAIL:Write error");
        }
    } else if (strcmp(data, "OTA:END") == 0) {
        if (Update.end(true)) {
            sendResponse("OTA:SUCCESS");
            Serial.println("OTA更新成功，准备重启...");
            delay(1000);
            ESP.restart();
        } else {
            sendResponse("OTA:FAIL:End failed");
        }
    }
}

void ESP32Control::handleControlCommand(const char* command) {
    if (strcmp(command, "CMD:RESTART") == 0) {
        sendResponse("RESTARTING...");
        delay(1000);
        ESP.restart();
    } else if (strcmp(command, "CMD:GET_STATUS") == 0) {
        char buf[128];
        getDeviceData(buf, sizeof(buf));
        char resp[160];
        snprintf(resp, sizeof(resp), "STATUS:%s", buf);
        sendResponse(resp);
    } else if (strncmp(command, "CMD:SET_GPIO:", 13) == 0) {
        int gpioPin = atoi(command + 13);
        char buf[32];
        snprintf(buf, sizeof(buf), "GPIO_SET:%d", gpioPin);
        sendResponse(buf);
    } else {
        sendResponse("ERROR:Unknown control command");
    }
}

void ESP32Control::sendResponse(const char* response) {
    if (pCharacteristic && deviceConnected) {
        pCharacteristic->setValue(response);
        pCharacteristic->notify();
        Serial.print("发送响应: ");
        Serial.println(response);
    }
}

void ESP32Control::setupWiFi(const char* ssid, const char* password) {
    Serial.println("正在连接WiFi...");
    Serial.print("Free heap before WiFi.begin: ");
    Serial.println(esp_get_free_heap_size());
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.println("调用 WiFi.begin 前");
    WiFi.begin(ssid, password);
    Serial.print("Free heap after WiFi.begin: ");
    Serial.println(esp_get_free_heap_size());
    if (WiFi.status() != WL_CONNECT_FAILED && WiFi.status() != WL_NO_SSID_AVAIL && WiFi.status() != WL_IDLE_STATUS) {
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
            delay(500);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi连接成功");
            Serial.print("IP地址: ");
            Serial.println(WiFi.localIP());
            char buf[64];
            snprintf(buf, sizeof(buf), "WIFI:Connected;%s", WiFi.localIP().toString().c_str());
            sendResponse(buf);
        } else {
            Serial.println("\nWiFi连接失败");
            sendResponse("WIFI:Failed to connect");
        }
    } else {
        Serial.println("WiFi.begin failed: 内存或配置错误");
        sendResponse("WIFI:Failed to start WiFi");
    }
}