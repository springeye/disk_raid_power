//
// Created by develop on 2025/9/6.
//

#include "ota.h"

#include <LittleFS.h>
#include <log.h>

#include "esp_ota_ops.h"
#include "esp_system.h"
#include "lvgl.h"
#include "ui_scota.h"
const char* ssid = "henjue";
const char* password = "miaogou501";

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
AsyncWebServer server(80);

volatile size_t ota_total_size = 0;
volatile size_t ota_written_size = 0;
esp_ota_handle_t ota_handle = 0;
const esp_partition_t* update_partition = nullptr;
bool ota_has_error = false;

void handleRoot(AsyncWebServerRequest *request)
{
    request->redirect("/update");
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
    server.on("/progress", HTTP_GET, [](AsyncWebServerRequest *request){
        int percent = 0;
        if (ota_total_size > 0) {
            percent = (int)((ota_written_size * 100) / ota_total_size);
        }
        String json = String("{\"percent\":") + percent + "}";
        request->send(200, "application/json", json);
    });

    server.on("/", HTTP_GET, handleRoot);
    server.serveStatic("/update", LittleFS, "/").setDefaultFile("ota.html");
    server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
        String message = ota_has_error ? "更新失败" : "更新成功。重新启动…";
        request->send(200, "text/html", "<span style='font-size: 24px;'>" + message + "</span>");
        delay(500);
        esp_restart();
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if (index == 0) {
            ota_total_size = 0;
            ota_written_size = 0;
            ota_has_error = false;
            mylog.printf("Update: %s\n", filename.c_str());
            update_partition = esp_ota_get_next_update_partition(NULL);
            if (!update_partition) {
                mylog.println("No OTA partition found!");
                ota_has_error = true;
                return;
            }
            esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
            if (err != ESP_OK) {
                mylog.printf("esp_ota_begin failed: %d\n", err);
                ota_has_error = true;
            }
        }
        ota_written_size += len;
        mylog.printf("write progress %d/%d\n", ota_written_size, request->contentLength());
        if (!ota_has_error) {
            esp_err_t err = esp_ota_write(ota_handle, data, len);
            if (err != ESP_OK) {
                mylog.printf("esp_ota_write failed: %d\n", err);
                ota_has_error = true;
            }
        }
        if (final) {
            ota_total_size = request->contentLength();
            if (!ota_has_error) {
                esp_err_t err = esp_ota_end(ota_handle);
                if (err != ESP_OK) {
                    mylog.printf("esp_ota_end failed: %d\n", err);
                    ota_has_error = true;
                } else {
                    err = esp_ota_set_boot_partition(update_partition);
                    if (err != ESP_OK) {
                        mylog.printf("esp_ota_set_boot_partition failed: %d\n", err);
                        ota_has_error = true;
                    } else {
                        mylog.printf("Update Success: %u bytes\n", ota_total_size);
                    }
                }
            }
        }
    });
    server.begin();
}

void ota_loop() {
    // ESPAsyncWebServer无需轮询
}

void destory_ota() {}
