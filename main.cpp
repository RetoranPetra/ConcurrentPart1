//Written by Caleb Evans ID:10288681

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

//Initialisation of constants
const int NO_TEAMS = 4;         //Number of teams in race
const int NO_MEMBERS = 4;       //Number of members per team
const int NO_TEAM_EXCHANGES = 3;//Number of exchange points per team

//Initialise threadmap
ThreadMap mp;

//Initialisation of arrays
std::thread theThreads[NO_TEAMS][NO_MEMBERS];
Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS];



//Agents setup, not put in seperate CPP/H files due to needing global variables
class SyncAgent {  //abstract base class 
public:
    SyncAgent() {} //constructor 
    // Declare  virtual methods to be overridden by derived classes 
    virtual void pause() = 0;
    virtual void proceed() = 0;
}; //end abstract class SyncAgent

class StartAgent : public SyncAgent {  //concrete class that CAN be instantiated 
public:
    StartAgent() {} //constructor 
    void pause() {
        {
            std::lock_guard<std::mutex> counterLock(counterMu);
            readyCounter++;
            //Checks if all teams are ready to start the race, if so releases all other threads and exits the function.
            if (readyCounter == NO_TEAMS) {
                proceed();
                return;
            }
        }
        //Implements condition variable check, will only break when proceed() is called. Otherwise caller of pause will be stuck in loop.
        std::unique_lock<std::mutex> lock(mu);
        while (!startingGun) {
            cv.wait(lock);
        }
        //Don't need to reset starting gun on release, as intended to release all threads only once.
    }
    void proceed() {
        // insert code to implement releasing of all athlete threads
        std::unique_lock<std::mutex> lock(mu);
        cv.notify_all();
        startingGun = true;
        //Don't need to "reset" starting gun, as it only needs to be "fired" once for race to start.
    }

private:
    // insert any necessary data members including variables, mutexes, locks, cond vars 
    int readyCounter = 0;//counter of all threads currently paused

    //Condition variable variables
    bool startingGun = false;
    std::mutex mu;
    std::condition_variable cv;
    
    //Mutex to avoid two simultaneous writes to counter
    std::mutex counterMu;
}; //end class StartAgent 

class EZAgent : public SyncAgent {  //concrete class that CAN be instantiated 
public:
    EZAgent() {} //constructor 
    void pause() {
        // insert code to implement pausing of next runner thread  
    }
    void proceed() {
        // insert code to implement releasing of next runner thread 
    }
private:
    // insert any necessary data members including variables, mutexes, locks, cond vars 
}; //end class EZAgent























//Setups up array of agents
EZAgent exchanges[NO_TEAMS][NO_TEAM_EXCHANGES];

//Mutex used to enforce sequential printing
std::mutex printLock;


std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)
int randGen(int low, int high) {
    std::uniform_int_distribution<> dis(low, high); //generate a random integer between 1-10.
    int n = dis(gen);
    return n;
}

//Passes competitor by reference
void run(Competitor& c,ThreadMap& mapIn,SyncAgent& agent) {
    //Doesn't need mutex as mutex has been implemented in class
    mapIn.insertThreadPair(c);
    //Waits for signal/batton
    agent.pause();

    //Random delay, using own funcs as rand isn't thread safe. Generates times between 10-15 seconds, as an estimate of athletes.
    std::this_thread::sleep_for(std::chrono::milliseconds(randGen(10000,15000)));
    //prints out values, to be legible needs to be mutex'd so prints display in order
    std::lock_guard<std::mutex> guard(printLock);
    c.printCompetitor();
}

int main() {
    //My Code

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
    /*
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
    */
    StartAgent smokingGun;
    for (int i = 0; i < NO_TEAMS; i++) {
        theThreads[i][0] = std::thread(run,
            std::ref(teamsAndMembers[i][0]),
            std::ref(mp),
            std::ref(smokingGun)
        );
    }
    for (int i = 0; i < NO_TEAMS; i++) {
        theThreads[i][0].join();
    }


    //Simple message to confirm execution
    std::cout << "All competitors finished!\n";
}
