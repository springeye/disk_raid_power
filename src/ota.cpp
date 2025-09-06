//
// Created by develop on 2025/9/6.
//

#include "ota.h"

#include <log.h>

#include "lvgl.h"
#include "ui_scota.h"
const char* ssid = "henjue";
const char* password = "miaogou501";
const char* updateIndex =
  "<html>"
  "<head>"
  "<meta charset=\"UTF-8\">"
  "<title>ESP32 OTA 更新</title>"
  "<style>"
  "body { font-family: Arial, sans-serif; text-align: center; }"
  "h1 { color: #333; }"
  "form { margin-top: 20px; }"
  "input[type=file] { display: block; margin: 20px auto; }"
  "input[type=submit] { margin-top: 20px; padding: 10px 20px; font-size: 18px; }"
  "</style>"
  "</head>"
  "<body>"
  "<h1>欢迎使用 ESP32 OTA 更新</h1>"
  "<form method='POST' action='/update' enctype='multipart/form-data'>"
  "<input type='file' name='update' accept='.bin'>"
  "<input type='submit' value='上传固件'>"
  "</form>"
  "</body>"
  "</html>";
WebServer server(80);
void handleRoot() {
    server.sendHeader("Location", "/update");
    server.send(302, "text/plain", "");
}
void setup_ota()
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        myprintln("Connecting to WiFi...");
    }
    //打印当前IP
    myprintln("Connected to WiFi!");
    auto local_ip = WiFi.localIP();
    myprintf("IP Address: %s",local_ip.toString().c_str());
    //这里给ui_ipaddr这个lvgl的label设置文本为当前IP
    char buf[32];
    snprintf(buf, sizeof(buf), "IP: %s", local_ip.toString().c_str());
    lv_label_set_text(uic_ipaddr, buf);

    // 初始化 OTA 更新
    if (Update.begin()) {
        Serial.println("OTA update begin...");
        // 在这里写入固件数据
        // 结束 OTA 更新过程
        Update.end();
        Serial.println("OTA update complete!");
    } else {
        Serial.println("OTA update failed!");
    }
    server.on("/", HTTP_GET, handleRoot); // 根路由重定向到 OTA 页面
    server.on("/update", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", updateIndex);
      });

    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");

      //动态显示结果
    String message = Update.hasError() ? "更新失败" : "更新成功。重新启动…";
    server.sendHeader("Content-Type", "text/html; charset=utf-8");
    server.send(200, "text/html", "<span style='font-size: 24px;'>" + message + "</span>");


      delay(1000);
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload(); //用于处理上传的文件数据
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // 以最大可用大小开始
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // 将接收到的数据写入Update对象
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { // 设置大小为当前大小
          Serial.printf("Update Success: %u bytes\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });
    server.begin();

}
void ota_loop()
{
  server.handleClient();
}

void destory_ota()
{
}
