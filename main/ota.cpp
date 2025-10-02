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
const char* ssid = "disk_raid_power";
const char* password = "12345678";

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
AsyncWebServer* server = nullptr;

volatile size_t ota_total_size = 0;
volatile size_t ota_written_size = 0;
esp_ota_handle_t ota_handle = 0;
const esp_partition_t* update_partition = nullptr;
bool ota_has_error = false;

void handleRoot(AsyncWebServerRequest *request)
{
    request->redirect("/update");
}
//显示一个二维码
void lv_example_qrcode_1(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
}
// 设置固定IP地址
IPAddress local_IP(192, 168, 44, 1);
IPAddress gateway(192, 168, 44, 1);
IPAddress subnet(255, 255, 255, 0);
void setup_ota()
{
    // 配置ESP32为AP模式
    WiFi.mode(WIFI_AP);
    // 设置固定IP
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        mylog.println("AP配置失败!");
        return;
    }
    // 启动热点
    if (WiFi.softAP(ssid, password)) {
        mylog.println("热点已启动");
        mylog.print("SSID: ");
        mylog.println(ssid);
        auto ip=WiFi.softAPIP();
        mylog.print("IP地址: ");
        char ipStr3[16];
        snprintf(ipStr3, sizeof(ipStr3), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        mylog.println(ipStr3);
        char buf[32];
        snprintf(buf, sizeof(buf), "IP: %s", ipStr3);
        lv_label_set_text(uic_ipaddr, buf);
    } else {
        mylog.println("热点启动失败!");
        lv_label_set_text(uic_ipaddr, "热点启动失败!");
    }

    server = new AsyncWebServer(80);
    server->on("/progress", HTTP_GET, [](AsyncWebServerRequest *request){
        int percent = 0;
        if (ota_total_size > 0) {
            percent = (int)((ota_written_size * 100) / ota_total_size);
        }
        char json[32];
        snprintf(json, sizeof(json), "{\"percent\":%d}", percent);
        request->send(200, "application/json", json);
    });
    server->on("/", HTTP_GET, handleRoot);
    server->serveStatic("/update", LittleFS, "/web/").setDefaultFile("ota.html");
    server->serveStatic("/", LittleFS, "/web/");
    server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
        const char* message = ota_has_error ? "更新失败" : "更新成功。重新启动…";
        char html[128];
        snprintf(html, sizeof(html), "<span style='font-size: 24px;'>%s</span>", message);
        request->send(200, "text/html", html);
        delay(500);
        if (server) { server->end(); delete server; server = nullptr; }
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
    server->begin();
}

void ota_loop() {
    // ESPAsyncWebServer无需轮询
    // 显示连接到热点的设备数量
    mylog.print("连接设备数: ");
    mylog.println(WiFi.softAPgetStationNum());
}

void destory_ota()
{
    // 断开所有连接的设备并关闭热点
    bool result = WiFi.softAPdisconnect(true);

    if(result) {
        mylog.println("热点已成功关闭");
    } else {
        mylog.println("热点关闭失败");
    }

    // 可选：完全禁用WiFi以节省功耗
    WiFi.mode(WIFI_OFF);
    mylog.println("WiFi已完全禁用");
}
