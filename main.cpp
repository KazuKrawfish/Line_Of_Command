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

//Global variables need to be moved.

int main()
{
	mainMenu MainMenu;
	MasterBoard GameBoard;
	inputLayer InputLayer(&MainMenu);
	compie ComputerPlayer;
	WINDOW* mywindow = initscr();
	

	resize_term(40, 90);
	start_color();

	//Terrain we can see - black background.
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_YELLOW, COLOR_BLACK);

	//Outside of vision, terrain - shrouded in fog.
	init_pair(7, COLOR_GREEN, COLOR_WHITE);
	init_pair(8, COLOR_RED, COLOR_WHITE);
	init_pair(9, COLOR_BLUE, COLOR_WHITE);
	init_pair(10, COLOR_CYAN, COLOR_WHITE);
	init_pair(11, COLOR_MAGENTA, COLOR_WHITE);
	init_pair(12, COLOR_YELLOW, COLOR_WHITE);

	MainMenu.playGame(&GameBoard, &InputLayer, &ComputerPlayer, mywindow);
	

}


