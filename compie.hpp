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
	int checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int checkSingleTileForCombatValue(int attackerX, int attackerY, int defenderX, int defenderY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord);
	int determinePotentialMinionTasking(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int executeMinionTasks(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int takeMyTurn(MasterBoard* boardToUse);
	int determineProduction(MasterBoard* boardToUse);
	int analyzeMap(MasterBoard* boardToUse);
	

	//New functions for new compie 2_0
	int buildCompieMinionRoster(MasterBoard* boardToUse);
		

	std::vector <compieMinionRecord> compieMinionRoster;
	std::vector<std::vector<pathSquare>> compiePathMap;
	mainMenu* menuPointer = NULL;
	inputLayer* InputLayer = NULL;

	int compiePlayerFlag = -1;	//Needs to get playerFlag.
								//This would be updated in the load/new scenario functions.
	int headquartersX = 0;
	int headquartersY = 0;
	int totalPropertiesOnMap = 0;
	int totalContinents = 0;

};

#endif /* COMPIE_H__ */

