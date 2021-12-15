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
	
	MasterBoard(sf::Texture* inputTexture);
	int NUMBEROFPLAYERS = 2;
	int BOARD_WIDTH = 12;
	int BOARD_HEIGHT = 10;
	int WINDOW_HEIGHT = 10;
	int WINDOW_WIDTH = 12;

	int buildPath(bool isItInitialCall, int x, int y, char minionType, std::vector<std::vector<pathSquare>> & pathMapPointer);
	int buildApparentPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion);
	int buildTerrainOnlyPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion);

	int setRangeField(int inputX, int inputY);
	int setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY, int playerToSee);
	int setAttackField(int inputX, int inputY, int inputRange);
	int setDropField(int inputX, int inputY);
	int setVisionField(int observerNumber);
	double calculateDamageDealt(Minion* attackingMinion, Minion* defendingMinion, bool & wouldAmmoBeUsed, int & weaponUsed, bool ignoreLimitations);
	int selectMinion(int inputX, int inputY);
	int moveMinion(int inputX, int inputY, inputLayer* InputLayer, int  observerNumber);
	int deselectMinion();
	int attemptPurchaseMinion(char inputType, int inputX, int inputY, int inputTeam);
	int createMinion(char inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy, int beingTransported, int inputFuel, int inputPriAmmo, int inputSecAmmo);
	int endTurn(inputLayer* InputLayer);
	int upkeep(inputLayer* InputLayer);
	int attackMinion(int inputX, int inputY, inputLayer* InputLayer, int observerNumber);
	int destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer, bool AdminKill);
	int pickUpMinion(int inputX, int inputY);
	int validatePath(int& inputX, int& inputY, inputLayer* graphicsLayer, int whoIsWatching);
	int setCursorPath(bool firstTime, int inputX, int inputY);
	int individualResupply(Minion* SupplyUnit, bool isItDuringUpkeep);
	int dropOffMinion();
	int checkWindow();
	int repairMinions();
	int resupplyMinions();
	int clearBoard(inputLayer* InputLayer);
	int consultMinionCostChart(char minionType, char propertyType);
	int playerDefeat(int playerNumber, int winningPlayer, inputLayer* InputLayer);

	std::string captureProperty(tile* inputTile, Minion* inputMinion, inputLayer* InputLayer,  int  observerNumber);
	Minion* minionRoster[GLOBALSUPPLYCAP];
	Cursor cursor;

	std::vector<std::vector<tile>>  Board;
	

	std::vector<playerRosterEntry> playerRoster;

	int windowLocationX = 0;
	int windowLocationY = 0;
	int playerFlag;
	int totalNumberOfMinions;
	bool isItSinglePlayerGame = false;
	bool interruptFlag = false;

	//GRAPHICS	////////////////////////////////
	sf::Texture * myTexture;
	//GRAPHICS	///////////////////////////////


	//MissionInfo///////////////////////////////
	bool missionFlag= false;
	std::string campaignName = "_";
	std::string scenarioOrMissionName = "_";
	int missionTurnLength = 0;	//Turn limit, or 0 if no turn limit.
	int whoHoldsOut = 0;		//0 If missionLength is not on, otherwise indicates who wins if turns run out.
	//MissionInfo///////////////////////////////

private:

};


#endif /* MASTERBOARD_HPP__ */

