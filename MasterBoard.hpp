#ifndef MASTERBOARD_HPP__
#define MASTERBOARD_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "inputLayer.hpp"


const int BOARD_SIZE		= BOARD_WIDTH * BOARD_HEIGHT;
const int NUMBEROFPLAYERS	= 2;
const int GLOBALSUPPLYCAP	= 100;		//Global variables declarations

class MasterBoard
{
public:
	
	MasterBoard();
	
	int treasury[NUMBEROFPLAYERS+1];	//1 extra to avoid potential error when accessing, later.
	int setRangeField(int inputX, int inputY, int inputRange);
	int setAttackField(int inputX, int inputY, int inputRange);
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status);
	int endTurn();
	int attackMinion(int inputX, int inputY, inputLayer* InputLayer);
	int destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer);
	int checkWindow();
	int clearBoard(inputLayer* InputLayer);
	int consultMinionCostChart(char minionType);
	std::string captureProperty(tile* inputTile, Minion* inputMinion);
	Minion* minionRoster[GLOBALSUPPLYCAP];
	tile  Board[BOARD_WIDTH][BOARD_HEIGHT];
	Cursor cursor;
	int windowLocation;
	int playerFlag;
	int totalNumberOfMinions;

private:

//To move variables later. Not now!
};


#endif /* MASTERBOARD_HPP__ */

