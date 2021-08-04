#ifndef MAINMENU_HPP__
#define MAINMENU_HPP__

#include "Minion.hpp"
#include "Tile.hpp"
#include "Cursor.hpp"
#include "MasterBoard.hpp"
#include <curses.h>

enum mainMenuStatus { topmenu,  playingMap, waitingForRemotePlayer };
enum interactionMethod {unchosen, local, remote};

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
	std::string playerNames[NUMBEROFPLAYERS+1];
	std::string sessionName = "localsession";
	std::string myPlayerName = "~";
	int gameLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame);
	int scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer);
	int setCharacteristics(MasterBoard* LoadBoard);
	int gameSave(std::string inputSaveGameName, MasterBoard* boardToPrint);
	int scenarioLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame, bool isSaveGame);
	int playGame(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int printTopMenu();
	int topMenuInput(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int topMenuJoin(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer, WINDOW* mywindow);
	int printWaitingScreen();
	int waitForRemotePlayer(MasterBoard* boardToSave, inputLayer* InputLayer, compie* ComputerPlayer);
	
	
	//Multiplayer specific functions
	//The below pushes the saved game to the remote server, and should be used at end of player turn.
	int multiplayerPushSaveGame(MasterBoard* boardToSave);
	//The below pulls from remote and opens the save game if there is an update. This should be used while player is waiting for others to finish their turn.
	int multiplayerPullSaveGame();


};

#endif /* MAINMENU_HPP__ */
