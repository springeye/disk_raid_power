//
// Created by develop on 2025/9/6.
//

#ifndef DISK_RAID_POWER_OTA_H
#define DISK_RAID_POWER_OTA_H
#include <WiFi.h>
#include <Update.h>
#include <cmath>
#include <WebServer.h>
#include <Update.h>


void setup_ota();
void destory_ota();
void ota_loop();
#endif //DISK_RAID_POWER_OTA_H