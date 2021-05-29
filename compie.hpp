#ifndef COMPIE_H__
#define COMPIE_H__

#include "Minion.hpp"
#include <string>
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"

enum minionTaskingType { attackLocalMinion, captureLocalProperty, moveTowardsEnemy };
class inputLayer;


class compie 
{
public:
	int checkAdjacentTilesForEnemies(int currentX, int currentY, int* distanceToTileAdjacentToClosestEnemy,  Cursor* myCursor, MasterBoard* boardToUse);
	int findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY);
	int findEnemiesWithinLocalArea(MasterBoard* boardToUse);
	int determinePropertyTasks();
	int determineMinionTasks(MasterBoard* boardToUse);
	int executeMinionTasks(MasterBoard* boardToUse);
	int moveMinions(MasterBoard* boardToUse);

	minionTaskingType minionTasking;
	tile* closestTileAdjacentToEnemy = NULL;
	tile* tileToTarget = NULL;

	inputLayer* InputLayer = NULL;

};

#endif /* COMPIE_H__ */

