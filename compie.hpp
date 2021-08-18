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


class compie 
{
public:
	compie();
	int deployMove(MasterBoard* boardToUse);
	int checkAdjacentTilesForEnemies(int currentX, int currentY, int* distanceToTileAdjacentToClosestEnemy,  Cursor* myCursor, MasterBoard* boardToUse);
	int findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY);
	int findEnemiesWithinLocalArea(MasterBoard* boardToUse);
	
	int determineMinionTasks(MasterBoard* boardToUse);
	int executeMinionTasks(MasterBoard* boardToUse);
	int moveMinions(MasterBoard* boardToUse);
	int determineProduction(MasterBoard* boardToUse);

	minionTaskingType minionTasking = holdPosition;
	tile* closestTileAdjacentToEnemy = NULL;
	tile* tileToTarget = NULL;

	inputLayer* InputLayer = NULL;

};

#endif /* COMPIE_H__ */

