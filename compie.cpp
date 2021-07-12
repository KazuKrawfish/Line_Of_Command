#include "compie.hpp"
#include <iostream>


//Below functions are "utilities" that need to find a home.

int computeDistance(int inputX1, int inputX2, int inputY1, int inputY2)
{
	return abs(inputX1 - inputX2) + abs(inputY1 - inputY2);

}

//Above functions are "utilities" that need to find a home.

//Search for the closest enemy property to capture.
int compie::deployMove(MasterBoard* boardToUse) 
{	
	int distanceToObjective = 999;
	int distanceToTargetTile = 999;
	tileToTarget = NULL;
	tile* objectiveTile = NULL;

		//Now go through the whole board.
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//If the current tile is an enemy property
			if (boardToUse->Board[x][y].checkForProperty() == true && boardToUse->Board[x][y].controller != boardToUse->playerFlag)
			{
				//If it is closer than what we previously had targeted, set it as the objective.
				int rangeToProp = computeDistance(x, boardToUse->cursor.selectMinionPointer->locationX, y, boardToUse->cursor.selectMinionPointer->locationY);
				if (rangeToProp < distanceToObjective)
				{
					distanceToObjective = rangeToProp;
					objectiveTile = &(boardToUse->Board[x][y]);
				}
			}
		}
	}

	//Now we set a new search box that is much smaller. 
	//This is for actual tiles we can move to, that are closest to the enemy tile we're targeting.
	
	Cursor* myCursor = &(boardToUse->cursor);
	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > BOARD_WIDTH)
	{
		maxX = 0;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > BOARD_HEIGHT)
	{
		maxY = 0;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//Tile must be free of minions and within range
			if (boardToUse->Board[x][y].hasMinionOnTop == false && boardToUse->Board[x][y].withinRange == true)
			{
				//If the ccurrent tile is closest to the objective (NOT THE MINION!).
				int rangeToTile = computeDistance(x, objectiveTile->locationX, y, objectiveTile->locationY);
				if (rangeToTile < distanceToTargetTile)
				{
					distanceToTargetTile = rangeToTile;
					tileToTarget = &(boardToUse->Board[x][y]);
				}
			}
		}	
	}

	//If the minion cannot move for some reason (Too many friendly units, etc) This should return 999
	return distanceToTargetTile;

}

int compie::checkAdjacentTilesForEnemies(int currentX, int currentY, int* distanceToTileAdjacentToClosestEnemy, Cursor* myCursor, MasterBoard* boardToUse)
{
		//If the square to the left has a minion and that minion is enemy.
		//Important to note that the x-1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentX > 1 && boardToUse->Board[currentX - 1][currentY].hasMinionOnTop && boardToUse->Board[currentX - 1][currentY].minionOnTop->team != boardToUse->playerFlag)
		{
			//If the current tile is within actual range of our minion (Not just local search area)
			if (boardToUse->Board[currentX][currentY].withinRange == true)
			{
				//Compute distance to that tile, then compare to current distance, if it is less, swap out.
				int newDistance = computeDistance(currentX, myCursor->selectMinionPointer->locationX, currentY, myCursor->selectMinionPointer->locationY);
				if (newDistance < *distanceToTileAdjacentToClosestEnemy)
				{
					//Now the closest tile adjacent to an enemy is updated, as well as its distance.
					//Then return, avoid doing this for 3 more tiles.
					closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
					*distanceToTileAdjacentToClosestEnemy = newDistance;
					tileToTarget = &(boardToUse->Board[currentX - 1][currentY]);
					return 0;
				}
			}
		}

		//If the square to the right has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentX < BOARD_WIDTH-1 && boardToUse->Board[currentX + 1][currentY].hasMinionOnTop && boardToUse->Board[currentX + 1][currentY].minionOnTop->team != boardToUse->playerFlag)
		{
			//If the current tile is within actual range of our minion (Not just local search area)
			if (boardToUse->Board[currentX][currentY].withinRange == true)
			{
				//Compute distance to that tile, then compare to current distance, if it is less, swap out.
				int newDistance = computeDistance(currentX, myCursor->selectMinionPointer->locationX, currentY, myCursor->selectMinionPointer->locationY);
				if (newDistance < *distanceToTileAdjacentToClosestEnemy)
				{
					//Now the closest tile adjacent to an enemy is updated, as well as its distance.
					//Then return, avoid doing this for 3 more tiles.
					closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
					*distanceToTileAdjacentToClosestEnemy = newDistance;
					tileToTarget = &(boardToUse->Board[currentX + 1][currentY]);
					return 0;
				}
			}
		}

		//If the square below has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentY < BOARD_HEIGHT-1 && boardToUse->Board[currentX][currentY+1].hasMinionOnTop && boardToUse->Board[currentX][currentY+1].minionOnTop->team != boardToUse->playerFlag)
		{
			//If the current tile is within actual range of our minion (Not just local search area)
			if (boardToUse->Board[currentX][currentY].withinRange == true)
			{
				//Compute distance to that tile, then compare to current distance, if it is less, swap out.
				int newDistance = computeDistance(currentX, myCursor->selectMinionPointer->locationX, currentY, myCursor->selectMinionPointer->locationY);
				if (newDistance < *distanceToTileAdjacentToClosestEnemy)
				{
					//Now the closest tile adjacent to an enemy is updated, as well as its distance.
					//Then return, avoid doing this for 3 more tiles.
					closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
					*distanceToTileAdjacentToClosestEnemy = newDistance;
					tileToTarget = &(boardToUse->Board[currentX][currentY+1]);
					return 0;
				}
			}
		}

		//If the square above has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentY > 1 && boardToUse->Board[currentX][currentY - 1].hasMinionOnTop && boardToUse->Board[currentX][currentY - 1].minionOnTop->team != boardToUse->playerFlag)
		{
			//If the current tile is within actual range of our minion (Not just local search area)
			if (boardToUse->Board[currentX][currentY].withinRange == true)
			{
				//Compute distance to that tile, then compare to current distance, if it is less, swap out.
				int newDistance = computeDistance(currentX, myCursor->selectMinionPointer->locationX, currentY, myCursor->selectMinionPointer->locationY);
				if (newDistance < *distanceToTileAdjacentToClosestEnemy)
				{
					//Now the closest tile adjacent to an enemy is updated, as well as its distance.
					//Then return, avoid doing this for 3 more tiles.
					closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
					*distanceToTileAdjacentToClosestEnemy = newDistance;
					tileToTarget = &(boardToUse->Board[currentX][currentY - 1]);
					return 0;
				}
			}
		}

		return 1;

}

int compie::findEnemiesWithinLocalArea(MasterBoard* boardToUse)
{
	Cursor* myCursor = &(boardToUse->cursor);
	//These variables hold the information of the closest enemy, range, and tile next door.
	int distanceToTileAdjacentToClosestEnemy = 999;


	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0) 
	{
		minX = 0;
	}
	if (maxX > BOARD_WIDTH) 
	{
		maxX = 0;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > BOARD_HEIGHT)
	{
		maxY = 0;
	}
	
	//Now go through the search area.
	for (int x = minX ; x < maxX ;  x++)
	{
		for (int y = minY ; y < maxY ; y++) 
		{	
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			if (boardToUse->Board[x][y].hasMinionOnTop == false)
			{
				checkAdjacentTilesForEnemies(x, y, &distanceToTileAdjacentToClosestEnemy,  myCursor, boardToUse );
				//Already checked for closeness in the function itself so no need to do anything else
			}
		}
	}

	return distanceToTileAdjacentToClosestEnemy;
}

int compie::findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY)
{
	Cursor* myCursor = &(boardToUse->cursor);
	//These variables hold the information of the closest enemy property and range.
	//Start with NULL and 999 to indicate no valid.
	int distanceToClosestEnemyProperty = 999;
	tileToTarget = NULL;

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > BOARD_WIDTH)
	{
		maxX = 0;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > BOARD_HEIGHT)
	{
		maxY = 0;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//If the tile is property and is enemy controlled.
			if (boardToUse->Board[x][y].checkForProperty() == true && boardToUse->Board[x][y].controller != boardToUse->playerFlag)
			{
				//If the current tile DOES not have minion on top (IE we will be able to move there)
				if (boardToUse->Board[x][y].hasMinionOnTop == false) 
				{
					//If the current tile is within movement range AND is less distant than current closestEnemyProp.
					//This means the minion has to be "selected", to turn on withinRange flags.
					int rangeToProp = computeDistance(x, myCursor->selectMinionPointer->locationX, y, myCursor->selectMinionPointer->locationY);
					if(boardToUse->Board[x][y].withinRange == true && rangeToProp < distanceToClosestEnemyProperty)
					{
						distanceToClosestEnemyProperty = rangeToProp;
						tileToTarget = &(boardToUse->Board[x][y]);
						*returnY = y;
						*returnX = x;
					}
					
				}
				//Just need to check if property is ours or theirs, and then take it.
			}
		}
	}

	return distanceToClosestEnemyProperty;
}

//This is for construction of new minions.
int compie::determinePropertyTasks() 
{
	return 0;
}

//Determine whether the minion will attack local enemies, capture property, or move strategically.
int compie::determineMinionTasks(MasterBoard* boardToUse) 
{


	closestTileAdjacentToEnemy = NULL;
	tileToTarget = NULL;
	int returnX = 0;
	int returnY = 0;
	
	//AKA if there IS an enemy within local area
	//Should not need NULL check but doing it anyway.
	int distance = findEnemiesWithinLocalArea(boardToUse);
	if (distance  < 999 && tileToTarget != NULL)
	{
		minionTasking = attackLocalMinion;
		std::cout << "recommend attacking " << tileToTarget->minionOnTop->description << " at "
			<< tileToTarget->minionOnTop->locationX << "," << tileToTarget->minionOnTop->locationY << std::endl;
		return 1;
	} 
	else 
	{
		distance = findPropertyWithinLocalArea(boardToUse, &returnX, &returnY);
		if ( distance < 999 && tileToTarget != NULL)
		{
			minionTasking = captureLocalProperty;
			//DEBUG
			std::cout << "recommend capturing " << tileToTarget->description << " at " << returnX << "," << returnY << std::endl;
			return 1;
		}
	}
	
	distance = deployMove(boardToUse);
	if (distance < 999) 
	{
		minionTasking = moveTowardsEnemy;
		//DEBUG
		std::cout << "Recommend move towards enemies " << std::endl;
		return 1;
	}

	minionTasking = holdPosition;
	return 0;
}

int compie::executeMinionTasks(MasterBoard* boardToUse) 
{
	if (minionTasking == attackLocalMinion)
	{
		//Move cursor
		boardToUse->cursor.XCoord = closestTileAdjacentToEnemy->locationX;
		boardToUse->cursor.YCoord = closestTileAdjacentToEnemy->locationY;

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);

		
		//The move automatically deselects. Thus reselect.
		boardToUse->selectMinion(boardToUse->cursor.getX(), boardToUse->cursor.getY());
		boardToUse->attackMinion(tileToTarget->locationX, tileToTarget->locationY, InputLayer);
		std::cout << "Execute" << std::endl;
		
		return 0;
	}

	if (minionTasking == captureLocalProperty) 
	{
		//Move cursor
		boardToUse->cursor.XCoord = tileToTarget->locationX;
		boardToUse->cursor.YCoord = tileToTarget->locationY;

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);

		//Capture enemy property
		//The move automatically deselects. Thus reselect.
		boardToUse->selectMinion(boardToUse->cursor.getX(), boardToUse->cursor.getY());
		boardToUse->captureProperty(tileToTarget, boardToUse->cursor.selectMinionPointer);
	
		boardToUse->deselectMinion();
	}

	if (minionTasking == moveTowardsEnemy) 
	{
		//Move cursor
		boardToUse->cursor.XCoord = tileToTarget->locationX;
		boardToUse->cursor.YCoord = tileToTarget->locationY;

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);
	}

	if (minionTasking == holdPosition)
	{
		boardToUse->deselectMinion();
	}

	return 1;
}

int compie::moveMinions(MasterBoard* boardToUse)
{
	//Go through minion Roster.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//Must be a valid minion, and our team's minion
		if (boardToUse->minionRoster[i] != NULL && boardToUse->minionRoster[i]->team == boardToUse->playerFlag)
		{
			//Move cursor, then select minion, then determine tasks, then execute tasks.
			boardToUse->cursor.XCoord = boardToUse->minionRoster[i]->locationX;
			boardToUse->cursor.YCoord = boardToUse->minionRoster[i]->locationY;
			
			//Reminder that "selectMinion" is required to set the "withinRange" characteristic for tiles.
			//This is used for firing and moving.
			boardToUse->selectMinion(boardToUse->minionRoster[i]->locationX, boardToUse->minionRoster[i]->locationY);
			
			determineMinionTasks(boardToUse);

			executeMinionTasks(boardToUse);
		}
	}

	return 1;
}