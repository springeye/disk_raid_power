//
// Created by develop on 2025/9/6.
//

#include "ota.h"

#include <log.h>
#include <ota_web_ui.h>

#include "esp_ota_ops.h"
#include "esp_system.h"
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

esp_ota_handle_t ota_handle = 0;
const esp_partition_t* update_partition = nullptr;
bool ota_has_error = false;

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
        // auto content = getOTAPageHTML().c_str();
        // server.setContentLength(strlen_P(content));
        // server.send(200, "text/html", "");
        // server.sendContent_P(content);
    });

    server.on("/update", HTTP_POST, []()
              {
                  server.sendHeader("Connection", "close");
                  String message = ota_has_error ? "更新失败" : "更新成功。重新启动…";
                  server.sendHeader("Content-Type", "text/html; charset=utf-8");
                  server.send(200, "text/html", "<span style='font-size: 24px;'>" + message + "</span>");
                  delay(500);
                  esp_restart();
              }, []()
              {
                  HTTPUpload& upload = server.upload();
                  if (upload.status == UPLOAD_FILE_START)
                  {
                      ota_total_size = 0;
                      ota_written_size = 0;
                      ota_has_error = false;
                      mylog.printf("Update: %s\n", upload.filename.c_str());
                      update_partition = esp_ota_get_next_update_partition(NULL);
                      if (!update_partition)
                      {
                          mylog.println("No OTA partition found!");
                          ota_has_error = true;
                          return;
                      }
                      esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
                      if (err != ESP_OK)
                      {
                          mylog.printf("esp_ota_begin failed: %d\n", err);
                          ota_has_error = true;
                      }
                  }
                  else if (upload.status == UPLOAD_FILE_WRITE)
                  {
                      ota_written_size += upload.currentSize;
                      mylog.printf("write progress %d/%d\n",ota_written_size,upload.totalSize);
                      if (!ota_has_error)
                      {
                          esp_err_t err = esp_ota_write(ota_handle, upload.buf, upload.currentSize);
                          if (err != ESP_OK)
                          {
                              mylog.printf("esp_ota_write failed: %d\n", err);
                              ota_has_error = true;
                          }
                      }
                  }
                  else if (upload.status == UPLOAD_FILE_END)
                  {

                      ota_total_size = upload.totalSize;
                      if (!ota_has_error)
                      {
                          esp_err_t err = esp_ota_end(ota_handle);
                          if (err != ESP_OK)
                          {
                              mylog.printf("esp_ota_end failed: %d\n", err);
                              ota_has_error = true;
                          }
                          else
                          {
                              err = esp_ota_set_boot_partition(update_partition);
                              if (err != ESP_OK)
                              {
                                  mylog.printf("esp_ota_set_boot_partition failed: %d\n", err);
                                  ota_has_error = true;
                              }
                              else
                              {
                                  mylog.printf("Update Success: %u bytes\n", upload.totalSize);
                              }
                          }
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
