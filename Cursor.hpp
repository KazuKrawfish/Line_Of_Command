#ifndef CURSOR_HPP__
#define CURSOR_HPP__

#include "Minion.hpp"

//Implemenent "window" to follow cursor and minimize print size
const int WINDOW_HEIGHT = 5;										//Still need to be eliminated as globals
const int WINDOW_WIDTH = 5;
const int BOARD_WIDTH = 20;  //Size of total board
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

