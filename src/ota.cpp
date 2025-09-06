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
    "#otaProgress { width: 80%; height: 30px; margin: 20px auto; }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>欢迎使用 ESP32 OTA 更新</h1>"
    "<form id='otaForm' method='POST' action='/update' enctype='multipart/form-data'>"
    "<input type='file' name='update' accept='.bin'>"
    "<input type='submit' value='上传固件'>"
    "</form>"
    "<progress id='otaProgress' value='0' max='100' style='width:80%;height:30px;'></progress>"
    "<div id='progressText' style='font-size:18px;margin-top:10px;'></div>"
    "<script>"
    "var form = document.getElementById('otaForm');"
    "var progress = document.getElementById('otaProgress');"
    "var progressText = document.getElementById('progressText');"
    "form.onsubmit = function(e) {"
    "  e.preventDefault();"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('POST', '/update', true);"
    "  xhr.upload.onprogress = function(ev) {"
    "    if (ev.lengthComputable) {"
    "      var percent = Math.round((ev.loaded / ev.total) * 100);"
    "      progress.value = percent;"
    "      progressText.innerText = '更新进度: ' + percent + '%';"
    "    }"
    "  };"
    "  xhr.onreadystatechange = function() {"
    "    if (xhr.readyState == 4) {"
    "      progress.value = 100;"
    "      progressText.innerText = '烧录完成，设备即将重启！';"
    "    }"
    "  };"
    "  var fd = new FormData(form);"
    "  xhr.send(fd);"
    "};"
    "</script>"
    "</body>"
    "</html>";
WebServer server(80);

volatile size_t ota_total_size = 0;
volatile size_t ota_written_size = 0;

void handleRoot()
{
    server.sendHeader("Location", "/update");
    server.send(302, "text/plain", "");
}

void setup_ota()
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        mylog.println("Connecting to WiFi...");
    }
    //打印当前IP
    mylog.println("Connected to WiFi!");
    auto local_ip = WiFi.localIP();
    mylog.printf("IP Address: %s", local_ip.toString().c_str());
    //这里给ui_ipaddr这个lvgl的label设置文本为当前IP
    char buf[32];
    snprintf(buf, sizeof(buf), "IP: %s", local_ip.toString().c_str());
    lv_label_set_text(uic_ipaddr, buf);

    // OTA进度接口
    server.on("/progress", HTTP_GET, []() {
        int percent = 0;
        if (ota_total_size > 0) {
            percent = (int)((ota_written_size * 100) / ota_total_size);
        }
        server.send(200, "application/json", String("{\"percent\":") + percent + "}");
    });

    server.on("/", HTTP_GET, handleRoot); // 根路由重定向到 OTA 页面
    server.on("/update", HTTP_GET, []()
    {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", updateIndex);
    });

    server.on("/update", HTTP_POST, []()
              {
                  server.sendHeader("Connection", "close");
                  String message = Update.hasError() ? "更新失败" : "更新成功。重新启动…";
                  server.sendHeader("Content-Type", "text/html; charset=utf-8");
                  server.send(200, "text/html", "<span style='font-size: 24px;'>" + message + "</span>");
                  delay(500);
                  ESP.restart();
              }, []()
              {
                  HTTPUpload& upload = server.upload(); //用于处理上传的文件数据
                  if (upload.status == UPLOAD_FILE_START)
                  {
                      ota_total_size = 0;
                      ota_written_size = 0;
                      mylog.printf("Update: %s\n", upload.filename.c_str());
                      if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                      {
                          // 以最大可用大小开始
                          Update.printError(mylog);
                      }
                  }
                  else if (upload.status == UPLOAD_FILE_WRITE)
                  {
                      ota_written_size += upload.currentSize;
                      // 将接收到的数据写入Update对象
                      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                      {
                          Update.printError(USBSerial);
                      }
                  }
                  else if (upload.status == UPLOAD_FILE_END)
                  {
                      ota_total_size = upload.totalSize;
                      if (Update.end(true))
                      {
                          // 设置大小为当前大小
                          mylog.printf("Update Success: %u bytes\n", upload.totalSize);
                      }
                      else
                      {
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
