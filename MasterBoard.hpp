#ifndef MASTERBOARD_HPP__
#define MASTERBOARD_HPP__

#include "Cursor.hpp"
#include "Minion.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include <vector>

class Cursor;
const int GLOBALSUPPLYCAP	= 100;		//Global variables declarations

enum playerTypeKind {	humanPlayer, computerPlayer };

class pathSquare
{
public:
	int distanceFromMinion = -1;
	bool wasVisited = false;

};

class playerRosterEntry 
{
public:
	playerRosterEntry() 
	{
		NULL;
	}

	playerRosterEntry(int inputTreasury, std::string inputName, int inputPlayerFlag)
	{
		treasury = inputTreasury;
		name = inputName;
		playerFlag = inputPlayerFlag;
		
	};

	int treasury = 0;
	playerTypeKind playerType = humanPlayer;
	std::string name;
	int playerFlag = -1;
	bool stillAlive = true;
	int numberOfMinions = 0;
};



class MasterBoard
{
public:
	
	MasterBoard();
	int NUMBEROFPLAYERS = 2;
	int BOARD_WIDTH = 15;
	int BOARD_HEIGHT = 20;
	int buildPath(bool isItInitialCall, int x, int y, char minionType, std::vector<std::vector<pathSquare>> & pathMapPointer);
	int buildApparentPathMap(bool isItInitialCall, int x, int y, char minionType);

	int setRangeField(int inputX, int inputY);
	int setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY, int playerToSee);
	int setAttackField(int inputX, int inputY, int inputRange);
	int setDropField(int inputX, int inputY);
	int setVisionField(int observerNumber);
	double calculateDamageDealt(Minion* attackingMinion, Minion* defendingMinion, bool & wouldAmmoBeUsed, int & weaponUsed, bool ignoreLimitations);
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY);
	int deselectMinion();
	int attemptPurchaseMinion(char inputType, int inputX, int inputY, int inputTeam);
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy, int beingTransported, int inputFuel, int inputPriAmmo, int inputSecAmmo);
	int endTurn(inputLayer* InputLayer);
	int upkeep(inputLayer* InputLayer);
	int attackMinion(int inputX, int inputY, inputLayer* InputLayer);
	int destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer);
	int pickUpMinion(int inputX, int inputY);
	int validatePath(int & inputX, int&  inputY);
	int setCursorPath(bool firstTime, int inputX, int inputY);
	int individualResupply(Minion* SupplyUnit, bool isItDuringUpkeep);
	int dropOffMinion();
	int checkWindow();
	int repairMinions();
	int resupplyMinions();
	int clearBoard(inputLayer* InputLayer);
	int consultMinionCostChart(char minionType, char propertyType);
	std::string captureProperty(tile* inputTile, Minion* inputMinion);
	Minion* minionRoster[GLOBALSUPPLYCAP];
	Cursor cursor;
	std::vector<std::vector<pathSquare>> myPathMap;
	std::vector<std::vector<pathSquare>> compiePathMap;
	std::vector<std::vector<tile>>  Board;

	std::vector<playerRosterEntry> playerRoster;

	int windowLocationX = 0;
	int windowLocationY = 0;
	int playerFlag;
	int totalNumberOfMinions;
	bool isItSinglePlayerGame = false;

private:

};


#endif /* MASTERBOARD_HPP__ */

