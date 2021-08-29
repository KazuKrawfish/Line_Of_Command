#ifndef CURSOR_HPP__
#define CURSOR_HPP__

#include "Minion.hpp"


//Window HAS been implemented to minimize total print size!!
//Needs to be anti-consted
const int WINDOW_HEIGHT = 10;										
const int WINDOW_WIDTH = 12;

class MasterBoard;

class Cursor 
{	public:

	//Obviously this needs to move to cursor.cpp but I can't get the thing to work yet.
	int move(char* Input);


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

