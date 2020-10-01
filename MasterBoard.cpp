#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include <string>

using namespace std;

extern Cursor cursor;
extern int turnFlag;
extern Minion minionRoster[GLOBALSUPPLYCAP];
extern string eventText;


bool isAdjacent(int input1, int input2) 
{
	if (abs(input1 - input2) == 1)		//Horizontally adjacent
		return true;
	if (abs(input1 - input2) == BOARD_WIDTH)		//Vertically adjacent
		return true;

	return false;
}

//Attacker vs defender matrix. Attacker determines row number, while defender determines column number.
//In order they are Infantry, Armor, Artillery, Cavalry, and Rocket.
												 // I   T    A    C    R
const double ATTACK_VALUES_MATRIX[5][5] = {		0.50, 0.05,0.10,0.10,0.25,
												0.65,0.50,0.60,0.60,0.70,
												0.60,0.40,0.50,0.55,0.60,
												0.60,0.10,0.20,0.35,0.45,
												0.80,0.60,0.65,0.70,0.80};

double consultAttackValuesChart(char attackerType, char defenderType)			//Assign numeric values for different units to access attack values matrix easier.
{
	int x, y;
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


	return ATTACK_VALUES_MATRIX[y][x];
}

MasterBoard::MasterBoard() 
{
	for (int i = 0; i < BOARD_SIZE; i++)			//Initialize with clear terrain. Need to fill with other terrain types.
	{
		Board[i].symbol = '.';
		Board[i].description = "Clear terrain.";
		Board[i].hasMinionOnTop = false;
		Board[i].withinRange = false;
	}

}

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

int MasterBoard::setRangeField(int inputLocation, int inputRange) 
{



	if (Board[inputLocation].hasMinionOnTop == true && Board[inputLocation].minionOnTop->team != turnFlag) 
	{ 
		return 0; 
	}

	if (inputRange == 0)									//If this is the edge of the range, set within range and return.
	{ 
		Board[inputLocation].withinRange = true; 
		return 0; 
	}		

	inputRange--;
	Board[inputLocation].withinRange = true;
	
//Otherwise, perform function on all adjacent spaces without enemies.				
//Recursion on each direction, with IF statements to prevent us from leaving the matrix.
	if (inputLocation % BOARD_WIDTH != 0)					setRangeField(inputLocation - 1, inputRange);
	if (inputLocation % BOARD_WIDTH != BOARD_WIDTH - 1)		setRangeField(inputLocation + 1, inputRange);
	if (inputLocation < (BOARD_HEIGHT- 1) * (BOARD_WIDTH))		setRangeField(inputLocation + BOARD_WIDTH, inputRange);
	if (inputLocation > BOARD_WIDTH)						setRangeField(inputLocation - BOARD_WIDTH, inputRange);
	
	return 0;
}

int MasterBoard::setAttackField(int inputLocation, int inputRange)		//Primary difference between move and attack is attack range goes over all units, ignoring them.
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		int inputLocationX = inputLocation % BOARD_WIDTH;
		int inputLocationY = (inputLocation - (inputLocation % BOARD_WIDTH)) / BOARD_WIDTH;	//Convert input coordinate to x and y.

		int iX = i % BOARD_WIDTH;
		int iY = (i - (i % BOARD_WIDTH)) / BOARD_WIDTH;	//Do the same with i.

		int distanceX = abs(inputLocationX - iX);
		int distanceY = abs(inputLocationY - iY);

		if ((distanceX + distanceY) <= Board[inputLocation].minionOnTop->attackRange)
			Board[i].withinRange = true;
		else Board[i].withinRange = false;
	}
	return 0;

}

int MasterBoard::createMinion(char inputType, int inputLocation, int inputTeam)
{
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
		if (minionRoster[i].isAlive == false)
		{
			minionRoster[i].addMinion(i, inputLocation, inputType, inputTeam);
			i = GLOBALSUPPLYCAP;
		}

	return 0;

}

int MasterBoard::selectMinion(int inputLocation) 
{
	if (Board[inputLocation].hasMinionOnTop == true && Board[inputLocation].minionOnTop->team == turnFlag) 
	{
		Board[inputLocation].minionOnTop->isMinionSelected = true;
		cursor.selectMinionPointer = Board[inputLocation].minionOnTop;
		cursor.selectMinionFlag = true;
		if (cursor.selectMinionPointer->hasMoved == false)
			setRangeField(inputLocation, cursor.selectMinionPointer->movementRange);
		else if (cursor.selectMinionPointer->hasAttacked == false && (cursor.selectMinionPointer->artilleryCanAttack == true || (cursor.selectMinionPointer->type != 'A' && cursor.selectMinionPointer->type != 'R')))	
			setAttackField(inputLocation, cursor.selectMinionPointer->attackRange);
	}
	return 0;
}

int MasterBoard::moveMinion(int inputLocation)
{
	if (Board[inputLocation].withinRange == false)
		return 1;
	if (cursor.selectMinionPointer->hasMoved == true)
		return 1;

	int OLD = cursor.selectMinionPointer->Location;		//Find old address of the minion
	Board[OLD].hasMinionOnTop = false;					//Clear the old tile, set the new tile.
	Board[inputLocation].hasMinionOnTop = true;

	Board[inputLocation].minionOnTop = Board[OLD].minionOnTop;
	Board[OLD].minionOnTop = NULL;

	cursor.selectMinionPointer->Location = inputLocation;
	cursor.selectMinionPointer->hasMoved = true;
	return 0;
}

int MasterBoard::deselectMinion() 
{
	cursor.selectMinionPointer->isMinionSelected = false;
	cursor.selectMinionFlag = false;
	cursor.selectMinionPointer = NULL;
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		Board[i].withinRange = false;
	}
		
	return 0;
}

int MasterBoard::attackMinion(int inputLocation) 
{
	if (cursor.selectMinionPointer->hasAttacked == true)			//Cannot attack twice!
		return 1;

	if ((cursor.selectMinionPointer->type == 'A' || cursor.selectMinionPointer->type == 'R') && (isAdjacent(cursor.Location, cursor.selectMinionPointer->Location) ))	 //If artillery type, cannot attack adjacent (MIN range)
		return 1;																																																				//Also, if artillery type, cannot attack if it's actually moved that turn.				

	double attackFactor = consultAttackValuesChart(cursor.selectMinionPointer->type, Board[inputLocation].minionOnTop->type);		//First perform offensive fire
	Board[inputLocation].minionOnTop->health -= attackFactor * cursor.selectMinionPointer->health;				//Decrease health by attack value, for now it's 1.

	if (Board[inputLocation].minionOnTop->health <= 0)
		destroyMinion((Board[inputLocation].minionOnTop));
	else
		if (Board[inputLocation].minionOnTop->type != 'A' && Board[inputLocation].minionOnTop->type != 'R' && cursor.selectMinionPointer->type != 'A' && cursor.selectMinionPointer->type != 'R')		//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		{
			attackFactor = consultAttackValuesChart(Board[inputLocation].minionOnTop->type, cursor.selectMinionPointer->type);	//If still alive, then perform defensive counterfire.
			cursor.selectMinionPointer->health -= attackFactor * Board[inputLocation].minionOnTop->health;
		}	
	
	cursor.selectMinionPointer->hasAttacked = true;

	if (cursor.selectMinionPointer->health <= 0)			//The attacker can be destroyed too!
		destroyMinion(cursor.selectMinionPointer);

	return 0;

}

int MasterBoard::destroyMinion(Minion * inputMinion) 
{ 
	inputMinion->isAlive = false;
	Board[inputMinion->Location].hasMinionOnTop = false;		//Tell the board it has no minions associated.
	eventText += "PLAYER ";									//Create event text stream telling us it was destroyed.
	eventText += char(turnFlag-32);							//MUST FIX IMPLEMENTATION!!!!
	eventText += "'s ";
	eventText += inputMinion->description;
	eventText += " DESTROYED!";
	return 0;													
}

int MasterBoard::endTurn() {
	
	if (turnFlag < NUMBEROFPLAYERS)					//Either increment turnFlag or set it to zero, thus cycling through the players.
		turnFlag++;
	else if (turnFlag >= NUMBEROFPLAYERS)
		turnFlag = 1;

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i].hasAttacked = false;
		minionRoster[i].hasMoved = false;
		minionRoster[i].artilleryCanAttack = true;
	}
		
	
	return 0;

}

