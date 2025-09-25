#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H
#include <functional>
#include <vector>
#include <Arduino.h>

class TaskScheduler {
public:
    struct Task {
        std::function<void()> callback;
        unsigned long interval;
        unsigned long lastRun;
        Task(std::function<void()> cb, unsigned long inter)
            : callback(cb), interval(inter), lastRun(0) {}
    };
    std::vector<Task> tasks;
    void addTask(std::function<void()> cb, unsigned long interval) {
        tasks.emplace_back(cb, interval);
    }
    void tick() {
        unsigned long now = millis();
        for (auto& t : tasks) {
            if (now - t.lastRun >= t.interval) {
                t.callback();
                t.lastRun = now;
            }
        }
    }
};

#endif // TASK_SCHEDULER_H

