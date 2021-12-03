#include "Agents.h"
namespace agents {
	//Simple function to grab teamnumber from current thread
	int getTeamNumber() {
		return std::stoi(mp.getThreadId().getTeam().substr(4, 1));
	}
	//Simple function to grab member number from current thread
	int getMemberNumber() {
		return std::stoi(mp.getThreadId().getPerson().substr(5, 1));
	}
}