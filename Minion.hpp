#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>


//Forward declaration for MasterBoard.
class MasterBoard;

//Status indicators are true also for transports, engineers, etc. hasfired will be true if the transport dropped a guy, for instance.
enum minionStatus { hasntmovedorfired, hasmovedhasntfired, hasfired, gaveupmovehasntfired };
enum RangeType { directFire, rangedFire, hybridRange };
enum specialtyGroupType { normal, infantry, transport, stealth };
//Domain type is used not for terrain but everything else. Defensive bonus from terrain, fuel requirement, refuel, move through enemy units, etc.
enum domainType { land, air, sea, helo };


class Minion
{
public:

	//Constructors
	Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam, MasterBoard* Environment,
		int inputHealth, int inputVeterancy, int beingTransported, int inputFuel, int inputAmmo, int inputSecAmmo);
	Minion();
	~Minion();


	//Member variables. Should be protected with functions dealing with them!

	int seniority = -1;		//Used to indicated where the minion resides within the minion array. Used to construct and destruct the minion.
	int locationX = -1;
	int locationY = -1;
	char type = 'x';			//Type of minion  -- equivalent to symbol for tiles.
	int movementRange = 0;
	int attackRange = 0;
	int minAttackRange = 0;	//Only matters for ranged attacks (Whether Hybrid or rangedFire)
	int visionRange = 0;
	bool isCapturing = false;

	int maxFuel = 0;
	int currentFuel = 0;

	//New ammo rule: both secondary and primary use separate ammo, and maxAmmo == 0 indicates infinte ammo for that weapon.
	//Also, maxAmmo == -1 indicates no weapon in that slot.
	int currentPriAmmo = 0;
	int currentSecAmmo = 0;
	int maxPriAmmo = -1;
	int maxSecAmmo = -1;
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
	minionStatus status = hasntmovedorfired;
	RangeType rangeType = directFire;
	specialtyGroupType specialtyGroup = normal;
	domainType domain = land;

};


#endif /* MINION_HPP__ */