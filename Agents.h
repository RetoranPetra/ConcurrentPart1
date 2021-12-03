#pragma once
#include <mutex>
#include "ThreadMap.h"
#include "Competitor.h"
#include <string>

//Needed for random generation
#include <ctime>
#include <random>


namespace agents {
	//Initialisation of constants
	const int NO_TEAMS = 4;         //Number of teams in race
	const int NO_MEMBERS = 4;       //Number of members per team
	const int NO_TEAM_EXCHANGES = 3;//Number of exchange points per team

	//Initialise threadmap
	ThreadMap mp;

	//Initialisation of arrays
	std::thread theThreads[NO_TEAMS][NO_MEMBERS];
	Competitor teamsAndMembers[NO_TEAMS][NO_MEMBERS];


	//As both classes need to access EZAgent's condition vars, mutexes and bools, they will be declared here. There might be a better way to do this, but so far I have found no other way.
	std::mutex muS[NO_TEAMS][NO_TEAM_EXCHANGES];
	std::condition_variable cvS[NO_TEAMS][NO_TEAM_EXCHANGES];
	bool batonFlags[NO_TEAMS][NO_TEAM_EXCHANGES] = {};

	//Simple function to grab teamnumber from current thread
    int getTeamNumber();
	//Simple function to grab member number from current thread
    int getMemberNumber();

	std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)
	int randGen(int low, int high) {
		std::uniform_int_distribution<> dis(low, high); //generate a random integer between 1-10.
		int n = dis(gen);
		return n;
	}


	void runnerDelay(void) {
		//Random delay, using own funcs as rand isn't thread safe. Generates times between 10-15 seconds, as an estimate of athletes.
		std::this_thread::sleep_for(std::chrono::milliseconds(randGen(10000, 15000)));
	}

	void startUpDelay(void) {
		//Emulates reaction speed to gunshot + time to get moving
		std::this_thread::sleep_for(std::chrono::milliseconds(randGen(100, 300)));
	}

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
                    //waits before starting, simulates ready time of real athlete
                    startUpDelay();
                    proceed();
                }
            }
            //Implements condition variable check, will only break when proceed() is called. Otherwise caller of pause will be stuck in loop.
            //Scope needed to prevent inteference with runnerdelay afterwards due to mutexlock.
            {
                std::unique_lock<std::mutex> lock(mu);
                while (!startingGun) {
                    cv.wait(lock);
                }
            }
            runnerDelay();
            //Releases thread next in line to run
            int teamNum = getTeamNumber();
            std::unique_lock<std::mutex> lock(muS[teamNum][0]);
            cvS[teamNum][0].notify_all();
            batonFlags[teamNum][0] = true;

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
        EZAgent() {}
        void pause() {
            teamNum = getTeamNumber();
            memberNum = getMemberNumber() - 1;//Needs to be -1 for correct index
            //Locks down current thread, needs unlocking from another.
            {
                std::unique_lock<std::mutex> lock(muS[teamNum][memberNum]);
                while (!batonFlags[teamNum][memberNum]) {
                    cvS[teamNum][memberNum].wait(lock);
                }
            }
            runnerDelay();
            proceed();//Allows next runner to continue
        }
        void proceed() {
            teamNum = getTeamNumber();
            memberNum = getMemberNumber();//Needs to be -1 for correct index
            if (memberNum == NO_MEMBERS - 1) { return; }
            // insert code to implement releasing of next runner thread
            std::unique_lock<std::mutex> lock(muS[teamNum][memberNum]);
            cvS[teamNum][memberNum].notify_all();
            batonFlags[teamNum][memberNum] = true;
            //Don't need to "reset" starting gun, as it only needs to be "fired" once for race to start.
        }
    private:
        // insert any necessary data members including variables, mutexes, locks, cond vars
        int teamNum;
        int memberNum;
    }; //end class EZAgent
}