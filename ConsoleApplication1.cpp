#include <iostream>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>

//BEGIN CODE REQUIRED FOR GRAPHICS
//#include "stdafx.h"
//#include <gdiplus.h>
#include <windows.h>
//#include <objidl.h>
//using namespace Gdiplus;
//#pragma comment (lib,"Gdiplus.lib")
//END CODE REQUIRED FOR GRAPHICS
#elif defined(__unix__)
#include <iostream>
#include <stdio.h>
#define SetConsoleTextAttribute(X,ATTR)
#define GetStdHandle(x)
#define STD_OUTPUT_HANDLE
#define _getch() getc(stdin)
#else
#error "Unknown build target!"
#endif


#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "Minion.hpp"

using namespace std;

//This is an edit.

char Input					= ' ';	
class Minion;
int turnFlag				= 1;
string eventText = "";

Cursor cursor;
MasterBoard GameBoard;
Minion minionRoster[GLOBALSUPPLYCAP];

int printScreen(MasterBoard * boardToPrint) 
{
	
	int i, j;
	
	int y = GameBoard.windowLocation / BOARD_WIDTH;
	int x = GameBoard.windowLocation % BOARD_WIDTH;


	for (i = y; i < y + WINDOW_HEIGHT; i++)
	{
		for (j = x; j < x + WINDOW_WIDTH; j++)
		{
			if (i * BOARD_WIDTH + j == cursor.Location)										//Print whomever has "priority. cursor first, then unit, then terrain."
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Is there a cursor there?
				cout << '+';
			}
			else if (boardToPrint->Board[i * BOARD_WIDTH + j].hasMinionOnTop == true)						//Is there a minion there?
			{
				switch (boardToPrint->Board[i * BOARD_WIDTH + j].minionOnTop->team)							//Determine team and then set the color.
				{
				case(0):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
					break;
				case(1):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
					break;
				case(2):
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
					break;
				}

				cout << boardToPrint->Board[i * BOARD_WIDTH + j].minionOnTop->type;							//Print out the minion.
			}
			else if (boardToPrint->Board[i * BOARD_WIDTH + j].withinRange == true)								
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//If no minion, see if it's "in range" is set
				cout << ':';																				//And if so print the symbol for "in range"
			}\
			else 
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Otherwise put out the terrain for that square.
				cout << boardToPrint->Board[i * BOARD_WIDTH + j].symbol; 
			}
		}
		cout << endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	cout << "Player " << turnFlag << "'s Turn." << endl;
	cout << boardToPrint->Board[cursor.Location].description<<endl;				//Print out description of minion and terrain.
	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
		cout << "Player " << boardToPrint->Board[cursor.Location].minionOnTop->team << "'s " << boardToPrint->Board[cursor.Location].minionOnTop->description << ": "<<boardToPrint->Board[cursor.Location].minionOnTop->health <<" Health Left." << endl;
	else cout << endl;

	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[cursor.Location].minionOnTop->hasMoved == true)
			cout << "Has moved this turn." << endl;
		else cout << "Ready to move." << endl;
	}
	else cout << endl;
	
	if (boardToPrint->Board[cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[cursor.Location].minionOnTop->hasAttacked == true)
			cout << "Has attacked this turn." << endl;
		else
			if (boardToPrint->Board[cursor.Location].minionOnTop->artilleryCanAttack == true || boardToPrint->Board[cursor.Location].minionOnTop->type != 'R' || boardToPrint->Board[cursor.Location].minionOnTop->type != 'A')
				cout << "Ready to attack." << endl;
			else cout << "Cannot attack." << endl;
	
	}

	else cout << endl;
	cout << eventText << "Cursor Location: " << cursor.getX()<<cursor.getY()<<endl;
	cout << GameBoard.windowLocation<<endl;
	eventText = "";
	for (i = 0; i < 10; i++)
		cout << endl;

	return 0;
}

int userInput(char * Input, MasterBoard * boardToInput)		
{
		*Input = _getch();
	if (*Input == 'a')											//Take user input and move cursor around.
		if (cursor.getX() != 0)
		{
			cursor.Location--;
		}

	if (*Input == 'd')
		if (cursor.getX() != BOARD_WIDTH-1)
		{
			cursor.Location++;
		}

	if (*Input == 's')
		if (cursor.getY() < BOARD_HEIGHT-1)
		{
			cursor.Location += BOARD_WIDTH;
		}

	if (*Input == 'w')
		if (cursor. getY() > 0)
		{
			cursor.Location -= BOARD_WIDTH;
		}

	

	if (*Input == 't')
		if (cursor.selectMinionFlag == false && GameBoard.Board[cursor.Location].hasMinionOnTop == true)
			GameBoard.selectMinion(cursor.Location);
		else if(cursor.selectMinionFlag == true) GameBoard.deselectMinion();

	if (*Input == 'm' && cursor.selectMinionFlag == true && GameBoard.Board[cursor.Location].hasMinionOnTop == false)		//If not on top, then move the unit.
	{
		if (GameBoard.moveMinion(cursor.Location) == 0)
		{
			cursor.selectMinionPointer->artilleryCanAttack = false;			//Successful movement means artillery cannot fire this turn.
			GameBoard.deselectMinion();
		}
	}

	if(*Input == 'm' && cursor.selectMinionFlag == true && cursor.selectMinionPointer->Location == cursor.Location)			//If already on top, just "move" by not moving. This allows the user to fire without actually changing position.
	{
		cursor.selectMinionPointer->hasMoved = true;
		GameBoard.deselectMinion();
	}

	if (*Input == 'r' && cursor.selectMinionFlag == true && GameBoard.Board[cursor.Location].hasMinionOnTop == true			//Can attack if minion is selected
		&& cursor.Location != cursor.selectMinionPointer->Location															//And you're not targeting your selected minion
		&& GameBoard.Board[cursor.Location].minionOnTop->team != cursor.selectMinionPointer->team							//And it's enemy team's.
		&& GameBoard.Board[cursor.Location].withinRange == true)															//And it's within range.
	{
		if(GameBoard.attackMinion(cursor.Location) == 0)
			GameBoard.deselectMinion();
	}

	if (*Input == 'p')									//Ends the turn and passes it to the next player.
	{
		if (cursor.selectMinionFlag == true)
			GameBoard.deselectMinion();
		GameBoard.endTurn();
	}

	GameBoard.checkWindow();

	return 0;
}

int main()
{
	GameBoard.createMinion('T', 17, 1);
	GameBoard.createMinion('T', 07, 1);
	GameBoard.createMinion('C', 07, 1);

	GameBoard.createMinion('i', 46, 2);
	GameBoard.createMinion('i', 47, 2);
	GameBoard.createMinion('i', 48, 2);
	GameBoard.createMinion('R', 87, 2);
	
	printScreen(&GameBoard);
	for (int i = 0; i > -1; i++)		//Run as long as the user wants. Infinite for loop.
	{
		userInput(&Input, &GameBoard);
		printScreen(&GameBoard);
	}

}


