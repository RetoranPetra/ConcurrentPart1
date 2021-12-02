#pragma once
#include <iostream>
#include <thread>
#include <map>
#include <string>
#include "Competitor.h"
#include <mutex>
class ThreadMap {
private:
    std::map <std::thread::id, Competitor> threadComp;  //map of thread ids v. Competitors
    std::mutex mapWriteLock;
public:
    ThreadMap();
    void insertThreadPair(Competitor c);
    Competitor getThreadId();
    void printMapContents();
    int ThreadMapSize();
};
