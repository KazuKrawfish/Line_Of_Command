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

	inputLayer(mainMenu* inputMainMenu, sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* cour);
	
	//Print Functions:///////////////////////////////////////////////////////
	//Non-game play
	int printPlayerDefeat(int playerDefeated, MasterBoard* boardToPrint);
	int printPlayerVictory(int playerVictorious, MasterBoard* boardToPrint);
	int printWaitingScreen(MasterBoard* boardToPrint);
	int printMissionBriefing(MasterBoard* boardToInput);

	//Player Options
	int printMinionMenu(MasterBoard* boardToPrint);
	int printBoardMenu(MasterBoard* boardToPrint);
	int	printPropertyMenu(MasterBoard* boardToPrint);
	int printMenu(MasterBoard* boardToPrint);
	int printInsertMinion(MasterBoard* boardToPrint);
	int printInsertTile(MasterBoard* boardToPrint);

	//Print Whole Screen
	int printStatus(MasterBoard* boardToPrint, int observerNumber);
	int printLowerScreen(MasterBoard* boardToPrint, int observerNumber);
	int printUpperScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation);
	int printScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation);
	int printSingleTile(int screenX, int screenY, int actualX, int actualY, MasterBoard* boardToPrint, int playerNumber, bool withinAnimation);

	//Graphics Print
	int movementGraphics(MasterBoard* boardToPrint, int observerNumber, Minion * minionToMove, int locationX , int locationY);
	int combatGraphics(MasterBoard* boardToPrint, int observerNumber, tile* tileAttacking, tile* tileBeingAttacked);
	//EnPrint Functions:///////////////////////////////////////////////////////

	//Non-Print Functions:////////////////////////////////////////////////////
	//Inputs
	int insertMinionInput(char* Input, MasterBoard* boardToInput);
	int deleteMinionInput(MasterBoard* boardToInput);
	int insertTileInput(char* Input, MasterBoard* boardToInput);
	int gameBoardInput(char* Input, MasterBoard* boardToInput);
	int menuInput(char* Input, MasterBoard* boardToInput);
	int waitingScreenInput(MasterBoard* boardToInput);
	int propertyMenuInput(char* Input, MasterBoard* boardToInput);
	int minionInput(char* Input, MasterBoard* boardToInput);


	//Interconnex
	int restartGame(MasterBoard* boardToInput);
	int exitToMainMenu(MasterBoard* boardToInput);
	int NextMission(MasterBoard* boardToInput);
	//End Non-Print Functions ////////////////////////////////////////////////

	std::string eventText = "";

	gameInputLayer status = gameBoard;
	specialKey minionVisibleStatus = showMinions;
	mainMenu* MainMenu;

	//MenuCrawlerObjects /////////////////////////
	int menuCursor = 0;
	std::vector <std::string> menuOptions { "End Turn","Save Game","Main Menu","Load Game","Restart Map","Return to Game"	};
	std::vector <std::string> factoryOptions{ "Infantry\t1000", "Specialist\t3000", "Cavalry\t4000","APC\t5000","Artillery\t6000", "Armor\t7000",
												"Anti-Aircraft\t8000", "Rocket Artillery\t15000", "Heavy Armor\t16000" };
	std::vector <std::string> airbaseOptions{ "Transport Copter\t5000", "Attack Copter\t9000", "Interceptor\t15000", "Bomber\t18000"};
	std::vector <std::string> portOptions{ "Gunboat\t6000", "Cruiser\t14000", "Landing Ship\t12000", "Submarine\t18000", "Battleship\t25000", "Aircraft Carrier\t25000" };
	//MenuCrawlerObjects /////////////////////////

	//SFML Objects		/////////////////////////
	sf::Texture* inputLayerTexture;
	sf::Font* inputLayerFont;
	sf::RenderWindow* inputLayerWindow;
	int menuLineTracker = 0;
	//SFML Objects end	/////////////////////////


private:

};


#endif /* INPUT_HPP__ */


