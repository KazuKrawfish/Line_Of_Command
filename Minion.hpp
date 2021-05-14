#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>


//Forward declaration for MasterBoard.
class MasterBoard;
enum minionStatus { hasntmovedorfired, hasmovedhasntfired, hasfired, gaveupmovehasntfired};
enum rangeType {directFire, rangedFire};
class Minion
{	
	public:
	
	//Constructors
	Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam, MasterBoard* Environment, int inputHealth);
	Minion();
	~Minion();
	
	
	//Member variables. Should be protected with functions dealing with them!
	
	int seniority;				//Used to indicated where the minion resides within the minion array. Used to construct and destruct the minion.
	int locationX;
	int locationY;
	char type;					//Type of minion i.e. infantry, artillery, etc.
	int movementRange;
	int attackRange;
	std::string description;	//For user interface only.
	int team;
	double health;
	MasterBoard* minionEnvironment;
	minionStatus status;
	rangeType rangeType;
};


class typesOfMinions {
	char type;
	int movementRange;
	int attackRange;
	int cost;
	//Probably should move all minion aspects that wouldn't change, over here.
};

#endif /* MINION_HPP__ */

