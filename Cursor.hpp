//Copyright 2022, Supercontinent Software Ltd.
//
//	Cursor.hpp
//

#ifndef CURSOR_HPP__
#define CURSOR_HPP__

#include "Minion.hpp"

//Max window size is the physical size of the window:
const int MAX_WINDOW_HEIGHT = 14;
const int MAX_WINDOW_WIDTH = 16;	//50*16=800 total pixel width


class MasterBoard;

class Cursor 
{	public:
	
	//Obviously this needs to move to cursor.cpp but I can't get the thing to work yet.
	int move(sf::Keyboard::Key* Input);
	int initialize(sf::Texture * inputTexture, MasterBoard* inputBoardToPlay);

	//GRAPHICS ///////////////////////////
	sf::Sprite mySprite;
	sf::Texture* myTexture;
	//GRAPHICS ///////////////////////////

	bool selectMinionFlag = false;
	Minion*  selectMinionPointer;
	MasterBoard* boardToPlay;
	
	int XCoord;
	int YCoord;
	int getX() { return XCoord; }				
	int getY() { return YCoord; }
	int relocate(int inputX, int inputY);
};

#endif /* CURSOR_HPP__ */

