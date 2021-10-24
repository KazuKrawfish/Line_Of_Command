#ifndef INPUT_HPP__
#define INPUT_HPP__

#include "compie.hpp"


//Fwd dec
class compie;
class mainMenu;



enum gameInputLayer { gameBoard, menu, minionAction, propertyAction, waitingForNextLocalPlayer, compieTakingTurn, insertMinion, insertTile};
enum specialKey {showMinions, hideMinions};
enum colorCode {
	unused, fogLandTile, waterTile, fogWaterTile, player1Minion,
	player2Minion, player3Minion, player4Minion, player5Minion, player6Minion,
	player1Tile, player2Tile, player3Tile, player4Tile, player5Tile, player6Tile,
	fogPlayer1Tile, fogPlayer2Tile, fogPlayer3Tile, fogPlayer4Tile,
	fogPlayer5Tile, fogPlayer6Tile, neutralTile, fogNeutralTile, landTile, cursorSymbol,
	attackRangeSymbol, moveRangeSymbol, player1MinionInverse, player2MinionInverse,
	player3MinionInverse, player4MinionInverse, player5MinionInverse, player6MinionInverse
};


class inputLayer
{
public:

	inputLayer(mainMenu* inputMainMenu);
	int printStatus(MasterBoard* boardToPrint, int observerNumber);
	int printWaitingScreen(MasterBoard* boardToPrint);
	int printMinionMenu(MasterBoard* boardToPrint);
	int printBoardMenu();
	int	printPropertyMenu(MasterBoard* boardToPrint);
	int printMenu();
	int printInsertMinion();
	int printInsertTile();
	int printPlayerDefeat(int playerDefeated, MasterBoard* boardToPrint);
	int printPlayerVictory(int playerVictorious, MasterBoard* boardToPrint);
	int printLowerScreen(MasterBoard* boardToPrint, int observerNumber);
	int printUpperScreen(MasterBoard* boardToPrint, int observerNumber);
	int printScreen(MasterBoard* boardToPrint, int observerNumber);
	int insertMinionInput(char* Input, MasterBoard* boardToInput);
	int insertTileInput(char* Input, MasterBoard* boardToInput);
	int gameBoardInput(char* Input, MasterBoard* boardToInput);
	int menuInput(char* Input, MasterBoard* boardToInput);
	int waitingScreenInput( MasterBoard* boardToInput);
	int propertyMenuInput(char* Input, MasterBoard* boardToInput);
	int minionInput(char* Input, MasterBoard* boardToInput);
	int printSingleTile(int inputX, int inputY, std::string inputString, int teamNumber, Minion* minionToPrint);
	int exitToMainMenu(MasterBoard* boardToInput);
	
	std::string eventText = "";

	gameInputLayer status = gameBoard;
	specialKey minionVisibleStatus = showMinions;
	char requestedMinionToBuy = '\n';
	int requestedUnitPrice = -1;
	mainMenu* MainMenu;


private:

};


#endif /* INPUT_HPP__ */


