//Copyright 2022, Supercontinent Software Ltd.
//
//	Minion.cpp
//

#include "Minion.hpp"
#include "MasterBoard.hpp"
#include <string>

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
	stealthMode = false;

	// 0 is no faction/neutral/base group, 1 is North Redonia, 2 is South Redonia, 3 is Ormosa, 4 is Torran.
	

	//Basic image init below
	mySprite.setTextureRect(rectArray[0][4]);
	mySecondSprite.setTextureRect(rectArray[0][4]);


}

//
//Always use this constructor, it gives the new Minion all required properties.
//When updating Minion(), also update ATTACK_VALUES_MATRIX, consultAttackValuesChart, movement costs and consultMinionCostChart.
//This assumes that the transport already exists if the minion is being carried. Can cause NULL dereference!
Minion::Minion(int inputSeniority, int inputX, int inputY, std::string inputType, int inputTeam,
	MasterBoard* Environment, int inputHealth, int inputVeterancy, int beingTransported, int inputFuel,
	int inputPriAmmo, int inputSecAmmo, sf::Texture* inputTexture , sf::Texture* secondInputTexture)
{
	veterancy = inputVeterancy;
	seniority = inputSeniority;
	minionEnvironment = Environment;
	type = inputType;
	health = inputHealth;
    stealthMode = false;
	
    myTexture = inputTexture;
	if (myTexture != NULL)
	{
		mySprite.setTexture(*myTexture);
		mySprite.setTextureRect(rectArray[0][4]);
	}

	mySecondTexture = secondInputTexture;
	if (secondInputTexture != NULL)
	{
		mySecondSprite.setTexture(*secondInputTexture);
		mySecondSprite.setTextureRect(rectArray[0][4]);
	}
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
			if (Environment->Board[inputX][inputY].minionOnTop->firstMinionBeingTransported == NULL)
				Environment->Board[inputX][inputY].minionOnTop->firstMinionBeingTransported = this;
			else Environment->Board[inputX][inputY].minionOnTop->secondMinionBeingTransported = this;
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

	
	if(inputType == "Infantry")
	{
		description = "Infantry";
		movementRange = 3;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
        captureCapable = true;
		mySprite.setTextureRect(rectArray[0][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[0][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
	}
    else
	if (inputType == "Engineer")
	{
		description = "Engineer";
		movementRange = 3;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		captureCapable = true;
		mySprite.setTextureRect(rectArray[36][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[36][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
	}
	else
	if(inputType == "Insurgent")
	{
		description = "Insurgent";
		movementRange = 3;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		captureCapable = true;
		mySprite.setTextureRect(rectArray[30][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[30][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 1;
		maxSecAmmo = 0;
	}
	else
	if(inputType == "Operative")
	{
		description = "Operative";
		movementRange = 4;
		attackRange = 1;
		visionRange = 4;
		rangeType = directFire;
		specialtyGroup = infantry;
		captureCapable = true;
        stealthMode = true;
		
		mySprite.setTextureRect(rectArray[29][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[29][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
	}
	else
	if (inputType == "Specialist")
	{
		description = "Specialist";
		movementRange = 2;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
        captureCapable = true;
		mySprite.setTextureRect(rectArray[1][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[1][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 3;
		maxSecAmmo = 0;
		myAttackSound = rpg;
		myMoveSound = infantryMove;
		
	}
	else
    	if (inputType == "Cavalry")
	{
		description = "Cavalry";
		movementRange = 5;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		captureCapable = true;
		mySprite.setTextureRect(rectArray[35][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[35][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;
		
	}
	else
	if(inputType == "Armor")
	{
		description = "Armor";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[3][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[3][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
    if(inputType == "Upgunned_Armor")
	{
		description = "Upgunned Armor";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		
		mySprite.setTextureRect(rectArray[27][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[27][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
	}
	else
    if(inputType == "Assault_Gun")
	{
		description = "Assault Gun";
		movementRange = 6;
		attackRange = 2;
        minAttackRange = 1;
		visionRange = 3;
		rangeType = hybridRange;
		specialtyGroup = normal;
		
		mySprite.setTextureRect(rectArray[28][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[28][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 6;
		maxSecAmmo = 6;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
	}
	else
	if(inputType == "Anti-Aircraft")
	{
		description = "Anti-Aircraft";
		movementRange = 6;
		attackRange = 3;
		visionRange = 3;
		rangeType = hybridRange;
		specialtyGroup = normal;
		minAttackRange = 1;
		mySprite.setTextureRect(rectArray[7][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[7][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 6;
		maxSecAmmo = 6;
		myAttackSound = antiAircraftCannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Heavy_Armor")
	{
		description = "Heavy Armor";
		movementRange = 5;
		attackRange = 1;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = normal;
		
		mySprite.setTextureRect(rectArray[6][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[6][inputTeam + 4]);
		maxFuel = 60;
		maxPriAmmo = 6;
		maxSecAmmo = 0;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Modern_Armor")
	{
		description = "Modern Armor";
		movementRange = 6;
		attackRange = 1;
		visionRange = 4;
		rangeType = directFire;
		specialtyGroup = normal;
		
		mySprite.setTextureRect(rectArray[26][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[26][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
    if(inputType == "Super_Heavy_Armor")
	{
		description = "Super Heavy Armor";
		movementRange = 5;
		attackRange = 1;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[34][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[34][inputTeam + 4]);
		maxFuel = 60;
		maxPriAmmo = 9;
		maxSecAmmo = 0;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Artillery")
	{
		description = "Artillery";
		movementRange = 5;
		attackRange = 3;
		visionRange = 2;
		rangeType = rangedFire;
		specialtyGroup = normal;
		minAttackRange = 1;
		
		mySprite.setTextureRect(rectArray[5][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[5][inputTeam + 4]);
		maxFuel = 50;
		maxPriAmmo = 6;
		maxSecAmmo = -1;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Recon")
	{
		description = "Recon";
		movementRange = 8;
		attackRange = 1;
		visionRange = 5;
		rangeType = directFire;
		specialtyGroup = normal;
		
		mySprite.setTextureRect(rectArray[2][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[2][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;

		myMoveSound = vehicleMove;
		
	}
	else
    if(inputType == "Technical")
	{
		description = "Technical";
		movementRange = 10;
		attackRange = 1;
		visionRange = 4;
		rangeType = directFire;
		specialtyGroup = normal;
		captureCapable = true;
		mySprite.setTextureRect(rectArray[31][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[31][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = -1;
		maxSecAmmo = 0;

		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Rocket_Artillery")
	{
		description = "Rocket Artillery";
		movementRange = 5;
		attackRange = 5;
		visionRange = 2;
		rangeType = rangedFire;
		minAttackRange = 2;
		specialtyGroup = normal;
		mySprite.setTextureRect(rectArray[8][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[8][inputTeam + 4]);
		maxFuel = 50;
		maxPriAmmo = 6;
		maxSecAmmo = -1;

		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
    if(inputType == "Victory_Launcher")
	{
		description = "Victory Launcher";
		movementRange = 4;
		attackRange = 5;
		visionRange = 2;
		rangeType = rangedFire;
		minAttackRange = 2;
		specialtyGroup = splashAttack;
		mySprite.setTextureRect(rectArray[33][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[33][inputTeam + 4]);
		maxFuel = 50;
		maxPriAmmo = 6;
		maxSecAmmo = -1;

		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Artillery_Emplacement")
	{
		description = "Artillery Emplacement";
		movementRange = 0;
		attackRange = 4;
		visionRange = 3;
		rangeType = rangedFire;
		minAttackRange = 1;
		specialtyGroup = defense;
		mySprite.setTextureRect(rectArray[22][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[22][inputTeam + 4]);
		maxFuel = 10;
		maxPriAmmo = 0;
		maxSecAmmo = -1;
		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "SAM_Site")
	{
		description = "SAM Site";
		movementRange = 0;
		attackRange = 5;
		visionRange = 4;
		rangeType = rangedFire;
		minAttackRange = 2;
		specialtyGroup = defense;
		mySprite.setTextureRect(rectArray[23][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[23][inputTeam + 4]);
		maxFuel = 10;
		maxPriAmmo = 0;
		maxSecAmmo = -1;

		myAttackSound = cannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Attack_Copter")
	{
		description = "Attack Copter";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;

		mySprite.setTextureRect(rectArray[9][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[9][inputTeam + 4]);
		maxFuel = 80;
		domain = air;
		maxPriAmmo = 9;
		maxSecAmmo = 0;

		myAttackSound = cannon;
		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "Transport_Copter")
	{
		description = "Transport Copter";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = smallTransport;
		mySprite.setTextureRect(rectArray[10][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[10][inputTeam + 4]);
		maxFuel = 80;
		domain = air;


		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "APC")
	{
		description = "APC";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = smallTransport;
		
		mySprite.setTextureRect(rectArray[4][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[4][inputTeam + 4]);
		maxFuel = 70;


		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "IFV")
	{
		description = "IFV";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = smallTransport;
		
		mySprite.setTextureRect(rectArray[25][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[25][inputTeam + 4]);
		maxFuel = 70;

		maxPriAmmo = 9;
		maxSecAmmo = -1;

		myAttackSound = antiAircraftCannon;
		myMoveSound = vehicleMove;
		
	}
	else
	if(inputType == "Interceptor")
	{
		description = "Interceptor";
		movementRange = 10;
		attackRange = 1;
		visionRange = 4;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;
		mySprite.setTextureRect(rectArray[11][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[11][inputTeam + 4]);
		maxFuel = 60;
		maxPriAmmo = 3;
		maxSecAmmo = 0;

		myAttackSound = rpg;
		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "Advanced_Fighter")
	{
		description = "Advanced Fighter";
		movementRange = 12;
		attackRange = 1;
		visionRange = 5;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;
		
		mySprite.setTextureRect(rectArray[24][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[24][inputTeam + 4]);
		maxFuel = 75;
		maxPriAmmo = 6;
		maxSecAmmo = -1;

		myAttackSound = rpg;
		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "Multirole")
	{
		description = "Multirole";
		movementRange = 8;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;
		
		mySprite.setTextureRect(rectArray[20][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[20][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 6;
		maxSecAmmo = 0;

		myAttackSound = antiAircraftCannon;
		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "Bomber")
	{
		description = "Bomber";
		movementRange = 8;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		domain = air;

		mySprite.setTextureRect(rectArray[12][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[12][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 3;
        maxSecAmmo = -1;

		myAttackSound = cannon;
		myMoveSound = aircraftMove;
		
	}
	else
	if(inputType == "Battleship")
	{
		description = "Battleship";
		movementRange = 5;
		attackRange = 6;
		visionRange = 2;
		rangeType = rangedFire;
		domain = sea;
		minAttackRange = 1;
		specialtyGroup = normal;

		mySprite.setTextureRect(rectArray[13][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[13][inputTeam + 4]);
		maxFuel = 80;
		maxPriAmmo = 6;
        maxSecAmmo = -1;

		myAttackSound = cannon;

		
	}
	else
	if(inputType == "Cruiser")
	{
		description = "Cruiser";
		movementRange = 7;
		attackRange = 3;
		visionRange = 4;
		rangeType = hybridRange;
		minAttackRange = 1;
		specialtyGroup = normal;
		domain = sea;

		mySprite.setTextureRect(rectArray[14][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[14][inputTeam + 4]);
		maxFuel = 80;
		maxPriAmmo = 6;
		maxSecAmmo = 6;
		
	}
	else
		if (inputType == "Gunboat")
		{
		description = "Gunboat";
		movementRange = 7;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = smallTransport;
		domain = sea;
		mySprite.setTextureRect(rectArray[16][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[16][inputTeam + 4]);
		maxFuel = 70;
		maxPriAmmo = 1;
		maxSecAmmo = -1;

		}
	else
	if(inputType == "Lander")
	{
		description = "Lander";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = largeTransport;
		domain = sea;
		mySprite.setTextureRect(rectArray[15][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[15][inputTeam + 4]);
		maxFuel = 80;
		
	}
	else
	if(inputType == "Submarine")
	{
		description = "Submarine";
		movementRange = 6;
		attackRange = 1;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = normal;
        stealthMode = false;
		domain = sea;
		mySprite.setTextureRect(rectArray[18][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[18][inputTeam + 4]);
		maxFuel = 80;
		maxPriAmmo = 6;
        maxSecAmmo = -1;
		
	}
	else
		if (inputType == "Aircraft_Carrier")
		{
		description = "Aircraft Carrier";
		movementRange = 7;
		attackRange = 0;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = aircraftCarrier;
		domain = sea;
		mySprite.setTextureRect(rectArray[17][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[17][inputTeam + 4]);
		maxFuel = 80;
		maxPriAmmo = 6;
		maxSecAmmo = -1;

		}
    else
	if(inputType == "Landmine")
	{
		description = "Landmine";
		movementRange = 0;
		attackRange = 1;
		visionRange = 1;
		rangeType = directFire;
        stealthMode = true;
		domain = land;
		mySprite.setTextureRect(rectArray[32][inputTeam + 4]);
		mySecondSprite.setTextureRect(rectArray[32][inputTeam + 4]);
		maxFuel = 0;
		maxPriAmmo = 1;
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