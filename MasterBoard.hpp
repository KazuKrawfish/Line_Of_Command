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
	int selectMinion(int inputLocation);
	int moveMinion(int inputLocation);
	int deselectMinion();
	int createMinion(char inputType, int inputLocation, int inputTeam);
	tile Board[BOARD_SIZE];
	int setRangeField(int inputLocation, int inputRange);
	int setAttackField(int inputLocation, int inputRange);
	int endTurn();
	int attackMinion(int inputLocation);
	int destroyMinion(Minion * inputMinion);
	int windowLocation;
	int checkWindow();
	int newTurnFlag;
	Cursor cursor;
};

#endif /* MASTERBOARD_HPP__ */

