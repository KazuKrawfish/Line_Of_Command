#include "Minion.hpp"
#include "MasterBoard.hpp"
#include "inputLayer.hpp"
#include "mainmenu.h"
#include <iostream>
#include <ctype.h>
#include <fstream>
#include "compie.hpp"

class inputLayer;
class MasterBoard;

mainMenu::mainMenu(WINDOW* myWindow)
{
	playerNames.resize(5); //May be more but for now it works
	mywindow = myWindow;

}
//Gameplay note:
//As currently constructed, host for a new scenario must be player 1. Anyone join will play the turn assigned based on their player name.
//Thus host never says "his" player name, because that is the first one inputted!
//Similarly, if you host for a loaded game, you are playing as whose turn it was, so you should only host for whose turn was current when saved.
//Finally, for host, you currently must start with a unique session name, and they will be cleared every day to start.
 
//Messes up minions!
//Still need to add them after.
//This lumps all the terrain at the end, need to find a way to count neighbors and based on those, distribute tiles.
int mainMenu::scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer)
{
	int BOARD_SIZE = LoadBoard->BOARD_WIDTH * LoadBoard->BOARD_HEIGHT;
	LoadBoard->clearBoard(InputLayer);

	int numberOfHillsAndMountains = BOARD_SIZE / 10;	//Actual amount you want to see.
	int mountainWeight = numberOfHillsAndMountains;		//This will change throughout function
	int numberOfForests = BOARD_SIZE / 10;
	int forestWeight = numberOfForests;
	int plainsWeight = BOARD_SIZE - numberOfHillsAndMountains - numberOfForests;

	for (int x = 0; x < LoadBoard->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < LoadBoard->BOARD_HEIGHT; y++)
		{
			LoadBoard->Board[x][y].clearTile();
			int mountainChance = (rand() % 100) * mountainWeight;
			int forestChance = (rand() % 100) * forestWeight;
			int plainsChance = (rand() % 100) * plainsWeight;
			if (mountainChance > forestChance && mountainChance > plainsChance)
			{
				if ((rand() % 100) < 50)
				{
					LoadBoard->Board[x][y].symbol = 'M';
					LoadBoard->Board[x][y].description = "Mountain.";
					LoadBoard->Board[x][y].defenseFactor = 1.4;
				}
				else
				{
					LoadBoard->Board[x][y].symbol = '^';
					LoadBoard->Board[x][y].description = "Hill.";
					LoadBoard->Board[x][y].defenseFactor = 1.1;
				}
				mountainWeight--;
			}
			else if (forestChance > plainsChance)
			{
				LoadBoard->Board[x][y].symbol = '+';
				LoadBoard->Board[x][y].description = "Forest.";
				LoadBoard->Board[x][y].defenseFactor = 1.2;
				forestWeight--;
			}
			else plainsWeight--;
		}
	}
	return 0;
}

int mainMenu::setCharacteristics(MasterBoard* LoadBoard)
{
	for (int x = 0; x < LoadBoard->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < LoadBoard->BOARD_HEIGHT; y++)
		{
			LoadBoard->Board[x][y].locationX = x;
			LoadBoard->Board[x][y].locationY = y;

			LoadBoard->Board[x][y].setCharacterstics();
		}
	}
	return 0;
}

int mainMenu::gameSave(std::string inputSaveGameName, MasterBoard* boardToPrint)
{
	std::ofstream saveGame(inputSaveGameName);

	//Unique to save_game vs scenario. First save number of players, and then player names (User names):
	saveGame << "Number_of_players_below:" << std::endl;
	saveGame << boardToPrint->NUMBEROFPLAYERS << std::endl;
	saveGame << "Names_and_treasury_totals_below:" << std::endl;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		saveGame << playerNames[i] << std::endl;
		saveGame << boardToPrint->treasury[i] << std::endl;
	}
	
	//Then save the game turn.
	saveGame << "Game_turn_below:" << std::endl;
	saveGame << gameTurn << std::endl;
	
	//Then save the map size:
	saveGame << "Map_width_below:" << std::endl;
	saveGame << boardToPrint->BOARD_WIDTH << std::endl;
	saveGame << "Map_height_below:" << std::endl;
	saveGame << boardToPrint->BOARD_HEIGHT << std::endl;

	//Then save whos turn it is:
	saveGame << "Player_whose_turn_it_is_below:" << std::endl;
	saveGame << boardToPrint->playerFlag << std::endl;

	//Terrain save:
	//Iterate through board and save the exact symbol.
	//Saving symbol and controller separately for ease of edit and viewing.
	//Only pain point is do mass control-edits, but viewability is protected.
	saveGame << "Map_terrain_below:" << std::endl;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			saveGame << boardToPrint->Board[x][y].symbol;
		}
		saveGame << std::endl;
	}

	//Iterate through board and save the controller.
	saveGame << "Property_controller_number_below:" << std::endl;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			saveGame << boardToPrint->Board[x][y].controller;

		}
		saveGame << std::endl;
	}

	//Note the number of minions:
	saveGame << "Total_minions_below:" << std::endl;
	saveGame << boardToPrint->totalNumberOfMinions << std::endl;

	//Go through entire minionRoster and save each value associated with each minion, one line per minion.
	saveGame << "Minion_roster_below_(XCoord,YCoord,Team,Seniority,Status,Health,Veterancy,CapPoints,TransportStatus,Fuel,Ammo):" << std::endl;
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//First cycle through all non transported minions and do them
		if (boardToPrint->minionRoster[i] != NULL && boardToPrint->minionRoster[i]->transporter == NULL)
		{
			int beingTransported = 0;

		saveGame << boardToPrint->minionRoster[i]->type << " "
			<< boardToPrint->minionRoster[i]->locationX << " "
			<< boardToPrint->minionRoster[i]->locationY << " "
			<< boardToPrint->minionRoster[i]->team << " "
			<< boardToPrint->minionRoster[i]->seniority << " "
			<< int(boardToPrint->minionRoster[i]->status) << " "
			<< int(boardToPrint->minionRoster[i]->health) << " "
			<< int(boardToPrint->minionRoster[i]->veterancy) << " "
			<< int(boardToPrint->Board[boardToPrint->minionRoster[i]->locationX][boardToPrint->minionRoster[i]->locationY].capturePoints) <<" "
			<< beingTransported << " "
			<< boardToPrint->minionRoster[i]->currentFuel << " "
			<< boardToPrint->minionRoster[i]->currentPriAmmo << " "
			<< boardToPrint->minionRoster[i]->currentSecAmmo << " "
			<< std::endl;

		}
	}

	//Then do again for all transported minions - they have to be last or loading will  cause them to NULL dereference when they
	//Are assigned a transport that hasn't been initialized yet.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If being transported, save the location of the transporter.
		int beingTransported = 0;
		if (boardToPrint->minionRoster[i] != NULL && boardToPrint->minionRoster[i]->transporter != NULL)
		{
			beingTransported = 1;
			saveGame << boardToPrint->minionRoster[i]->type << " "
				<< boardToPrint->minionRoster[i]->transporter->locationX << " "
				<< boardToPrint->minionRoster[i]->transporter->locationY << " "
				<< boardToPrint->minionRoster[i]->team << " "
				<< boardToPrint->minionRoster[i]->seniority << " "
				<< int(boardToPrint->minionRoster[i]->status) << " "
				<< int(boardToPrint->minionRoster[i]->health) << " "
				<< int(boardToPrint->minionRoster[i]->veterancy) << " "
				<< 20 << " "		//Default capture points to avoid breaking
				<< beingTransported << " "
				<< boardToPrint->minionRoster[i]->currentFuel << " " 
				<< boardToPrint->minionRoster[i]->currentPriAmmo << " "
				<< boardToPrint->minionRoster[i]->currentSecAmmo << 
				std::endl;
		}
	}
	saveGame.close();
	return 1;
}

//Scenario Load is for new scenarios from a non-saved game.
//Game Load is for saved games, which already have player data.
int mainMenu::gameLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame)
{ 
	std::string ThrowawayString;
	

	//First load number of players from save
	//Remember to have one exta for ease of access (Player "0" is blank)
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->NUMBEROFPLAYERS;
	playerNames.resize(boardToPrint->NUMBEROFPLAYERS+1);
	boardToPrint->treasury.resize(boardToPrint->NUMBEROFPLAYERS + 1);

	//Unique to save_game vs scenario. Load player names (User names):
	*saveGame >> ThrowawayString;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		*saveGame >> playerNames[i];
		*saveGame >> boardToPrint->treasury[i];
	}

	//Then load the game turn.
	*saveGame >> ThrowawayString;
	*saveGame >> gameTurn;
		
	//Although I don't love the name scenarioLoad, this is performing the same action as scenarioLoad so we're using it as good practice.
	scenarioLoad(boardToPrint, InputLayer, ComputerPlayer, saveGame,  true);
	return 0; 

}

//Load scenario game and initialize the board with its contents.
int mainMenu::scenarioLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer, std::ifstream* saveGame, bool isSaveGame) {

	veryFirstTurn = true;

	//Clear board in case scenario load was called by player menu later in game.
	boardToPrint->clearBoard(InputLayer);
	InputLayer->computerPlayer = ComputerPlayer;
	ComputerPlayer->InputLayer = InputLayer;

	//If this is a new game, clear the treasury.
	//Otherwise leave the values from the loadGame portion.
	if (isSaveGame == false) 
	{
		//Reset treasury
		for (int i = 0; i < boardToPrint->NUMBEROFPLAYERS + 1; i++)
		{
			boardToPrint->treasury[i] = 0;
		}
	}

	std::string ThrowawayString;
	//First load the map size:
	//Ideally we can create new vector or whatever to have different map size:
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->BOARD_WIDTH;
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->BOARD_HEIGHT;

	//Resize the map based on new data.
	//Not sure why we have to give an additional line but it segfaulted otherwise
	boardToPrint->Board.resize(boardToPrint->BOARD_WIDTH + 1);
	for (int i = 0; i < boardToPrint->BOARD_WIDTH; i++)
	{
		boardToPrint->Board[i].resize(boardToPrint->BOARD_HEIGHT);
	}

	boardToPrint->myPathMap.resize(boardToPrint->BOARD_WIDTH + 1);
	boardToPrint->compiePathMap.resize(boardToPrint->BOARD_WIDTH + 1);
	for (int i = 0; i < boardToPrint->BOARD_WIDTH; i++)
	{
		boardToPrint->myPathMap[i].resize(boardToPrint->BOARD_HEIGHT);
		boardToPrint->compiePathMap[i].resize(boardToPrint->BOARD_HEIGHT);
	}

	//Then load player data:
	//This is the current player whos turn it is:
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->playerFlag;

	//Then load map
	*saveGame >> ThrowawayString;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			*saveGame >> boardToPrint->Board[x][y].symbol;

			//Also resize for withinVision
			boardToPrint->Board[x][y].withinVision.resize(boardToPrint->NUMBEROFPLAYERS + 1);
			for (int n = 0; n < boardToPrint->NUMBEROFPLAYERS + 1; n++)
				boardToPrint->Board[x][y].withinVision[n] = false;

		}
	}
	setCharacteristics(boardToPrint);

	//This is for properties.
	char inputProperties;
	*saveGame >> ThrowawayString;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			*saveGame >> inputProperties;
			boardToPrint->Board[x][y].controller = (int(inputProperties)) - 48;
		}
	}
	//Then load minion data:
	*saveGame >> ThrowawayString;
	int numberOfMinions;
	*saveGame >> numberOfMinions;
	int health, locationX, locationY, team, seniority, status, veterancy, capturePoints, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo;
	char type;
	*saveGame >> ThrowawayString;

	//Initialize all minions.
	for (int y = 0; y < numberOfMinions; y++)
	{
		*saveGame >> type
			>> locationX
			>> locationY
			>> team
			>> seniority
			>> status
			>> health
			>> veterancy
			>> capturePoints
			>> beingTransported
			>> inputFuel
			>> inputPriAmmo
			>> inputSecAmmo;

		//Regardless of transport status, we pass the saved location- it either represents the minion's position
		//Or the transport's position.
		boardToPrint->createMinion(type, locationX, locationY, team, health, status, veterancy, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo);
	
		//Set capture status.
		if (capturePoints != 20)
		{
			boardToPrint->Board[locationX][locationY].minionOnTop->isCapturing = true;
			boardToPrint->Board[locationX][locationY].capturePoints = capturePoints;
		}


	}

	saveGame->close();
	return 1;
}

int mainMenu::playGame(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer)
{
	wclear(mywindow);
	waddstr(mywindow,"Line of Command\n");
	waddstr(mywindow,"Copyright 2021, Park Family Software Laboratory (ParkLab)\n");
	waddstr(mywindow,"Press any key to continue.");
	wrefresh(mywindow);
	
	char Input = '~';

	//Run as long as the user wants. Infinite while loop.
	while (true)		
	{
		//This is a utility function to prevent "hanging" screens
		if (skipOneInput == true) 
		{
			skipOneInput = false;
			Input = '~';
		}
		else 
		{
			Input = wgetch(mywindow);
		}


		//My attempt to test escape character.
		/*if (Input == 27) 
		{
			waddstr(mywindow,"BSLALIJASLDJ");
		}*/


		//If we're still on top menu, do that instead of game/inputLayer.
		if (menuStatus == topmenu)
		{
			printTopMenu();
			topMenuInput(&Input, boardToPlay, InputLayer, ComputerPlayer);
		}
		else

		//If we are waiting for remote, do that instead of game/inputLayer.
		if (menuStatus == waitingForRemotePlayer)
		{
			printWaitingScreen();
			waitForRemotePlayer(boardToPlay, InputLayer, ComputerPlayer);
		}
		else
		
		if (menuStatus == playingMap)
		{
			//Always call upkeep once before play commences.
			if (veryFirstTurn == true)
			{		
				boardToPlay->upkeep(InputLayer);
				veryFirstTurn = false;
			}

			if (InputLayer->status == gameBoard)
			{
				InputLayer->gameBoardInput(&Input, boardToPlay);
			}
			else if (InputLayer->status == minionAction)
			{
				InputLayer->minionInput(&Input, boardToPlay);
			}
			else if (InputLayer->status == menu)
			{
				InputLayer->menuInput(&Input, boardToPlay);
			}
			else if (InputLayer->status == propertyAction)
			{
				InputLayer->propertyMenuInput(&Input, boardToPlay);
			}
			else if (InputLayer->status == waitingForNextLocalPlayer)
			{
				InputLayer->waitingScreenInput(boardToPlay);
			}
			else if (InputLayer->status == insertMinion)
			{
				InputLayer->insertMinionInput(&Input, boardToPlay);
			}
			else if (InputLayer->status == insertTile)
			{
				InputLayer->insertTileInput(&Input, boardToPlay);
			}

			//Computer takes turn if it is his turn to do so.
			//Note that this doesn't deal with "status".
			if (playerNames[boardToPlay->playerFlag] == "compie")
			{
				ComputerPlayer->takeMyTurn(boardToPlay);
				
			}

			//This prints the screen AFTER the latest input has taken effect.
			//Is this messing with remote play? Not sure.
			boardToPlay->checkWindow();
			InputLayer->printScreen(boardToPlay, boardToPlay->playerFlag);

		}
	}
	return 0;
}

int mainMenu::topMenuInput(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer)
{
	*Input = wgetch(mywindow);

	//If user wants to load a map.
	if (*Input == 'l') 
	{
		topMenuLoad(Input, boardToPlay, InputLayer, ComputerPlayer);
		skipOneInput = true;
		InputLayer->status = gameBoard;
	} 
	else
	//Newgame
	if (*Input == 'n')
	{
 		topMenuNew(Input, boardToPlay, InputLayer, ComputerPlayer);
		skipOneInput = true;
		InputLayer->status = gameBoard;
	}
	else
	//Join a remote game
	if (*Input == 'j')
	{
		topMenuJoin(boardToPlay, InputLayer, ComputerPlayer);
		skipOneInput = true;
		InputLayer->status = gameBoard;
	}else
	//Toggle debug mode.
	if (*Input == '~')
	{
		if (debugMode == false)
			debugMode = true;
		else if (debugMode == true)
			debugMode = false;
	}

	return 0;
}

int mainMenu::printTopMenu()
{
	wclear(mywindow);
	waddstr(mywindow,"Main Menu\n");
	waddstr(mywindow,"Load saved game (l) | Start new game (n) | Join remote game (j) \n");
	
	if(debugMode == false)
		waddstr(mywindow,"To enter debug mode, press '~' \n");
	else waddstr(mywindow, "In debug mode. To leave debug mode, press '~' \n");
	
	wrefresh(mywindow);
	return 0;
}

int mainMenu::printWaitingScreen() 
{
	wclear(mywindow);
	waddstr(mywindow,"Waiting for other player(s) \n");
	wrefresh(mywindow);
	return 0;
}

int mainMenu::topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer)
{	
	//Determine if game is remote or local.
	waddstr(mywindow,"Local (l) or remote (r) game?\n");
	gameType = unchosen;
	while (gameType == unchosen)
	{
		*Input = wgetch(mywindow);

		if (*Input == 'l')
		{
			wclear(mywindow);
			waddstr(mywindow,"Local game selected.\n");
			wrefresh(mywindow);
			gameType = local;
		}
		else if (*Input == 'r') 
		{
			wclear(mywindow);
			waddstr(mywindow,"Remote game selected.\n");
			wrefresh(mywindow);
			gameType = remote;
		}
	}

	//Load the actual map
	std::ifstream newGameMap;
	char scenarioToLoad[100];
	char* pointToScenarioName = &scenarioToLoad[0];
	bool loadsuccessful = false;

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		waddstr(mywindow,"Choose which scenario to load (Case sensitive): \n");
		wgetstr( mywindow,pointToScenarioName);
		std::string newScenario = scenarioToLoad;
		newGameMap.open(".\\scenarios\\"+ newScenario + ".txt");
		if (newGameMap.is_open())
		{
			waddstr(mywindow,"Successfully loaded!\n");
			loadsuccessful = true;
		}
		else
		{
			waddstr(mywindow,"Could not load scenario. Please check that it exists and the right spelling was used.\n");

		}
	
	}
	//Actually load scenario. Initialize board, etc.
	scenarioLoad(boardToPlay, InputLayer, ComputerPlayer, &newGameMap, false);
	newGameMap.close();

	std::ifstream loadSession;
	bool sessionCreationSuccessful = false;
	//Prompt user for session name and ensure it is unique:
	//IF IT IS REMOTE
	while (gameType == remote && sessionCreationSuccessful == false)
	{
		//Get attempted session name:
		waddstr(mywindow,"Input session name:\n");
		char inputSessionName[100];
		wgetstr( mywindow,&inputSessionName[0]);
		sessionName = inputSessionName;

		//Now append filepath and atempt to open. If open succeeds, this session already exists!
		loadSession.open(".\\multiplayer\\lineOfCommandMultiplayer\\" + sessionName + "_save.txt");

		if (loadSession.is_open())
		{
			waddstr(mywindow,"This session already exists. Enter a different session name. \n");

		}
		else
		{
			waddstr(mywindow,"This session is unique.\n");
			sessionCreationSuccessful = true;
		}

	}



	//Determine player names for number of players
	//Currently this is 2:
	//We added one to the array, just like treasury, for easy access.
	char inputName[100];
	char outputName[100];
	for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS ; i++)
	{
		snprintf(&outputName[0], 100, "Input Player %d's name: \n", i);
		waddstr(mywindow,&outputName[0]);
		wgetstr( mywindow,&inputName[0]);
		playerNames[i] = inputName;
	}

	//Host is always player one for a new game.
	myPlayerName = playerNames[1];


	//Determines if they print or not.
	if (playerNames[2] == "compie")
	{
		boardToPlay->isItSinglePlayerGame = true;
	}
	else boardToPlay->isItSinglePlayerGame = false;

	menuStatus = playingMap;

	return 0;
}

int mainMenu::topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer)
{ 
	//Determine if game is remote or local.
	waddstr(mywindow,"Local (l) or remote(r) game?\n");
	gameType = unchosen;
	while (gameType == unchosen)
	{
		*Input = wgetch(mywindow);

		if (*Input == 'l')
		{
			//wclear(mywindow);
			//waddstr(mywindow,"Local game selected.\n");
			//wrefresh(mywindow);
			gameType = local;
		}
		else if (*Input == 'r')
		{
			//wclear(mywindow);
			//waddstr(mywindow,"Remote game selected.\n");
			//wrefresh(mywindow);
			gameType = remote;
		}
	}

	//Load the actual save game
	//This garbage is necessary because for some reason, getstr will not play with strings, even if you provide & and [0]
	std::ifstream loadGameSave;
	std::string saveToLoad;
	char scenarioToLoad[100];
	char* pointToSaveName = &scenarioToLoad[0];
	bool loadsuccessful = false;

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		wclear(mywindow);

		if (gameType == local)
		{
			waddstr(mywindow, "Local game selected.\n");
		}
		else if (gameType == remote)
		{
			waddstr(mywindow, "Remote game selected.\n");
		}

		waddstr(mywindow,"Choose which save game to load (Case sensitive. Do not use _save portion of save.): \n");
		wrefresh(mywindow);

		wgetstr( mywindow,&pointToSaveName[0]);
		saveToLoad = scenarioToLoad;
		saveToLoad += "_save.txt";
		loadGameSave.open(".\\savegames\\" + saveToLoad);
		if (loadGameSave.is_open())
		{
			waddstr(mywindow,"Save game successfully loaded!\n");
			loadsuccessful = true;
		}
		else
		{
			waddstr(mywindow,"Could not load save game. Please check that it exists and the right spelling was used.\n");

		}

	}
	//Actually load scenario. Initialize board, etc.
	gameLoad(boardToPlay, InputLayer, ComputerPlayer, &loadGameSave);
	loadGameSave.close();

	std::ifstream loadSession;
	bool sessionCreationSuccessful = false;
	//Prompt user for session name and ensure it is unique:
	//IF IT IS REMOTE
	while (gameType == remote && sessionCreationSuccessful == false)
	{
		//Get attempted session name:
		waddstr(mywindow,"Input session name:\n");
		char inputSessionName[100];
		wgetstr( mywindow,&inputSessionName[0]);
		sessionName = inputSessionName;

		//Now append filepath and atempt to open. If open succeeds, this session already exists!
		sessionName += "_save.txt";
		loadSession.open(".\\multiplayer\\lineOfCommandMultiplayer\\" + sessionName);

		if (loadSession.is_open())
		{
			waddstr(mywindow,"This session already exists. Enter a different session name. \n");
			
		}
		else
		{
			waddstr(mywindow,"This session is unique.\n");
			sessionCreationSuccessful = true;
		}

	}

	//Host will play as whoever's turn it is.
	myPlayerName = playerNames[boardToPlay->playerFlag];

	//Determines if they print or not.
	if (playerNames[2] == "compie")
	{
		boardToPlay->isItSinglePlayerGame = true;
	}
	else boardToPlay->isItSinglePlayerGame = false;



	menuStatus = playingMap;
	return 0;
}

int mainMenu::topMenuJoin(MasterBoard* boardToPlay, inputLayer* InputLayer, compie* ComputerPlayer) 
{
	//Set flags to remote and waiting for first turn for the session.
	gameType = remote;
	awaitingFirstTurnThisSession = true;

	//Get session name:
	char inputSessionName[100];
	waddstr(mywindow,"Input session name: (Case sensitive)\n");
	wgetstr( mywindow,&inputSessionName[0]);
	sessionName = inputSessionName;

	char inputPlayerName[100];
	waddstr(mywindow,"Input your player name: (Case sensitive)\n");
	wgetstr( mywindow,&inputPlayerName[0]);
	myPlayerName = inputPlayerName;

	waitForRemotePlayer(boardToPlay, InputLayer, ComputerPlayer);

	return 0;
}

int mainMenu::multiplayerPushSaveGame(MasterBoard* boardToSave)
{
	//Should save the current game into the multiplayer folder, and then call the bat file to push to remote.
	//Ensure game is saved into the right directory.
	//This is different from local saves. Thus must handle save names before passing to saveGame.
	std::string saveGameName = ".\\multiplayer\\lineOfCommandMultiplayer\\";
	saveGameName += sessionName;
	saveGameName += "_save.txt";
	gameSave(saveGameName, boardToSave);

	//Call .bat to push.
	std::string batchPushCommand = "pushSaveToServer.bat";
	system(&batchPushCommand[0]);

	return 0;

}

int mainMenu::multiplayerPullSaveGame() 
{
	//Should attempt to pull and then compare the pulled filename against some previous quantity, for now, gameturn and playerturn.

	//Call pullSaveFromServer.bat to pull.
	std::string batchPullCommand = "pullSaveFromServer.bat";
	system(&batchPullCommand[0]);
	return 0;
}

int mainMenu::waitForRemotePlayer(MasterBoard* boardToSave, inputLayer* InputLayer, compie* ComputerPlayer)
{
	waddstr(mywindow,"waitForRemotePlayer \n");
	
	//We are waiting for updated save game.
	bool isItMyTurnYet = false;
	   
	while (isItMyTurnYet == false)
	{
		//The batch file will wait 5 real world seconds.
		multiplayerPullSaveGame();

		//Then load game and see if it's changed at all.
		std::ifstream loadGameSave;
		loadGameSave.open(".\\multiplayer\\lineOfCommandMultiplayer\\" + sessionName + "_save.txt");
		
		//Current rule is you should not be attempting to join a session that already exists but was abandoned.
		//The host must always start with a unique session name, so someone waiting to join just waits to open that particular file.
		//Have to successfully open the game file before we can examine it!
		if (loadGameSave.is_open() == true) 
		{
			gameLoad(boardToSave, InputLayer, ComputerPlayer, &loadGameSave);
			
			//If it is "my turn" then we are good to go.
			if ( myPlayerName == playerNames[boardToSave->playerFlag] )
			{
				isItMyTurnYet = true;
			}
		}

	}

	menuStatus = playingMap;
	return 0;
}
