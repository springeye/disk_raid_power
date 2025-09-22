#include "ESP32Control.h"

// 静态成员变量初始化
BLEServer* ESP32Control::pServer = nullptr;
BLECharacteristic* ESP32Control::pCharacteristic = nullptr;
bool ESP32Control::deviceConnected = false;
bool ESP32Control::oldDeviceConnected = false;
int ESP32Control::deviceParameter = 0;
String ESP32Control::deviceStatus = "Ready";

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
        
        // 定期发送设备状态
        String status = "STATUS:Param=" + String(deviceParameter) + 
                       ",WiFi=" + String(isWiFiConnected() ? "Connected" : "Disconnected") +
                       ",RSSI=" + String(WiFi.RSSI());
        sendResponse(status);
    }
}

bool ESP32Control::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String ESP32Control::getDeviceData() {
    return "Device Parameter: " + String(deviceParameter) + 
           ", WiFi: " + String(isWiFiConnected() ? "Connected" : "Disconnected") +
           ", Free Memory: " + String(esp_get_free_heap_size()) + " bytes";
}

void ESP32Control::setDeviceParameter(int value) {
    deviceParameter = value;
    Serial.print("设备参数已更新: ");
    Serial.println(value);
    
    // 通知客户端参数变化
    if (deviceConnected) {
        sendResponse("PARAM_UPDATED:" + String(value));
    }
}

// 蓝牙服务器回调
void ESP32Control::MyServerCallbacks::onConnect(BLEServer* pServer) {
    ESP32Control::deviceConnected = true;
    Serial.println("客户端已连接");
}

void ESP32Control::MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    ESP32Control::deviceConnected = false;
    Serial.println("客户端已断开连接");
}

// 特征回调
void ESP32Control::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
        Serial.print("收到数据: ");
        Serial.println(value.c_str());
        handleBluetoothData(value);
    }
}

void ESP32Control::handleBluetoothData(String data) {
    String receivedData = String(data.c_str());

    // 处理WiFi配置
    if (receivedData.startsWith("WIFI:")) {
        int separatorIndex = receivedData.indexOf(';', 5);
        if (separatorIndex != -1) {
            String ssid = receivedData.substring(5, separatorIndex);
            String password = receivedData.substring(separatorIndex + 1);
            handleWiFiConfig(ssid, password);
        }
    }
    // 处理OTA更新
    else if (receivedData.startsWith("OTA:")) {
        handleOTAUpdate(data);
    }
    // 处理控制命令
    else if (receivedData.startsWith("CMD:")) {
        handleControlCommand(data);
    }
    // 处理读取请求
    else if (receivedData == "READ_DATA") {
        sendResponse("DATA:" + getDeviceData());
    }
    // 处理参数设置
    else if (receivedData.startsWith("SET_PARAM:")) {
        int value = receivedData.substring(10).toInt();
        setDeviceParameter(value);
    }
    else {
        Serial.print("未知命令: ");
        Serial.println(receivedData.c_str());
        sendResponse("ERROR:Unknown command");
    }
}

void ESP32Control::handleWiFiConfig(String ssid, String password) {
    Serial.print("配置WiFi: ");
    Serial.print(ssid);
    Serial.print("/");
    Serial.println(password);

    sendResponse("WIFI:Connecting...");
    setupWiFi(ssid, password);
}

void ESP32Control::handleOTAUpdate(String data) {
    String otaCommand = String(data.c_str());

    if (otaCommand.startsWith("OTA:START:")) {
        // 开始OTA更新
        size_t fileSize = otaCommand.substring(10).toInt();
        Serial.print("开始OTA更新，文件大小: ");
        Serial.println(fileSize);

        if (Update.begin(fileSize)) {
            sendResponse("OTA:READY");
        } else {
            sendResponse("OTA:FAIL:Begin failed");
        }
    }
    else if (otaCommand.startsWith("OTA:DATA")) {
        // 处理OTA数据
        String otaData = data.substring(8);
        size_t written = Update.write((uint8_t*)otaData.c_str(), otaData.length());

        if (written != otaData.length()) {
            sendResponse("OTA:FAIL:Write error");
        }
    }
    else if (otaCommand == "OTA:END") {
        // 结束OTA
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

void ESP32Control::handleControlCommand(String command) {
    String cmd = String(command.c_str());

    if (cmd == "CMD:RESTART") {
        sendResponse("RESTARTING...");
        delay(1000);
        ESP.restart();
    }
    else if (cmd == "CMD:GET_STATUS") {
        sendResponse("STATUS:" + getDeviceData());
    }
    else if (cmd.startsWith("CMD:SET_GPIO:")) {
        // 示例：GPIO控制
        int gpioPin = cmd.substring(13).toInt();
        // 这里可以添加实际的GPIO控制逻辑
        sendResponse("GPIO_SET:" + String(gpioPin));
    }
    else {
        sendResponse("ERROR:Unknown control command");
    }
}

void ESP32Control::sendResponse(String response) {
    if (pCharacteristic && deviceConnected) {
        pCharacteristic->setValue(response.c_str());
        pCharacteristic->notify();
        Serial.print("发送响应: ");
        Serial.println(response);
    }
}

void ESP32Control::setupWiFi(String ssid, String password) {
    Serial.println("正在连接WiFi...");
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi连接成功");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        
        sendResponse("WIFI:Connected;" + WiFi.localIP().toString());
    } else {
        Serial.println("\nWiFi连接失败");
        sendResponse("WIFI:Failed to connect");
    }
}