#ifndef MAINMENU_HPP__
#define MAINMENU_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include <curses.h>


class mainMenu
{
public:
	int scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer);
	int setCharacteristics(MasterBoard* LoadBoard);
	int scenarioSave(std::string saveGameName, MasterBoard* boardToPrint);
	int scenarioLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer);
	int playGame(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	
};

#endif /* MAINMENU_HPP__ */
