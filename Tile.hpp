#ifndef TILE_H__
#define TILE_H__

#include "Minion.hpp"
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>

//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, and Minion().
//When updating move values matrix for new terrain, also update set characteristics in mainmenu and checkForProperty in tile.
//												        . + ^ M  H m n h Q = ~  - A  P `
const int MOVE_VALUES_MATRIX[19][15] =			/*i*/  {1,1,2,3, 1,1,1,1,1,1,99,2,1, 1, 1,
												/*s*/   1,1,1,1, 1,1,1,1,1,1,99,1,1, 1, 1,
												/*a*/	1,2,2,99,1,1,1,1,1,1,99,99,1,1, 2,
												/*r*/	1,2,2,99,1,1,1,1,1,1,99,99,1,1, 2,
												/*c*/	2,3,3,99,1,1,1,1,1,1,99,99,1,1, 3,
												/*R*/	2,3,3,99,1,1,1,1,1,1,99,99,1,1, 3,
												/*T*/	1,2,2,99,1,1,1,1,1,1,99,99,1,1, 2,
												/*A*/	1,2,2,99,1,1,1,1,1,1,99,99,1,1, 2,
												/*v*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,1, 1,
												/*h*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,1, 1,
												/*P*/	1,2,2,99,1,1,1,1,1,1,99,99,1,1, 2,
												/*f*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,1, 1,
												/*b*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,1, 1,	
												/*L*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1,
												/*B*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1,
												/*C*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1,
												/*G*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1,
												/*U*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1,
												/*V*/	99,99,99,99,99,99,99,99,99,99,1,99,99,99,1 };

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
		withinVision.resize(3);
		withinVision[0] = false;
		withinVision[1] = false;
		withinVision[2] = false;
		withinApparentRange = false;
		minionOnTop = NULL;
		controller = 0;
		production = 0;
		capturePoints = 20;
		withinCursorPath = false;
		hasBeenValidated = false;
		//Image already initialized
		
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
		case('P'):	//APC
			x = 10;
			break;
		case('f'):	//Interceptor
			x = 11;
			break;
		case('b'):	//Bomber
			x = 12;
			break;
		case('L'):	//Lander
			x = 13;
			break;
		case('B'):	//Battleship
			x = 14;
			break;
		case('C'):	//Cruiser
			x = 15;
			break;
		case('G'):	//Gunboat
			x = 16;
			break;
		case('U'):	//Submarine
			x = 17;
			break;
		case('V'):	//Aircraft carrier
			x = 18;
			break;
		}

		//   . + ^ M  H m n h Q = ~ - A P
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
		case('~'):
			y = 10;
			break;
		case('-'):
			y = 11;
			break;
		case('A'):
			y = 12;
			break;
		case('P'):	//Port
			y = 13;
			break;
		case('\''):	//Beach
			y = 14;
			break;
		}

		if (x == -1 || y == -1)
		{
			//std::cout << "Failed to find movement chart" << std::endl;
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
		case('~'):
		case('-'):
		case('\''):
			isProperty = false;
			break;
		case('A'):
		case('P'):
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
	bool withinRange = false;
	std::vector<bool> withinVision;
	bool withinApparentRange = false;
	std::string description;
	double defenseFactor;		//Higher is better (ie mountains are 1.4)
	int capturePoints;
	int locationX;
	int locationY;
	bool withinCursorPath = false;
	bool hasBeenValidated = false;
	std::string Image =	{	'.','.','.',
							'.','.','.',
							'.','.','.' };

	int clearTile() {
		symbol = '.';
		defenseFactor = 1.1;
		description = "Clear";
		hasMinionOnTop = false;
		minionOnTop = NULL;
		withinRange = false;
		minionOnTop = NULL;
		controller = 0;
		production = 0;
		capturePoints = 20;
		withinCursorPath = false;


		return 0;
	}

	int setCharacterstics() 
	{

		switch (symbol)
		{
		case('.'):
		{
			description = "Clear terrain.";
			defenseFactor = 1.1;
			Image = { '.','.','.',
												'.','.','.',
												'.','.','.' };
			break;
		}
		case('H'):
		{
			description = "City.";
			defenseFactor = 1.3;
			production = 2000;
			Image = { 'H','=','H',
												'H','=','H',
												'H','=','H' };
			break;
		}
		case('m'):
		{
			description = "Mine.";
			defenseFactor = 1.2;
			production = 3000;
			Image = { ' ','_',' ',
												'/','n','\\',
												'.','.','.' };
			break;
		}
		case('n'):
		{
			description = "Settlement.";
			defenseFactor = 1.3;
			production = 1000;
			Image = { 'n','.','n',
												'.','.','.',
												'n','.','n' };
			break;
		}
		case('h'):
		{
			description = "Factory.";
			defenseFactor = 1.3;
			production = 1000;
			Image = { '|','|','|',
												'H','H','H',
												'H','H','H' };
			break;
		}
		case('Q'):
		{
			description = "Headquarters.";
			defenseFactor = 1.4;
			production = 1000;
			Image = { '|','*','|',
												'|','H','|',
												'|','H','|' };
			break;
		}
		case('='):
		{
			description = "Road.";
			defenseFactor = 1.0;
			Image = { '=',' ','=',
												'=',' ','=',
												'=',' ','=' };
			break;
		}
		case('^'):
		{
			description = "Hill.";
			defenseFactor = 1.1;
			Image = { '/','\\','.',
												'.','/','\\',
												'/','\\','.' };
			break;
		}
		case('M'):
		{
			description = "Mountain.";
			defenseFactor = 1.4;
			Image = { ' ','^',' ',
												'/','_','\\',
												'.','.','.' };
			break;
		}
		case('+'):		//Would like to have convertible to woodlot by engineer.....maybe
		{
			description = "Forest.";
			defenseFactor = 1.2;
			Image = { '^','^','^',
												'^','^','^',
												'|','|','|' };
			break;
		}
		case('~'):
		{
			description = "High seas.";
			defenseFactor = 1.0;
			Image = { '~','~','~',
											'~','~','~',
											'~','~','~' };
			break;
		}

		case('-'):
		{
			description = "River.";
			defenseFactor = 1.0;
			Image = { '~',' ','~',
											' ','~',' ',
											'~',' ','~' };
			break;
		}
		case('A'):
		{
			description = "Airbase.";
			defenseFactor = 1.3;
			Image = { '\\','n','.',
											'|','\\','n',
											'|','.','\\' };
			production = 1000;
			break;
		}
		case('P'):
		{
			description = "Port.";
			defenseFactor = 1.3;
			Image = { 'n','_','_',
											'|','~','~',
											'|','~','~' };
			production = 1000;
			break;
		}
		case('\''):
		{
			description = "Beach.";
			defenseFactor = 1.0;
			Image = {	'~','.','~',
						'.','~','.',
						'~','.','~' };
			break;
		}
		}
		//Other terrain types go here
	
		return 0;
	}


};

#endif /* TILE_H__ */

