//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_TEMP_H
#define DISK_RAID_POWER_TEMP_H

void init_temp();
float read_temp();
float read_temp_avg(int samples = 20);
#endif //DISK_RAID_POWER_TEMP_H