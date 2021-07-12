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
	
	int treasury[NUMBEROFPLAYERS+1];	//1 extra "player" so that treasury[playerFlag] behaves as expected. (There is no player 0.)
	int setRangeField(int inputX, int inputY, int inputRange);
	int setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY);
	int setAttackField(int inputX, int inputY, int inputRange);
	int setVisionField();
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status);
	int endTurn(inputLayer* InputLayer);
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

