//Copyright 2022, Supercontinent Software Ltd.
//
//	Minion.hpp
//

#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

//Forward declaration for MasterBoard.
class MasterBoard;

//Status indicators are true also for transports, engineers, etc. hasfired will be true if the transport dropped a guy, for instance.
enum minionStatus { hasntmovedorfired, hasmovedhasntfired, hasfired, gaveupmovehasntfired };
//Hybrid fire rules:
//Can move and shoot like direct fire, but only with secondary weapon.
//Can stand still and fire pri or sec. Primary always has a minimum range of 1, and sec always has max range of 1.
//Thus AA can fire at ground units next to regardless of move status, but can only shoot air units outside 1, if it stood still.
enum RangeType { directFire, rangedFire, hybridRange };
enum specialtyGroupType { normal, infantry, smallTransport, largeTransport, stealth, defense };
//Domain type is used not for terrain but everything else. Defensive bonus from terrain, fuel requirement, refuel, move through enemy units, etc.
enum domainType { land, air, sea, helo };

enum soundEffectType {machineGun, rpg, cannon, antiAircraftCannon, infantryMove, vehicleMove, aircraftMove, buildUnit, capture, resupply, repair , trapped};


class pathSquare
{
public:
	int distanceFromMinion = -1;
	bool wasVisited = false;

};

class Minion
{
public:

	//Constructors
	Minion(int inputSeniority, int inputX, int inputY, std::string inputType, int inputTeam, MasterBoard* Environment,
		int inputHealth, int inputVeterancy, int beingTransported, int inputFuel, int inputAmmo, int inputSecAmmo, sf::Texture* inputTexture);
	Minion();
	~Minion();


	//Member variables. Should be protected with functions dealing with them!

	int seniority = -1;		//Used to indicated where the minion resides within the minion array. Used to construct and destruct the minion.
	int locationX = -1;
	int locationY = -1;
	std::string type = "None";			//Type of minion  -- equivalent to symbol for tiles.
	int movementRange = 0;
	int attackRange = 0;
	int minAttackRange = 0;	//Only matters for ranged attacks (Whether Hybrid or rangedFire)
	int visionRange = 0;
	bool isCapturing = false;
	bool stealthMode = false;

	// 0 is no faction/neutral/base group, 1 is North Redonia, 2 is South Redonia, 3 is Ormosa, 4 is Torran.
	bool factionAllowed[5] = {true, true, true, true, true};

	//True path.
	std::vector<std::vector<pathSquare>> truePathMap;			
	//Does not account for minions that are not visible. Shows how the path appears to the player.
	std::vector<std::vector<pathSquare>> apparentPathMap;
	//Indicates how path is without accounting for minions in the way. Good for pathfinding for compie.
	std::vector<std::vector<pathSquare>> terrainOnlyPathMap;	

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
	Minion* firstMinionBeingTransported = NULL;
	Minion* secondMinionBeingTransported = NULL;
	Minion* transporter = NULL;
	int veterancy = 0;

	//GRAPHICS ///////////////////////////
	sf::Sprite mySprite;
	sf::Texture* myTexture;
	bool invisible = false;
	//GRAPHICS ///////////////////////////
	
	//SOUND EFFECTS //////////////////////
	soundEffectType myAttackSound = machineGun;
	soundEffectType myMoveSound = infantryMove;
	//SOUND EFFECTS //////////////////////

	

	double calculateVetBonus();

	MasterBoard* minionEnvironment = NULL;
	minionStatus status = hasntmovedorfired;
	RangeType rangeType = directFire;
	specialtyGroupType specialtyGroup = normal;
	domainType domain = land;

};


#endif /* MINION_HPP__ */