#pragma once
struct TSTask {
    TSTask(const char *, unsigned long, void (*)()) {}
};
struct TSScheduler {
    void addTask(TSTask *) {}
    void enableAll() {}
    void execute() {}
};
