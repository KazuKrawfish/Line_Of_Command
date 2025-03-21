//Copyright 2022, Supercontinent Software Ltd.
//
//	masterboard.hpp
//
/*
MasterBoard is responsible for all "mechanical" gameplay actions. It contains the actual board with all tiles and minions,
and calculates movements, attacks, supply, production, etc. Masterboard and inputLayer interact as a team, with cursor, minion, and tile
classes as subordinate supporting classes. NB: It is understood that public members are dangerous but this has been done to speed
development, and since this is almost entirely a solo effort.
*/

#ifndef MASTERBOARD_HPP__
#define MASTERBOARD_HPP__

#include "Cursor.hpp"
#include "Minion.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include <vector>

class Cursor;
const int GLOBALSUPPLYCAP = 100;		//Global variables declarations

//Types of players. Note: Local and Remote Human are still treated as similar for many game functions.
enum playerTypeKind { localHumanPlayer, computerPlayer , remoteHumanPlayer };	

enum factionType {NeutralFaction, NorthRedonia, SouthRedonia, Ormosa, Torran};

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
	playerTypeKind playerType = localHumanPlayer;
	std::string name;
	int playerFlag = -1;
	bool stillAlive = true;
	int numberOfMinions = 0;
	factionType playerFaction = NeutralFaction;
};



class MasterBoard
{
public:

	MasterBoard(sf::Texture* inputTexture,  sf::Texture* inputSecondTexture);


	//Board Management Functions//////////////
	int checkWindow();
	int clearBoard(inputLayer* InputLayer);
	int computeDistance(int inputX1, int inputX2, int inputY1, int inputY2);
	double consultAttackValuesChart(std::string attackingType, std::string defendingType, double& primaryAttackValue, double& secondaryAttackValue, double& bestAttackValue);
	double consultAttackValuesChart(Minion& attackingMinion, Minion& defendingMinion, bool& isAmmoUsed, int& weaponUsed, bool ignoreLimitations);
	//Board Management Functions//////////////

	//Pathfinding Functions////////////////////
	int buildPath(bool isItInitialCall, int x, int y, std::string minionType, std::vector<std::vector<pathSquare>>& pathMapPointer);
	int buildApparentPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion);
	int buildTerrainOnlyPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion);
	int buildTerrainOnlyPathMap(bool isItInitialCall, int x, int y, std::string inputType, std::vector<std::vector<pathSquare>>& inputPathMap);
	bool hasAdjacentMinion(int inputX, int inputY, int observerNumber);
	//Pathfinding Functions////////////////////

	//Field Manipulation Functions/////////////
	int setRangeField(int inputX, int inputY);
	int setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY, int playerToSee);
	int setAttackField(int inputX, int inputY, int inputRange);
	int setDropField(int inputX, int inputY);
	int setVisionField(int observerNumber);
	//Field Manipulation Functions/////////////

	//Select and Move Functions////////////////
	int selectMinion(int inputX, int inputY);
	int setCursorPath(bool firstTime, int inputX, int inputY);
	int deselectMinion();
	int moveMinion(int inputX, int inputY, inputLayer* InputLayer, int  observerNumber);
	int diveOrSurfaceSub();
	int validatePath(int& inputX, int& inputY, inputLayer* graphicsLayer, int whoIsWatching, bool transportMove);
	int landmineCheck(int inputX, int inputY, inputLayer* InputLayer, int observerNumber );
	//Select and Move Functions////////////////

	//Combat Functions/////////////////////////
	int attackMinion(int inputX, int inputY, inputLayer* InputLayer, int observerNumber);
	double calculateDamageDealt(Minion* attackingMinion, Minion* defendingMinion, bool& wouldAmmoBeUsed, int& weaponUsed, bool ignoreLimitations);
	int destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer, bool AdminKill);
	int landmineAttack(int attackingX, int attackingY, inputLayer* InputLayer, int observerNumber);
	int splashAttackMinion(Minion* splashAttackingMinion , int inputX, int inputY, inputLayer* InputLayer, int observerNumber, int splashFactorInverse);
	//Combat Functions/////////////////////////

	//Special Action Functions/////////////////
	int pickUpMinion(int inputX, int inputY, inputLayer* InputLayer, int  observerNumber);
	int dropOffMinion();
	std::string captureProperty(tile* inputTile, Minion* inputMinion, inputLayer* InputLayer, int  observerNumber);
	//Special Action Functions/////////////////

	//Purchase Functions///////////////////////
	int attemptPurchaseMinion(std::string inputType, int inputX, int inputY, int inputTeam);
	int consultMinionCostChart(std::string minionType, char propertyType);
	bool checkFactionAvailability(std::string minionType, int inputPlayer);
	int deployLandmine(inputLayer* InputLayer, int inputX, int inputY);
	int buildImprovement(inputLayer* InputLayer, int inputX, int inputY);
	int createMinion(std::string, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy, int beingTransported, int inputFuel, int inputPriAmmo, int inputSecAmmo);
	//Purchase Functions///////////////////////

	//Begin/End Turn Functions/////////////////
	int endTurn(inputLayer* InputLayer);
	int upkeep(inputLayer* InputLayer, int observerNumber);
	int playerDefeat(int playerNumber, int winningPlayer, inputLayer* InputLayer);
	int playerVictory(int winningPlayer, inputLayer* InputLayer);
	//Begin/End Turn Functions/////////////////

	//Upkeep Functions/////////////////////////
	void singleSquareResupply(Minion* SupplyUnit, bool isItDuringUpkeep, inputLayer* InputLayer, int observerNumber, int xVal, int yVal);
	int individualResupply(Minion* SupplyUnit, bool isItDuringUpkeep, inputLayer* InputLayer, int observerNumber);
	int individualRepair(Minion* MinionToRepair, inputLayer* InputLayer, int observerNumber);
	int repairMinions(inputLayer* InputLayer, int observerNumber);
	int resupplyMinions(inputLayer* InputLayer, int observerNumber);
	//Upkeep Functions/////////////////////////

	//Window Data//////////////////////////////
	int BOARD_WIDTH = 12;
	int BOARD_HEIGHT = 10;
	int WINDOW_HEIGHT = 10;
	int WINDOW_WIDTH = 12;
	int windowLocationX = 0;
	int windowLocationY = 0;
	//Window Data//////////////////////////////	

	//Minions//////////////////////////////////
	Minion* minionRoster[GLOBALSUPPLYCAP];
	int totalNumberOfMinions;
	//Minions//////////////////////////////////

	Cursor cursor;

	std::vector<std::vector<tile>>  Board;

	//Player Data//////////////////////////////
	int NUMBEROFPLAYERS = 2;
	std::vector<playerRosterEntry> playerRoster;
	int playerFlag;
	bool isItSinglePlayerGame = false;
	//Player Data//////////////////////////////

	bool interruptFlag = false;

	//GRAPHICS	///////////////////////////////
	sf::Texture* myTexture;
	sf::Texture* mySecondTexture;
	//GRAPHICS	///////////////////////////////


	//MissionInfo///////////////////////////////
	bool fogOfWar = false;
	bool missionFlag = false;
	std::string campaignName = "_";
	std::string scenarioOrMissionName = "_";
	int missionTurnLength = 0;	//Turn limit, or 0 if no turn limit.
	int whoHoldsOut = 0;		//0 If missionLength is not on, otherwise indicates who wins if turns run out.
	std::vector <std::string> banList;
	//MissionInfo///////////////////////////////

private:

};


#endif /* MASTERBOARD_HPP__ */
