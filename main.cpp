//Crisis Front (Project X)
//Or maybe call it Line of Command
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
#include "inputLayer.hpp"
#include "compie.hpp"
#include <curses.h>
#include "mainmenu.h"



int main()
{
	WINDOW* debugWindow;
	WINDOW* unusedWindow = initscr();

	//Old window depended on 40x90.
	//So should be good for anything after that.
	resize_term(40, 150);


	//Lines, columns, startY, startX
	debugWindow = newwin(40, 20, 0, 100);
	wclear(debugWindow);
	waddstr(debugWindow, "I am debug window.");
	wrefresh(debugWindow);

	WINDOW* mainWindow = newwin(40, 90, 0, 0);
	wclear(mainWindow);
	winsch(mainWindow, 'b');
	wrefresh(mainWindow);

	mainMenu MainMenu(mainWindow);
	inputLayer InputLayer(&MainMenu);
	MasterBoard GameBoard;


	start_color();

	//Terrain we can see - black background.
	init_pair(landTile, COLOR_BLACK, COLOR_GREEN);
	init_pair(waterTile, COLOR_GREEN, COLOR_BLUE);
	
	//Player minion colors
	init_pair(player1Minion, COLOR_RED, COLOR_YELLOW);
	init_pair(player2Minion, COLOR_BLUE, COLOR_CYAN);
	init_pair(player3Minion, COLOR_CYAN, COLOR_BLACK);
	init_pair(player4Minion, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(player5Minion, COLOR_YELLOW, COLOR_BLACK);
	init_pair(player6Minion, COLOR_GREEN, COLOR_BLACK);

	//Player minion inverse colors
	init_pair(player1MinionInverse, COLOR_YELLOW, COLOR_RED);
	init_pair(player2MinionInverse, COLOR_CYAN, COLOR_BLUE);
	init_pair(player3MinionInverse, COLOR_BLACK, COLOR_CYAN);
	init_pair(player4MinionInverse, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(player5MinionInverse, COLOR_BLACK, COLOR_YELLOW);
	init_pair(player6MinionInverse, COLOR_BLACK, COLOR_GREEN);		
	
	//Player tile colors
	init_pair(player1Tile, COLOR_RED, COLOR_GREEN);
	init_pair(player2Tile, COLOR_BLUE, COLOR_GREEN);
	init_pair(player3Tile, COLOR_CYAN, COLOR_GREEN);
	init_pair(player4Tile, COLOR_MAGENTA, COLOR_GREEN);
	init_pair(player5Tile, COLOR_YELLOW, COLOR_GREEN);
	init_pair(player6Tile, COLOR_GREEN, COLOR_BLACK);
	init_pair(neutralTile, COLOR_WHITE, COLOR_GREEN);

	//Outside of vision, terrain - shrouded in fog.
	init_pair(fogLandTile, COLOR_BLACK, COLOR_WHITE);
	init_pair(fogWaterTile, COLOR_BLUE, COLOR_WHITE);
	init_pair(fogPlayer1Tile, COLOR_RED, COLOR_WHITE);
	init_pair(fogPlayer2Tile, COLOR_BLUE, COLOR_WHITE);
	init_pair(fogPlayer3Tile, COLOR_CYAN, COLOR_WHITE);
	init_pair(fogPlayer4Tile, COLOR_MAGENTA, COLOR_WHITE);
	init_pair(fogPlayer5Tile, COLOR_YELLOW, COLOR_WHITE);
	init_pair(fogPlayer6Tile, COLOR_GREEN, COLOR_WHITE);
	init_pair(fogNeutralTile, COLOR_BLACK, COLOR_WHITE);

	//Utility colors
	init_pair(cursorSymbol, COLOR_BLACK, COLOR_WHITE);
	init_pair(attackRangeSymbol, COLOR_BLACK, COLOR_RED);
	init_pair(moveRangeSymbol, COLOR_BLACK, COLOR_RED);

	MainMenu.playGame(&GameBoard, &InputLayer);
	

}


