//Project X
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

int scenarioLoad(MasterBoard* boardToPrint);


//These will all be wrapped into the inputLayer class soon.
std::string eventText			= "";
enum gameInputLayer { gameBoard, menu, minionAction, propertyAction};
gameInputLayer inputLayer = gameBoard;
char requestedMinionToBuy = '\n';
int unitPrice = -1;

//Messes up minions!
//Still need to add them after.
//This lumps all the terrain at the end, need to find a way to count neighbors and based on those, distribute tiles.
int scrambleMap(MasterBoard* LoadBoard) 
{
	int numberOfHillsAndMountains = BOARD_SIZE / 10;	//Actual amount you want to see.
	int mountainWeight = numberOfHillsAndMountains;		//This will change throughout function
	int numberOfForests = BOARD_SIZE / 10;	
	int forestWeight = numberOfForests;
	int plainsWeight = BOARD_SIZE - numberOfHillsAndMountains - numberOfForests;

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			LoadBoard->Board[x][y].clearTile();
			int mountainChance = (rand() % 100) * mountainWeight;         
			int forestChance = (rand() % 100) * forestWeight;
			int plainsChance = (rand() % 100) * plainsWeight;
			if (mountainChance > forestChance && mountainChance > plainsChance)
			{
				if ((rand() % 100) < 50)
				{
					LoadBoard->Board[x][y].symbol = 'M';
					LoadBoard->Board[x][y].description = "Mountain.";
					LoadBoard->Board[x][y].defenseFactor = 1.4;
				}
				else
				{
					LoadBoard->Board[x][y].symbol = '^';
					LoadBoard->Board[x][y].description = "Hill.";
					LoadBoard->Board[x][y].defenseFactor = 1.1;
				}
				mountainWeight--;
			}
			else if (forestChance > plainsChance)
			{
				LoadBoard->Board[x][y].symbol = '+';
				LoadBoard->Board[x][y].description = "Forest.";
				LoadBoard->Board[x][y].defenseFactor = 1.2;
				forestWeight--;
			}
			else plainsWeight--;
		}
	}
	return 0;
}

int setCharacteristics(MasterBoard* LoadBoard) 
{	
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			switch (LoadBoard->Board[x][y].symbol)
			{
			case('.'):
			{
				LoadBoard->Board[x][y].description = "Clear terrain.";
				LoadBoard->Board[x][y].defenseFactor = 1.1;
				break;
			}
			case('H'):
			{
				LoadBoard->Board[x][y].description = "City.";
				LoadBoard->Board[x][y].defenseFactor = 1.3;
				LoadBoard->Board[x][y].production = 2000;
				break;
			}
			case('m'):
			{
				LoadBoard->Board[x][y].description = "Mine.";
				LoadBoard->Board[x][y].defenseFactor = 1.2;
				LoadBoard->Board[x][y].production = 3000;
				break;
			}
			case('n'):
			{
				LoadBoard->Board[x][y].description = "Settlement.";
				LoadBoard->Board[x][y].defenseFactor = 1.3;
				LoadBoard->Board[x][y].production = 1000;
				break;
			}
			case('h'):
			{
				LoadBoard->Board[x][y].description = "Factory.";
				LoadBoard->Board[x][y].defenseFactor = 1.3;
				LoadBoard->Board[x][y].production = 1000;
				break;
			}			
			case('Q'):
			{
				LoadBoard->Board[x][y].description = "Headquarters.";
				LoadBoard->Board[x][y].defenseFactor = 1.4;
				LoadBoard->Board[x][y].production = 1000;
				break;
			}
			case('='):
			{
				LoadBoard->Board[x][y].description = "Road.";
				LoadBoard->Board[x][y].defenseFactor = 1.0;
				break;
			}
			case('^'):
			{
				LoadBoard->Board[x][y].description = "Hill.";	
				LoadBoard->Board[x][y].defenseFactor = 1.1;
				break;
			}
			case('M'):
			{
				LoadBoard->Board[x][y].description = "Mountain.";
				LoadBoard->Board[x][y].defenseFactor = 1.4;
				break;
			}
			case('+'):		//Would like to have convertible to woodlot by engineer.....maybe
			{
				LoadBoard->Board[x][y].description = "Forest.";
				LoadBoard->Board[x][y].defenseFactor = 1.2;
				break;
			}
			}
			//Other terrain types go here

		}
	}
	return 0;
}

int printStatus(MasterBoard* boardToPrint)
{
	tile currentTile = boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	std::cout << currentTile.description<< " ";

	if(currentTile.hasMinionOnTop == true)
	{
		Minion* currentMinion = currentTile.minionOnTop;
		std::cout << "Player " << currentMinion->team
			<< "'s " << currentMinion->description
			<< ": " << int(currentMinion->health) <<
			" Health Left." << std::endl;
	
		if (currentMinion->status == gaveupmovehasntfired) 
		{
			std::cout << "Holding position. ";
			std::cout << "Ready to attack. "<< std::endl;
		}

		if (currentMinion->status == hasmovedhasntfired)
		{
			std::cout << "Has already moved this turn. " ;
			if (currentMinion->rangeType == rangedFire) 
			{
				std::cout<<"Cannot attack after moving." << std::endl;
			}
			if (currentMinion->rangeType == directFire) 
			{ 
				std::cout << "Ready to attack." << std::endl; 
			}
		}
		if (currentMinion->status == hasfired)
		{
			std::cout << "Has already moved this turn. ";
			std::cout << "Has attacked this turn." << std::endl;
		}
		if (currentMinion->status == hasntmovedorfired)
		{
			std::cout << "Ready to move. ";
			std::cout << "Ready to attack." << std::endl;
		}
	}
	else
	{ 
		std::cout << std::endl << std::endl;
	}
	
	//Print current turn.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	std::cout << "Player " << boardToPrint->playerFlag << "'s Turn. Treasury total: " << boardToPrint->treasury[boardToPrint->playerFlag]<<std::endl;

	//Debug purposes- print cursor location coordinates and window coordinates.
	//std::cout << eventText << "Cursor Location: " << boardToPrint->cursor.getX() << boardToPrint->cursor.getY() << std::endl;
	//std::cout << boardToPrint->windowLocation << std::endl;
	eventText = "";

	return 0;
}

int printMinionMenu(MasterBoard* boardToPrint) {

	minionStatus mystatus = boardToPrint->cursor.selectMinionPointer->status;

	if (mystatus == hasntmovedorfired)
	{
		std::cout << "Move cursor(WASD) | Move minion (m)" << std::endl;
		std::cout << "Deselect minion (t) | Capture move (c)" << std::endl;
	}

	if (mystatus == hasmovedhasntfired || mystatus == gaveupmovehasntfired) 
	{
		std::cout << "Move cursor(WASD) | Attack (r)" << std::endl;
		std::cout << "Deselect minion (t) | Capture (c)" << std::endl;	
	}
	if (mystatus == hasfired)
	{
		std::cout << std::endl;
		std::cout << std::endl;
	}
	return 0;

}

int printBoardMenu() {
	std::cout << "Move cursor (WASD) |s Menu (m)" << std::endl;
	std::cout << "Select minion/property (t)" << std::endl;	
	return 0;
}

int	printPropertyMenu() {

	//If this is not the second valid purchase input
	if (requestedMinionToBuy == '\n')
	{
		std::cout << "Input Minion to Buy" << std::endl;
		std::cout << "Deselect Property (P)" << std::endl;

	}	
	else if (requestedMinionToBuy != '\n')
	{
		if (requestedMinionToBuy == '!') 
		{
			std::cout << "Can't afford, try another symbol." << std::endl;
			std::cout << "Input Minion to Buy | Deselect Property (P)" << std::endl;
		}
		else if (requestedMinionToBuy == '?') 
		{
			std::cout << "Invalid input. Try another symbol." << std::endl;
			std::cout << "Input Minion to Buy  | Deselect Property (P)" << std::endl;
		}
		else 
		{ 
			std::cout << requestedMinionToBuy << " costs: " << unitPrice << std::endl; 
			std::cout << "Confirm (C) | Cancel (P)" << std::endl;
		}
	}

	return 0;

}

int printMenu() { 
	std::cout << "Save game (s) | Load new game (L)" << std::endl;
	std::cout << "End turn (t) | Exit menu (m)" << std::endl;
	return 0; 
}

int printLowerScreen(MasterBoard* boardToPrint) {
	
	if (inputLayer == gameBoard) 
	{
		printBoardMenu();
	}else

	if (inputLayer == menu)
	{
		printMenu();
	}else

	if (inputLayer == minionAction)
	{
		printMinionMenu(boardToPrint);
	}else

	if (inputLayer == propertyAction)
	{
		printPropertyMenu();
	}

	printStatus(boardToPrint);

	return 0;

}

int printUpperScreen(MasterBoard* boardToPrint) {
	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocation / BOARD_WIDTH;
	int windowX = boardToPrint->windowLocation % BOARD_WIDTH;

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (int i = windowY; i < (windowY + WINDOW_HEIGHT); i++)
	{
		for (int j = windowX; j < (windowX + WINDOW_WIDTH); j++)
		{
			//Print whomever has priority. cursor first, then unit, then terrain.

			//If there is a cursor there, it takes priority for printing.
			if (i == boardToPrint->cursor.getY() && j == boardToPrint->cursor.getX()) // * BOARD_WIDTH + j == boardToPrint->cursor.Location)										
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				std::cout << '*';
			}
			else
				//Is there a minion there?
				if (boardToPrint->Board[j][i].hasMinionOnTop == true)
				{
					//Determine team and then set the color.
					switch (boardToPrint->Board[j][i].minionOnTop->team)
					{
					case(0):
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
						break;
					case(1):
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
						break;
					case(2):
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED);
						break;

					}
					//Print out the minion.
					std::cout << boardToPrint->Board[j][i].minionOnTop->type;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				}
				else if (boardToPrint->Board[j][i].withinRange == true)
				{
					//If no minion, see if it's "in range" is set
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
					//And if so print the symbol for "in range" which is ':'
					std::cout << ':';
				}
				else
				{
					//Otherwise put out the terrain for that square.
					switch (boardToPrint->Board[j][i].controller)
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
					std::cout << boardToPrint->Board[j][i].symbol;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				}
		}
		std::cout << std::endl;
	}

	return 0;
}

int printScreen(MasterBoard * boardToPrint) 
{
	printUpperScreen(boardToPrint);

	printLowerScreen(boardToPrint);

	//Buffer the screen to clear the old map window.
	for (int i = 0; i < 10; i++)
		std::cout << std::endl;

	return 0;
}

int scenarioSave(std::string saveGameName, MasterBoard* boardToPrint)
{
	saveGameName += ".txt";
	std::ofstream saveGame(saveGameName);

	//First save the map size:
	saveGame << BOARD_WIDTH << "_" << BOARD_HEIGHT << std::endl;

	//Then save player data:
	saveGame << boardToPrint->playerFlag << std::endl;

	//Terrain save:
	//Iterate through board and save the exact symbol.
	//Saving symbol and controller separately for ease of edit and viewing.
	//Only pain point is do mass control-edits, but viewability is protected.
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			saveGame << boardToPrint->Board[x][y].symbol;
		}
		saveGame << std::endl;
	}
	
	//Iterate through board and save the controller.
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			saveGame << boardToPrint->Board[x][y].controller;

		}
		saveGame << std::endl;
	}
	
	//Note the number of minions:
	saveGame << boardToPrint->totalNumberOfMinions << std::endl;

	//Go through entire minionRoster and save each value associated with each minion, one line per minion.
	//Need to ensure correctness.
	//FIX FIX FIX - need to not just check for initial NULL, there may be minions beyond that first NULL if a guy was killed and not replaced on
	//The array.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (boardToPrint->minionRoster[i] != NULL)
		{
			saveGame << boardToPrint->minionRoster[i]->type << " "
				<< boardToPrint->minionRoster[i]->locationX << " "
				<< boardToPrint->minionRoster[i]->locationY << " "
				<< boardToPrint->minionRoster[i]->team << " "
				<< boardToPrint->minionRoster[i]->seniority << " "
				<< int(boardToPrint->minionRoster[i]->rangeType) << " "
				<< int(boardToPrint->minionRoster[i]->status) << " "
				<< int(boardToPrint->minionRoster[i]->health) << std::endl;
		}
	}
	saveGame.close();
	return 1;
}

//Need to change to take different inputs based on current inputLayer.
//Handle with a single local variable doing one reference.
int gameBoardInput(char * Input, MasterBoard * boardToInput)		
{

	
		if (*Input == 'a' || *Input == 'd' || *Input == 's' || *Input == 'w')
		{
			boardToInput->cursor.move(Input);
		}

		//Select minion or property.
		if (*Input == 't')
		{
			//If minion is not selected, select it.Must be successful to set flag.
			if (boardToInput->cursor.selectMinionFlag == false //This is probably not needed since it's always true/false in conj. with inputLayer.
				&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
			{
				if (boardToInput->selectMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
				{
					inputLayer = minionAction;
				}
			}	//Else if empty property, select it. No minion on top, right team, must be factory.
			else 
				if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'h'
					&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].controller == boardToInput->playerFlag
					&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false)
				{
					inputLayer = propertyAction;
				}

		}

		if (*Input == 'm')
		{
			inputLayer = menu;
		}

	return 0;
}

int minionInput(char* Input, MasterBoard * boardToInput) {
	
	if (*Input == 'a' || *Input == 'd' || *Input == 's' || *Input == 'w')
	{
		boardToInput->cursor.move(Input);
	}

	//Deselect
	if (*Input == 't')
	{
		if (boardToInput->cursor.selectMinionFlag == true)
		{
			boardToInput->deselectMinion();
			inputLayer = gameBoard;
		}
	}

	//Move minion command
	//If not on top, then move the unit.
	if (*Input == 'm' && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false
		&& boardToInput->cursor.selectMinionPointer->status == hasntmovedorfired)
	{
		if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{	//Change status appropriately for successful movement.
			boardToInput->cursor.selectMinionPointer->status = hasmovedhasntfired;	//I think this is doubletap.
			boardToInput->deselectMinion();
			inputLayer = gameBoard;
		}
	}

	//If already on top, just "move" by not moving. This allows the user to fire without actually changing position.
	if (*Input == 'm' && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->locationX == boardToInput->cursor.getX()
		&& boardToInput->cursor.selectMinionPointer->locationY == boardToInput->cursor.getY()
		&& boardToInput->cursor.selectMinionPointer->status == hasntmovedorfired)
	{
		boardToInput->cursor.selectMinionPointer->status = gaveupmovehasntfired;	//I think this is doubletap.
		boardToInput->deselectMinion();
		inputLayer = gameBoard;
	}
	
	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true)
		if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
			if ((boardToInput->cursor.getX() != boardToInput->cursor.selectMinionPointer->locationX) || (boardToInput->cursor.getY() != boardToInput->cursor.selectMinionPointer->locationY))//Can attack if minion is selected
				if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team)//And it's enemy team's.
					if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true)	//In range
					{
						//This is the actual attack portion. Return of 0 indicates successful attack.
						bool attackSuccess = boardToInput->attackMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY());
						if (attackSuccess == 0)
						{
							boardToInput->cursor.selectMinionPointer->status = hasfired;
							boardToInput->deselectMinion();
							inputLayer = gameBoard;
						}
					}


	return 0;
}

int menuInput(char* Input, MasterBoard* boardToInput) {
	
	//This is a working key.
	if (*Input == 'g') 
	{
		scrambleMap(boardToInput);
	}

	//Ends the turn and passes it to the next player.
	//Autosave every turn.
	if (*Input == 't')
	{
		if (boardToInput->cursor.selectMinionFlag == true)
			boardToInput->deselectMinion();
		boardToInput->endTurn();
		scenarioSave("Autosave", boardToInput);
		inputLayer = gameBoard;
	}

	if (*Input == 'l') 
	{
		scenarioLoad(boardToInput);
		inputLayer = gameBoard;
		//Load new map
	}

	//Prompt user and save scenario.
	if (*Input == 's')
	{
		std::string saveName = "";
		std::cout << "Choose where to save your game:" << std::endl;
		std::cin >> saveName;
		scenarioSave(saveName, boardToInput);
		inputLayer = gameBoard;
	}

	//Exit menu
	if (*Input == 'm') 
	{
		inputLayer = gameBoard;
	}

	return 0;
}

int propertyMenuInput(char* Input, MasterBoard* boardToInput) {

	//Player treasury, cost of unit initialized to -1 to show bad inputs.
	unitPrice = -1;
	int treasury = boardToInput->treasury[boardToInput->playerFlag];

	//If this is NOT the second valid purchase input
	//IE we have not yet gotten any valid input for propertyLayer.
	//Thus one of the three states (prestate, too expensive, not a real unit).
	if (requestedMinionToBuy == '\n' || requestedMinionToBuy == '!' || requestedMinionToBuy == '?')
	{
		if (*Input == 'p')
		{
			//Deselect
			inputLayer = gameBoard;
			return 0;
		}
		//Consult cost table:
		unitPrice = boardToInput->consultMinionCostChart(*Input);
		
		//If it is a real unit we are trying to purchase
		//Aka unitPrica is not -1 aka non-error
		if (unitPrice > 0) 
		{
			requestedMinionToBuy = *Input;
			
			//If we can't afford, use special ! character
			if (unitPrice > treasury) 
			{
				//Indicates you can't afford
				requestedMinionToBuy = '!';
			}				
		}
		else 
		{
			requestedMinionToBuy = '?';
		}
			
	}

	//This is the second valid input.
	else 
	{
		if (*Input == 'p') 
		{
			//Cancel purchase
			requestedMinionToBuy = '\n';
		}
		if (*Input == 'c') 
		{
		//Confirm purchase
			boardToInput->createMinion(requestedMinionToBuy, boardToInput->cursor.getX(), boardToInput->cursor.getY(), boardToInput->playerFlag, 100, hasfired);
			boardToInput->treasury[boardToInput->playerFlag] -= unitPrice;
			inputLayer = gameBoard;
			requestedMinionToBuy = '\n';
		}
	}

	return 0;
}

//Load saved game and initialize the board with its contents.
int scenarioLoad(MasterBoard* boardToPrint) {
	
	//Clear board in case scenario load was called by player menu later in game.
	boardToPrint->clearBoard();

	std::ifstream saveGame;
	std::string line;
	char garbage;
	int garb1, garb2;
	std::string scenarioToLoad = "";
	std::string saveName = "";
	bool loadsuccessful = false;
	

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		std::cout << "Choose which scenario to load (Case sensitive): " << std::endl;
		scenarioToLoad = "";

		std::cin >> scenarioToLoad;

		saveGame.open(scenarioToLoad + ".txt");
		if (saveGame.is_open())
		{
			std::cout << "Successfully loaded!" << std::endl;
			loadsuccessful = true;
		}
		else
		{
			std::cout << "Could not load scenario. Please check that it exists and the right spelling was used." << std::endl;

		}


	}

	//First load the map size:
	saveGame >> garb1;
	saveGame >>  garbage;
	saveGame >> garb2;

	//Then load player data:
	saveGame >> boardToPrint->playerFlag;


		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			
			for (int x = 0; x < BOARD_WIDTH; x++)
			{
				saveGame >> boardToPrint->Board[x][y].symbol;
			}
		}
		setCharacteristics(boardToPrint);
		
		char inputGarb;
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{

			for (int x = 0; x < BOARD_WIDTH; x++)
			{
				saveGame >> inputGarb;
				boardToPrint->Board[x][y].controller = (int (inputGarb))-48;
			}
		}
	//Then load minion data:
	int numberOfMinions;
	saveGame >> numberOfMinions;
	int health, locationX, locationY, team, seniority, rangeType, status;
	char type;
	for (int y = 0; y < numberOfMinions; y++)
	{
		saveGame >> type 
			>> locationX 
			>> locationY 
			>> team 
			>> seniority 
			>> rangeType 
			>> status 
			>> health;
			boardToPrint->createMinion(type, locationX, locationY, team, health, status);
	}
	

saveGame.close();
return 1;
}


int main()
{
	MasterBoard GameBoard;
	
	scenarioLoad(&GameBoard);

	printScreen(&GameBoard);
	char Input = ' ';
	while (true)		//Run as long as the user wants. Infinite for loop.
	{
		Input = _getch();
		//Major change beginning: Use flag inputLayer to determine which input handler gets called.
		//Ned an input handler for menu and minion action (Attack, capture, etc)
		//Need each button press to potentially update the inputLayer flag as needed.
		if (inputLayer == gameBoard) 
		{
			gameBoardInput(&Input, &GameBoard);
		} 
		else if (inputLayer == minionAction)
		{
			minionInput(&Input, &GameBoard);
		}
		else if (inputLayer == menu)
		{
			menuInput(&Input, &GameBoard);
		}
		else if (inputLayer == propertyAction)
		{
			propertyMenuInput(&Input, &GameBoard);
		}

		GameBoard.checkWindow();
		printScreen(&GameBoard);
	}

}


