//Copyright 2022, Supercontinent Software Ltd.
//
//	inputLayer.hpp
//
/*
The inputLayer class is responsible for all player-related and interface activities. It interacts with the masterboard class, sending commands
to move minions and such, while receiving data from the board to print to screen for animations and static display. It also goes back up to mainmenu
when necessary, ie. when game ends or player wants to leave the current game.
*/

#ifndef INPUT_HPP__
#define INPUT_HPP__

#include "compie.hpp"
#include <SFML/Audio.hpp>


//Fwd dec
class compie;
class mainMenu;
class Button;


enum otherImages { startScreenBackground, topMenuBackground, startScreenStatement, topMenuBox };
enum gameInputLayer { gameBoard, menu, minionAction, propertyAction, waitingForNextLocalPlayer, compieTakingTurn, insertMinion, insertTile };
enum specialKey { showMinions, hideMinions };
enum rangeVisibleStatus { showRange, hideRange };
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

	inputLayer(mainMenu* inputMainMenu, sf::RenderWindow* myWindow, sf::Texture* gameTexture,
		sf::Font* cour, std::vector <sf::Sound>* inputSoundEffects, std::vector <Button>* inputMenuButtons, std::vector <sf::Texture>* statusTextures,
		sf::Music * inputGameMusic);

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
	int printStatusDialogBox(MasterBoard* boardToPrint);

	//Print Whole Screen
	int printStatus(MasterBoard* boardToPrint, int observerNumber);
	int printLowerScreen(MasterBoard* boardToPrint, int observerNumber);
	int printUpperScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation);
	int printScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation);
	int printSingleTile(int screenX, int screenY, int actualX, int actualY, MasterBoard* boardToPrint, int playerNumber, bool withinAnimation);

	//Graphics Print
	int movementGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToMove, int locationX, int locationY);
	int combatGraphics(MasterBoard* boardToPrint, int observerNumber, tile* tileAttacking, tile* tileBeingAttacked, bool splashGraphicsOn);
	int captureGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToCapture, int locationX, int locationY);
	int supplyGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToSupply, int locationX, int locationY);
	int repairGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToSupply, int locationX, int locationY);
	int trapGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionTrapped, int locationX, int locationY);
	//End Print Functions:///////////////////////////////////////////////////////

	//Non-Print Functions:////////////////////////////////////////////////////
	//Inputs
	int insertMinionInput(sf::Event* Input, MasterBoard* boardToInput);
	int deleteMinionInput(MasterBoard* boardToInput);
	int insertTileInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput);
	int gameBoardInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput);
	int menuInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput);
	int waitingScreenInput(MasterBoard* boardToInput);
	int propertyMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput);
	int minionInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput);


	//Interconnex
	int restartGame(MasterBoard* boardToInput);
	int exitToMainMenu(MasterBoard* boardToInput);
	int NextMission(MasterBoard* boardToInput);
	//End Non-Print Functions ////////////////////////////////////////////////

	std::string eventText = "";
	sf::String savedInsertMinionInput = "";

	int dialogBoxOpen = -1;
	gameInputLayer status = gameBoard;
	specialKey minionVisibleStatus = showMinions;
	rangeVisibleStatus showRangeStatus = hideRange;
	bool capsLockOn = false;
	mainMenu* MainMenu;


	//MenuCrawlerObjects /////////////////////////
	std::vector <Button>* menuButtons;
	std::vector <std::string> factoryOptions{ "Infantry", "Insurgent", "Operative", "Specialist", "Recon", "Technical",
												"APC", "IFV", "Artillery", "Armor", "Assault_Gun", "Upgunned_Armor",
												"Anti-Aircraft", "Rocket", "Heavy_Armor", "Modern_Armor", "Super_Heavy_Armor",
												"Victory_Launcher", "Cavalry"};
	std::vector <Button> factoryButtons;

	std::vector <std::string> airbaseOptions{ "Transport_Copter", "Attack_Copter", "Interceptor",
												"Advanced_Fighter", "Multirole", "Bomber" };
	std::vector <Button> airbaseButtons;

	std::vector <std::string> portOptions{ "Gunboat", "Cruiser", "Lander", "Submarine", "Battleship", "Aircraft_Carrier" };
	std::vector <Button> portButtons;

	std::vector <Button> statusButtons;
	//MenuCrawlerObjects /////////////////////////

	//SFML Objects		/////////////////////////
	std::vector <sf::Sound>* soundEffects;
	 sf::Music* gameMusic;
	bool soundsOn = true;
	int speedFactor = 1;		//1 is normal. 3 is "high". 10 is ultra fast.
	sf::Texture* inputLayerTexture;
	sf::Font* inputLayerFont;
	sf::RenderWindow* inputLayerWindow;
	int menuLineTracker = 1;	//Starts at offset of one.
	//SFML Objects end	/////////////////////////


private:

};

#endif /* INPUT_HPP__ */