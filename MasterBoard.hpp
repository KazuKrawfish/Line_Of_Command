#ifndef MASTERBOARD_HPP__
#define MASTERBOARD_HPP__

#include "Cursor.hpp"
#include "Minion.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include <vector>

class Cursor;
const int GLOBALSUPPLYCAP	= 100;		//Global variables declarations

class MasterBoard
{
public:
	
	MasterBoard();
	int NUMBEROFPLAYERS = 2;
	int BOARD_WIDTH = 15;
	int BOARD_HEIGHT = 20;



	std::vector<int> treasury;	//1 extra "player" so that treasury[playerFlag] behaves as expected. (There is no player 0.)
	
	int setRangeField(int inputX, int inputY, int inputRange);
	int setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY);
	int setAttackField(int inputX, int inputY, int inputRange);
	int setVisionField();
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int attemptPurchaseMinion(char inputType, int inputX, int inputY, int inputTeam);
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy);
	int endTurn(inputLayer* InputLayer);
	int upkeep(inputLayer* InputLayer);
	int attackMinion(int inputX, int inputY, inputLayer* InputLayer);
	int destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer);
	int checkWindow();
	int repairMinions();
	int clearBoard(inputLayer* InputLayer);
	int consultMinionCostChart(char minionType, char propertyType);
	std::string captureProperty(tile* inputTile, Minion* inputMinion);
	Minion* minionRoster[GLOBALSUPPLYCAP];
	Cursor cursor;
	std::vector<std::vector<tile>>  Board;
	int windowLocation = 0;
	int playerFlag;
	int totalNumberOfMinions;

private:

};


#endif /* MASTERBOARD_HPP__ */

