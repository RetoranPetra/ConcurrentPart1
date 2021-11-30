#pragma once
#include <string>
//Removed namespace STD as can cause problems in knowing which namespace you're in when included in header files

class Competitor { // created in main and never updated, passed to a thread, placed in map
private:
    std::string teamName;
    std::string personName;
public:
    Competitor();
    Competitor(std::string tN, std::string pN);
    void setTeam(std::string tN);
    std::string getTeam();
    void setPerson(std::string pN);
    std::string getPerson();
    static Competitor makeNull();
    void printCompetitor();
};

