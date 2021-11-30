#pragma once
#include <iostream>
#include <thread>
#include <map>
#include <string>
#include "Competitor.h"
class ThreadMap {
private:
    std::map <std::thread::id, Competitor> threadComp;  //map of thread ids v. Competitors
public:
    ThreadMap();
    void insertThreadPair(Competitor c);
    Competitor getThreadId();
    void printMapContents();
    int ThreadMapSize();
};
