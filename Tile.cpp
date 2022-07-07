//Copyright 2022, Supercontinent Software Ltd.
//
//	tile.cpp
//
/*	
The Tile class is the basic unit of terrain that makes up the gameboard. It contains data like terrain type, movement costs, defense bonuses, etc.
*/

#include "Tile.hpp"
#include "MasterBoard.hpp"


int tile::determineRiverRoadType( bool thisTileChanged,   MasterBoard* boardToSet)
{
	bool upNeigh = false;
	bool downNeigh = false;
	bool leftNeigh = false;
	bool rightNeigh = false;

	tile* thisTile =  & boardToSet->Board[locationX][locationY];

	//Check each neighbor to see if they are same type
	//If they are, note it, and call determineRiverRoadType on that tile.
	if (locationY + 1 >= boardToSet->BOARD_HEIGHT
		|| (boardToSet->Board[locationX][locationY + 1].symbol == symbol) 
		)
	{ 
		downNeigh = true;
		if(locationY + 1 < boardToSet->BOARD_HEIGHT && thisTileChanged == true)
			boardToSet->Board[locationX][locationY + 1].determineRiverRoadType(false, boardToSet);
	}
	if( locationY < boardToSet->BOARD_HEIGHT - 1  
		&& (  (boardToSet->Board[locationX][locationY + 1].symbol == '~' || boardToSet->Board[locationX][locationY + 1].symbol == '*')
			&& thisTile->symbol == '-'))
	{
		downNeigh = true;
	}

	if (locationY <= 0
		|| (boardToSet->Board[locationX][locationY - 1].symbol == symbol)
		)
	{
		upNeigh = true;
		if (locationY > 0 && thisTileChanged == true)
			boardToSet->Board[locationX][locationY - 1].determineRiverRoadType( false, boardToSet);
	}
	if (locationY > 0 
		&& ((boardToSet->Board[locationX][locationY - 1].symbol == '~' || boardToSet->Board[locationX][locationY - 1].symbol == '*')
			&& thisTile->symbol == '-'))
	{
		upNeigh = true;
	}

	if (locationX <= 0
		|| (boardToSet->Board[locationX - 1][locationY].symbol == symbol)
		)
	{
		leftNeigh = true;
		if (locationX > 0 && thisTileChanged == true)
			boardToSet->Board[locationX - 1][locationY].determineRiverRoadType( false, boardToSet);
	}
	if (locationX > 0 
		&& ((boardToSet->Board[locationX - 1 ][locationY ].symbol == '~' || boardToSet->Board[locationX- 1][locationY ].symbol == '*') 
			&& thisTile->symbol == '-'))
	{
		leftNeigh = true;
	}

	if (locationX + 1 >= boardToSet->BOARD_WIDTH
		|| (boardToSet->Board[locationX + 1][locationY].symbol == symbol)
		)
	{
		rightNeigh = true;
		if (locationX + 1 < boardToSet->BOARD_WIDTH && thisTileChanged == true)
			boardToSet->Board[locationX + 1][locationY].determineRiverRoadType(false , boardToSet);
	}
	if (locationX < boardToSet->BOARD_WIDTH - 1 && (
		(boardToSet->Board[locationX + 1 ][locationY ].symbol == '~' || boardToSet->Board[locationX + 1][locationY ].symbol == '*') 
		&& thisTile->symbol == '-'))
	{
		rightNeigh = true;
	}

	//Change sprite rect based on selections above
	//Default is up-down
	//Sideways
	if (downNeigh == false && upNeigh == false && ( leftNeigh == true ||  rightNeigh == true) )
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 1][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 1][textureRectAnchorY + 1]);

	}

	//Up-down
	if ( (downNeigh == true || upNeigh == true ) && leftNeigh == false && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX ][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX ][textureRectAnchorY + 1]);

	}

	//Four way
	if (downNeigh == true && upNeigh == true && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 2][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 2][textureRectAnchorY + 1]);

	}

	//3 Way
	if (downNeigh == true && upNeigh == true && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 3][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 3][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == true && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 4][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 4][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == true && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 5][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 5][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 6][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 6][textureRectAnchorY + 1]);

	}

	//Elbow shape
	if (downNeigh == false && upNeigh == true && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 7][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 7][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 8][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 8][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 9][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 9][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == true && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 10][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 10][textureRectAnchorY + 1]);

	}

	//Then determine new sprites for neighbors

	return 0;

}


int tile::determineSeaBeachType(bool thisTileChanged, MasterBoard* boardToSet)
{
	bool upNeigh = false;
	bool downNeigh = false;
	bool leftNeigh = false;
	bool rightNeigh = false;

	//Check each neighbor to see if they are same type
	//If they are, note it, and call determineSeaBeachType on that tile.
	if (locationY + 1 >= boardToSet->BOARD_HEIGHT
		|| (boardToSet->Board[locationX][locationY + 1].symbol == '~' || boardToSet->Board[locationX][locationY + 1].symbol == '*' ))
	{
		downNeigh = true;
		if (locationY + 1 < boardToSet->BOARD_HEIGHT && thisTileChanged == true)
			boardToSet->Board[locationX][locationY + 1].determineSeaBeachType(false, boardToSet);
	}
	if (locationY <= 0
		|| (boardToSet->Board[locationX][locationY - 1].symbol == '~' || boardToSet->Board[locationX][locationY - 1].symbol == '*' ))
	{
		upNeigh = true;
		if (locationY > 0 && thisTileChanged == true)
			boardToSet->Board[locationX][locationY - 1].determineSeaBeachType(false, boardToSet);
	}
	if (locationX <= 0
		|| (boardToSet->Board[locationX - 1][locationY ].symbol == '~' || boardToSet->Board[locationX - 1][locationY ].symbol == '*' ))
	{
		leftNeigh = true;
		if (locationX > 0 && thisTileChanged == true)
			boardToSet->Board[locationX - 1][locationY].determineSeaBeachType(false, boardToSet);
	}
	if (locationX + 1 >= boardToSet->BOARD_WIDTH
		|| (boardToSet->Board[locationX + 1][locationY].symbol == '~' || boardToSet->Board[locationX + 1][locationY].symbol == '*' ))
	{
		rightNeigh = true;
		if (locationX + 1 < boardToSet->BOARD_WIDTH && thisTileChanged == true)
			boardToSet->Board[locationX + 1][locationY].determineSeaBeachType(false, boardToSet);
	}

	
	//Change sprite rect based on selections above
	//Default is up-down
	//Sideways13 
	if (downNeigh == false && upNeigh == false && (leftNeigh == true || rightNeigh == true))
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 13][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 13][textureRectAnchorY + 1]);

	}

	//Up-down14
	if (downNeigh == true && upNeigh == true && leftNeigh == false && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 14][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX+14][textureRectAnchorY + 1]);

	}

	//Four way
	if (downNeigh == true && upNeigh == true && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 15][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 15][textureRectAnchorY + 1]);

	}

	//Pond
	if (downNeigh == false && upNeigh == false && leftNeigh == false && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 8][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 8][textureRectAnchorY + 1]);

	}

	//3 Way
	if (downNeigh == true && upNeigh == true && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 1][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 1][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == true && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 2][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 2][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == true && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 3][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 3][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == true && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX ][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX ][textureRectAnchorY + 1]);

	}

	//Diagonals
	if (downNeigh == false && upNeigh == true && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 6][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 6][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 7][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 7][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 4][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 4][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == true && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 5][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 5][textureRectAnchorY + 1]);

	}

	//Bays
	if (downNeigh == false && upNeigh == false && leftNeigh == false && rightNeigh == true)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 11][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 11][textureRectAnchorY + 1]);

	}
	if (downNeigh == true && upNeigh == false && leftNeigh == false && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 12][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 12][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == false && leftNeigh == true && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 9][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 9][textureRectAnchorY + 1]);

	}
	if (downNeigh == false && upNeigh == true && leftNeigh == false && rightNeigh == false)
	{
		mySprite.setTextureRect(rectArray[textureRectAnchorX + 10][textureRectAnchorY]);
		myFogSprite.setTextureRect(rectArray[textureRectAnchorX + 10][textureRectAnchorY + 1]);

	}


	return 0;

}


int tile::consultMovementChart(std::string minionType, char terrainType)
{

	int x = -1;
	int y = -1;

	if ( minionType == "Infantry")
		x = 0;
	else
	if ( minionType == "Specialist")
		x = 1;
	else
	if ( minionType == "Armor")
		x = 2;
	else
	if ( minionType == "Artillery")
		x = 3;
	else
	if ( minionType == "Recon")
		x = 4;
	else
	if ( minionType == "Rocket_Artillery")
		x = 5;
	else
	if ( minionType == "Heavy_Armor")
		x = 6;
	else
	if ( minionType == "Anti-Aircraft")
		x = 7;
	else
	if ( minionType == "Attack_Copter")
		x = 8;
	else
	if ( minionType == "Transport_Copter")
		x = 9;
	else
	if ( minionType == "APC")	
		x = 10;
	else
	if ( minionType == "Interceptor")
		x = 11;
	else
	if ( minionType == "Bomber")	
		x = 12;
	else
	if ( minionType == "Lander")	
		x = 13;
	else
	if ( minionType == "Battleship")	
		x = 14;
	else
	if ( minionType == "Cruiser")
		x = 15;
	else
	if ( minionType == "Gunboat")	
		x = 16;
	else
	if ( minionType == "Submarine")	
		x = 17;
	else
	if ( minionType == "Aircraft_Carrier")	
		x = 18;
	else
	if ( minionType == "Artillery_Emplacement")
		x = 19;
	else
	if ( minionType == "SAM_Site")	
		x = 20;


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
	case('*'):	//Beach
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
	


bool tile::checkForProperty(char terrainType)
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
	case('*'):
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