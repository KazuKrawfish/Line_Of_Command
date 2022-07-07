//Copyright 2022, Supercontinent Software Ltd.
//
//	tile.hpp
//
/*
The Tile class is the basic unit of terrain that makes up the gameboard. It contains data like terrain type, movement costs, defense bonuses, etc.
*/

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
//												        . + ^ M  H m n h Q = ~  - A			P *
const int MOVE_VALUES_MATRIX[21][15] =			/*i*/  {1,1,2,3, 1,1,1,1,1,1,99,2,1,		1, 1,
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
												/*B*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,99,
												/*C*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,99,
												/*G*/	99,99,99,99,99,99,99,99,99,99,1,1,99,1,1,
												/*U*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,99,
												/*V*/	99,99,99,99,99,99,99,99,99,99,1,99,99,1,99 ,
												/*K*/	1,1,2,3, 1,1,1,1,1,1,99,2,1,		1, 1,
												/*S*/	1,1,2,3, 1,1,1,1,1,1,99,2,1,		1, 1 };




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

	//Takes a given minion and terrain type, consults the movement chart, and returns the cost to move on this particular tile.
	int consultMovementChart(std::string minionType, char terrainType);
	
	bool checkForProperty(char terrainType);
	

	int determineRiverRoadType(bool thisTileChanged, MasterBoard* boardToSet);
	int determineSeaBeachType(bool thisTileChanged, MasterBoard* boardToSet);
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
	sf::Sprite * animationSprite = NULL;
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

			determineSeaBeachType(true, boardToSet);

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
			
			textureRectAnchorX = 32	;
			textureRectAnchorY = 0;

			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY + 1]);

			production = 1000;
			break;
		}
		case('P'):
		{
			description = "Port.";
			defenseFactor = 1.3;
			
			textureRectAnchorX = 27;
			textureRectAnchorY = 0;

			mySprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY]);
			myFogSprite.setTextureRect(rectArray[textureRectAnchorX + controller][textureRectAnchorY + 1]);

			production = 1000;
			break;
		}
		case('*'):
		{
			description = "Beach.";
			defenseFactor = 1.0;

			textureRectAnchorY = 11;
			textureRectAnchorX = 0;

			mySprite.setTextureRect(rectArray[0][11]);
			myFogSprite.setTextureRect(rectArray[0][12]);

			determineSeaBeachType(true, boardToSet);

			break;
		}
		}
		//Other terrain types go here
	
		return 0;
	}


};

#endif /* TILE_H__ */

