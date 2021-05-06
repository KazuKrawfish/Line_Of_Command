#ifndef MASTERBOARD_HPP__
#define MASTERBOARD_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"


const int BOARD_SIZE		= BOARD_WIDTH * BOARD_HEIGHT;
const int NUMBEROFPLAYERS	= 2;
const int GLOBALSUPPLYCAP	= 100;		//Global variables declarations

class MasterBoard
{
public:
	
	MasterBoard();
	
	int setRangeField(int inputX, int inputY, int inputRange);
	int setAttackField(int inputX, int inputY, int inputRange);
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status);
	int endTurn();
	int attackMinion(int inputX, int inputY);
	int destroyMinion(Minion* inputMinion);
	int checkWindow();
	Minion* minionRoster[GLOBALSUPPLYCAP];
	tile Board[BOARD_WIDTH][BOARD_HEIGHT];
	Cursor cursor;
	int windowLocation;
	int playerFlag;
	int totalNumberOfMinions;

private:

//To move variables later. Not now!
};

#endif /* MASTERBOARD_HPP__ */

