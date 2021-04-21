#ifndef CURSOR_HPP__
#define CURSOR_HPP__

#include "Minion.hpp"

//Window HAS been implemented to minimize total print size!!
const int WINDOW_HEIGHT = 5;										
const int WINDOW_WIDTH = 5;
const int BOARD_WIDTH = 20;  
const int BOARD_HEIGHT = 20;

class Cursor 
{	public:

	//Obviously this needs to move to cursor.cpp but I can't get the thing to work yet.
	int move(char * Input) {
		int didItMove =1 ;

		//Take user input and move cursor around.

		if (*Input == 'a')
			if (getX() != 0)
			{
				XCoord--;
				didItMove = 0;
			}

		if (*Input == 'd')
			if (getX() != BOARD_WIDTH - 1)
			{
				XCoord++;
				didItMove = 0;
			}

		if (*Input == 's')
			if (getY() < BOARD_HEIGHT - 1)
			{
				YCoord++;
				didItMove = 0;
			}

		if (*Input == 'w')
			if (getY() > 0)
			{
				YCoord--;
				didItMove = 0;
			}
		return didItMove;
	}


	bool selectMinionFlag = false;
	Minion*  selectMinionPointer;
	//int Location;
	int XCoord;
	int YCoord;
	int getX() { return XCoord; }				// { return Location % BOARD_WIDTH; }
	int getY() { return YCoord; }				// (return Location	 / BOARD_WIDTH; }
};

#endif /* CURSOR_HPP__ */

