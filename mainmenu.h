//Copyright 2022, Supercontinent Software Ltd.
//
//	mainMenu.h
//

#ifndef MAINMENU_HPP__
#define MAINMENU_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include <SFML/Audio.hpp>


enum mainMenuStatus { topmenu,  playingMap, waitingForRemotePlayer };
enum interactionMethod {unchosen, localSkirmish, localCampaign, remote};

//Forward declare global utilities
char playCharInput(sf::RenderWindow* myWindow);



class mainMenu
{
public:

	//gameTurn represents the overall turn, which increments once everyone has cycled through one player turn.
	int gameTurn = 1;
	//Initialized to false because only if we select join is this true, and that will be dealt with in that function..
	bool awaitingFirstTurnThisSession = false;
	//skipOneInput indicates that we skip on player input, in order to print the next screen. (Back to main menu, etc.)
	bool skipOneInput = false;
	//interactionMethod refers to whether this will be remote or local game.
	interactionMethod gameType = unchosen;
	//mainMenuStatus will indicate what interace the player is interacting with.
	mainMenuStatus menuStatus = topmenu;
	bool veryFirstTurn = true;
	bool isItSaveGame = false;
	bool debugMode = false;
	std::string sessionName = "localsession";
	std::string myPlayerName = "~";
	int scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer);
	int setCharacteristics(MasterBoard* LoadBoard);
	int gameSave(std::string inputSaveGameName, MasterBoard* boardToPrint);
	int introScreen(MasterBoard* boardToPlay, inputLayer* InputLayer);
	int gameLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, std::ifstream* saveGame);
	int playGame(MasterBoard* boardToPlay, inputLayer* InputLayer);
	
	int printWaitingScreen();
	int waitForRemotePlayer(MasterBoard* boardToSave, inputLayer* InputLayer);
	sf::String playerInputString(sf::RenderWindow* myWindow, sf::Font* inputFont, sf::String AnnouncementString, int LineNumber);
	
	//CONSTRUCTOR
	mainMenu(sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* cour, 
		sf::Texture* inputMenuWallpaper, sf::Texture* inputStartWallPaper, sf::Texture* inputTopMenu ,
		sf::Texture* inputstartScreenStatement, sf::Music* inputIntroMusic);
	//CONSTRUCTOR

	//Multiplayer specific functions
	//The below pushes the saved game to the remote server, and should be used at end of player turn.
	int multiplayerPushSaveGame(MasterBoard* boardToSave);
	//The below pulls from remote and opens the save game if there is an update. This should be used while player is waiting for others to finish their turn.
	int multiplayerPullSaveGame();
	const int menuTextSize = 20;
	std::string nextMissionName, missionBriefing;
	int briefingLineNumber = 0;

	//Main Menu Interface ///////////////////////
	int printTopMenu();
	int topMenuInput(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuJoin(MasterBoard* boardToPlay, inputLayer* InputLayer);
	int menuIterator = 0;
	std::vector <std::string> topMenuOptions{ "Load Game","New Game","Join Remote Game","Editor Mode"};
	//Main Menu Interface ///////////////////////



	std::vector <compie> computerPlayerRoster;

	//SFML Objects		/////////////////////////
	sf::RenderWindow* mywindow;
	sf::Texture* myTexture;
	sf::Texture* menuWallPaper;
	sf::Texture* startWallPaper;
	sf::Texture* startScreenStatement;
	sf::Texture* topMenuTexture;
	sf::Font* myFont;

	sf::Music* introMusic;
	//SFML Objects end	/////////////////////////


};

#endif /* MAINMENU_HPP__ */
