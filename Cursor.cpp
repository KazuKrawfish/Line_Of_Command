//Copyright 2022, Supercontinent Software Ltd.
//
//	Cursor.cpp
//

#include "Cursor.hpp"
#include "MasterBoard.hpp"


int Cursor::initialize(sf::Texture* inputTexture, MasterBoard* inputBoardToPlay)
{
	XCoord = 1;
	YCoord = 1;
	boardToPlay = inputBoardToPlay;
	myTexture = inputTexture;
	
	//Basic image init below
	//This is the cursor image, it should never change.
	mySprite.setTexture(*inputTexture);
	mySprite.setTextureRect(rectArray[0][2]);

	return 0;
}

int Cursor::move(sf::Keyboard::Key* Input)
{
int didItMove = 1;

//Take user input and move cursor around.

if (*Input == sf::Keyboard::Key::A)
if (getX() > 0)
{
	XCoord--;
	didItMove = 0;
}

if (*Input == sf::Keyboard::Key::D)
if (getX() < boardToPlay->BOARD_WIDTH - 1)
{
	XCoord++;
	didItMove = 0;
}

if (*Input == sf::Keyboard::Key::S)
if (getY() < boardToPlay->BOARD_HEIGHT - 1)
{
	YCoord++;
	didItMove = 0;
}

if (*Input == sf::Keyboard::Key::W)
if (getY() > 0)
{
	YCoord--;
	didItMove = 0;
}

boardToPlay->setCursorPath(true, XCoord, YCoord);
boardToPlay->checkWindow();

return didItMove;
	
}

int Cursor::relocate(int inputX, int inputY) //Returns 1 on fail
{
	if (inputX < boardToPlay->BOARD_WIDTH  && inputX >= 0)
	{
		XCoord = inputX;
	}
	else return 1;

	if (inputY < boardToPlay->BOARD_HEIGHT  && inputY >= 0)
	{
		YCoord = inputY;
	}
	else return 1;

	boardToPlay->setCursorPath(true, XCoord, YCoord);

	return 0;
}