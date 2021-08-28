#include "Cursor.hpp"
#include "MasterBoard.hpp"


int Cursor::move(char* Input)
{
int didItMove = 1;

//Take user input and move cursor around.

if (*Input == 'a')
if (getX() > 0)
{
	XCoord--;
	didItMove = 0;
}

if (*Input == 'd')
if (getX() < boardToPlay->BOARD_WIDTH - 1)
{
	XCoord++;
	didItMove = 0;
}

if (*Input == 's')
if (getY() < boardToPlay->BOARD_HEIGHT - 1)
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

int Cursor::relocate(int inputX, int inputY) //Returns 1 on fail
{
	if (inputX < boardToPlay->BOARD_WIDTH - 1 && inputX > 0)
	{
		XCoord = inputX;
	}
	else return 1;

	if (inputY < boardToPlay->BOARD_HEIGHT - 1 && inputY > 0)
	{
		YCoord = inputY;
	}
	else return 1;

	return 0;
}