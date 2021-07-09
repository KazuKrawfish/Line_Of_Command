#ifndef TILE_H__
#define TILE_H__

#include "Minion.hpp"
#include <string>
#include <stdio.h>

//Attacker vs defender matrix. Attacker determines row, while defender determines column.
//In order they are Infantry, Specialist, Armor, Artillery, Cavalry, and Rocket.
//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, and Minion().
//												        . + ^ M  H m n h Q =
const int MOVE_VALUES_MATRIX[8][10] =			/*i*/  {1,1,2,3, 1,1,1,1,1,1,
												/*s*/   1,1,1,1, 1,1,1,1,1,1,
												/*a*/	1,2,2,99,1,1,1,1,1,1,
												/*r*/	1,2,2,99,1,1,1,1,1,1,
												/*c*/	1,1,2,99,1,1,1,1,1,1,
												/*R*/	2,3,3,99,1,1,1,1,1,1,
												/*T*/	1,2,2,99,1,1,1,1,1,1,
												/*A*/	1,2,2,99,1,1,1,1,1,1, };

class tile 
{
public:
	tile() {
		symbol = '.';
		defenseFactor = 1.1;
		description = "Clear";
		hasMinionOnTop = false;
		minionOnTop = NULL;
		withinRange = false;
		withinVision = false;
		minionOnTop = NULL;
		controller = 0;
		production = 0;
		capturePoints = 20;
		
	}

	int consultMovementChart(char minionType, char terrainType)
	{

		int x = -1;
		int y = -1;

		switch (minionType)
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
		}

		//   . + ^ M  H m n h Q =
		switch (terrainType)
		{
		case('.'):
			y = 0;
			break;
		case('+'):
			y = 1;
			break;
		case('^'):
			y = 2;
			break;
		case('M'):
			y = 3;
			break;
		case('H'):
			y = 4;
			break;
		case('m'):
			y = 5;
			break;
		case('n'):
			y = 6;
			break;
		case('h'):
			y = 7;
			break;
		case('Q'):
			y = 8;
			break;
		case('='):
			y = 9;
			break;
		}

		if (x == -1 || y == -1)
		{
			return -1;
		}

		int answer = -1;
		answer = MOVE_VALUES_MATRIX[x][y];
		return answer;
	
	}

	bool checkForProperty()
	{
		bool isProperty = false;

		//Non property included for clarity.
		switch (this->symbol)
		{
		case('.'):
		case('+'):
		case('^'):
		case('M'):
		case('='):
			isProperty = false;
			break;
		case('H'):
		case('m'):
		case('n'):
		case('h'):
		case('Q'):
			isProperty = true;
			break;
		}

		return isProperty;
	}

	int production;				//Amount of money it produces
	int controller;				//Player number 1/2 or neutral 0.
	char symbol;
	bool hasMinionOnTop;
	Minion* minionOnTop;
	bool withinRange;
	bool withinVision;
	std::string description;
	double defenseFactor;		//Higher is better (ie mountains are 1.4)
	int capturePoints;
	int locationX;
	int locationY;
	int clearTile() {
		symbol = '.';
		defenseFactor = 1.1;
		description = "Clear";
		hasMinionOnTop = false;
		minionOnTop = NULL;
		withinRange = false;
		withinVision = false;
		minionOnTop = NULL;
		controller = 0;
		production = 0;
		capturePoints = 20;


		return 0;
	}


};

#endif /* TILE_H__ */

