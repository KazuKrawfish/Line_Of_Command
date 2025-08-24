//Copyright 2022, Supercontinent Software Ltd.
//
//	mainMenu.h
//
/*
MainMenu is responsible for the highest level parts of the game, including starting a new game, saving and loading, etc.
InputLayer and masterBoard may call it, so they are somewhat circular-dependent.
*/

#ifndef MAINMENU_HPP__
#define MAINMENU_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include <SFML/Audio.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Network.hpp>
#include "button.hpp"
#include <filesystem>	
#include "battleLab.hpp"

enum mainMenuStatus { topmenu, playingMap };

//Determine if game is skirmish or campaign, and remote or local
enum interactionMethod { unchosen, localSkirmish, localCampaign, remoteHost, remoteClient };

//Forward declare global utilities
char getValidPlayerInput(sf::RenderWindow* myWindow);

#define USE_MUSIC 0

class mainMenu
{
public:

	//CONSTRUCTOR///////////////////////////////
	mainMenu(	sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* inputFont, sf::Font* boldInputFont, std::vector <sf::Texture>* topMenuButtonTextureArray,
				std::vector <sf::Texture>* gameMenuButtonTextureArray, std::vector <sf::Texture>* inputOtherTextureArray, sf::Music * inputMusicArray,
				std::vector <sf::Texture>* factionButtonTextureArray , std::string inputConfigFileName, std::string inputMapListName);
	//CONSTRUCTOR///////////////////////////////
	//Battle lab constructor////////////////////
	mainMenu(std::string inputConfigFileName, std::string inputMapListName);

	//Main Menu Interface //////////////////////
	int printTopMenuBackground();
	int printTopMenu();
	int topMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	int topMenuRemote(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer);
	//Main Menu Interface //////////////////////

	//Game session management///////////////////
	int introScreen(MasterBoard* boardToPlay, inputLayer* InputLayer);
	bool determineGameToLoad( MasterBoard* boardToPlay, inputLayer* InputLayer, std::ifstream* loadGameStream);
	int loadGameData(MasterBoard* boardToPrint, inputLayer* InputLayer, std::ifstream* saveGame);
	int setCharacteristics(MasterBoard* LoadBoard);
	int playGame(MasterBoard* boardToPlay, inputLayer* InputLayer);
	int saveGameData(std::string inputSaveGameName, MasterBoard* boardToPrint);
	//Game session management///////////////////

	//Other functions///////////////////////////
	sf::String playerInputString(sf::RenderWindow* myWindow, sf::Font* inputFont, sf::String AnnouncementString, int LineNumber, std::string backgroundType, int offsetWidth);
	//Other functions///////////////////////////

	const int menuTextSize = 20;
	std::string nextMissionName, missionBriefing;
	int briefingLineNumber = 0;

	//Offsets for fullscreen
	int MM_WIDTH_OFFSET = 0;
	int MM_HEIGHT_OFFSET = 0;

	//gameTurn represents the overall turn, which increments once everyone has cycled through one player turn.
	int gameTurn = 1;

	//Initialized to false because only if we select join is this true, and that will be dealt with in that function..
	bool awaitingFirstTurnThisSession = false;

	//skipOneInput indicates that we skip on player input, in order to print the next screen. (Back to main menu, etc.)
	bool skipOneInput = false;

	//interactionMethod refers to whether this will be skirmish or campaign game.
	interactionMethod gameType = unchosen;

	//mainMenuStatus will indicate what interface the player is interacting with.
	mainMenuStatus menuStatus = topmenu;

	bool veryFirstTurn = true;
	bool isItSaveGame = false;

	//Allows player to insert minions, change map, etc.
	bool editorMode = false;

	std::vector <compie> computerPlayerRoster;

	//Buttons////////////////////////////////////
	std::vector <Button> gameMenuButtons;
	std::vector <Button> topMenuButtons;
	std::vector <Button> factionChoiceButtons;
	//Buttons////////////////////////////////////

	//SFML Objects///////////////////////////////
	sf::RenderWindow* mywindow;
	sf::Texture* myTexture;
	sf::Font* myFont;
	sf::Font* myBoldFont;
	std::vector <sf::Texture>* otherGameTextures;
	sf::Music* musicArray;
	sf::Clock printTimer;
	bool printSecond = false;
	//SFML Objects///////////////////////////////

	//Battle Lab/////////////////////////////////
	std::string battleLabConfigFileName = "";
	std::string battleLabScenarioName = "";
	bool battleLabOn = false; 
	bool battleLabReset = false;
	int battleLabTurnLimit = 0;
	int battleLabWinningPlayer = 0;
	int runBattleLab(MasterBoard* boardToPlay, inputLayer* InputLayer, std::ifstream* configFile);
	//Battle Lab/////////////////////////////////

	//Remote play////////////////////////////////
	//Handle sending/receiving game updates between host and client
	int waitingForRemoteClient(MasterBoard* boardToPlay , inputLayer* InputLayer );
	int waitingForRemoteHost(MasterBoard* boardToPlay , inputLayer* InputLayer);
	int updateRemoteHost(MasterBoard* boardToPlay, inputLayer* InputLayer);

	//Similar to loadGameData and saveGameData
	int loadGameDataFromPacket(MasterBoard* boardToPrint, sf::Packet* gamePacket , inputLayer* InputLayer);
	int saveGameDataToPacket( MasterBoard* boardToPrint , sf::Packet* gamePacket , inputLayer* InputLayer);

	sf::IpAddress * remoteHostIPAddress;
	int remoteHostPortNumber = 0;
	int myPlayerNumber = 0;
	float updateRequestDelay = 2;
	bool remoteClientAwaitingFirstUpdate = false;
	//Remote play////////////////////////////////

};

#endif /* MAINMENU_HPP__ */