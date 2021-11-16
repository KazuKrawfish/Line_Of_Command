#include "Minion.hpp"
#include "MasterBoard.hpp"

//
//Default Constructor. Should not be used.
//
Minion::Minion()
{
	locationX = -1;
	locationY = -1;
	type = ' ';
	team = 0;						//Team 0 is the neutral team.
	status = hasntmovedorfired;
	health = 100;

	//Basic image init below
	mySprite.setTextureRect(rectArray[0][4]);


}

//
//Always use this constructor, it gives the new Minion all required properties.
//When updating Minion(), also update ATTACK_VALUES_MATRIX, consultAttackValuesChart, movement costs and consultMinionCostChart.
//This assumes that the transport already exists if the minion is being carried. Can cause NULL dereference!
Minion::Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam,
	MasterBoard* Environment, int inputHealth, int inputVeterancy, int beingTransported, int inputFuel, 
	int inputPriAmmo, int inputSecAmmo, sf::Texture* inputTexture)
{
	veterancy = inputVeterancy;
	seniority = inputSeniority;
	minionEnvironment = Environment;
	type = inputType;
	health = inputHealth;

	myTexture = inputTexture;
	mySprite.setTexture(*myTexture);
	mySprite.setTextureRect(rectArray[0][4]);
	//Basic image init end

	truePathMap.resize(Environment->BOARD_WIDTH + 1);
	apparentPathMap.resize(Environment->BOARD_WIDTH + 1);
	terrainOnlyPathMap.resize(Environment->BOARD_WIDTH + 1);
	for (int i = 0; i < Environment->BOARD_WIDTH; i++)
	{
		truePathMap[i].resize(Environment->BOARD_HEIGHT);
		apparentPathMap[i].resize(Environment->BOARD_HEIGHT);
		terrainOnlyPathMap[i].resize(Environment->BOARD_HEIGHT);
	}

	//If being transported, this minion doesn't take up any space.
	//Also, it will be transported by whomever we indicate with InputX and Y
	if (beingTransported == 1)
	{
		if (Environment->Board[inputX][inputY].hasMinionOnTop != NULL)
		{
			transporter = Environment->Board[inputX][inputY].minionOnTop;
			Environment->Board[inputX][inputY].minionOnTop->minionBeingTransported = this;
		}
		else std::cout << "Bad transport, minion doesn't exist!!" << std::endl;

		locationX = -1;
		locationY = -1;
	}
	else	//If not transported, proceed as normal.
	{
		Environment->Board[inputX][inputY].hasMinionOnTop = true;
		locationX = inputX;
		locationY = inputY;
	}

	switch (inputType)
	{
	case('i'):
	{
		description = "Infantry";
		movementRange = 3;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		mySprite.setTextureRect(rectArray[0][inputTeam+4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
		break;
	}
	case('s'):
	{
		description = "Specialist";
		movementRange = 2;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		mySprite.setTextureRect(rectArray[1][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 3;
		maxSecAmmo = 0;
		break;
	}
	case('a'):
	{
		description = "Armor";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[3][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		break;
	}
	case('A'):
	{
		description = "Anti-Air";
		movementRange = 6;
		attackRange = 3;
		visionRange = 3;
		rangeType = hybridRange;
		specialtyGroup = normal;
		minAttackRange = 1;
		mySprite.setTextureRect(rectArray[7][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 6;
		maxSecAmmo = 6;
		break;
	}
	case('T'):
	{
		description = "Heavy Armor";
		movementRange = 5;
		attackRange = 1;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[6][inputTeam + 4]);	
		maxFuel = 60;
		maxPriAmmo = 6;
		maxSecAmmo = 0;
		break;
	}
	case('r'):
	{
		description = "Artillery";
		movementRange = 5;
		attackRange = 3;
		visionRange = 2;
		rangeType = rangedFire;
		specialtyGroup = normal;
		minAttackRange = 1;
		mySprite.setTextureRect(rectArray[5][inputTeam + 4]);
		maxFuel = 50;
		maxPriAmmo = 6;
		maxSecAmmo = -1;
		break;
	}
	case('c'):
	{
		description = "Cavalry";
		movementRange = 8;
		attackRange = 1;
		visionRange = 5;
		rangeType = directFire;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[2][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
		break;
	}
	case('R'):
	{
		description = "Rocket";
		movementRange = 5;
		attackRange = 4;
		visionRange = 2;
		rangeType = rangedFire;
		minAttackRange = 2;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[8][inputTeam + 4]);
		maxFuel = 50;
		maxPriAmmo = 6;
		maxSecAmmo = -1;
		break;
	}
	case('v'):
	{
		description = "Viper";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		
		maxFuel = 80;
		domain = helo;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		break;
	}
	case('h'):
	{
		description = "Huey";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = transport;
		
		maxFuel = 80;
		domain = helo;
		break;
	}
	case('P'):
	{
		description = "APC";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = transport;
		mySprite.setTextureRect(rectArray[4][inputTeam + 4]);
		maxFuel = 70;
		break;
	}
	case('f'):
	{
		description = "Interceptor";
		movementRange = 10;
		attackRange = 1;
		visionRange = 4;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;
		
		maxFuel = 60;
		maxPriAmmo = 3;
		maxSecAmmo = -1;
		break;
	}
	case('b'):
	{
		description = "Bomber";
		movementRange = 8;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;
		
		maxFuel = 70;
		maxPriAmmo = 3;
		break;
	}
	case('B'):
	{
		description = "Battleship";
		movementRange = 5;
		attackRange = 6;
		visionRange = 2;
		rangeType = rangedFire;
		domain = sea;
		minAttackRange = 1;
		specialtyGroup = normal;
		
		maxFuel = 80;
		maxPriAmmo = 6;
		break;
	}
	case('C'):
	{
		description = "Cruiser";
		movementRange = 7;
		attackRange = 3;
		visionRange = 4;
		rangeType = hybridRange;
		minAttackRange = 1;
		specialtyGroup = normal;
		domain = sea;
		
		maxFuel = 80;
		maxPriAmmo = 6;
		maxSecAmmo = 6;
		break;
	}
	case('G'):
	{
		description = "Gunboat";
		movementRange = 7;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = sea;
		
		maxFuel = 70;
		maxPriAmmo = 1;
		break;
	}
	case('L'):
	{
		description = "Lander";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = transport;
		domain = sea;
		
		maxFuel = 80;
		break;
	}
	case('U'):
	{
		description = "Submarine";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = stealth;
		domain = sea;
		
		maxFuel = 80;
		maxPriAmmo = 6;
		break;
	}
	case('V'):
	{
		description = "Aircraft Carrier";
		movementRange = 7;
		attackRange = 0;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = transport;
		domain = sea;
	
		maxFuel = 80;
		maxPriAmmo = 6;
		break;
	}
	}

	team = inputTeam;

	//Either minion is new (-1) or provide the input fuel from the save or wherever.
	if (inputFuel == -1)
	{
		currentFuel = maxFuel;
	}
	else currentFuel = inputFuel;

	if (inputPriAmmo == -1)
	{
		currentPriAmmo = maxPriAmmo;
	}
	else currentPriAmmo = inputPriAmmo;

	if (inputSecAmmo == -1)
	{
		currentSecAmmo = maxSecAmmo;
	}
	else currentSecAmmo = inputSecAmmo;



}

double Minion::calculateVetBonus()
{
	double combatBonus = 1.0;

	if (veterancy == 1)
	{
		combatBonus = 1.05;
	}
	else if (veterancy == 2)
	{
		combatBonus = 1.1;
	}
	else if (veterancy == 3)
	{
		combatBonus = 1.2;
	}

	return combatBonus;

}

//
//Destructor. Minion has no dynamic variables to clean up so it is empty.
//
Minion::~Minion() {}