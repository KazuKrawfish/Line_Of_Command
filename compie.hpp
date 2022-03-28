#ifndef COMPIE_H__
#define COMPIE_H__

#include "Minion.hpp"
#include <string>
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include "compieMinionRecord.hpp"




class inputLayer;
class Cursor;
class mainMenu;
class compieMinionRecord;

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
	int initalizeCompie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer, MasterBoard* boardToUse);
	int strategicAdvance(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);	//Does a long-term move for either attack or capture
	int strategicWithdraw(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord); //Does a long term move for either refuel or repair
	int findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY, compieMinionRecord* selectedMinionRecord);
	double findBestValuedEnemyWithinLocalArea(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int defendHeadquarters(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int defendProperty(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int checkSingleTileForCombatValue(int attackerX, int attackerY, int defenderX, int defenderY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int determinePotentialMinionTasking(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int executeMinionTasks(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int takeMyTurn(MasterBoard* boardToUse);
	int determineProduction(MasterBoard* boardToUse);

	//Analysis tools called at game load.
	int analyzeMap(MasterBoard* boardToUse);
	int buildLandMassMap(MasterBoard* boardToUse, int x, int y, int nextNumber);
	int analyzeSingleLandMass(MasterBoard* boardToUse, int landMassNumber);
	int statusUpdate(MasterBoard* boardToUse);

	//New functions for new compie 2_0
	int buildCompieMinionRoster(MasterBoard* boardToUse);
	
	//Used for compie minion calculations.
	std::vector <compieMinionRecord*>  compieMinionRoster;

	//LandMass contains a grid of squares
	landMass compieLandMassMap; 

	mainMenu* menuPointer = NULL;
	inputLayer* InputLayer = NULL;



	int compiePlayerFlag = -1;	//Needs to get playerFlag.
								//This would be updated in the load/new scenario functions.
	int headquartersX = 0;
	int headquartersY = 0;
	int totalPropertiesOnMap = 0;
	double armamentFactor = 1;



};

#endif /* COMPIE_H__ */

