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
	Minion* minionRoster[GLOBALSUPPLYCAP];
	MasterBoard();
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int createMinion(char inputType, int inputX, int inputY, int inputTeam);
	tile Board[BOARD_WIDTH][BOARD_HEIGHT];
	int setRangeField(int inputX, int inputY, int inputRange);
	int setAttackField(int inputX, int inputY, int inputRange);
	int endTurn();
	int attackMinion(int inputX, int inputY);
	int destroyMinion(Minion * inputMinion);
	int windowLocation;
	int checkWindow();
	int playerFlag;
	Cursor cursor;
};

#endif /* MASTERBOARD_HPP__ */

