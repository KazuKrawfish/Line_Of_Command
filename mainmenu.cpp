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
	mywindow = myWindow;
	computerPlayerRoster.resize(1);	//Arbitray resize to prevent exceptions.
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

	saveGame << "Savegame_YesNo" << std::endl;
	saveGame << 1 << std::endl;

	//Unique to save_game vs scenario. First save number of players, and then player names (User names):
	saveGame << "Number_of_players_below:" << std::endl;
	saveGame << boardToPrint->NUMBEROFPLAYERS << std::endl;


	//*********************************************//
	//Input "Mission load" data here
	saveGame << "Mission_On_Off" << std::endl;		//Mission or not
	saveGame << boardToPrint->missionFlag << std::endl;

	saveGame << "Campaign_Name" << std::endl;		//Mission or not
	saveGame << boardToPrint->campaignName << std::endl;

	saveGame << "ScenarioOrMissionName" << std::endl;
	saveGame << boardToPrint->scenarioOrMissionName << std::endl;	//Must be exactly the same as the txt file's name

	saveGame << "TurnLimit_Or_0_ForOff" << std::endl;		//TurnLength - either 0 for no max turn length, or int to indicate length
	saveGame << boardToPrint->missionTurnLength << std::endl;

	saveGame << "Who_wins_after_turn_limit" << std::endl;		//Indicates who will win if turn length is met.
	saveGame << boardToPrint->whoHoldsOut << std::endl;

	saveGame << "Name_of_next_mission_(Same_Directory)" << std::endl;		//Name of next mission
	saveGame << nextMissionName << std::endl;

	saveGame << "Mission_Briefing" << std::endl;	//String with mission info
	saveGame << missionInfo << std::endl;

	//End Mission Data Load
	//*********************************************//



	saveGame << "Player_Data:_Name_PlayerType_StillAlive_Treasury" << std::endl;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		saveGame << boardToPrint->playerRoster[i].name << std::endl;
		saveGame << int(boardToPrint->playerRoster[i].playerType) << std::endl;
		saveGame << int(boardToPrint->playerRoster[i].stillAlive) << std::endl;
		saveGame << boardToPrint->playerRoster[i].treasury << std::endl;
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

//Load scenario game and initialize the board with its contents.
int mainMenu::gameLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, std::ifstream* saveGame) 
{
	veryFirstTurn = true;

	//Clear board in case scenario load was called by player menu later in game.
	boardToPrint->clearBoard(InputLayer);

	bool isItSaveGame = false;

	std::string ThrowawayString;

	*saveGame >> ThrowawayString;
	*saveGame >> isItSaveGame;

	//First load number of players from save
	//Remember to have one exta for ease of access (Player "0" is blank)
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->NUMBEROFPLAYERS;
	boardToPrint->playerRoster.resize(boardToPrint->NUMBEROFPLAYERS + 1);

	//*********************************************//
	//Input "Mission load" data here
	*saveGame >> ThrowawayString;		//Campaign_game
	*saveGame >> boardToPrint->missionFlag;

	*saveGame >> ThrowawayString;		//Campaign Name - used to navigate menu for next level
	*saveGame >> boardToPrint->campaignName;

	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->scenarioOrMissionName;	//Must be exactly the same as the txt file's name

	*saveGame >> ThrowawayString;		//TurnLength - either 0 for no max turn length, or int to indicate length
	*saveGame >> boardToPrint->missionTurnLength;

	*saveGame >> ThrowawayString;		//Indicates who will win if turn length is met.
	*saveGame >> boardToPrint->whoHoldsOut;

	*saveGame >> ThrowawayString;		//Name of next mission
	*saveGame >> nextMissionName;

	*saveGame >> ThrowawayString;		//String with mission info
	*saveGame >> missionInfo;

	//End Mission Data Load
	//*********************************************//


	//Unique to save_game vs scenario. Load player names (User names):
	*saveGame >> ThrowawayString;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		int playerType = 0;

		*saveGame >> boardToPrint->playerRoster[i].name;		//For new game this should be ~
		*saveGame >> playerType;
		*saveGame >> boardToPrint->playerRoster[i].stillAlive;
		*saveGame >> boardToPrint->playerRoster[i].treasury;

		if (playerType == 0)
			boardToPrint->playerRoster[i].playerType = humanPlayer;
		else boardToPrint->playerRoster[i].playerType = computerPlayer;

	}

	//Then load the game turn.
	*saveGame >> ThrowawayString;
	*saveGame >> gameTurn;


	//If this is a new game, clear the treasury.
	//(If going from a game in middle of play, to new game).
	//Otherwise leave the values from the loadGame portion.
	//Clear treasury not necessary, just use whatever is in the scenario.

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
			char checkonMe = 'L';
			*saveGame >> checkonMe;
			boardToPrint->Board[x][y].symbol = checkonMe;

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

int mainMenu::playGame(MasterBoard* boardToPlay, inputLayer* InputLayer)
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

		//If we're still on top menu, do that instead of game/inputLayer.
		if (menuStatus == topmenu)
		{
			printTopMenu();
			topMenuInput(&Input, boardToPlay, InputLayer);
		}
		else

		//If we are waiting for remote, do that instead of game/inputLayer.
		if (menuStatus == waitingForRemotePlayer)
		{
			printWaitingScreen();
			waitForRemotePlayer(boardToPlay, InputLayer);
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
			if ( boardToPlay->playerRoster[boardToPlay->playerFlag].playerType == computerPlayer && computerPlayerRoster[boardToPlay->playerFlag].gameOver == false )
			{
				computerPlayerRoster[boardToPlay->playerFlag].takeMyTurn(boardToPlay);
				
			}

			//This prints the screen AFTER the latest input has taken effect.
			//Is this messing with remote play? Not sure.
			boardToPlay->checkWindow();
			InputLayer->printScreen(boardToPlay, boardToPlay->playerFlag);

		}
	}
	return 0;
}

int mainMenu::topMenuInput(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	*Input = wgetch(mywindow);

	//If user wants to load a map.
	if (*Input == 'l') 
	{
		topMenuLoad(Input, boardToPlay, InputLayer);
		skipOneInput = true;
		InputLayer->status = gameBoard;
	} 
	else
	//Newgame
	if (*Input == 'n')
	{
 		topMenuNew(Input, boardToPlay, InputLayer);
		skipOneInput = true;
		InputLayer->status = gameBoard;
	}
	else
	//Join a remote game
	if (*Input == 'j')
	{
		topMenuJoin(boardToPlay, InputLayer);
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

int mainMenu::topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{	
	if(computerPlayerRoster.empty() == false)
	{
	computerPlayerRoster.clear();
	}


	//Determine if game is remote or local.
	waddstr(mywindow, "Local skirmish (s), local campaign (c), or remote (r) game?\n");
	gameType = unchosen;
	while (gameType == unchosen)
	{
		*Input = wgetch(mywindow);

		if (*Input == 's')
		{
			wclear(mywindow);
			waddstr(mywindow,"Local skirmish selected.\n");
			wrefresh(mywindow);
			gameType = localSkirmish;
		}
		else if (*Input == 'r') 
		{
			wclear(mywindow);
			waddstr(mywindow,"Remote game selected.\n");
			wrefresh(mywindow);
			gameType = remote;
		}
		else if (*Input == 'c')
		{
			wclear(mywindow);
			waddstr(mywindow, "Local campaign selected.\n");
			wrefresh(mywindow);
			gameType = localCampaign;
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
		//Need to print out mission/scenario printout here
		if (gameType == localSkirmish || gameType == remote) 
		{
			waddstr(mywindow, "Choose which scenario to load (Case sensitive): \n");
			wgetstr(mywindow, pointToScenarioName);
			std::string newScenario = scenarioToLoad;
			newGameMap.open(".\\scenarios\\" + newScenario + ".txt");
			if (newGameMap.is_open())
			{
				waddstr(mywindow, "Successfully loaded!\n");
				loadsuccessful = true;
			}
			else
			{
				waddstr(mywindow, "Could not load scenario. Please check that it exists and the right spelling was used.\n");

			}
		}
		else if (gameType == localCampaign) 
		{
			waddstr(mywindow, "Choose which campaign to start (Case sensitive): \n");
			wgetstr(mywindow, pointToScenarioName);
			std::string newCampaignName = scenarioToLoad;
			char missionToLoad[100];
			std::string newMission = missionToLoad;
			
			std::ifstream newCampaign;
			newCampaign.open(".\\campaigns\\" + newCampaignName + "\\"+ newCampaignName +".txt");
			
			if (newCampaign.is_open())
			{
				waddstr(mywindow, "Successfully loaded!\n");
				loadsuccessful = true;
				std::string ThrowawayString;
				newCampaign >> ThrowawayString;
				newCampaign >> newMission;
				newGameMap.open(".\\campaigns\\" + newCampaignName + "\\" + newMission + ".txt");
			}
			else
			{
				waddstr(mywindow, "Could not load campaign. Please check that it exists and the right spelling was used.\n");

			}
		}
	}
	//Actually load scenario. Initialize board, etc.
	gameLoad(boardToPlay, InputLayer, &newGameMap);
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

	//We added one to the array, just like treasury, for easy access.
	int numberOfCompies = 0;
	char inputName[100];
	char outputName[100];

	//Resize computer Roster for easy access.
	computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);

	if (gameType == localSkirmish || gameType == remote) //Campaign map sticks to names supplied
	{
		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			snprintf(&outputName[0], 100, "Input Player %d's name: \n", i);
			waddstr(mywindow, &outputName[0]);
			wgetstr(mywindow, &inputName[0]);
			boardToPlay->playerRoster[i].name = inputName;

			bool playerTypeDecided = false;


			while (!playerTypeDecided)
			{
				char playerTypeInput = ' ';
				snprintf(&outputName[0], 100, "Is this player human (h) or computer (c)? \n");
				waddstr(mywindow, &outputName[0]);
				playerTypeInput = wgetch(mywindow);
				if (playerTypeInput == 'c' || playerTypeInput == 'h')
				{
					if (playerTypeInput == 'h')
					{
						boardToPlay->playerRoster[i].playerType = humanPlayer;
					}
					else if (playerTypeInput == 'c')
					{
						boardToPlay->playerRoster[i].playerType = computerPlayer;
						computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay);
					}
					playerTypeDecided = true;
				}
			}

		}
	}
	else	//If campaign game we just give player 1's name
	{
		snprintf(&outputName[0], 100, "Input Player's name: \n");
		waddstr(mywindow, &outputName[0]);
		wgetstr(mywindow, &inputName[0]);
		boardToPlay->playerRoster[1].name = inputName;
	}

	//Host is always player one for a new game.
	myPlayerName = boardToPlay->playerRoster[1].name;

	//Determines if they print or not.
	int numberOfHumanPlayers = 0;
	for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
	{
		if (boardToPlay->playerRoster[i].playerType == humanPlayer)
			numberOfHumanPlayers++;
	}
	
	if (numberOfHumanPlayers < 2)
	{
		boardToPlay->isItSinglePlayerGame = true;
	}
	else boardToPlay->isItSinglePlayerGame = false;

	menuStatus = playingMap;

	return 0;
}

/*int mainMenu::restartMission() 
{

}*/

/*int mainMenu::nextMission() 
{
	waddstr(mywindow, "Choose which campaign to start (Case sensitive): \n");
	wgetstr(mywindow, pointToScenarioName);
	std::string newMission = scenarioToLoad;

	std::ifstream newCampaign;
	newCampaign.open(".\\campaigns\\" + newMission + ".txt");

	if (newCampaign.is_open())
	{
		waddstr(mywindow, "Successfully loaded!\n");
		loadsuccessful = true;
		std::string ThrowawayString;
		newCampaign >> ThrowawayString;
		newCampaign >> newMission;
		newGameMap.open(".\\campaigns\\" + newMission + ".txt");
	}
	else
	{
		waddstr(mywindow, "Could not load campaign. Please check that it exists and the right spelling was used.\n");

	}


}*/

int mainMenu::topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{ 
	//Clear out the old roster. Then we can make new one.
	if (computerPlayerRoster.empty() == false)
	{
		computerPlayerRoster.clear();
	}

	//Determine if game is remote or local.
	waddstr(mywindow,"Local skirmish (s), local campaign (c), or remote(r) game?\n");
	gameType = unchosen;
	while (gameType == unchosen)
	{
		*Input = wgetch(mywindow);

		if (*Input == 'c')
		{
			gameType = localCampaign;
		}
		else if (*Input == 'r')
		{
			gameType = remote;
		}
		else if (*Input == 's')
		{
			gameType = localSkirmish;
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

		if (gameType == localSkirmish)
		{
			waddstr(mywindow, "Local skirmish selected.\n");
		}
		else if (gameType == remote)
		{
			waddstr(mywindow, "Remote game selected.\n");
		}
		else if (gameType == localCampaign)
		{
			waddstr(mywindow, "Local campaign selected.\n");
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
	gameLoad(boardToPlay, InputLayer, &loadGameSave);
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
	myPlayerName =	boardToPlay->playerRoster[boardToPlay->playerFlag].name;

	computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS+1);
	//Determines if they print or not.
	int numberOfHumanPlayers = 0;
	for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
	{
		if (boardToPlay->playerRoster[i].playerType == humanPlayer)
		{
			numberOfHumanPlayers++;
		}
		else if (boardToPlay->playerRoster[i].playerType == computerPlayer)
		{
			computerPlayerRoster[i].initalizeCompie( this, i, InputLayer, boardToPlay);
		}

	}

	if (numberOfHumanPlayers < 2)
	{
		boardToPlay->isItSinglePlayerGame = true;
	}
	else boardToPlay->isItSinglePlayerGame = false;

	menuStatus = playingMap;
	return 0;
}

int mainMenu::topMenuJoin(MasterBoard* boardToPlay, inputLayer* InputLayer) 
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

	waitForRemotePlayer(boardToPlay, InputLayer);

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

int mainMenu::waitForRemotePlayer(MasterBoard* boardToSave, inputLayer* InputLayer)
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
			gameLoad(boardToSave, InputLayer, &loadGameSave);
			
			//If it is "my turn" then we are good to go.
			if ( myPlayerName == (boardToSave->playerRoster[boardToSave->playerFlag].name)	)
			{
				isItMyTurnYet = true;
			}
		}

	}

	menuStatus = playingMap;
	return 0;
}
