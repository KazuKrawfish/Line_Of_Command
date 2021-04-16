#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include <string>
#include <iostream>

extern Minion* minionRoster[GLOBALSUPPLYCAP];
extern std::string eventText;

//If the two input coordinates are next to each other, return true. Otherwise, return false.
//This can be either horizontal or vertical adjacency.
bool isAdjacent(int inputX1, int inputX2, int inputY1, int inputY2)
{
	if ((abs(inputX1 - inputX2) == 1) || (abs(inputY1 - inputY2) == 1))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

//Attacker vs defender matrix. Attacker determines row number, while defender determines column number.
//In order they are Infantry, Armor, Artillery, Cavalry, and Rocket.	
//												  I    T    A     C    R
const double ATTACK_VALUES_MATRIX[5][5] = {		0.50, 0.05,0.10,0.10,0.25,
												0.65,0.50,0.60,0.60,0.70,
												0.60,0.40,0.50,0.55,0.60,
												0.60,0.10,0.20,0.35,0.45,
												0.80,0.60,0.65,0.70,0.80};

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
	case('T'):
		x = 1;
		break;
	case('A'):
		x = 2;
		break;
	case('C'):
		x = 3;
		break;
	case('R'):
		x = 4;
		break;
	}

	switch (attackerType)
	{
	case('i'):
		y = 0;
		break;
	case('T'):
		y = 1;
		break;
	case('A'):
		y = 2;
		break;
	case('C'):
		y = 3;
		break;
	case('R'):
		y = 4;
		break;
	}

	if (x == -1 || y == -1)
	{
		std::cout << "ERROR ERROR ERROR" << std::endl;
		return -1;
	}

	return ATTACK_VALUES_MATRIX[y][x];
}

//Initialize with clear terrain. Need to fill with other terrain types.
//Put your test minions here!
MasterBoard::MasterBoard()
{
	playerFlag = 1;
	cursor.XCoord = 1;
	cursor.YCoord = 1;
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].symbol = '.';
			Board[x][y].description = "Clear terrain.";
			Board[x][y].hasMinionOnTop = false;
			Board[x][y].withinRange = false;

		}
	}

	//Initialize MinionRoster to NULL.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i] = NULL;
	}
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

//Determine movement range field.
//Recursive function -_- but makes sense for this type of action.
int MasterBoard::setRangeField(int inputX, int inputY, int inputRange) 
{
	
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team != playerFlag) 
	{ 
		return 0; 
	}

	//If this is the edge of the range, set within range and return.
	if (inputRange == 0)									
	{ 
		Board[inputX][inputY].withinRange = true; 
		return 0; 
	}		

	inputRange--;
	Board[inputX][inputY].withinRange = true;
	
//Otherwise, perform function on all adjacent spaces without enemies.				
//Recursion on each direction, with IF statements to prevent us from leaving the matrix.
	if (inputX != 0)							setRangeField(inputX - 1, inputY, inputRange);
	if (inputX != (BOARD_WIDTH - 1))			setRangeField(inputX + 1, inputY, inputRange);
	if (inputY < (BOARD_HEIGHT- 1))				setRangeField(inputX, inputY +1, inputRange);
	if (inputY > 0)								setRangeField(inputX, inputY -1, inputRange);
	
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
			//int inputLocationX = inputLocation % BOARD_WIDTH;
			//int inputLocationY = (inputLocation - (inputLocation % BOARD_WIDTH)) / BOARD_WIDTH;	//Convert input coordinate to x and y.

			distanceX = abs(inputX - x);
			distanceY = abs(inputY - y);

			if ((distanceX + distanceY) <= Board[inputX][inputY].minionOnTop->attackRange)
				Board[x][y].withinRange = true;
			else Board[x][y].withinRange = false;
		}
	}
	return 0;

}

int MasterBoard::createMinion(char inputType, int inputX, int inputY, int inputTeam)
{
	//Loop through and find the next NULL pointer indicating a non-allocated part of the array.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] == NULL)
		{
			//Successful creation of new minion.
			minionRoster[i] = new Minion(i, inputX, inputY, inputType, inputTeam, this);
			if (minionRoster != NULL)
			{
				return 0;
			}
		}
	}
	
	//No room found, print error message and return 0.
	std::cout << "ERROR, no more supply cap for new minion!" << std::endl;
	return 0;

}

//Still need to figure out minor situation of: minion has already moved and shot, you shouldn't be able to select.
int MasterBoard::selectMinion(int inputX, int inputY) 
{
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team == playerFlag) 
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
		return 1;
	if (cursor.selectMinionPointer->status == hasfired || cursor.selectMinionPointer->status == hasmovedhasntfired || cursor.selectMinionPointer->status == gaveupmovehasntfired)
		return 1;

	//Find old address of the minion
	int oldX = selectedMinion->locationX;
	int oldY = selectedMinion->locationY;
	
	//Clear the old tile, set the new tile.
	Board[oldX][oldY].hasMinionOnTop = false;					
	Board[inputX][inputY].hasMinionOnTop = true;

	Board[inputX][inputY].minionOnTop = Board[oldX][oldY].minionOnTop;
	Board[oldX][oldY].minionOnTop = NULL;

	//"Move" the minion to the new location.
	selectedMinion->locationX = inputX;
	selectedMinion->locationY = inputY;

	selectedMinion->status = hasmovedhasntfired;

	
	return 0;
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
int MasterBoard::attackMinion(int inputX, int inputY)
{
	if (cursor.selectMinionPointer->hasAttacked == true)			//Cannot attack twice!
		return 1;

	if ((cursor.selectMinionPointer->type == 'A' || cursor.selectMinionPointer->type == 'R') 
		&& (isAdjacent(cursor.getX(), cursor.selectMinionPointer->locationX, cursor.getY(), cursor.selectMinionPointer->locationY) ))	 //If artillery type, cannot attack adjacent (MIN range)
		return 1;																																																				//Also, if artillery type, cannot attack if it's actually moved that turn.				

	double attackerFirePower = consultAttackValuesChart(cursor.selectMinionPointer->type, Board[inputX][inputY].minionOnTop->type);		//First perform offensive fire
	Board[inputX][inputY].minionOnTop->health -= attackerFirePower * cursor.selectMinionPointer->health;				//Decrease health by attack value, for now it's 1.

	if (Board[inputX][inputY].minionOnTop->health <= 0)
		destroyMinion((Board[inputX][inputY].minionOnTop));
	else
		if (Board[inputX][inputY].minionOnTop->type != 'A' && Board[inputX][inputY].minionOnTop->type != 'R' && cursor.selectMinionPointer->type != 'A' && cursor.selectMinionPointer->type != 'R')		//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		{
			double defenderFirePower = consultAttackValuesChart(Board[inputX][inputY].minionOnTop->type, cursor.selectMinionPointer->type);	//If still alive, then perform defensive counterfire.
			cursor.selectMinionPointer->health -= defenderFirePower * Board[inputX][inputY].minionOnTop->health;
		}	
	
	cursor.selectMinionPointer->status = hasfired;

	if (cursor.selectMinionPointer->health <= 0)			//The attacker can be destroyed too!
		destroyMinion(cursor.selectMinionPointer);

	return 0;

}

int MasterBoard::destroyMinion(Minion * inputMinion) 
{
	//Tell the board it has no minions associated in the location where the Minion was alive.
	Board[inputMinion->locationX][inputMinion->locationY].hasMinionOnTop = false;		
	
	//Create event text telling player it was destroyed.
	eventText += "PLAYER ";								
	eventText += char(playerFlag-32);							//MUST FIX IMPLEMENTATION!!!!
	eventText += "'s ";
	eventText += inputMinion->description;
	eventText += " DESTROYED!";
	
	//Clean up.
	minionRoster[inputMinion->seniority] = NULL;
	delete inputMinion;
	
	return 0;													
}

int MasterBoard::endTurn() {
	
	if (playerFlag < NUMBEROFPLAYERS)					//Either increment turnFlag or set it to zero, thus cycling through the players.
		playerFlag++;
	else if (playerFlag >= NUMBEROFPLAYERS)
		playerFlag = 1;

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i]->status = hasntmovedorfired;
		minionRoster[i]->artilleryCanAttack = true;
	}
		
	
	return 0;

}

