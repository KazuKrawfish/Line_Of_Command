//Tile needs "appearsWithinRange"


#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include <string>
#include <iostream>
#include "inputLayer.hpp"
#include "compie.hpp"
#include <cmath>


//If the two input coordinates are next to each other, return true. Otherwise, return false.
//This can be either horizontal or vertical adjacency.
bool isAdjacent(int inputX1, int inputX2, int inputY1, int inputY2)
{
	if ((abs(inputX1 - inputX2) <= 1) && (abs(inputY1 - inputY2) == 0)
		|| (abs(inputX1 - inputX2) == 0) && (abs(inputY1 - inputY2) <= 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int MasterBoard::individualResupply(Minion* SupplyUnit, bool isItDuringUpkeep)
{

	int x = SupplyUnit->locationX;
	int y = SupplyUnit->locationY;

	//Unit must be APC
	if (SupplyUnit->type != 'P')
		return 1;

	//Check each surrounding tile for a ground or sea unit and resupply them.
	if (x < BOARD_WIDTH - 1 && Board[x + 1][y].hasMinionOnTop == true && Board[x + 1][y].minionOnTop->team == playerFlag && Board[x + 1][y].minionOnTop->domain != air)
	{
		Board[x + 1][y].minionOnTop->currentFuel = Board[x + 1][y].minionOnTop->maxFuel;
		Board[x + 1][y].minionOnTop->currentAmmo = Board[x + 1][y].minionOnTop->maxAmmo;
	}
	if (x > 0 && Board[x - 1][y].hasMinionOnTop == true && Board[x - 1][y].minionOnTop->team == playerFlag && Board[x - 1][y].minionOnTop->domain != air)
	{
		Board[x - 1][y].minionOnTop->currentFuel = Board[x - 1][y].minionOnTop->maxFuel;
		Board[x - 1][y].minionOnTop->currentAmmo = Board[x - 1][y].minionOnTop->maxAmmo;
	}
	if (y < BOARD_HEIGHT - 1 && Board[x][y + 1].hasMinionOnTop == true && Board[x][y + 1].minionOnTop->team == playerFlag && Board[x][y + 1].minionOnTop->domain != air)
	{
		Board[x][y + 1].minionOnTop->currentFuel = Board[x][y + 1].minionOnTop->maxFuel;
		Board[x][y + 1].minionOnTop->currentAmmo = Board[x][y + 1].minionOnTop->maxAmmo;
	}
	if (y > 0 && Board[x][y - 1].hasMinionOnTop == true && Board[x][y - 1].minionOnTop->team == playerFlag && Board[x][y - 1].minionOnTop->domain != air)
	{
		Board[x][y - 1].minionOnTop->currentFuel = Board[x][y - 1].minionOnTop->maxFuel;
		Board[x][y - 1].minionOnTop->currentAmmo = Board[x][y - 1].minionOnTop->maxAmmo;
	}

	if (isItDuringUpkeep == false)
	{
		SupplyUnit->status = hasfired;
		deselectMinion();
	}

	return 0;
}

//Attacker vs defender matrix. Attacker determines row, while defender determines column.
//In order they are Infantry, Specialist, Armor, Artillery, Cavalry, Rocket, Heavy Armor, and Anti-Air.
//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, movement cost, and Minion(). (Tile, masteboard, minion.)
//														i     s     a     r     c     R     T     A     v	  h		P  	  f		b
const double ATTACK_VALUES_MATRIX[13][13] = {	/*i*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,
/*s*/	0.0,  0.00, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,
/*a*/	0.0,  0.00, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,
/*r*/	0.90, 0.85, 0.70, 0.75, 0.75, 0.75, 0.40, 0.75, 0,	  0,	0.70, 0,	0,
/*c*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,
/*R*/	0.95, 0.90, 0.80, 0.85, 0.85, 0.85, 0.50, 0.85, 0,	  0,	0.80, 0,	0,
/*T*/	0.0,  0.00, 0.75, 0.80, 0.80, 0.85, 0.55, 0.80, 0.00, 0.00, 0.90, 0,	0,
/*A*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.95, 1.00, 0, 	0.60, 0.70,
/*v*/	0.00, 0.00, 0.50, 0.50, 0.60, 0.70, 0.35, 0.50, 0.00, 0.00, 0.60, 0,	0,
/*h*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,
/*P*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,
/*f*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.80, 1.0,	0,	  0.60,	0.70,
/*b*/	0.95, 0.90, 0.80, 0.85, 0.85, 0.85, 0.50, 0.85, 0,	  0,	0.80, 0,	0 };

//		i     s     a     r     c     R     T     A     v     h     P		f  b
const double SECONDARY_ATTACK_VALUES_MATRIX[13][13] = {	/*i*/	0.55, 0.50, 0.05, 0.10, 0.15, 0.25, 0.01, 0.05, 0.05, 0.10, 0.10,	0, 0,
/*s*/	0.60, 0.55, 0.08, 0.12, 0.18, 0.30, 0.02, 0.08, 0.08, 0.12, 0.12,	0, 0,
/*a*/	0.70, 0.65, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.10, 0.15, 0.35,	0, 0,
/*r*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,		0, 0,
/*c*/	0.75, 0.70, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.10, 0.15, 0.35,	0, 0,
/*R*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,		0, 0,
/*T*/	0.85, 0.80, 0.15, 0.25, 0.40, 0.50, 0.10, 0.20, 0.15, 0.25, 0.40,	0, 0,
/*A*/	1.0,  0.95, 0.20, 0.30, 0.40, 0.45, 0.05, 0.20, 0.95, 1.00, 0.40,	0, 0,
/*v*/	0.65, 0.65, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.55, 0.75, 0.35,	0, 0,
/*h*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,		0, 0,
/*P*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,		0,	0,
/*f*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.40, 0.5,	0,		0.30,	0.35,
/*b*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,		0,	0 };

//Assign numeric values for different units to access attack values matrix easier.
//Needs defaults to catch error!!!!
double consultAttackValuesChart(char attackerType, char defenderType, bool& isAmmoUsed, int AmmoAvailable, minionStatus inputStatus, RangeType minionRangeType)
{
	//Assume ammo is not used until told otherwise.
	isAmmoUsed = false;

	int x = -1;
	int y = -1;

	switch (defenderType)
	{
	case('i'):
		x = 0;
		break;
	case('s'):
		x = 1;
		break;
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
	case('T'):
		x = 6;
		break;
	case('A'):
		x = 7;
		break;
	case('v'):
		x = 8;
		break;
	case('h'):
		x = 9;
		break;
	case('P'):
		x = 10;
		break;
	case('f'):
		x = 11;
		break;
	case('b'):
		x = 12;
		break;
	}

	switch (attackerType)
	{
	case('i'):
		y = 0;
		break;
	case('s'):
		y = 1;
		break;
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
	case('T'):
		y = 6;
		break;
	case('A'):
		y = 7;
		break;
	case('v'):
		y = 8;
		break;
	case('h'):
		y = 9;
		break;
	case('P'):
		y = 10;
		break;
	case('f'):
		y = 11;
		break;
	case('b'):
		y = 12;
		break;
	}

	if (x == -1 || y == -1)
	{
		std::cout << "ERROR Does not exist within attack values matrix" << std::endl;
		return -1;
	}

	double attackScore = 0;

	if (minionRangeType != hybridRange)//If minion is not hybrid do attack like normal.
	{
		//If ammo is available and the main weapon is stronger than secondary in this case, use it.
		//Otherwise, use secondary and no ammo needed.
		if (AmmoAvailable > 0 && ATTACK_VALUES_MATRIX[y][x] > SECONDARY_ATTACK_VALUES_MATRIX[y][x])
		{
			isAmmoUsed = true;
			attackScore = ATTACK_VALUES_MATRIX[y][x];
		}
		else
		{
			isAmmoUsed = false;
			attackScore = SECONDARY_ATTACK_VALUES_MATRIX[y][x];
		}
	}
	else	//If minion is hybrid	
	{
		//This trusts the setAttackRange function to set possible attack range based on hybrid movement status.
		//If hybrid and moved, used secondary chart for hybrid. Don't need ammo. This is direct attack.
		if (inputStatus == hasmovedhasntfired)
		{
			isAmmoUsed = false;

			//Antiaircraft is a special case
			if (attackerType == 'A')
				isAmmoUsed = true;

			attackScore = SECONDARY_ATTACK_VALUES_MATRIX[y][x];
		}
		//If hybrid and didn't move, use primary. Still need ammo. This is "range attack"
		if (inputStatus == gaveupmovehasntfired && AmmoAvailable > 0)
		{
			isAmmoUsed = true;
			attackScore = ATTACK_VALUES_MATRIX[y][x];
		}
	}

	return attackScore;

}


//Return of -1 indicates the minion requested does not exist.
int MasterBoard::consultMinionCostChart(char minionType, char propertyType)
{
	bool canItBeBoughtHere = false;

	//These represent what that property repairs, not necessarily if you can buy units there. Only h can buy ground troops.
	if (propertyType == 'A' && (minionType == 'v' || minionType == 'h' || minionType == 'f' || minionType == 'b'))
	{
		canItBeBoughtHere = true;
	}
	if ((propertyType == 'n' || propertyType == 'H' || propertyType == 'Q' || propertyType == 'h') &&
		(minionType == 'i' || minionType == 's' || minionType == 'a' || minionType == 'r' || minionType == 'R'
			|| minionType == 'T' || minionType == 'A' || minionType == 'c' || minionType == 'P'))
	{
		canItBeBoughtHere = true;
	}

	if (canItBeBoughtHere == false)
	{
		return -1;
	}

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
		price = 6000;
		break;
	case('c'):
		price = 4000;
		break;
	case('R'):
		price = 15000;
		break;
	case('T'):
		price = 16000;
		break;
	case('A'):
		price = 8000;
		break;
	case('v'):
		price = 9000;
		break;
	case('h'):
		price = 5000;
		break;
	case('P'):
		price = 5000;
		break;
	case('f'):
		price = 15000;
		break;
	case('b'):
		price = 18000;
		break;
	}

	return price;
}

//Currently this is not doing what it should be doing- only partial initialization.
MasterBoard::MasterBoard()
{
	//Not sure why we have to do an extra row, whatever! (Tried with one less and it caused out of bounds..... i am insane?!?!)
	//First resize board to meet savegame specifications.
	//Also resize mypathMap.
	Board.resize(BOARD_WIDTH + 1);
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		Board[i].resize(BOARD_HEIGHT);
	}
	myPathMap.resize(BOARD_WIDTH + 1);
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		myPathMap[i].resize(BOARD_HEIGHT);
	}


	//Initialize cursor.
	cursor.XCoord = 1;
	cursor.YCoord = 1;
	cursor.boardToPlay = this;

	totalNumberOfMinions = 0;

	//Initialize MinionRoster to NULL.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i] = NULL;
	}

	treasury.resize(NUMBEROFPLAYERS + 1);
	for (int i = 0; i < NUMBEROFPLAYERS + 1; i++)
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

//This service builds a parallel set of distances that ignores enemy units that you cannot see.
//Used for doing "traps".
int MasterBoard::buildApparentPathMap(bool isItInitialCall, int x, int y, char minionType)
{

	//If this is first call, reset myPathMap
	if (isItInitialCall == true)
	{
		//Clear myPathMap
		for (int i = 0; i < BOARD_WIDTH; i++)
		{
			for (int j = 0; j < BOARD_HEIGHT; j++)
			{
				myPathMap[i][j].distanceFromMinion = -1;
				myPathMap[i][j].wasVisited = false;
			}
		}
	}

	if (Board[x][y].consultMovementChart(minionType, Board[x][y].symbol) == 99)
		return 0;



	//Check each neighbor to find lowest path FROM minion to this square, from among them.
	int lowestNeighboringPath = 99999;
	if (x - 1 >= 0 && myPathMap[x - 1][y].wasVisited == true)
	{
		if (myPathMap[x - 1][y].distanceFromMinion != -1 && myPathMap[x - 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x - 1][y].distanceFromMinion;

		}
	}
	if (y - 1 >= 0 && myPathMap[x][y - 1].wasVisited == true)
	{
		if (myPathMap[x][y - 1].distanceFromMinion != -1 && myPathMap[x][y - 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x][y - 1].distanceFromMinion;

		}
	}
	if (x + 1 < BOARD_WIDTH && myPathMap[x + 1][y].wasVisited == true)
	{
		if (myPathMap[x + 1][y].distanceFromMinion != -1 && myPathMap[x + 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x + 1][y].distanceFromMinion;
		}
	}
	if (y + 1 < BOARD_HEIGHT && myPathMap[x][y + 1].wasVisited == true)
	{
		if (myPathMap[x][y + 1].distanceFromMinion != -1 && myPathMap[x][y + 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x][y + 1].distanceFromMinion;

		}
	}

	//We now have lowest path to objective from visited neighbors.
	//If no visited neighbors, lowestNeighboringPath should be 99999. This means we can't get here from minion.
	if (isItInitialCall == true)
	{
		//Initialize start point
		//Cost to get to here from here 0.
		myPathMap[x][y].distanceFromMinion = 0;
	}
	else if (lowestNeighboringPath == 99999)
	{   //It's physically impossible to get here from minion
		myPathMap[x][y].distanceFromMinion = -1;
	}
	else
	{
		myPathMap[x][y].distanceFromMinion = lowestNeighboringPath + Board[x][y].consultMovementChart(minionType, Board[x][y].symbol);
	}

	myPathMap[x][y].wasVisited = true;


	//Now call the function for eaching neighbor that is passable (not 99 move cost or off board), and hasn't been visited, or
	//is passable,  (And has been visited) and has a higher (path score - that place's cost):
	//This is to either do a first look or to update a square that has a higher current score than ours
	//Also no enemies can be here
	//This does not include air and non-air. They can pass through each other.
	if (x - 1 >= 0)
	{
		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol) != 99 && myPathMap[x - 1][y].wasVisited != true) ||
			(Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol) != 99 && (myPathMap[x - 1][y].distanceFromMinion - Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x - 1][y].withinVision == false || Board[x - 1][y].hasMinionOnTop != true || Board[x - 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x - 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x - 1][y].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x - 1, y, minionType);
			}
	}

	if (y - 1 >= 0)
	{
		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol) != 99 && myPathMap[x][y - 1].wasVisited != true) || (Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol) != 99 && (myPathMap[x][y - 1].distanceFromMinion - Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x][y - 1].withinVision == false || Board[x][y - 1].hasMinionOnTop != true || Board[x][y - 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y - 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y - 1].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x, y - 1, minionType);
			}
	}

	if (x + 1 < BOARD_WIDTH)
	{
		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol) != 99 && myPathMap[x + 1][y].wasVisited != true) || (Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol) != 99 && (myPathMap[x + 1][y].distanceFromMinion - Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x + 1][y].withinVision == false || Board[x + 1][y].hasMinionOnTop != true || Board[x + 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x + 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x + 1][y].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x + 1, y, minionType);
			}
	}

	if (y + 1 < BOARD_HEIGHT)
	{
		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol) != 99 && myPathMap[x][y + 1].wasVisited != true) || (Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol) != 99 && (myPathMap[x][y + 1].distanceFromMinion - Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x][y + 1].withinVision == false || Board[x][y + 1].hasMinionOnTop != true || Board[x][y + 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y + 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y + 1].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x, y + 1, minionType);
			}
	}


	return 1;
}


int MasterBoard::buildPath(bool isItInitialCall, int x, int y, char minionType)
{

	//If this is first call, reset myPathMap
	if (isItInitialCall == true)
	{
		//Clear myPathMap
		for (int i = 0; i < BOARD_WIDTH; i++)
		{
			for (int j = 0; j < BOARD_HEIGHT; j++)
			{
				myPathMap[i][j].distanceFromMinion = -1;
				myPathMap[i][j].wasVisited = false;
			}
		}
	}

	if (Board[x][y].consultMovementChart(minionType, Board[x][y].symbol) == 99)
		return 0;



	//Check each neighbor to find lowest path FROM minion to this square, from among them.
	int lowestNeighboringPath = 99999;
	if (x - 1 >= 0 && myPathMap[x - 1][y].wasVisited == true)
	{
		if (myPathMap[x - 1][y].distanceFromMinion != -1 && myPathMap[x - 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x - 1][y].distanceFromMinion;

		}
	}
	if (y - 1 >= 0 && myPathMap[x][y - 1].wasVisited == true)
	{
		if (myPathMap[x][y - 1].distanceFromMinion != -1 && myPathMap[x][y - 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x][y - 1].distanceFromMinion;

		}
	}
	if (x + 1 < BOARD_WIDTH && myPathMap[x + 1][y].wasVisited == true)
	{
		if (myPathMap[x + 1][y].distanceFromMinion != -1 && myPathMap[x + 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x + 1][y].distanceFromMinion;
		}
	}
	if (y + 1 < BOARD_HEIGHT && myPathMap[x][y + 1].wasVisited == true)
	{
		if (myPathMap[x][y + 1].distanceFromMinion != -1 && myPathMap[x][y + 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = myPathMap[x][y + 1].distanceFromMinion;

		}
	}

	//We now have lowest path to objective from visited neighbors.
	//If no visited neighbors, lowestNeighboringPath should be 99999. This means we can't get here from minion.
	if (isItInitialCall == true)
	{
		//Initialize start point
		//Cost to get to here from here 0.
		myPathMap[x][y].distanceFromMinion = 0;
	}
	else if (lowestNeighboringPath == 99999)
	{   //It's physically impossible to get here from minion
		myPathMap[x][y].distanceFromMinion = -1;
	}
	else
	{
		myPathMap[x][y].distanceFromMinion = lowestNeighboringPath + Board[x][y].consultMovementChart(minionType, Board[x][y].symbol);
	}

	myPathMap[x][y].wasVisited = true;


	//Now call the function for eaching neighbor that is passable (not 99 move cost or off board), and hasn't been visited, or
	//is passable,  (And has been visited) and has a higher (path score - that place's cost):
	//This is to either do a first look or to update a square that has a higher current score than ours
	//Also no enemies can be here
	//This does not include air and non-air. They can pass through each other.
	if (x - 1 >= 0)
	{
		if ((Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol) != 99 && myPathMap[x - 1][y].wasVisited != true) ||
			(Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol) != 99 && (myPathMap[x - 1][y].distanceFromMinion - Board[x - 1][y].consultMovementChart(minionType, Board[x - 1][y].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x - 1][y].hasMinionOnTop != true || Board[x - 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x - 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x - 1][y].minionOnTop->domain != air))
			{
				buildPath(false, x - 1, y, minionType);
			}
	}

	if (y - 1 >= 0)
	{
		if ((Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol) != 99 && myPathMap[x][y - 1].wasVisited != true) || (Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol) != 99 && (myPathMap[x][y - 1].distanceFromMinion - Board[x][y - 1].consultMovementChart(minionType, Board[x][y - 1].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x][y - 1].hasMinionOnTop != true || Board[x][y - 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y - 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y - 1].minionOnTop->domain != air))
			{
				buildPath(false, x, y - 1, minionType);
			}
	}

	if (x + 1 < BOARD_WIDTH)
	{
		if ((Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol) != 99 && myPathMap[x + 1][y].wasVisited != true) || (Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol) != 99 && (myPathMap[x + 1][y].distanceFromMinion - Board[x + 1][y].consultMovementChart(minionType, Board[x + 1][y].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x + 1][y].hasMinionOnTop != true || Board[x + 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x + 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x + 1][y].minionOnTop->domain != air))
			{
				buildPath(false, x + 1, y, minionType);
			}
	}

	if (y + 1 < BOARD_HEIGHT)
	{
		if ((Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol) != 99 && myPathMap[x][y + 1].wasVisited != true) || (Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol) != 99 && (myPathMap[x][y + 1].distanceFromMinion - Board[x][y + 1].consultMovementChart(minionType, Board[x][y + 1].symbol)) > myPathMap[x][y].distanceFromMinion))
			if (Board[x][y + 1].hasMinionOnTop != true || Board[x][y + 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y + 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y + 1].minionOnTop->domain != air))
			{
				buildPath(false, x, y + 1, minionType);
			}
	}


	return 1;
}

//Tells us how an attacker would perform against an enemy. Useful for actual attack or calculation - doesn't actually deal damage.
//Returns a number between 0 - 100 - actual health, not percent.
//Also sets wouldAmmoBeUsed so caller can use for actual damage calcs and then subtract ammo if needed
double MasterBoard::calculateDamageDealt(Minion* attackingMinion, Minion* defendingMinion, bool& wouldAmmoBeUsed)
{
	//This will be immediately reset but we'll set it anyway.

	int ammoAvailable = attackingMinion->currentAmmo;

	//First find attacking fire power. 
	double attackerFirePower = consultAttackValuesChart(attackingMinion->type, defendingMinion->type, wouldAmmoBeUsed, ammoAvailable, attackingMinion->status, attackingMinion->rangeType);
	attackerFirePower = attackerFirePower * attackingMinion->calculateVetBonus();

	//Calculate defense factor. If air unit it remains 1.
	double defenseFactor = 1;
	if (defendingMinion->domain != air && defendingMinion->domain != helo)
	{
		defenseFactor = Board[defendingMinion->locationX][defendingMinion->locationY].defenseFactor;
	}
	defenseFactor = defenseFactor * defendingMinion->calculateVetBonus();

	return attackerFirePower * attackingMinion->health / defenseFactor;
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



	return 0;
}

//Ensures cursor stays within the window.
int MasterBoard::checkWindow()
{

	//Incremental moves
	if (cursor.getX() == (windowLocationX) && cursor.getX() != 0)						//If the cursor moves to the left edge of the window AND it's not at the edge of the board
		windowLocationX--;																			//Shift the window left

	if (cursor.getX() == (windowLocationX + WINDOW_WIDTH - 1) && (windowLocationX + WINDOW_WIDTH != BOARD_WIDTH))		//If the cursor moves to the right edge of the window AND it's not at the edge of the board
		windowLocationX++;																																//Shift the window to the right

	if (cursor.getY() == windowLocationY && cursor.getY() != 0)						//If the cursor moves to the top edge of the window AND it's not at the edge of the board
		windowLocationY--;																//Shift the window up.

	if (cursor.getY() == (windowLocationY + WINDOW_HEIGHT - 1) && (windowLocationY + WINDOW_HEIGHT != BOARD_HEIGHT))				//If the cursor moves to the bottom of the window AND it's not at the bottom of the board
		windowLocationY++;																														//Shift the window down once.



		//Non incremental moves. If cursor somehow got to the right of the window																																	//Attempt to center cursor on window if it got moved non-incrementally:
	if (cursor.getX() >= windowLocationX + WINDOW_WIDTH - 1 && (windowLocationX + WINDOW_WIDTH <= BOARD_WIDTH))
	{
		if (cursor.getX() >= BOARD_WIDTH - WINDOW_WIDTH - 1)
		{
			windowLocationX = BOARD_WIDTH - WINDOW_WIDTH;
		}
		else
		{
			windowLocationX = cursor.getX();
		}

	}
	//Non incremental moves. If cursor somehow got below our window
	if (cursor.getY() >= windowLocationY + WINDOW_HEIGHT - 1 && (windowLocationY + WINDOW_HEIGHT <= BOARD_HEIGHT))
	{
		if (cursor.getY() >= BOARD_HEIGHT - WINDOW_HEIGHT - 1)
		{
			windowLocationY = BOARD_HEIGHT - WINDOW_HEIGHT;
		}
		else
		{
			windowLocationY = cursor.getY();
		}

	}

	//Non incremental moves. If cursor somehow got to the left of our window.
	if (cursor.getX() < windowLocationX)
	{
		windowLocationX = cursor.getX();
	}

	//Non incremental moves. If cursor somehow got above our window.
	if (cursor.getY() < windowLocationY)
	{
		windowLocationY = cursor.getY();
	}

	return 0;
}

//This is called when selectMinion is called. It sets the visible squares the minion can move through.
int MasterBoard::setRangeField(int inputX, int inputY)
{
	//Build a path map for this particular minion.
	buildPath(true, inputX, inputY, cursor.selectMinionPointer->type);

	//Set the minion's tile to within Range, always.
	Board[inputX][inputY].withinRange = true;

	//Go thru entire board and set withinRange or not
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//Also must be within range
			//Also must have enough fuel
			if (myPathMap[x][y].distanceFromMinion != -1 && myPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->movementRange && myPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->currentFuel)
				if (Board[x][y].hasMinionOnTop != true || Board[x][y].minionOnTop->team == playerFlag)
				{
					Board[x][y].withinRange = true;

				}
		}
	}

	//Then re-build the path map for apparent path.
	buildApparentPathMap(true, inputX, inputY, cursor.selectMinionPointer->type);

	//Set the minion's tile to within Range, always.
	Board[inputX][inputY].withinApparentRange = true;

	//Go thru entire board and set withinRange or not
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//Also must be within range
			//Also must have enough fuel
			//Again, here if the tile is not visible we are pretending to allow movement.
			if (myPathMap[x][y].distanceFromMinion != -1 && myPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->movementRange && myPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->currentFuel)
				if (Board[x][y].withinVision == false || Board[x][y].hasMinionOnTop != true || Board[x][y].minionOnTop->team == playerFlag)
				{
					Board[x][y].withinApparentRange = true;

				}
		}
	}



	return 0;


}

//First call will be cursor location
int MasterBoard::setCursorPath(bool firstTime, int inputX, int inputY)
{
	if (cursor.selectMinionFlag == false || cursor.selectMinionPointer->status != hasntmovedorfired)
		return 1;

	if (firstTime == true)
	{
		//First clear board of paths
		//Go thru entire board and set withinCursorPath to false
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				Board[x][y].withinCursorPath = false;
			}
		}
	}

	int lowestCostToSelectedMinion = myPathMap[inputX][inputY].distanceFromMinion;
	int choice = 0;

	if (Board[inputX][inputY].withinApparentRange == true)
		Board[inputX][inputY].withinCursorPath = true;

	if (Board[inputX][inputY].withinApparentRange == false && firstTime == true)
	{
		return 1;
	}

	if (inputX > 0 && (myPathMap[inputX - 1][inputY].distanceFromMinion < lowestCostToSelectedMinion && myPathMap[inputX - 1][inputY].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myPathMap[inputX - 1][inputY].distanceFromMinion;
		choice = 1;
	}
	if (inputY > 0 && (myPathMap[inputX][inputY - 1].distanceFromMinion < lowestCostToSelectedMinion && myPathMap[inputX][inputY - 1].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myPathMap[inputX][inputY - 1].distanceFromMinion;
		choice = 2;
	}
	if (inputY < BOARD_HEIGHT - 1 && (myPathMap[inputX][inputY + 1].distanceFromMinion < lowestCostToSelectedMinion && myPathMap[inputX][inputY + 1].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myPathMap[inputX][inputY + 1].distanceFromMinion;
		choice = 3;
	}
	if (inputX < BOARD_WIDTH - 1 && (myPathMap[inputX + 1][inputY].distanceFromMinion < lowestCostToSelectedMinion && myPathMap[inputX + 1][inputY].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myPathMap[inputX + 1][inputY].distanceFromMinion;
		choice = 4;
	}

	switch (choice)
	{
	case(1):
		setCursorPath(false, inputX - 1, inputY);
		break;
	case(2):
		setCursorPath(false, inputX, inputY - 1);
		break;
	case(3):
		setCursorPath(false, inputX, inputY + 1);
		break;
	case(4):
		setCursorPath(false, inputX + 1, inputY);
		break;
	case(0):
		return 1;
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
	if (inputX != 0)
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
			setIndividualVisionField(inputX, nextY, visionLeft - 1, minionX, minionY);
		}

	}

	if (inputY != 0)
	{

		nextY = inputY - 1;

		if (visionLeft >= 1)
		{
			setIndividualVisionField(inputX, nextY, visionLeft - 1, minionX, minionY);
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
			//If you do control a property, it is actually visible.
			if (Board[x][y].controller == playerFlag)
				Board[x][y].withinVision = true;
			else
				Board[x][y].withinVision = false;
		}
	}

	//Go through the roster of minions and determine if it's player controlled.
	//Then set vision for everything that minion can see.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL && minionRoster[i]->transporter == NULL)
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

	//Transport cannot attack so its attack field is 0.
	//If out of ammo and no secondary, attack field is 0.
	if (cursor.selectMinionPointer->type == transport || (cursor.selectMinionPointer->currentAmmo == 0 && cursor.selectMinionPointer->hasSecondaryWeapon != true))
	{
		return 1;
	}

	//Hybrid unit can shoot direct fire after moving.
	if (cursor.selectMinionPointer->rangeType == hybridRange && cursor.selectMinionPointer->status == hasmovedhasntfired)
	{
		if (inputX < BOARD_WIDTH - 1)
			Board[inputX + 1][inputY].withinRange = true;

		if (inputX > 0)
			Board[inputX - 1][inputY].withinRange = true;

		if (inputY < BOARD_HEIGHT - 1)
			Board[inputX][inputY + 1].withinRange = true;

		if (inputY > 0)
			Board[inputX][inputY - 1].withinRange = true;

	}
	else	//If any other type of unit.
	{

		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{

				distanceX = abs(inputX - x);
				distanceY = abs(inputY - y);

				if ((distanceX + distanceY) <= Board[inputX][inputY].minionOnTop->attackRange)
					Board[x][y].withinRange = true;
				else Board[x][y].withinRange = false;
			}
		}

		//If it is ranged unit, cannot attack next door.
		//Hybrid fire unit also acts like "ranged" if hasn't moved that turn.
		if (cursor.selectMinionPointer->rangeType == rangedFire || (cursor.selectMinionPointer->rangeType == hybridRange && cursor.selectMinionPointer->status == gaveupmovehasntfired))
		{
			if (inputX < BOARD_WIDTH - 1)
				Board[inputX + 1][inputY].withinRange = false;

			if (inputX > 0)
				Board[inputX - 1][inputY].withinRange = false;

			if (inputY < BOARD_HEIGHT - 1)
				Board[inputX][inputY + 1].withinRange = false;

			if (inputY > 0)
				Board[inputX][inputY - 1].withinRange = false;
		}



	}

	return 0;
}

int MasterBoard::setDropField(int inputX, int inputY)
{

	//If not transport its drop field is zero.
	if (cursor.selectMinionPointer->specialtyGroup != transport)
	{
		return 1;
	}


	//Check each adjacent tile, if it is not impassible or off map, put in range for transport.
	if (inputX < BOARD_WIDTH - 1 && Board[inputX + 1][inputY].consultMovementChart(cursor.selectMinionPointer->type, Board[inputX + 1][inputY].symbol) != 99)
		Board[inputX + 1][inputY].withinRange = true;

	if (inputX > 0 && Board[inputX - 1][inputY].consultMovementChart(cursor.selectMinionPointer->type, Board[inputX - 1][inputY].symbol) != 99)
		Board[inputX - 1][inputY].withinRange = true;

	if (inputY < BOARD_HEIGHT - 1 && Board[inputX][inputY + 1].consultMovementChart(cursor.selectMinionPointer->type, Board[inputX][inputY + 1].symbol) != 99)
		Board[inputX][inputY + 1].withinRange = true;

	if (inputY > 0 && Board[inputX][inputY - 1].consultMovementChart(cursor.selectMinionPointer->type, Board[inputX][inputY - 1].symbol) != 99)
		Board[inputX][inputY - 1].withinRange = true;

	return 0;
}

int MasterBoard::attemptPurchaseMinion(char inputType, int inputX, int inputY, int inputTeam)
{
	if (treasury[playerFlag] >= consultMinionCostChart(inputType, Board[inputX][inputY].symbol) && Board[inputX][inputY].hasMinionOnTop == false)
	{
		createMinion(inputType, inputX, inputY, playerFlag, 100, hasfired, 0, 0, -1, -1);
		treasury[playerFlag] -= consultMinionCostChart(inputType, Board[inputX][inputY].symbol);
		return 0;
	}
	else return 1;


}

int MasterBoard::createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy, int beingTransported, int inputFuel, int inputAmmo)
{
	//Loop through and find the next NULL pointer indicating a non-allocated part of the array.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] == NULL)
		{
			//Successful creation of new minion.
			minionRoster[i] = new Minion(i, inputX, inputY, inputType, inputTeam, this, inputHealth, veterancy, beingTransported, inputFuel, inputAmmo);
			if (minionRoster[i] != NULL)
			{
				minionRoster[i]->status = (minionStatus)status;
				totalNumberOfMinions++;
				if (beingTransported == 0) //Only access the board if minion has legit location
				{
					Board[inputX][inputY].minionOnTop = minionRoster[i];
				}
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
		(Board[inputX][inputY].minionOnTop->rangeType == directFire || Board[inputX][inputY].minionOnTop->rangeType == hybridRange || Board[inputX][inputY].minionOnTop->status != hasmovedhasntfired))
	{
		cursor.selectMinionPointer = Board[inputX][inputY].minionOnTop;
		cursor.selectMinionFlag = true;

		//If minion hasn't moved or fired yet, display its movement range.
		if (cursor.selectMinionPointer->status == hasntmovedorfired)
		{
			setRangeField(inputX, inputY);
			return 0;
		}
		else //If minion has moved but hasn't fired, and is direct combat, display attack range.
			//And is not transport
			if (cursor.selectMinionPointer->status == hasmovedhasntfired && (cursor.selectMinionPointer->rangeType == directFire || cursor.selectMinionPointer->rangeType == hybridRange) && cursor.selectMinionPointer->specialtyGroup != transport)
			{
				setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
				return 0;
			}

			else //If minion stood in place and hasn't fired, display attack range.
				if (cursor.selectMinionPointer->status == gaveupmovehasntfired && cursor.selectMinionPointer->specialtyGroup != transport)
				{
					setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
					return 0;
				}		//If transport that either moved or stood in place, we still select. But may not display drop field.
				else if ((cursor.selectMinionPointer->status == hasmovedhasntfired || cursor.selectMinionPointer->status == gaveupmovehasntfired)
					&& cursor.selectMinionPointer->specialtyGroup == transport)
				{
					//If minion is transport and has a guy embarked, show drop field.
					if (cursor.selectMinionPointer->minionBeingTransported != NULL)
						setDropField(inputX, inputY);
					return 0;
				}
	}
	return 1;
}


//Must find the closest valid tile where we can move, other than inputX/inputY.
int MasterBoard::validatePath(int inputX, int inputY)
{

	Minion* selectedMinion = cursor.selectMinionPointer;

	//If we have reached the cursor, return.
	if (cursor.getX() == inputX && cursor.getY() == inputY)
		return 0;

	//Check each adjacent space. If it is within the cursor path and doesn't have enemy minion, turn cursorPath here off, and return its validatePath value.
	//If it does have an enemy minion, return 1.

	if (inputX > 0 && Board[inputX - 1][inputY].withinCursorPath == true)
	{
		if (Board[inputX - 1][inputY].hasMinionOnTop == true && Board[inputX - 1][inputY].minionOnTop->team != playerFlag)
		{
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

			cursor.selectMinionPointer->currentFuel -= myPathMap[inputX][inputY].distanceFromMinion;

			cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
			deselectMinion();

			setVisionField();
			return 1;
		}
		else
		{
			Board[inputX - 1][inputY].withinCursorPath = false; // Prevent infinite recursion.
			return validatePath(inputX - 1, inputY);
		}
	}

	if (inputX < BOARD_WIDTH - 1 && Board[inputX + 1][inputY].withinCursorPath == true)
	{
		if (Board[inputX + 1][inputY].hasMinionOnTop == true && Board[inputX + 1][inputY].minionOnTop->team != playerFlag)
		{
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

			cursor.selectMinionPointer->currentFuel -= myPathMap[inputX][inputY].distanceFromMinion;

			cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
			deselectMinion();

			setVisionField();
			return 1;
		}
		else
		{
			Board[inputX + 1][inputY].withinCursorPath = false; // Prevent infinite recursion.
			return validatePath(inputX + 1, inputY);
		}
	}

	if (inputY > 0 && Board[inputX][inputY - 1].withinCursorPath == true)
	{
		if (Board[inputX][inputY - 1].hasMinionOnTop == true && Board[inputX][inputY - 1].minionOnTop->team != playerFlag)
		{
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

			cursor.selectMinionPointer->currentFuel -= myPathMap[inputX][inputY].distanceFromMinion;

			cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
			deselectMinion();

			setVisionField();
			return 1;
		}
		else
		{
			Board[inputX][inputY - 1].withinCursorPath = false; // Prevent infinite recursion.
			return validatePath(inputX, inputY - 1);
		}
	}

	if (inputY < BOARD_HEIGHT - 1 && Board[inputX][inputY + 1].withinCursorPath == true)
	{
		if (Board[inputX][inputY + 1].hasMinionOnTop == true && Board[inputX][inputY + 1].minionOnTop->team != playerFlag)
		{
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

			cursor.selectMinionPointer->currentFuel -= myPathMap[inputX][inputY].distanceFromMinion;

			cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
			deselectMinion();

			setVisionField();
			return 1;
		}
		else
		{
			Board[inputX][inputY + 1].withinCursorPath = false; // Prevent infinite recursion.
			return validatePath(inputX, inputY + 1);
		}
	}


}



int MasterBoard::moveMinion(int inputX, int inputY)
{
	Minion* selectedMinion = cursor.selectMinionPointer;

	//First make sure the move is legal. Must be within range, must not have moved.
	//If in "apparent range" that means there's an enemy hiding there, we proceed until validatePath.
	if (Board[inputX][inputY].withinRange == false && Board[inputX][inputY].withinApparentRange == false)
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
	//Again, this is for visible tiles.
	if (Board[inputX][inputY].hasMinionOnTop == true
		&& Board[inputX][inputY].withinVision == true
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


	//This is the "trap" check. If it appears movable, but has a minion, you get trapped.
	//validatePath will actually move the minion, so we need to return afterwards.
	if (validatePath(cursor.selectMinionPointer->locationX,cursor.selectMinionPointer->locationY) == 1)
		return 0;


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

	cursor.selectMinionPointer->currentFuel -= myPathMap[inputX][inputY].distanceFromMinion;

	cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
	deselectMinion();

	setVisionField();

	return 0;
}

int MasterBoard::pickUpMinion(int inputX, int inputY)
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

	//If there is a minion below cursor and it is not the selected minion:
	if (Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true
		&& !(cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		//Must be transport type, and selected minion must be infantry.
		//Transport cannot already have an infantry.
		if (selectedMinion->specialtyGroup == infantry && Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == transport)
		{
			//Transport can't be carrying someone already.
			if (Board[cursor.getX()][cursor.getY()].minionOnTop->minionBeingTransported == NULL)
			{
				//Put the minion in the transport.
				Board[cursor.getX()][cursor.getY()].minionOnTop->minionBeingTransported = selectedMinion;
				selectedMinion->transporter = Board[cursor.getX()][cursor.getY()].minionOnTop;

				//Find old address of the minion
				int oldX = selectedMinion->locationX;
				int oldY = selectedMinion->locationY;

				//Clear the old tile, set the new tile.
				Board[oldX][oldY].hasMinionOnTop = false;
				Board[oldX][oldY].minionOnTop = NULL;

				//Reset capture points for tile.
				Board[oldX][oldY].capturePoints = 20;

				//"Move" the minion to the new location.
				//This is for "debugging", pretty bad but it needs to be invalid.
				selectedMinion->locationX = -1;
				selectedMinion->locationY = -1;

				cursor.selectMinionPointer->status = hasmovedhasntfired;
				deselectMinion();

				setVisionField();
			}




			return 0;

		}


	}


	return 1;
}

int MasterBoard::dropOffMinion()
{
	//Cannot drop off if recently deployed.
	if (cursor.selectMinionPointer->status == hasfired)
	{
		return 1;
	}

	//If not in range cannot drop off
	if (Board[cursor.getX()][cursor.getY()].withinRange == false)
	{
		return 1;
	}

	//Otherwise drop off minion.
	//Also reset the transport to no longer carrying a guy.
	Board[cursor.getX()][cursor.getY()].hasMinionOnTop = true;
	Board[cursor.getX()][cursor.getY()].minionOnTop = cursor.selectMinionPointer->minionBeingTransported;

	cursor.selectMinionPointer->minionBeingTransported = NULL;
	Board[cursor.getX()][cursor.getY()].minionOnTop->transporter = NULL;
	Board[cursor.getX()][cursor.getY()].minionOnTop->status = hasfired;

	//"Move" the minion to the new location.
	Board[cursor.getX()][cursor.getY()].minionOnTop->locationX = cursor.getX();
	Board[cursor.getX()][cursor.getY()].minionOnTop->locationY = cursor.getY();


	cursor.selectMinionPointer->status = hasfired;
	deselectMinion();

	setVisionField();
	return 0;
}

std::string MasterBoard::captureProperty(tile* inputTile, Minion* inputMinion)
{
	//Check for actual property before we capture
	if (inputTile->checkForProperty() != true)
	{
		return "Not a property";
	}

	std::string textToReturn = "";
	//Subtract capturing minion's health
	int pointsToTake = int(std::round(inputMinion->health / 10));
	inputTile->capturePoints -= pointsToTake;
	inputMinion->isCapturing = true;

	if (inputTile->capturePoints <= 0)
	{
		inputTile->controller = inputMinion->team;
		inputTile->capturePoints = 20;

		inputMinion->isCapturing = false;

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

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].withinRange = false;
			Board[x][y].withinCursorPath = false;
			Board[x][y].withinApparentRange = false;
		}
	}

	cursor.selectMinionFlag = false;
	cursor.selectMinionPointer = NULL;


	return 0;
}

//Additionally, can only attack once. If artillery, cannot have moved or be adjacent.
//If hybrid, different attack based on if you have moved or not.
int MasterBoard::attackMinion(int inputX, int inputY, inputLayer* InputLayer)
{


	//Simplify by finding shorthand values first.
	Minion* attackingMinion = cursor.selectMinionPointer;
	Minion* defendingMinion = Board[inputX][inputY].minionOnTop;
	bool isAmmoUsed = false;


	//Need to ensure we can't attack if we can't do any damage.
	if (calculateDamageDealt(attackingMinion, defendingMinion, isAmmoUsed) <= 0)
		return 1;


	bool defenderAlive = true;

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
	}

	//Also, if artillery type, cannot attack if it's actually moved that turn.				


	isAmmoUsed = false;
	//Decrease defender's health by attack value. Decrease ammo as needed.
	defendingMinion->health -= calculateDamageDealt(attackingMinion, defendingMinion, isAmmoUsed);
	if (isAmmoUsed == true)
	{
		attackingMinion->currentAmmo--;
	}


	if (defendingMinion->health < 5)
	{
		//If defender falls below 4, it dies.
		bool printMessage = true;
		destroyMinion(defendingMinion, printMessage, InputLayer);
		defenderAlive = false;
		if (attackingMinion->veterancy <= 3)
		{
			attackingMinion->veterancy++;
		}
	}
	else	//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		if ((defendingMinion->rangeType == directFire || defendingMinion->rangeType == hybridRange) && (isAdjacent(cursor.getX(), attackingMinion->locationX, cursor.getY(), attackingMinion->locationY)))
		{
			//If defender still alive, then perform defensive counterfire.
			//Same calculations as above - includes veterancy
			isAmmoUsed = false;

			attackingMinion->health -= calculateDamageDealt(defendingMinion, attackingMinion, isAmmoUsed);

			if (isAmmoUsed == true)
			{
				defendingMinion->currentAmmo--;
			}


		}

	if (attackingMinion->health < 5)			//The attacker can be destroyed too!
	{
		bool printMessage = true;
		destroyMinion(attackingMinion, printMessage, InputLayer);
		if (defenderAlive == true && defendingMinion->veterancy <= 3)
		{
			defendingMinion->veterancy++;
		}
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
	//If carrying a guy, kill that guy too.
	if (inputMinion->minionBeingTransported != NULL)
	{
		destroyMinion(inputMinion->minionBeingTransported, printMessage, InputLayer);
	}

	//If minion is being transported, then do not attmept to deselect or talk to its location.
	if (inputMinion->transporter == NULL)
	{
		deselectMinion();
		//Tell the board it has no minions associated in the location where the Minion was alive.
		//Reset capture points for the property.
		Board[inputMinion->locationX][inputMinion->locationY].hasMinionOnTop = false;
		Board[inputMinion->locationX][inputMinion->locationY].capturePoints = 20;
	}

	if (printMessage == true)
	{
		//Create event text telling player it was destroyed.
		InputLayer->eventText += "PLAYER ";
		InputLayer->eventText += char(playerFlag + '0');
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

int MasterBoard::endTurn(inputLayer* InputLayer) {

	int gameTurnIncrement = 0;
	//Either increment playerFlag or set it to zero, thus cycling through the players.
	if (playerFlag < NUMBEROFPLAYERS)
	{
		playerFlag++;

	}
	else
		if (playerFlag >= NUMBEROFPLAYERS)
		{
			//We need to tell inputLayer to tell mainMenu to increment gameTurn.
			gameTurnIncrement = 1;
			playerFlag = 1;
		}

	//Set minionToBuy to the default null value.
	InputLayer->requestedMinionToBuy = '\n';

	//Reset every minion's status.
	//Got messed up by people dying
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL)
		{
			minionRoster[i]->status = hasntmovedorfired;
		}
	}

	//Shouldn't matter if local or not
	InputLayer->status = waitingForNextLocalPlayer;

	upkeep(InputLayer);

	return gameTurnIncrement;

}

//Upkeep
int MasterBoard::upkeep(inputLayer* InputLayer)
{
	//Set vision field for current player
	setVisionField();

	//Provide income for the current player based on properties he controls.
	//Move cursor to HQ if it exists
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (Board[x][y].controller == playerFlag)
			{
				treasury[playerFlag] += Board[x][y].production;
				if (Board[x][y].symbol == 'Q')
				{
					cursor.relocate(x, y);
				}
			}

		}
	}

	repairMinions();

	resupplyMinions();

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If it's a minion you own and it's not being transported
		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag && minionRoster[i]->transporter == NULL)
		{
			if (minionRoster[i]->domain == helo && Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol == 'A')
			{
				minionRoster[i]->currentFuel -= 2;
				if (minionRoster[i]->currentFuel <= 0)
				{
					destroyMinion(minionRoster[i], "No fuel", InputLayer);
				}
			}
			if (minionRoster[i]->domain == air && Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol == 'A')
			{
				minionRoster[i]->currentFuel -= 5;
				if (minionRoster[i]->currentFuel <= 0)
				{
					destroyMinion(minionRoster[i], "No fuel", InputLayer);
				}
			}
		}
	}

	//Move cursor to HQ
	return 0;
}

int MasterBoard::repairMinions()
{
	//This was terminating "early" since it would hit a NULL spot so minions wouldn't get seen to be healed.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If it's a minion you own and it's not being transported
		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag && minionRoster[i]->transporter == NULL)
		{
			tile* tileToExamine = &Board[minionRoster[i]->locationX][minionRoster[i]->locationY];
			//If it is on a player controlled tile, and that tile is a "repairing" tile for the given unit.
			if (tileToExamine->controller == playerFlag && consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) != -1)
			{
				if (minionRoster[i]->health < 100)
				{

					//silent repair
					if (minionRoster[i]->health > 94)
					{
						minionRoster[i]->health = 100;
					}
					//"1 health" repair
					else if (minionRoster[i]->health <= 94 && minionRoster[i]->health > 84)
					{
						minionRoster[i]->health = 100;
						treasury[playerFlag] -= int(consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) / 10);
					}
					//"2" repair, but close enough to not just add
					else if (minionRoster[i]->health <= 84 && minionRoster[i]->health > 80)
					{
						minionRoster[i]->health = 100;
						treasury[playerFlag] -= int(consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) / 5);
					}
					//Standard "2" repair
					else
					{
						minionRoster[i]->health += 20;
						treasury[playerFlag] -= int(consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) / 5);
					}
				}
			}
		}

	}
	return 0;
}

int MasterBoard::resupplyMinions()
{

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If it's a minion you own and it's not being transported
		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag && minionRoster[i]->transporter == NULL)
		{
			tile* tileToExamine = &Board[minionRoster[i]->locationX][minionRoster[i]->locationY];
			//If it is on a player controlled tile, and that tile is a "repairing/resupplying" tile for the given unit.
			if (tileToExamine->controller == playerFlag && consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) != -1)
			{
				minionRoster[i]->currentFuel = minionRoster[i]->maxFuel;
				minionRoster[i]->currentAmmo = minionRoster[i]->maxAmmo;
			}

			if (minionRoster[i]->type == 'P')
			{
				individualResupply(minionRoster[i], true);
			}

		}
	}

	//Need to add APC in too

	return 0;
}