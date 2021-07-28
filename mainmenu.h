#ifndef MAINMENU_HPP__
#define MAINMENU_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include <curses.h>

enum mainMenuStatus { topmenu,  playingMap, waitingForRemotePlayer };
enum interactionMethod {unchosen, local, remote};

class mainMenu
{
public:
	//gameTurn represents the overall turn, which increments once everyone has cycled through one player turn.
	int gameTurn = 1;
	//interactionMethod refers to whether this will be remote or local game.
	interactionMethod gameType = unchosen;
	//mainMenuStatus will indicate what interace the player is interacting with.
	mainMenuStatus menuStatus = topmenu;
	std::string playerNames[NUMBEROFPLAYERS+1];
	int gameLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame);
	int scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer);
	int setCharacteristics(MasterBoard* LoadBoard);
	int gameSave(std::string saveGameName, MasterBoard* boardToPrint);
	int scenarioLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame);
	int playGame(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int printTopMenu();
	int topMenuInput(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	
};

#endif /* MAINMENU_HPP__ */
