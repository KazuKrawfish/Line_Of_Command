//Advance Wars Simulation
//Copyright 2021, Park Family Software Laboratory (ParkLab)


#include <iostream>
#include <ctype.h>
#include <fstream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
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
#include "Tile.hpp"
#include "Minion.hpp"

//Global variables need to be moved.

	
std::string eventText			= "";
Minion* minionRoster[GLOBALSUPPLYCAP];

int printStatus(MasterBoard* boardToPrint) 
{
	tile currentTile = boardToPrint->Board[boardToPrint->cursor.getY()][boardToPrint->cursor.getX()];

	std::cout << currentTile.description << std::endl;

	if(currentTile.hasMinionOnTop == true)
	{
		Minion* currentMinion = currentTile.minionOnTop;
		std::cout << "Player " << currentMinion->team
			<< "'s " << currentMinion->description
			<< ": " << currentMinion->health <<
			" Health Left." << std::endl;
	
		if (currentMinion->hasMoved == true)
			std::cout << "Has moved this turn." << std::endl;
		else std::cout << "Ready to move." << std::endl;

		if (currentMinion->hasAttacked == true)
			std::cout << "Has attacked this turn." << std::endl;
		else
			if (currentMinion->artilleryCanAttack == true || 
				currentMinion->type != 'R' || currentMinion->type != 'A')//Arty is a mess to fix.
				std::cout << "Ready to attack." << std::endl;
			else std::cout << "Cannot attack." << std::endl;
	}
	else
	{ 
		std::cout << std::endl << std::endl << std::endl; 
	}
	
	//Print current turn.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	std::cout << "Player " << boardToPrint->newTurnFlag << "'s Turn." << std::endl;

	//Debug purposes- print cursor location coordinates and window coordinates.
	std::cout << eventText << "Cursor Location: " << boardToPrint->cursor.getX() << boardToPrint->cursor.getY() << std::endl;
	std::cout << boardToPrint->windowLocation << std::endl;
	eventText = "";

	return 0;
}

int printScreen(MasterBoard * boardToPrint) 
{
	//These are just iterators.
	int i, j;
	

	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocation / BOARD_WIDTH;
	int windowX = boardToPrint->windowLocation % BOARD_WIDTH;

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (i = windowY; i < windowY + WINDOW_HEIGHT; i++)
	{
		for (j = windowX; j < windowX + WINDOW_WIDTH; j++)
		{
			//Print whomever has priority. cursor first, then unit, then terrain.

			//If there is a cursor there, it takes priority for printing.
			if ( i == boardToPrint->cursor.getY() && j == boardToPrint->cursor.getX() ) // * BOARD_WIDTH + j == boardToPrint->cursor.Location)										
			{	
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					
				std::cout << '+';
			}	
			else
				//Is there a minion there?
				if (boardToPrint->Board[i][j].hasMinionOnTop == true)						
			{	
				//Determine team and then set the color.
				switch (boardToPrint->Board[i][j].minionOnTop->team)
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
				//Print out the minion.
				std::cout << boardToPrint->Board[i][j].minionOnTop->type;
			}
			else if (boardToPrint->Board[i][j].withinRange == true)
			{	
				//If no minion, see if it's "in range" is set
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					
				//And if so print the symbol for "in range" which is ':'
				std::cout << ':';																				
			}
			else 
			{
				//Otherwise put out the terrain for that square.
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					
				std::cout << boardToPrint->Board[i][j].symbol;
			}
		}
		std::cout << std::endl;
	}

	printStatus(boardToPrint);

	//Buffer the screen to clear the old map window.
	for (i = 0; i < 10; i++)		
		std::cout << std::endl;

	return 0;
}

//Needs serious refactoring. Chains of if statements that can probably be handled differently, plus multiple references to variables.
//Handle with a single local variable doing one reference.
int userInput(char * Input, MasterBoard * boardToInput)		
{
		*Input = _getch();
	if (*Input == 'a')											//Take user input and move cursor around.
		if (boardToInput->cursor.getX() != 0)
		{
			boardToInput->cursor.XCoord--;
		}

	if (*Input == 'd')
		if (boardToInput->cursor.getX() != BOARD_WIDTH-1)
		{
			boardToInput->cursor.XCoord++;
		}

	if (*Input == 's')
		if (boardToInput->cursor.getY() < BOARD_HEIGHT-1)
		{
			boardToInput->cursor.YCoord++;
		}

	if (*Input == 'w')
		if (boardToInput->cursor. getY() > 0)
		{
			boardToInput->cursor.YCoord--;
		}

	
	//Select minion command
	if (*Input == 't')	
		if (boardToInput->cursor.selectMinionFlag == false && boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
		{
			boardToInput->selectMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY());
		}
		else 
			if (boardToInput->cursor.selectMinionFlag == true)
			{
				boardToInput->deselectMinion();
			}

	//Move minion command
	//If not on top, then move the unit.
	if (*Input == 'm' && boardToInput->cursor.selectMinionFlag == true && boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false)
	{
		if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{
			boardToInput->cursor.selectMinionPointer->artilleryCanAttack = false;			//Successful movement means artillery cannot fire this turn.
			boardToInput->deselectMinion();
		}
	}

	//If already on top, just "move" by not moving. This allows the user to fire without actually changing position.
	if(*Input == 'm' && boardToInput->cursor.selectMinionFlag == true && boardToInput->cursor.selectMinionPointer->locationX == boardToInput->cursor.getX() && boardToInput->cursor.selectMinionPointer->locationY == boardToInput->cursor.getY())
	{
		boardToInput->cursor.selectMinionPointer->hasMoved = true;
		boardToInput->deselectMinion();
	}

	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true && boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true			//Can attack if minion is selected
		&& boardToInput->cursor.getX() != boardToInput->cursor.selectMinionPointer->locationX															//And you're not targeting your selected minion
		&& boardToInput->cursor.getY() != boardToInput->cursor.selectMinionPointer->locationY
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team							//And it's enemy team's.
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true)															//And it's within range.
	{	
		//This is the actual attack portion. Return of 0 indicates successful attack.
		bool attackSuccess = boardToInput->attackMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY());
		if (attackSuccess == 0)
		{
			boardToInput->deselectMinion();
		}
	}

	if (*Input == 'p')									//Ends the turn and passes it to the next player.
	{
		if (boardToInput->cursor.selectMinionFlag == true)
			boardToInput->deselectMinion();
		boardToInput->endTurn();
	}

	boardToInput->checkWindow();

	return 0;
}

int scenarioSave(std::string saveGameName) 
{//Still need to implement terrain saving.
	saveGameName += ".txt";
	std::ofstream saveGame(saveGameName);

	//Go through entire minionRoster and save each value associated with each minion, one line per minion.
	for (int i = 0; minionRoster[i] != NULL; i++) {
		saveGame << minionRoster[i]->type 
			<< minionRoster[i]->locationX
			<< minionRoster[i]->locationY
			<< minionRoster[i]->team
			<< int(minionRoster[i]->hasAttacked)
			<< int(minionRoster[i]->hasMoved)
			<< minionRoster[i]->health << std::endl;
	}
	saveGame.close();
	return 1;
}

//Load saved game and initialize the board with its contents.
//Still needs editing to load scenario.
int scenarioLoad(std::string scenarioName) {
	std::ifstream saveGame;
	saveGame.open(scenarioName+ ".txt");
	if (saveGame.is_open())
	{
		std::cout << "Successfully loaded!" << std::endl;
	}

	std::string line;
	while (getline(saveGame, line))
	{
		std::cout << line << '\n';
	}

saveGame.close();
	return 1;
}

int main()
{
	MasterBoard GameBoard;
	std::string scenarioToLoad = "";
	std::string saveName = "";

	//Prompt user and load scenario
	std::cout << "Choose which scenario to load (Case sensitive): " << std::endl;
	std::cin >> scenarioToLoad;
	scenarioLoad(scenarioToLoad);

	//Prompt user and save scenario. This should happen when user presses save keyword.
	std::cout << "Choose where to save your game:" << std::endl;
	std::cin >> saveName;
	scenarioSave(saveName);

	
	printScreen(&GameBoard);
	char Input = ' ';
	while (true)		//Run as long as the user wants. Infinite for loop.
	{
		userInput(&Input, &GameBoard);
		printScreen(&GameBoard);
	}

}


