//Removed masterboard pointer frome very functionand added to class variable
//Added buildScoreMap() function
//Edited transport checking for minion movement.

//int moveScore needs to be added to minionRosterElement, which indicates highest score available.Or it could just be done via :
//scoreMap[tileToMove.x][tileToMove.y]

//Copyright 2022, Supercontinent Software Ltd.
//
//	compie.hpp
//

#ifndef COMPIE_H__
#define COMPIE_H__

#include "Minion.hpp"
#include <string>
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include "compieMinionRecord.hpp"
#include "compiePropertyRecord.hpp"


class inputLayer;
class Cursor;
class mainMenu;
class compieMinionRecord;
class compiePropertyRecord;

//Used to make up compie's land mass map.

struct landMassSquare
{
public:
	int landMassNumber = 0;
	//  0 - square hasn't been visited yet.
	// -1 - square is impassible.
	//  # - indicates land mass this belongs to.

};

struct landMassInfoEntry
{
	bool initialized = false;
	double totalLandSquareCount = 0;
	double totalArea = 0;

	int leftX = 0;
	int rightX = 0;
	int topY = 0;
	int bottomY = 0;

	double edginessFactor = 1.0;

	//Don't really need other than for debug
	int landEdges = 0;
	int impassEdges = 0;

	double landinessFactor = 1.0;

	//These determine the number and quality of units the compie will build on a certain land mass.
	int maxArmySize = 0;
	double infantryMaxHere = 0;
	double armySizeHere = 0;
	double onlyInfantryArmySizeHere = 0;


};

class landMass
{
public:
	//Used for compie production calculations.
	std::vector<std::vector<landMassSquare>> grid;
	std::vector <landMassInfoEntry> roster;

};


class compie
{
public:

	compie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer);
	compie() { return; };
	int initalizeCompie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer, MasterBoard* boardToUse, int inputRepairThreshold,
		int inputMinionAggressionBonus, int inputInfantryAttackBonus, int inputInfantryBaseline, int inputArmySizeFactor);
	int transportSearchForPickup(compieMinionRecord* selectedMinionRecord);
	int transportSearchForDrop(compieMinionRecord* selectedMinionRecord);
	int transportAttemptDropOff(compieMinionRecord* selectedMinionRecord);
	int strategicAdvance(compieMinionRecord* selectedMinionRecord);	//Does a long-term move for either attack or capture
	int strategicWithdraw(compieMinionRecord* selectedMinionRecord); //Does a long term move for either refuel or repair
	int findPropertyWithinLocalArea(int* returnX, int* returnY, compieMinionRecord* selectedMinionRecord);
	int findClosestTileToObjective(compieMinionRecord* selectedMinionRecord);
	double findBestValuedEnemyWithinLocalArea(compieMinionRecord* selectedMinionRecord);
	int defendHeadquarters(compieMinionRecord* selectedMinionRecord);
	int defendProperty(compieMinionRecord* selectedMinionRecord);
	int checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int checkSingleTileForCombatValue(int attackerX, int attackerY, int defenderX, int defenderY, Cursor* myCursor, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int determinePotentialMinionTasking(compieMinionRecord* selectedMinionRecord);
	int executeMinionTasks(compieMinionRecord* selectedMinionRecord);
	int takeMyTurn();
	int determineProduction();
	tile* findClosestAirbase(Minion* inputMinion);

	//Analysis tools called at game load.
	int analyzeMap();
	int buildLandMassMap(int x, int y, int nextNumber);
	int analyzeSingleLandMass(int landMassNumber);
	int statusUpdate();

	//New functions for new compie 2_0
	int buildCompieMinionRoster();

	//New functions for new compie 3_0
	int newDetermineProduction();
	int buildMinionScoreMap(compieMinionRecord* selectedMinionRecord);	//Builds score map for given minion.
	int buildCompieProductionPropertyRoster();

	//Used for compie minion calculations.
	std::vector <compieMinionRecord*>  compieMinionRoster;
	std::vector <compiePropertyRecord> compiePropertyRoster;

	//LandMass contains a grid of squares
	landMass compieLandMassMap;

	mainMenu* menuPointer = NULL;
	inputLayer* InputLayer = NULL;
	MasterBoard* masterBoardPointer = NULL;



	int compiePlayerFlag = -1;	//Needs to get playerFlag.
								//This would be updated in the load/new scenario functions.
	int headquartersX = 0;
	int headquartersY = 0;
	int totalPropertiesOnMap = 0;


	//BattleLab machine learning numbers
	int repairThreshold = 40;					//10-90 represents when a minion will attempt to repair, on a 100 hp scale. 40 gives best behavior so far.
	int compieMinionAggressionBonus = 0;		//Intended range is -20 - 20 for reasonable behavior. Allows compie minions to attack even when the odds are against them.
	int compieInfantryAttackBonus = 2;			//Intended range is 1-3 for reasonable behavior. Makes attacking infantry more attractive - would be difficult to attack them with expensive minions otherwise
	int compieInfantryBaseline = 5;				//No idea what a reasonable range is, probably between 0 - 10. Describes min. number of infantry required in army.
	int compieArmySizeFactor = 4;				//Reasonable range is 1 - 8. 1 makes larger armies, 8 smaller.

};

#endif /* COMPIE_H__ */

