#ifndef TILE_H__
#define TILE_H__

#include "Minion.hpp"
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>


//Forward declare
extern std::vector <std::vector<sf::IntRect>> rectArray;
class Masterboard;


//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, and Minion().
//When updating move values matrix for new terrain, also update set characteristics in mainmenu and checkForProperty in tile.
//												        . + ^ M  H m n h Q = ~  - A			P `
const int MOVE_VALUES_MATRIX[19][15] =			/*i*/  {1,1,2,3, 1,1,1,1,1,1,99,2,1,		1, 1,
												/*s*/   1,1,1,1, 1,1,1,1,1,1,99,1,1,		1, 1,
												/*a*/	1,2,2,99,1,1,1,1,1,1,99,99,1,		1, 2,
												/*r*/	1,2,2,99,1,1,1,1,1,1,99,99,1,		1, 2,
												/*c*/	2,3,3,99,1,1,1,1,1,1,99,99,1,		1, 3,
												/*R*/	2,3,3,99,1,1,1,1,1,1,99,99,1,		1, 3,
												/*T*/	1,2,2,99,1,1,1,1,1,1,99,99,1,		1, 2,
												/*A*/	1,2,2,99,1,1,1,1,1,1,99,99,1,		1, 2,
												/*v*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,		1, 1,
												/*h*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,		1, 1,
												/*P*/	1,2,2,99,1,1,1,1,1,1,99,99,1,		1, 2,
												/*f*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,		1, 1,
												/*b*/	1,1,1,1, 1,1,1,1,1,1,1, 1, 1,		1, 1,	
												/*L*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,1,
												/*B*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,1,
												/*C*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,1,
												/*G*/	99,99,99,99,99,99,99,99,99,99,1,1,99,1,1,
												/*U*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,1,
												/*V*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,1 };




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

		//Basic image init below
		//This is "plains"
		//Fog of war tile will always be immediately below the normal tile.
		textureRectAnchorX = 0;
		textureRectAnchorY = 0;
		mySprite.setTextureRect(rectArray[textureRectAnchorX][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX][textureRectAnchorY +1]);
		
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
			return -1;
		}

		int answer = -1;
		answer = MOVE_VALUES_MATRIX[x][y];
		return answer;
	
	}

	bool checkForProperty( char terrainType)
	{
		bool isProperty = false;

		//Non property included for clarity.
		switch (terrainType)
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

	int determineRiverRoadType(bool thisTileChanged, MasterBoard* boardToSet);
	int determineSeaType(bool thisTileChanged, MasterBoard* boardToSet);
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

	//GRAPHICS ///////////////////////////
	sf::Sprite mySprite;
	sf::Sprite myFogSprite;
	sf::Texture* myTexture;
	//AnchorX and Y track the location of the "base" tile ie. neutral.
	//Only matters if sprite may change (Properties and roads/rivers)
	//Fog of war tile will always be immediately below the normal tile.
	int textureRectAnchorY;
	int textureRectAnchorX;
	//GRAPHICS ///////////////////////////

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

	int setCharacterstics(sf::Texture * inputTexture, MasterBoard* boardToSet)
	{
		myTexture = inputTexture;
		mySprite.setTexture(*myTexture);
		myFogSprite.setTexture(*myTexture);

		//Sprite had defualt texture-rect set in the constructor
		
		switch (symbol)
		{
		case('.'):
		{	
			description = "Clear terrain.";
			defenseFactor = 1.1;
			mySprite.setTextureRect(rectArray[0][0]);
			myFogSprite.setTextureRect(rectArray[0][1]);
			break;
		}
		case('H'):
		{
			description = "City.";
			defenseFactor = 1.3;
			production = 2000;

			textureRectAnchorX = 12;
			textureRectAnchorY = 0;

			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY + 1]);
			break;
		}
		case('m'):
		{
			description = "Mine.";
			defenseFactor = 1.2;
			production = 3000;

			break;
		}
		case('n'):
		{
			description = "Settlement.";
			defenseFactor = 1.3;
			production = 1000;

			textureRectAnchorX = 7;
			textureRectAnchorY = 0;
			
			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller ][textureRectAnchorY + 1]);
			break;
		}
		case('h'):
		{
			description = "Factory.";
			defenseFactor = 1.3;
			production = 1000;
			textureRectAnchorX = 17;
			textureRectAnchorY = 0;

			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY + 1]);
			
			break;
		}
		case('Q'):
		{
			description = "Headquarters.";
			defenseFactor = 1.4;
			production = 1000;
			textureRectAnchorX = 22;
			textureRectAnchorY = 0;

			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY + 1]);
			break;
		}
		case('='):
		{
			description = "Road.";
			defenseFactor = 1.0;

			textureRectAnchorY = 2;
			textureRectAnchorX = 20;

			mySprite.setTextureRect(rectArray[20][2]);
			myFogSprite.setTextureRect(rectArray[20][3]);

			determineRiverRoadType(true, boardToSet);

			break;
		}
		case('^'):
		{
			description = "Hill.";
			defenseFactor = 1.1;
			mySprite.setTextureRect(rectArray[2][0]);
			myFogSprite.setTextureRect(rectArray[2][1]);
			break;
		}
		case('M'):
		{
			description = "Mountain.";
			defenseFactor = 1.4;
			mySprite.setTextureRect(rectArray[3][0]);
			myFogSprite.setTextureRect(rectArray[3][1]);
			break;
		}
		case('+'):		//Would like to have convertible to woodlot by engineer.....maybe
		{
			description = "Forest.";
			defenseFactor = 1.2;
			mySprite.setTextureRect(rectArray[1][0]);
			myFogSprite.setTextureRect(rectArray[1][1]);
			break;
		}
		case('~'):
		{
			description = "High seas.";
			defenseFactor = 1.0;

			textureRectAnchorY = 9;
			textureRectAnchorX = 0;

			mySprite.setTextureRect(rectArray[0][9]);
			myFogSprite.setTextureRect(rectArray[0][10]);

			determineSeaType(true, boardToSet);

			break;
		}

		case('-'):
		{
			description = "River.";
			defenseFactor = 1.0;

			mySprite.setTextureRect(rectArray[9][2]);
			myFogSprite.setTextureRect(rectArray[9][3]);

			textureRectAnchorY = 2;
			textureRectAnchorX = 9;

			determineRiverRoadType( true, boardToSet);
			break;
		}
		case('A'):
		{
			description = "Airbase.";
			defenseFactor = 1.3;
			
			production = 1000;
			break;
		}
		case('P'):
		{
			description = "Port.";
			defenseFactor = 1.3;
			
			production = 1000;
			break;
		}
		case('\''):
		{
			description = "Beach.";
			defenseFactor = 1.0;
			
			break;
		}
		}
		//Other terrain types go here
	
		return 0;
	}


};

#endif /* TILE_H__ */

