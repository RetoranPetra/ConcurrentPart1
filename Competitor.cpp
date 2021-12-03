#include "Competitor.h"

Competitor::Competitor() {}

Competitor::Competitor(std::string tN, std::string pN) : teamName(tN), personName(pN) {}

void Competitor::setTeam(std::string tN) { teamName = tN; }
std::string Competitor::getTeam() { return teamName; }

void Competitor::setPerson(std::string pN) { personName = pN; }
std::string Competitor::getPerson() { return personName; }

Competitor Competitor::makeNull() {
    return *(new Competitor(" ", " "));
}

void Competitor::printCompetitor() {
    std::cout << "Team = " << teamName << " Person = " << personName << "\n";
}
void Competitor::setRaceTime(int rt){
    raceTime = rt;
}
int Competitor::getRaceTime(){
    return raceTime;
}