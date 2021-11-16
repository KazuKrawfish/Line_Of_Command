#include "Tile.hpp"
#include "MasterBoard.hpp"


int tile::determineRiverRoadType( bool thisTileChanged,   MasterBoard* boardToSet)
{
	bool upNeigh = false;
	bool downNeigh = false;
	bool leftNeigh = false;
	bool rightNeigh = false;

	//Check each neighbor to see if they are same type
	//If they are, note it, and call determineRiverRoadType on that tile.
	if (locationY + 1 >= boardToSet->BOARD_HEIGHT
		|| (boardToSet->Board[locationX][locationY + 1].symbol == symbol))
	{
		downNeigh = true;
		if(locationY + 1 < boardToSet->BOARD_HEIGHT && thisTileChanged == true)
			boardToSet->Board[locationX][locationY + 1].determineRiverRoadType(false, boardToSet);
	}
	if (locationY <= 0
		|| (boardToSet->Board[locationX][locationY - 1].symbol == symbol))
	{
		upNeigh = true;
		if (locationY > 0 && thisTileChanged == true)
			boardToSet->Board[locationX][locationY - 1].determineRiverRoadType( false, boardToSet);
	}
	if (locationX <= 0
		|| (boardToSet->Board[locationX - 1][locationY].symbol == symbol))
	{
		leftNeigh = true;
		if (locationX > 0 && thisTileChanged == true)
			boardToSet->Board[locationX - 1][locationY].determineRiverRoadType( false, boardToSet);
	}
	if (locationX + 1 >= boardToSet->BOARD_WIDTH
		|| (boardToSet->Board[locationX + 1][locationY].symbol == symbol))
	{
		rightNeigh = true;
		if (locationX + 1 < boardToSet->BOARD_WIDTH && thisTileChanged == true)
			boardToSet->Board[locationX + 1][locationY].determineRiverRoadType(false , boardToSet);
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