#ifndef COMPIE_H__
#define COMPIE_H__

#include "Minion.hpp"
#include <string>
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"


enum minionTaskingType { attackLocalMinion, captureLocalProperty, moveTowardsEnemy, holdPosition };

class inputLayer;
class Cursor;
class mainMenu;


class compie 
{
public:
	compie(mainMenu* inputMenu);
	int deployMove(MasterBoard* boardToUse);
	int checkAdjacentTilesForEnemies(int currentX, int currentY, int* distanceToTileAdjacentToClosestEnemy,  Cursor* myCursor, MasterBoard* boardToUse);
	int findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY);
	double findEnemiesWithinLocalArea(MasterBoard* boardToUse);
	int checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore);
	int determineMinionTasks(MasterBoard* boardToUse);
	int executeMinionTasks(MasterBoard* boardToUse);
	int takeMyTurn(MasterBoard* boardToUse);
	int determineProduction(MasterBoard* boardToUse);

	
	minionTaskingType minionTasking = holdPosition;
	tile* closestTileAdjacentToEnemy = NULL;
	tile* tileToTarget = NULL;
	mainMenu* menuPointer = NULL;

	inputLayer* InputLayer = NULL;

};

#endif /* COMPIE_H__ */

