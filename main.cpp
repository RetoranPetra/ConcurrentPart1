// AssignmentPart1.cpp : This file contains the 'main' function. Program execution begins and ends there.

//Std libraries

#include <iostream>
#include <string>
#include <mutex>
//Needed for random generation
#include <ctime>
#include <random>
//Internal headers
#include "Competitor.h"
#include "ThreadMap.h"

const int NO_TEAMS = 4;
const int NO_MEMBERS = 4;

//Mutex used to enforce sequential printing
std::mutex printLock;

std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)

int randGen(int low, int high) {
    std::uniform_int_distribution<> dis(low, high); //generate a random integer between 1-10.
    int n = dis(gen);
    return n;
}

//Passes competitor by reference
void run(Competitor& c,ThreadMap& mapIn) {
    //Doesn't need mutex as mutex has been implemented in class
    mapIn.insertThreadPair(c);


    //Random delay, using own funcs as rand isn't thread safe. Generates times between 10-15 seconds, as an estimate of athletes.
    std::this_thread::sleep_for(std::chrono::milliseconds(randGen(10000,15000)));
    //prints out values, to be legible needs to be mutex'd so prints display in order
    std::lock_guard<std::mutex> guard(printLock);
    c.printCompetitor();
}

int main() {
    //My Code

    //Initialisation of arrays
    std::thread theThreads[NO_TEAMS][NO_MEMBERS];
    Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS];

    //Initialise threadmap
    ThreadMap mp;

    std::cout << "Race Start!\n";

    //Defining teams and members
    //Defines team names as 1 to NO_TEAMS, defines 

    int totalCount = 0;
    for (int i = 0; i < NO_TEAMS; i++) {
        for (int j = 0; j < NO_MEMBERS; j++,totalCount++) {
            teamsAndMembers[i][j] = Competitor(         //Setting values of undefined competitors as competitors defined by the structure.
                "Team" + std::to_string(i + 1),         //i used to create team names for each competitor.
                "Robot"+std::to_string(totalCount+1));  //totalcount used to give unique names to the runners
        }
    }

    //Creation of threads
    for (int i = 0; i < NO_TEAMS; i++) {
        for (int j = 0; j < NO_MEMBERS; j++) {
            theThreads[i][j] = std::thread(run,   //Constructs thread to execute the function "run"
                std::ref(teamsAndMembers[i][j]),
                std::ref(mp)); //need to use std::ref wrapper to pass by reference to threads.
        }
    }

    //Joining the threads
    for (int i = 0; i < NO_TEAMS; i++) {
        for (int j = 0; j < NO_MEMBERS; j++) {
            theThreads[i][j].join();              //Joins all threads
        }
    }
    //Simple message to confirm execution
    std::cout << "All competitors finished!\n";
}
