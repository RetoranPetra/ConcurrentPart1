#include "ThreadMap.h"

ThreadMap::ThreadMap() {};

// ThreadMap will compile but won't do anyhing useful until code is added to insertThreadPair and printMapContents
void ThreadMap::insertThreadPair(Competitor c) {
	std::lock_guard<std::mutex> guard(mapWriteLock);//needs mutex for exclusion to prevent two writes to map at same time.

    // create a threadID, Competitor pair using a call to std::make_pair 
    // store the pair in the map using the map insert member function
	
	//gets thread ID from thread it's running on
	threadComp.insert(std::make_pair(std::this_thread::get_id(), c));
}

Competitor ThreadMap::getThreadId() {
	std::lock_guard<std::mutex> guard(mapWriteLock);//needs mutex for exclusion to prevent read/write at same time

	std::map <std::thread::id, Competitor>::iterator it = threadComp.find(std::this_thread::get_id());
	if (it == threadComp.end())
		return Competitor::makeNull();						//	alternatively	return *(new Competitor(" ", " "));
	else 
		return it->second;									// i.e. the second item in the pair
}

void ThreadMap::printMapContents() {
	std::lock_guard<std::mutex> guard(mapWriteLock);//needs mutex for exclusion to prevent read/write at same time, more sophisticated version would allow multiple reads.

	std::cout << "MAP CONTENTS:" << "\n";
	int size = threadComp.size();

	for (
		std::map<std::thread::id, Competitor>::iterator index = threadComp.begin();//Constructs iterator for map datatype, then sets it to beginning of threadcomp
		index != threadComp.end();//While it hasn't reached the end, continue
		index++//Iterate index
		)
	{
		//Index points to the pair, so in order to access the pair's operators -> must be used.
		std::cout << "ID: " << index->first << "\n";
		index->second.printCompetitor(); //accesses print competitor for type competitor.
	}

	std::cout << "END MAP CONTENTS" << "\n";
}

int ThreadMap::ThreadMapSize() { 
	std::lock_guard<std::mutex> guard(mapWriteLock);
	return threadComp.size();
}
