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


class compie 
{
public:
	compie(mainMenu* inputMenu);
	int strategicAdvance(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);	//Does a long-term move for either attack or capture
	int strategicWithdraw(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord); //Does a long term move for either refuel or repair
	int findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY);
	double findBestValuedEnemyWithinLocalArea(MasterBoard* boardToUse);
	int checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore);
	int determinePotentialMinionTasking(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int executeMinionTasks(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord);
	int takeMyTurn(MasterBoard* boardToUse);
	int determineProduction(MasterBoard* boardToUse);


	//New functions for new compie 2_0
	int buildCompieMinionRoster(MasterBoard* boardToUse);
		

	std::vector< compieMinionRecord*> compieMinionRoster;
	mainMenu* menuPointer = NULL;

	inputLayer* InputLayer = NULL;

};

#endif /* COMPIE_H__ */

