#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include <string>
#include <iostream>
#include "inputLayer.hpp"
#include "compie.hpp"


//If the two input coordinates are next to each other, return true. Otherwise, return false.
//This can be either horizontal or vertical adjacency.
bool isAdjacent(int inputX1, int inputX2, int inputY1, int inputY2)
{
	if (   (abs(inputX1 - inputX2) <= 1) && (abs(inputY1 - inputY2) == 0)
		|| (abs(inputX1 - inputX2) == 0) && (abs(inputY1 - inputY2) <= 1) )
	{
		return true;
	}
	else 
	{
		return false;
	}
}

//Attacker vs defender matrix. Attacker determines row, while defender determines column.
//In order they are Infantry, Heavy Infantry, Armor, Artillery, Cavalry, and Rocket.
//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, and Minion().
//												i     s     a     r     c     R
const double ATTACK_VALUES_MATRIX[6][6] = {		0.50, 0.50, 0.05, 0.10, 0.15, 0.25,
												0.55, 0.55, 0.50, 0.50, 0.60, 0.70,
												0.65, 0.65,	0.50, 0.60, 0.60, 0.70,
												0.60, 0.60,	0.40, 0.50, 0.55, 0.60,
												0.60, 0.60, 0.10, 0.20, 0.35, 0.45,
												0.80, 0.80,	0.60, 0.65, 0.70, 0.80};

//Assign numeric values for different units to access attack values matrix easier.
//Needs defaults to catch error!!!!
double consultAttackValuesChart(char attackerType, char defenderType)			
{
	int x = -1;
	int y = -1;

	switch (defenderType)
	{
	case('i'):
		x = 0;
		break;
	case('s'):
		x = 1;
	case('a'):
		x = 2;
		break;
	case('r'):
		x = 3;
		break;
	case('c'):
		x = 4;
		break;
	case('R'):
		x = 5;
		break;
	}

	switch (attackerType)
	{
	case('i'):
		y = 0;
		break;
	case('s'):
		y = 1;
	case('a'):
		y = 2;
		break;
	case('r'):
		y = 3;
		break;
	case('c'):
		y = 4;
		break;
	case('R'):
		y = 5;
		break;
	}

	if (x == -1 || y == -1)
	{
		std::cout << "ERROR ERROR ERROR" << std::endl;
		return -1;
	}

	return ATTACK_VALUES_MATRIX[y][x];
}

//Return of -1 indicates the minion requested does not exist.
int MasterBoard::consultMinionCostChart(char minionType)
{
	int price = -1;
	switch (minionType)
	{
	case('i'):
		price = 1000;
		break;
	case('s'):
		price = 3000;
		break;
	case('a'):
		price = 7000;
		break;
	case('r'):
		price = 7000;
		break;
	case('c'):
		price = 4000;
		break;
	case('R'):
		price = 15000;
		break;
	}
	
	return price;
}

//Currently this is not doing what it should be doing- only partial initialization.
MasterBoard::MasterBoard()
{

	cursor.XCoord = 1;
	cursor.YCoord = 1;
	totalNumberOfMinions = 0;

	//Initialize MinionRoster to NULL.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i] = NULL;
	}

	for (int i = 0; i < NUMBEROFPLAYERS+1; i++)
	{
		treasury[i] = 0;
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].locationX = x;
			Board[x][y].locationY = y;
		}
	}
}

int MasterBoard::clearBoard(inputLayer* InputLayer) 
{
	//Need a way to clear board before loading. Using constructor doesn't do the trick.

	cursor.XCoord = 1;
	cursor.YCoord = 1;
	//totalNumberOfMinions = 0; Don't do this because destroyMinion will decrement the totalNumberOfMinions so we don't want to double dip.

	//Initialize MinionRoster to NULL AND kill all minions.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL)
		{
			destroyMinion(minionRoster[i], false, InputLayer);
			minionRoster[i] = NULL;
		}
	}

	//Clear map
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].clearTile();
		}
	}

	//Reset treasury
	for (int i = 0; i < NUMBEROFPLAYERS + 1; i++)
	{
		treasury[i] = 0;
	}

	return 0;
}

//Ensures cursor stays within the window.
int MasterBoard::checkWindow() 
{
	if (cursor.getX() == (windowLocation % BOARD_WIDTH) && cursor.getX() != 0)						//If the cursor moves to the left edge of the window AND it's not at the edge of the board
		windowLocation--;																			//Shift the window left

	if (cursor.getX() == ((windowLocation % BOARD_WIDTH) + WINDOW_WIDTH - 1)   && windowLocation % BOARD_WIDTH + WINDOW_WIDTH != BOARD_WIDTH)			//If the cursor moves to the right edge of the window AND it's not at the edge of the board
		windowLocation++;																																//Shift the window to the right
	
	if (cursor.getY() == (windowLocation / BOARD_WIDTH) && cursor.getY() != 0)						//If the cursor moves to the top edge of the window AND it's not at the edge of the board
		windowLocation -= BOARD_WIDTH;																//Shift the window up.

	if (cursor.getY() == ((windowLocation / BOARD_WIDTH) + WINDOW_HEIGHT- 1) && (windowLocation / BOARD_WIDTH + WINDOW_HEIGHT != BOARD_HEIGHT))				//If the cursor moves to the bottom of the window AND it's not at the bottom of the board
		windowLocation += BOARD_WIDTH;																														//Shift the window down once.

	return 0;
}

int MasterBoard::setRangeField(int inputX, int inputY, int moveLeft) 
{
	//If an enemy minion on this square, return.
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team != playerFlag)
	{
		return 0;
	}

	//We made it, so set to within range.
	Board[inputX][inputY].withinRange = true;
	   
	if (moveLeft <= 0)
	{
		return 0;
	}

	int nextX = inputX;
	int nextY = inputY;

	//Each adjacent square will be called.
	if (inputX != 0)
	{
		nextX = inputX - 1;
		int costToMoveHere = Board[nextX][inputY].consultMovementChart(this->cursor.selectMinionPointer->type, Board[nextX][inputY].symbol);
		if (costToMoveHere <= moveLeft)
		{
			setRangeField(nextX, inputY, moveLeft-costToMoveHere);
		}
	}


	if (inputX < (BOARD_WIDTH - 1))
	{
		nextX = inputX + 1;
		int costToMoveHere = Board[nextX][inputY].consultMovementChart(this->cursor.selectMinionPointer->type, Board[nextX][inputY].symbol);
		if (costToMoveHere <= moveLeft)
		{
			setRangeField(nextX, inputY, moveLeft - costToMoveHere);
		}
	}

	if (inputY < (BOARD_HEIGHT - 1))
	{
		
		nextY = inputY + 1;
		int costToMoveHere = Board[inputX][nextY].consultMovementChart(this->cursor.selectMinionPointer->type, Board[inputX][nextY].symbol);
		if (costToMoveHere <= moveLeft)
		{
			setRangeField(inputX, nextY, moveLeft - costToMoveHere);
		}
		
	}
	if (inputY != 0)
	{

		nextY = inputY - 1;
		int costToMoveHere = Board[inputX][nextY].consultMovementChart(this->cursor.selectMinionPointer->type, Board[inputX][nextY].symbol);
		if (costToMoveHere <= moveLeft)
		{
			setRangeField(inputX, nextY, moveLeft - costToMoveHere);
		}

	}
	return 0;
	
	
}

int MasterBoard::setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY) 
{
	//If this is the minion starting point, we can see.
	if (inputX == minionX && inputY == minionY) 
	{
		Board[inputX][inputY].withinVision = true;
	}

	//As long as this tile is not a non-adjacent forest, we can see.
	if (Board[inputX][inputY].symbol != '+' || isAdjacent(inputX, minionX, inputY, minionY))
	{
		Board[inputX][inputY].withinVision = true;
	}

	if (Board[inputX][inputY].controller == playerFlag) 
	{
		Board[inputX][inputY].withinVision = true;
	}

	//If this was our last tile, we had the chance to "see it", but we won't go further.
	if (visionLeft <= 0)
	{
		return 1;
	}

	int nextX = inputX;
	int nextY = inputY;

	//Each adjacent square will be called if it hasn't already been found withinVision == true. 
	if (inputX != 0 ) 
	{
		nextX = inputX - 1;
		
		if (visionLeft >= 1)
		{
			setIndividualVisionField(nextX, inputY, visionLeft - 1, minionX, minionY);
		}
	}

	if (inputX < (BOARD_WIDTH - 1))
	{
		nextX = inputX + 1;
		if (visionLeft >= 1)
		{
			setIndividualVisionField(nextX, inputY, visionLeft - 1, minionX, minionY);
		}
	}

	if (inputY < (BOARD_HEIGHT - 1))
	{

		nextY = inputY + 1;
		
		if (visionLeft >= 1)
		{
			setIndividualVisionField(inputX, nextY, visionLeft-1, minionX, minionY);
		}

	}

	if (inputY != 0)
	{

		nextY = inputY - 1;
		
		if (visionLeft >= 1)
		{
			setIndividualVisionField(inputX, nextY, visionLeft-1, minionX, minionY);
		}

	}
	return 0;

}

//Determine vision field for all minions for a player.
//This should be called every time the turn changes, and every time  minion moves or attacks. Also any time a minion is created. 
int MasterBoard::setVisionField() 
{
	//First set the board to no vision initially.
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].withinVision = false;
		}
	}

	//Go through the roster of minions and determine if it's player controlled.
	//Then set vision for everything that minion can see.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL)
		{
			if (minionRoster[i]->team == playerFlag)
			{
				Minion* myMinion = minionRoster[i];
				setIndividualVisionField(myMinion->locationX, myMinion->locationY, myMinion->visionRange, myMinion->locationX, myMinion->locationY);
			}
		}
	}

	return 0;
}

//Determine attack range field.
int MasterBoard::setAttackField(int inputX, int inputY, int inputRange)		//Primary difference between move and attack is attack range goes over all units, ignoring them.
{
	int distanceX = 0;
	int distanceY = 0;

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y= 0; y < BOARD_HEIGHT; y++)
		{
			
			distanceX = abs(inputX - x);
			distanceY = abs(inputY - y);

			if ((distanceX + distanceY) <= Board[inputX][inputY].minionOnTop->attackRange)
				Board[x][y].withinRange = true;
			else Board[x][y].withinRange = false;
		}
	}

	//If it is ranged unit, cannot attack next door.
	if (cursor.selectMinionPointer->rangeType == rangedFire)
	{
		Board[inputX - 1][inputY].withinRange = false;
		Board[inputX + 1][inputY].withinRange = false;
		Board[inputX][inputY - 1].withinRange = false;
		Board[inputX][inputY + 1].withinRange = false;
	}

	return 0;

}

int MasterBoard::createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status)
{
	//Loop through and find the next NULL pointer indicating a non-allocated part of the array.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] == NULL)
		{
			//Successful creation of new minion.
			minionRoster[i] = new Minion(i, inputX, inputY, inputType, inputTeam, this, inputHealth);
			if (minionRoster != NULL)
			{
				minionRoster[i]->status = (minionStatus)status;
				Board[inputX][inputY].minionOnTop = minionRoster[i];
				totalNumberOfMinions++;
				setVisionField();
				return 0;
			}
		}
	}
	
	//No room found, print error message and return 0.
	std::cout << "ERROR, no more supply cap for new minion!" << std::endl;
	return 0;

}

//First check ensures if minion has moved and shot, you can't select. Also if it's ranged and moved, you can't.
int MasterBoard::selectMinion(int inputX, int inputY) 
{
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team == playerFlag
		&& Board[inputX][inputY].minionOnTop->status != hasfired && 
		(Board[inputX][inputY].minionOnTop->rangeType != rangedFire || Board[inputX][inputY].minionOnTop->status != hasmovedhasntfired))
	{
		cursor.selectMinionPointer = Board[inputX][inputY].minionOnTop;
		cursor.selectMinionFlag = true;

		//If minion hasn't moved or fired yet, display its movement range.
		if (cursor.selectMinionPointer->status == hasntmovedorfired)		
		{
			setRangeField(inputX, inputY, cursor.selectMinionPointer->movementRange);
			return 0;
		}
		else //If minion has moved but hasn't fired, and is direct combat, display attack range.
			if (cursor.selectMinionPointer->status == hasmovedhasntfired && cursor.selectMinionPointer->rangeType == directFire)
			{
				setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
				return 0;
			}
		
			else //If minion stood in place and hasn't fired, display attack range.
				if (cursor.selectMinionPointer->status == gaveupmovehasntfired)
				{
					setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
					return 0;
				}
	}
	return 1;
}

int MasterBoard::moveMinion(int inputX, int inputY)
{
	Minion* selectedMinion = cursor.selectMinionPointer;

	//First make sure the move is legal. Must be within range, must not have moved.
	if (Board[inputX][inputY].withinRange == false)
	{
		return 1;
	}
	
	//Make sure minion is able to move.
	if (cursor.selectMinionPointer->status == hasfired ||
		cursor.selectMinionPointer->status == hasmovedhasntfired ||
		cursor.selectMinionPointer->status == gaveupmovehasntfired)
	{
		return 1;
	}

	//If there is a minion below cursor AND it's not this exact minion, movement failure
	if (Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true
		&& !(cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		return 1;
	}

	//If there is a minion below cursor AND it IS this exact minion, "fake" move.
	if (Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true
		&& (cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		cursor.selectMinionPointer->status = gaveupmovehasntfired;
		deselectMinion();
		return 0;
	}

	//Otherwise move.

	//Find old address of the minion
	int oldX = selectedMinion->locationX;
	int oldY = selectedMinion->locationY;
	
	//Clear the old tile, set the new tile.
	Board[oldX][oldY].hasMinionOnTop = false;					
	Board[inputX][inputY].hasMinionOnTop = true;

	Board[inputX][inputY].minionOnTop = Board[oldX][oldY].minionOnTop;
	Board[oldX][oldY].minionOnTop = NULL;

	//Reset capture points for tile.
	Board[oldX][oldY].capturePoints = 20;

	//"Move" the minion to the new location.
	selectedMinion->locationX = inputX;
	selectedMinion->locationY = inputY;

	cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
	deselectMinion();

	setVisionField();

	return 0;
}

std::string MasterBoard::captureProperty(tile* inputTile, Minion* inputMinion) 
{
	std::string textToReturn = "";
	//Subtract capturing minion's health
	int pointsToTake = int((inputMinion->health) / 10);
	inputTile->capturePoints -= pointsToTake;
	
	//
	if (inputTile->capturePoints <= 0)
	{
		inputTile->controller = inputMinion->team;
		inputTile->capturePoints = 20;
		
		//Create event text telling player property was captured.
		textToReturn += "PLAYER ";
		textToReturn += char(this->playerFlag);							//MUST FIX IMPLEMENTATION of Char-32 nonsense.
		textToReturn += "'s ";
		textToReturn += inputTile->description;
		textToReturn += " CAPTURED!";
	}

	inputMinion->status = hasfired;
	return textToReturn;
}

int MasterBoard::deselectMinion() 
{
	cursor.selectMinionFlag = false;
	cursor.selectMinionPointer = NULL;
	
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].withinRange = false;
		}
	}
		
	return 0;
}

//Additionally, can only attack once. If artillery, cannot have moved or be adjacent.
int MasterBoard::attackMinion(int inputX, int inputY, inputLayer* InputLayer)
{
	//Simplify by finding shorthand values first.
	Minion* attackingMinion = cursor.selectMinionPointer;
	Minion* defendingMinion = Board[inputX][inputY].minionOnTop;
	
	//Cannot attack twice!
	if (attackingMinion->status == hasfired)			
	{
		return 1;
	}

	//If artillery type, cannot attack adjacent (Minimum range)
	if ((attackingMinion->rangeType == rangedFire)
		&& (isAdjacent(cursor.getX(), attackingMinion->locationX, cursor.getY(), attackingMinion->locationY)))
	{
		return 1;
	}																																																			//Also, if artillery type, cannot attack if it's actually moved that turn.				
	
	//First find attacking fire power. 
	double attackerFirePower = consultAttackValuesChart(attackingMinion->type, defendingMinion->type);		
	
	//Decrease defender's health by attack value.
	double defenderDefenseFactor = Board[inputX][inputY].defenseFactor;
	defendingMinion->health -= attackerFirePower * attackingMinion->health / defenderDefenseFactor;				

	if (defendingMinion->health <= 0)
	{
		//If defender falls below 0, it dies.
		bool printMessage = true;
		destroyMinion(defendingMinion, printMessage, InputLayer);
	}
	else	//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		if (defendingMinion->rangeType == directFire && (isAdjacent(cursor.getX(), attackingMinion->locationX, cursor.getY(), attackingMinion->locationY)))
		{
			//If defender still alive, then perform defensive counterfire.
			double defenderFirePower = consultAttackValuesChart(defendingMinion->type, attackingMinion->type);	
			double attackerDefenseFactor = Board[cursor.selectMinionPointer->locationX][cursor.selectMinionPointer->locationY].defenseFactor;
			attackingMinion->health -= defenderFirePower * defendingMinion->health / attackerDefenseFactor;
		}	

	if (attackingMinion->health <= 0)			//The attacker can be destroyed too!
	{	
		bool printMessage = true;
		destroyMinion(attackingMinion, printMessage, InputLayer);
		setVisionField();
	}
	else 
	{
		//If the attacker survived, deselect.
		attackingMinion->status = hasfired;
		deselectMinion();
	}



	return 0;

}

int MasterBoard::destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer)
{
	//Tell the board it has no minions associated in the location where the Minion was alive.
	//Reset capture points for the property.
	Board[inputMinion->locationX][inputMinion->locationY].hasMinionOnTop = false;
	Board[inputMinion->locationX][inputMinion->locationY].capturePoints = 20;


	if (printMessage == true) 
	{
	//Create event text telling player it was destroyed.
	InputLayer->eventText += "PLAYER ";
	InputLayer->eventText += char(playerFlag - 32);							//MUST FIX IMPLEMENTATION of Char-32 nonsense.
	InputLayer->eventText += "'s ";
	InputLayer->eventText += inputMinion->description;
	InputLayer->eventText += " DESTROYED!";
	}
	
	//Clean up. Currently since we're not cleaning up the minion roster after a death, there is a hole that may prevent saving properly.
	//FIX FIX FIX
	totalNumberOfMinions--;
	minionRoster[inputMinion->seniority] = NULL;
	delete inputMinion;
	
	return 0;													
}

int MasterBoard::endTurn() {
	
	//Either increment playerFlag or set it to zero, thus cycling through the players.
	if (playerFlag < NUMBEROFPLAYERS)					
	{
		playerFlag++;
	}
	else 
		if (playerFlag >= NUMBEROFPLAYERS)
		{
			playerFlag = 1;
		}

	for (int i = 0; minionRoster[i] != NULL; i++)
	{
		minionRoster[i]->status = hasntmovedorfired;
	}
	
	//Provide income for the next player based on properties he controls.
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (this->Board[x][y].controller == playerFlag) 
			{
				this->treasury[playerFlag] += Board[x][y].production;
			}

		}
	}
	
	setVisionField();

	return 0;

}

