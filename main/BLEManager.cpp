#include "BLEManager.h"

BLEManager::BLEManager() : 
    pServer(NULL),
    pService(NULL),
    pSsidCharacteristic(NULL),
    pPassCharacteristic(NULL),
    pStatusCharacteristic(NULL),
    pDataCharacteristic(NULL),
    isWifiConfigured(false),
    isWifiConnected(false) {
}

void BLEManager::begin() {
    Serial.println("ESP32 Starting...");
    initWiFi();
    setupBle();
}

void BLEManager::updateWifiStatus(const String& status) {
    if (pStatusCharacteristic != NULL) {
        pStatusCharacteristic->setValue(status);
        pStatusCharacteristic->notify();
        Serial.println(("Status Notification: " + status).c_str());
    }
}

BLEManager::WifiConfigCallbacks::WifiConfigCallbacks(BLEManager* mgr) : manager(mgr) {}

void BLEManager::WifiConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    String ssid = manager->pSsidCharacteristic->getValue();
    String pass = manager->pPassCharacteristic->getValue();

    if (ssid.length() > 0 && pass.length() > 0) {
        Serial.println("Received WiFi Credentials.");
        Serial.print("SSID: ");
        Serial.println(ssid);
        Serial.print("Password: ");
        Serial.println(pass);

        manager->updateWifiStatus("Connecting...");

        // 保存凭证到 NVS
        manager->preferences.begin("wifi-creds", false);
        manager->preferences.putString("ssid", ssid);
        manager->preferences.end();
        Serial.println("Credentials saved. Restarting ESP32 to apply...");

        // 通知 App 保存成功，并准备重启
        manager->updateWifiStatus("Credentials Saved. Restarting...");
        delay(1000);

        // 重启 ESP32 以应用新的 WiFi 设置
        ESP.restart();
    }
}

void BLEManager::initWiFi() {
    // 从 NVS (非易失性存储) 读取 WiFi 凭证
    preferences.begin("wifi-creds", true); // read-only
    String storedSsid = preferences.getString("ssid", "");
    String storedPassword = preferences.getString("password", "");
    preferences.end();

    if (storedSsid.length() > 0) {
        isWifiConfigured = true;
        Serial.println("WiFi is configured. Trying to connect...");
        Serial.print("SSID: ");
        Serial.println(storedSsid);

        WiFi.mode(WIFI_STA);
        WiFi.begin(storedSsid.c_str(), storedPassword.c_str());

        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            if (millis() - startTime > 15000) { // 15秒连接超时
                Serial.println("\nWiFi Connection Failed!");
                isWifiConnected = false;
                return;
            }
        }
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        isWifiConnected = true;
    } else {
        Serial.println("WiFi is not configured.");
        isWifiConfigured = false;
        isWifiConnected = false;
    }
}

void BLEManager::setupBle() {
    BLEDevice::init(DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);

    if (!isWifiConnected) {
        // --- 配网模式 ---
        Serial.println("Starting BLE in WiFi Configuration Mode.");

        // 创建 SSID 特征值
        pSsidCharacteristic = pService->createCharacteristic(
                                  CHARACTERISTIC_UUID_SSID,
                                  BLECharacteristic::PROPERTY_WRITE
                              );
        
        // 创建 Password 特征值
        pPassCharacteristic = pService->createCharacteristic(
                                  CHARACTERISTIC_UUID_PASS,
                                  BLECharacteristic::PROPERTY_WRITE
                              );
        // 为密码特征值设置回调，当密码写入后，触发配网逻辑
        pPassCharacteristic->setCallbacks(new WifiConfigCallbacks(this));

        // 创建 Status 特征值，用于通知 App
        pStatusCharacteristic = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID_STATUS,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_NOTIFY
                                );
        pStatusCharacteristic->setValue("Not Configured");
        
    } else {
        // --- 常规通信模式 ---
        Serial.println("Starting BLE in Normal Communication Mode.");
        pDataCharacteristic = pService->createCharacteristic(
                                  CHARACTERISTIC_UUID_DATA,
                                  BLECharacteristic::PROPERTY_READ |
                                  BLECharacteristic::PROPERTY_NOTIFY
                              );
        pDataCharacteristic->setValue("Hello from ESP32!");
    }

    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); 
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("BLE Advertising started.");
}

void BLEManager::loop() {
    if (isWifiConnected) {
        // --- WiFi 已连接，处于常规通信模式 ---
        // 检查 WiFi 连接状态，如果断开则尝试重连
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected. Reconnecting...");
            WiFi.reconnect();
            // 实际项目中可能需要更复杂的重连逻辑和状态管理
        }

        // 模拟数据更新，每2秒发送一次
        static unsigned long lastTime = 0;
        if (millis() - lastTime > 2000) {
            lastTime = millis();
            String data = "Uptime: " + String(millis() / 1000) + "s";
            pDataCharacteristic->setValue(data.c_str());
            pDataCharacteristic->notify();
            Serial.println("Sent data: " + data);
        }
    } else {
        // --- WiFi 未连接，处于配网模式 ---
        // 在此模式下，loop 中无事可做，所有逻辑由 BLE 回调驱动
    }
    delay(100); // 降低 CPU 占用
}