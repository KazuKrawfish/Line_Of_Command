#include "compie.hpp"
#include <thread>
#include <iostream>


//Below functions are "utilities" that need to find a home.

int buildOptimalPath(MasterBoard* boardToCheck) 
{
	return 0;
}

int computeDistance(int inputX1, int inputX2, int inputY1, int inputY2)
{
	return abs(inputX1 - inputX2) + abs(inputY1 - inputY2);

}
//Above functions are "utilities" that need to find a home.


compie::compie()
{


}

//Search for the closest enemy property to capture.
int compie::deployMove(MasterBoard* boardToUse) 
{	
	int distanceToObjective = 999;
	int distanceToTargetTile = 999;
	tileToTarget = NULL;
	tile* objectiveTile = NULL;

		//Now go through the whole board.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
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

	if (objectiveTile == NULL) 
	{
		return -1;
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
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
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

//Check adjacent tiles for enemy minions. Assumes this tile is with movement range of your minion.
int compie::checkAdjacentTilesForEnemies(int currentX, int currentY, int* distanceToTileAdjacentToClosestEnemy, Cursor* myCursor, MasterBoard* boardToUse)
{
		//If the square to the left has a minion and that minion is enemy.
		//Important to note that the x-1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentX > 1 && boardToUse->Board[currentX - 1][currentY].hasMinionOnTop && boardToUse->Board[currentX - 1][currentY].minionOnTop->team != boardToUse->playerFlag)
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

		//If the square to the right has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentX < boardToUse->BOARD_WIDTH-1 && boardToUse->Board[currentX + 1][currentY].hasMinionOnTop && boardToUse->Board[currentX + 1][currentY].minionOnTop->team != boardToUse->playerFlag)
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

		//If the square below has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentY < boardToUse->BOARD_HEIGHT-1 && boardToUse->Board[currentX][currentY+1].hasMinionOnTop && boardToUse->Board[currentX][currentY+1].minionOnTop->team != boardToUse->playerFlag)
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

		//If the square above has a minion and that minion is enemy.
		//Important to note that the x+1 shenanigens a.k.a. the tile next door, is only used for seeing if there is a minion there.
		if (currentY > 1 && boardToUse->Board[currentX][currentY - 1].hasMinionOnTop && boardToUse->Board[currentX][currentY - 1].minionOnTop->team != boardToUse->playerFlag)
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

		return 1;

}

//New code:
//Needs to have valid minions passed, and a valid relativeSuitability score passed.
int compie::checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore)
{
	double attackerDamageDealt = 0;
	double defenderCounterAttackDamageDealt = 0;

	//relativeSuitabilityScore replaces distanceToTileAdjacentToClosestEnemy

	//Within each of four adjacent tiles:
	//Calculate the damage our minion would do to them, as well as the counterattack damage:
	
	if (currentX > 1 && boardToUse->Board[currentX - 1][currentY].hasMinionOnTop && boardToUse->Board[currentX - 1][currentY].minionOnTop->team != boardToUse->playerFlag)
	{
		int attackerCost = 0;
		int defenderCost = 0;
		
		//Determine minion costs based on types:
		//Have to use different building inputs because consultCostChart assumes you are using a real property involved
		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'A');
		}
		else attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'h');

		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX - 1][currentY].minionOnTop->type, 'A');
		}
		else defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX - 1][currentY].minionOnTop->type, 'h');
		
		bool willAmmoBeUsed = false;
		attackerDamageDealt = boardToUse->calculateDamageDealt(myCursor->selectMinionPointer, boardToUse->Board[currentX - 1][currentY].minionOnTop, willAmmoBeUsed);
		defenderCounterAttackDamageDealt = boardToUse->calculateDamageDealt(boardToUse->Board[currentX - 1][currentY].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed) * (boardToUse->Board[currentX - 1][currentY].minionOnTop->health - attackerDamageDealt) / 100;
		if (defenderCounterAttackDamageDealt < 0)
			defenderCounterAttackDamageDealt = 0;

		//This is the potential "value added" from combat, based on what we might lose vs gain.
		double newRelativeSuitabilityScore = attackerDamageDealt* defenderCost - defenderCounterAttackDamageDealt* attackerCost;
	
		//If it's a better score, switch targets.
		if ( newRelativeSuitabilityScore >  *relativeSuitabilityScore)
		{
			*relativeSuitabilityScore = newRelativeSuitabilityScore;
			closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
			tileToTarget = &(boardToUse->Board[currentX - 1][currentY]);
			//No return, we want to check all tiles
		}
	
	}

	if (currentX < boardToUse->BOARD_WIDTH - 1 && boardToUse->Board[currentX + 1][currentY].hasMinionOnTop && boardToUse->Board[currentX + 1][currentY].minionOnTop->team != boardToUse->playerFlag)
	{
		int attackerCost = 0;
		int defenderCost = 0;

		//Determine minion costs based on types:
		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'A');
		}
		else attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'h');

		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX + 1][currentY].minionOnTop->type, 'A');
		}
		else defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX + 1][currentY].minionOnTop->type, 'h');
		
		bool willAmmoBeUsed = false;
		attackerDamageDealt = boardToUse->calculateDamageDealt(myCursor->selectMinionPointer, boardToUse->Board[currentX + 1][currentY].minionOnTop, willAmmoBeUsed);
		defenderCounterAttackDamageDealt = boardToUse->calculateDamageDealt(boardToUse->Board[currentX + 1][currentY].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed) * (boardToUse->Board[currentX + 1][currentY].minionOnTop->health - attackerDamageDealt) / 100;
		if (defenderCounterAttackDamageDealt < 0)
			defenderCounterAttackDamageDealt = 0;

		//This is the potential "value added" from combat, based on what we might lose vs gain.
		double newRelativeSuitabilityScore = attackerDamageDealt * defenderCost - defenderCounterAttackDamageDealt * attackerCost;

		//If it's a better score, switch targets.
		if (newRelativeSuitabilityScore > * relativeSuitabilityScore)
		{
			*relativeSuitabilityScore = newRelativeSuitabilityScore;
			closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
			tileToTarget = &(boardToUse->Board[currentX + 1][currentY]);
			//No return, we want to check all tiles
		}

	}
	
	if (currentY < boardToUse->BOARD_HEIGHT - 1 && boardToUse->Board[currentX ][currentY + 1].hasMinionOnTop && boardToUse->Board[currentX ][currentY + 1].minionOnTop->team != boardToUse->playerFlag)
	{
		int attackerCost = 0;
		int defenderCost = 0;

		//Determine minion costs based on types:
		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'A');
		}
		else attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'h');

		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX][currentY + 1].minionOnTop->type, 'A');
		}
		else defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX][currentY + 1].minionOnTop->type, 'h');

		bool willAmmoBeUsed = false;
		attackerDamageDealt = boardToUse->calculateDamageDealt(myCursor->selectMinionPointer, boardToUse->Board[currentX ][currentY + 1].minionOnTop, willAmmoBeUsed);
		defenderCounterAttackDamageDealt = boardToUse->calculateDamageDealt(boardToUse->Board[currentX ][currentY + 1].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed) * (boardToUse->Board[currentX][currentY + 1].minionOnTop->health - attackerDamageDealt) / 100;
		if (defenderCounterAttackDamageDealt < 0)
			defenderCounterAttackDamageDealt = 0;

		//This is the potential "value added" from combat, based on what we might lose vs gain.
		double newRelativeSuitabilityScore = attackerDamageDealt * defenderCost - defenderCounterAttackDamageDealt * attackerCost;

		//If it's a better score, switch targets.
		if (newRelativeSuitabilityScore > * relativeSuitabilityScore)
		{
			*relativeSuitabilityScore = newRelativeSuitabilityScore;
			closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
			tileToTarget = &(boardToUse->Board[currentX][currentY + 1]);
			//No return, we want to check all tiles
		}

	}

	if (currentY > 1 && boardToUse->Board[currentX][currentY - 1].hasMinionOnTop && boardToUse->Board[currentX][currentY - 1].minionOnTop->team != boardToUse->playerFlag)
	{
		int attackerCost = 0;
		int defenderCost = 0;

		//Determine minion costs based on types:
		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'A');
		}
		else attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, 'h');

		if (myCursor->selectMinionPointer->type == 'h' || myCursor->selectMinionPointer->type == 'v')
		{
			defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX][currentY - 1].minionOnTop->type, 'A');
		}
		else defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[currentX][currentY - 1].minionOnTop->type, 'h');

		bool willAmmoBeUsed = false;
		attackerDamageDealt = boardToUse->calculateDamageDealt(myCursor->selectMinionPointer, boardToUse->Board[currentX ][currentY - 1].minionOnTop, willAmmoBeUsed);
		defenderCounterAttackDamageDealt = boardToUse->calculateDamageDealt(boardToUse->Board[currentX][currentY - 1].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed) * (boardToUse->Board[currentX][currentY - 1].minionOnTop->health - attackerDamageDealt)/100;
		if (defenderCounterAttackDamageDealt < 0)
			defenderCounterAttackDamageDealt = 0;

		//This is the potential "value added" from combat, based on what we might lose vs gain.
		double newRelativeSuitabilityScore = attackerDamageDealt * defenderCost - defenderCounterAttackDamageDealt * attackerCost;

		//If it's a better score, switch targets.
		if (newRelativeSuitabilityScore > * relativeSuitabilityScore)
		{
			*relativeSuitabilityScore = newRelativeSuitabilityScore;
			closestTileAdjacentToEnemy = &(boardToUse->Board[currentX][currentY]);
			tileToTarget = &(boardToUse->Board[currentX][currentY - 1]);
			//No return, we want to check all tiles
		}

	}
		
	return 0;
}

double compie::findEnemiesWithinLocalArea(MasterBoard* boardToUse)
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
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
	}
	
	double relativeSuitabilityScore = 0;
	//Now go through the search area.
	for (int x = minX ; x < maxX ;  x++)
	{
		for (int y = minY ; y < maxY ; y++) 
		{	
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//Also must be within range
			if ((boardToUse->Board[x][y].hasMinionOnTop == false  || (myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x))&& boardToUse->Board[x][y].withinRange == true)
			{
				//checkAdjacentTilesForEnemies(x, y, &distanceToTileAdjacentToClosestEnemy,  myCursor, boardToUse );
				checkAdjacentTilesForBestValuedEnemy(x, y, myCursor, boardToUse, &relativeSuitabilityScore);
				//Already checked for closeness in the function itself so no need to do anything else
			}
		}
	}

	return relativeSuitabilityScore;
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
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
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

//Determine whether the minion will attack local enemies, capture property, or move strategically.
//This function assumes the minion is already selected.
int compie::determineMinionTasks(MasterBoard* boardToUse) 
{
	closestTileAdjacentToEnemy = NULL;
	tileToTarget = NULL;
	int returnX = 0;
	int returnY = 0;

	if (boardToUse->cursor.selectMinionPointer == NULL) 
	{
		return 1;
	}

	//If already capturing continue that regardless.
	if (boardToUse->cursor.selectMinionPointer->isCapturing == true) 
	{
		tileToTarget = &boardToUse->Board[boardToUse->cursor.selectMinionPointer->locationX][boardToUse->cursor.selectMinionPointer->locationY];
		minionTasking = captureLocalProperty;
		return 1;
	}
	else
	{
		
		//Otherwise see if there are enemies in local area within range and suitable to attack.
		double relativeSuitabilityScore = findEnemiesWithinLocalArea(boardToUse);
		if (relativeSuitabilityScore > 0 && tileToTarget != NULL)
		{
			minionTasking = attackLocalMinion;
			//std::cout << "recommend attacking " << tileToTarget->minionOnTop->description << " at "
			//	<< tileToTarget->minionOnTop->locationX << "," << tileToTarget->minionOnTop->locationY << std::endl;
			return 1;
		}
		else //If infantry, attempt to capture local properties.
			if (boardToUse->cursor.selectMinionPointer->specialtyGroup == infantry)
			{
				int distance = findPropertyWithinLocalArea(boardToUse, &returnX, &returnY);
				if (distance < 999 && tileToTarget != NULL)
				{
					minionTasking = captureLocalProperty;
					//DEBUG
					//std::cout << "recommend capturing " << tileToTarget->description << " at " << returnX << "," << returnY << std::endl;
					return 1;
				}
			}
	}

	int distance = deployMove(boardToUse);
	if (distance < 999) 
	{
		minionTasking = moveTowardsEnemy;
		//DEBUG
		//std::cout << "Recommend move towards enemies " << std::endl;
		return 1;
	}

	//std::cout << "Recommend holding position" << std::endl;
	minionTasking = holdPosition;
	return 0;
}

int compie::executeMinionTasks(MasterBoard* boardToUse) 
{
	if (minionTasking == attackLocalMinion)
	{
		//Move cursor
		if (boardToUse->cursor.relocate(closestTileAdjacentToEnemy->locationX, closestTileAdjacentToEnemy->locationY) == 1 )
		{
		std::cout<< "fail move"<<std::endl;
		}
		

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);

		
		//The move automatically deselects. Thus reselect.
		boardToUse->selectMinion(boardToUse->cursor.getX(), boardToUse->cursor.getY());
		boardToUse->attackMinion(tileToTarget->locationX, tileToTarget->locationY, InputLayer);
		//std::cout << "Execute" << std::endl;
		
		return 0;
	}

	if (minionTasking == captureLocalProperty) 
	{
		//Move cursor
		if (boardToUse->cursor.relocate(tileToTarget->locationX, tileToTarget->locationY) == 1) 
		{
			std::cout << "fail move" << std::endl;
		}

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
		boardToUse->cursor.relocate(tileToTarget->locationX, tileToTarget->locationY);

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);

	}

	if (minionTasking == holdPosition)
	{
		//Do nothing
	}

	boardToUse->deselectMinion();
	return 1;
}

int compie::moveMinions(MasterBoard* boardToUse)
{
	//For now we re-initialize these member variables here:
	minionTasking = holdPosition;
	closestTileAdjacentToEnemy = NULL;
	tileToTarget = NULL;

	//Go through minion Roster.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		
		//Must be a valid minion, and our team's minion. Must have not moved fully yet.
		if (boardToUse->minionRoster[i] != NULL && boardToUse->minionRoster[i]->team == boardToUse->playerFlag && boardToUse->minionRoster[i]->status != hasfired)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			//Move cursor, then select minion, then determine tasks, then execute tasks.
			boardToUse->cursor.XCoord = boardToUse->minionRoster[i]->locationX;
			boardToUse->cursor.YCoord = boardToUse->minionRoster[i]->locationY;
			
			//Reminder that "selectMinion" is required to set the "withinRange" characteristic for tiles.
			//This is used for firing and moving.
			boardToUse->selectMinion(boardToUse->minionRoster[i]->locationX, boardToUse->minionRoster[i]->locationY);
			//std::cout << "For minion " << boardToUse->cursor.selectMinionPointer->description 
			//	<<" at " << boardToUse->cursor.getX() << "," << boardToUse->cursor.getY() << ": "<< std::endl;
			determineMinionTasks(boardToUse);

			executeMinionTasks(boardToUse);
		}
	}

	determineProduction(boardToUse);

	return 1;
}

//Right now this is actually buying units too.
int compie::determineProduction(MasterBoard* boardToUse)
{
	int totalFactoriesLeft = 0;	
	int numberOfTanks = 0;
	int numberOfInfantry = 0;
	int numberOfCavalry = 0;
	int numberOfSpecialists = 0;

	//Go through map and find all of our factories for totalFactories
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			//If the current tile is our factory
			if (boardToUse->Board[x][y].symbol == 'h' && boardToUse->Board[x][y].controller == boardToUse->playerFlag)
			{
				totalFactoriesLeft++;
			}
		}
	}

	//Go through minion Roster. Find various unit totals.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{

		if (boardToUse->minionRoster[i] != NULL && boardToUse->minionRoster[i]->team == boardToUse->playerFlag)
		{
			switch (boardToUse->minionRoster[i]->type)
			{
			case('i'):
			{
				numberOfInfantry++;
				break;
			}
			case('a'):
			case('T'):
			{
				numberOfTanks++;
				break;
			}
			case('c'):
			{
				numberOfCavalry++;

				break;
			}
			case('s'):
			{
				numberOfSpecialists++;
				break;
			}
			}
		}
	}

	//Go through each factory and purchase a unit based on unit totals and available treasury.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			//If the current tile is our factory and there is no one already on top
			if (boardToUse->Board[x][y].symbol == 'h' && boardToUse->Board[x][y].controller == boardToUse->playerFlag && boardToUse->Board[x][y].hasMinionOnTop == false)
			{
				//First determine "surplus" treasury, which accounts for at least purchasing an infantry at every possible factory.
				int availableTreasury = boardToUse->treasury[boardToUse->playerFlag] + 1000 - totalFactoriesLeft * 1000;
				//If we have a proper proportion of tanks, buy cavalry.
				if (int(numberOfTanks / 3) > numberOfCavalry && boardToUse->consultMinionCostChart('c', 'h') <= availableTreasury && boardToUse->treasury[boardToUse->playerFlag] >= boardToUse->consultMinionCostChart('c', 'h'))
				{
					//Must be able to actually afford the unit.				
					boardToUse->attemptPurchaseMinion('c', x, y, boardToUse->playerFlag);
					totalFactoriesLeft--;

				}
				else		//Infantry and specialists have a similar proportion.
					if (int(numberOfInfantry / 3) > numberOfSpecialists && boardToUse->consultMinionCostChart('s', 'h') <= availableTreasury && boardToUse->treasury[boardToUse->playerFlag] >= boardToUse->consultMinionCostChart('s', 'h'))
					{
						//Must be able to actually afford the unit.

						boardToUse->attemptPurchaseMinion('s', x, y, boardToUse->playerFlag);
						totalFactoriesLeft--;
					}
					else
						if (boardToUse->consultMinionCostChart('T', 'h') <= availableTreasury && boardToUse->treasury[boardToUse->playerFlag] >= boardToUse->consultMinionCostChart('T', 'h'))
						{
							//Must be able to actually afford the unit.
							boardToUse->attemptPurchaseMinion('T', x, y, boardToUse->playerFlag);
							totalFactoriesLeft--;
						}
						else if (boardToUse->consultMinionCostChart('a', 'h') <= availableTreasury && boardToUse->treasury[boardToUse->playerFlag] >= boardToUse->consultMinionCostChart('a', 'h'))
						{
							//Must be able to actually afford the unit.
							boardToUse->attemptPurchaseMinion('a', x, y, boardToUse->playerFlag);
							totalFactoriesLeft--;
						}
						else if (boardToUse->consultMinionCostChart('i', 'h') <= availableTreasury && boardToUse->treasury[boardToUse->playerFlag] >= boardToUse->consultMinionCostChart('i', 'h'))
						{

							//Must be able to actually afford the unit.
							boardToUse->attemptPurchaseMinion('i', x, y, boardToUse->playerFlag);
							totalFactoriesLeft--;
						}

			}
		}

	}
	return 0;
}