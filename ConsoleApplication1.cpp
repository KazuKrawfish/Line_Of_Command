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

int printScreen(MasterBoard * boardToPrint) 
{
	
	int i, j;
	
	int y = boardToPrint->windowLocation / BOARD_WIDTH;
	int x = boardToPrint->windowLocation % BOARD_WIDTH;


	for (i = y; i < y + WINDOW_HEIGHT; i++)
	{
		for (j = x; j < x + WINDOW_WIDTH; j++)
		{
			if (i * BOARD_WIDTH + j == boardToPrint->cursor.Location)										//Print whomever has priority. cursor first, then unit, then terrain.
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Is there a cursor there?
				std::cout << '+';
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

				std::cout << boardToPrint->Board[i * BOARD_WIDTH + j].minionOnTop->type;							//Print out the minion.
			}
			else if (boardToPrint->Board[i * BOARD_WIDTH + j].withinRange == true)								
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//If no minion, see if it's "in range" is set
				std::cout << ':';																				//And if so print the symbol for "in range"
			}\
			else 
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);					//Otherwise put out the terrain for that square.
				std::cout << boardToPrint->Board[i * BOARD_WIDTH + j].symbol; 
			}
		}
		std::cout << std::endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	std::cout << "Player " << boardToPrint->newTurnFlag << "'s Turn." << std::endl;

	//Print out description of minion and terrain.
	std::cout << boardToPrint->Board[boardToPrint->cursor.Location].description<<std::endl;				
	if (boardToPrint->Board[boardToPrint->cursor.Location].hasMinionOnTop == true)
		std::cout << "Player " << boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->team << "'s " << boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->description << ": "<<boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->health <<" Health Left." << std::endl;
	else std::cout << std::endl;

	if (boardToPrint->Board[boardToPrint->cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->hasMoved == true)
			std::cout << "Has moved this turn." << std::endl;
		else std::cout << "Ready to move." << std::endl;
	}
	else std::cout << std::endl;
	
	if (boardToPrint->Board[boardToPrint->cursor.Location].hasMinionOnTop == true)
	{
		if (boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->hasAttacked == true)
			std::cout << "Has attacked this turn." << std::endl;
		else
			if (boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->artilleryCanAttack == true || boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->type != 'R' || boardToPrint->Board[boardToPrint->cursor.Location].minionOnTop->type != 'A')
				std::cout << "Ready to attack." << std::endl;
			else std::cout << "Cannot attack." << std::endl;
	
	}

	else std::cout << std::endl;
	std::cout << eventText << "Cursor Location: " << boardToPrint->cursor.getX()<< boardToPrint->cursor.getY()<<std::endl;
	std::cout << boardToPrint->windowLocation<<std::endl;
	eventText = "";

	//Buffer the screen to clear the old map window.
	for (i = 0; i < 10; i++)		
		std::cout << std::endl;

	return 0;
}

int userInput(char * Input, MasterBoard * boardToInput)		
{
		*Input = _getch();
	if (*Input == 'a')											//Take user input and move cursor around.
		if (boardToInput->cursor.getX() != 0)
		{
			boardToInput->cursor.Location--;
		}

	if (*Input == 'd')
		if (boardToInput->cursor.getX() != BOARD_WIDTH-1)
		{
			boardToInput->cursor.Location++;
		}

	if (*Input == 's')
		if (boardToInput->cursor.getY() < BOARD_HEIGHT-1)
		{
			boardToInput->cursor.Location += BOARD_WIDTH;
		}

	if (*Input == 'w')
		if (boardToInput->cursor. getY() > 0)
		{
			boardToInput->cursor.Location -= BOARD_WIDTH;
		}

	
	//Select minion command
	if (*Input == 't')	
		if (boardToInput->cursor.selectMinionFlag == false && boardToInput->Board[boardToInput->cursor.Location].hasMinionOnTop == true)
			boardToInput->selectMinion(boardToInput->cursor.Location);
		else if(boardToInput->cursor.selectMinionFlag == true) boardToInput->deselectMinion();

	//Move minion command
	if (*Input == 'm' && boardToInput->cursor.selectMinionFlag == true && boardToInput->Board[boardToInput->cursor.Location].hasMinionOnTop == false)		//If not on top, then move the unit.
	{
		if (boardToInput->moveMinion(boardToInput->cursor.Location) == 0)
		{
			boardToInput->cursor.selectMinionPointer->artilleryCanAttack = false;			//Successful movement means artillery cannot fire this turn.
			boardToInput->deselectMinion();
		}
	}

	//If already on top, just "move" by not moving. This allows the user to fire without actually changing position.
	if(*Input == 'm' && boardToInput->cursor.selectMinionFlag == true && boardToInput->cursor.selectMinionPointer->Location == boardToInput->cursor.Location)			
	{
		boardToInput->cursor.selectMinionPointer->hasMoved = true;
		boardToInput->deselectMinion();
	}

	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true && boardToInput->Board[boardToInput->cursor.Location].hasMinionOnTop == true			//Can attack if minion is selected
		&& boardToInput->cursor.Location != boardToInput->cursor.selectMinionPointer->Location															//And you're not targeting your selected minion
		&& boardToInput->Board[boardToInput->cursor.Location].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team							//And it's enemy team's.
		&& boardToInput->Board[boardToInput->cursor.Location].withinRange == true)															//And it's within range.
	{
		if(boardToInput->attackMinion(boardToInput->cursor.Location) == 0)
			boardToInput->deselectMinion();
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
			<< minionRoster[i]->Location
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


