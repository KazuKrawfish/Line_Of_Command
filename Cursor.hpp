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
	bool selectMinionFlag = false;
	Minion*  selectMinionPointer;
	int Location;
	int getX() { return Location % BOARD_WIDTH; }
	int getY() { return Location / BOARD_WIDTH; }
};

#endif /* CURSOR_HPP__ */

