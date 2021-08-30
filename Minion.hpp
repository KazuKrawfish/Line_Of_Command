#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>


//Forward declaration for MasterBoard.
class MasterBoard;
enum minionStatus { hasntmovedorfired, hasmovedhasntfired, hasfired, gaveupmovehasntfired};
enum RangeType {directFire, rangedFire};
enum specialtyGroupType {normal, infantry, transport};
enum domainType {land, air, sea};


class Minion
{	
	public:
	
	//Constructors
	Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam, MasterBoard* Environment, int inputHealth, int inputVeterancy, int beingTransported, int inputFuel);
	Minion();
	~Minion();
	
	
	//Member variables. Should be protected with functions dealing with them!
	
	int seniority = -1;		//Used to indicated where the minion resides within the minion array. Used to construct and destruct the minion.
	int locationX = -1;
	int locationY = -1;
	char type = 'x';			//Type of minion  -- equivalent to symbol for tiles.
	int movementRange = 0;
	int attackRange = 0;
	int visionRange = 0 ;
	bool isCapturing = false;
	int maxFuel = 0;
	int currentFuel = 0;
	std::string description = "ASDASD";	//For user interface only.
	int team = 0;
	double health = 0;
	Minion* minionBeingTransported = NULL;
	Minion* transporter = NULL;
	std::string Image = { '.','.','.',
						'.','.','.',
						'.','.','.' };
	int veterancy = 0;

	double calculateVetBonus();

	MasterBoard* minionEnvironment = NULL;
	minionStatus status;
	RangeType rangeType;
	specialtyGroupType specialtyGroup;
	domainType domain = land;

};



#endif /* MINION_HPP__ */

