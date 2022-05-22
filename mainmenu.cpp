//Copyright 2022, Supercontinent Software Ltd.
//
//	mainMenu.cpp
//

#include "Minion.hpp"
#include "MasterBoard.hpp"
#include "inputLayer.hpp"
#include "mainmenu.h"
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include "compie.hpp"
#include <filesystem>


class inputLayer;
class MasterBoard;


//Because we're having difficulty getting strings to work, we're leaving this for now.
//Linenumber indicates offset for input field.
//If you provide an announcement string with 2 lines, LineNumber should be 3.
sf::String mainMenu::playerInputString(sf::RenderWindow* myWindow, sf::Font* inputFont, sf::String AnnouncementString, int LineNumber, std::string backgroundType)
{
	sf::String inputString = "";
	sf::Event event;
	bool stringFinished = false;

	myWindow->clear();

	int boxType = 0;
	if (backgroundType == "load") 
	{
		boxType = 5;
	}
	else if (backgroundType == "new")
	{
		boxType = 6;
	}
	else if (backgroundType == "save") 
	{
		boxType = 7;
	}

	//Load or new game box
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(otherGameTextures->at(boxType));
	mywindow->draw(backgroundSprite);

	sf::Text announceText(AnnouncementString, *inputFont, menuTextSize);
	announceText.setFillColor(sf::Color::Black);
	announceText.setPosition(300, 200);
	myWindow->draw(announceText);
	
	myWindow->display();

	while (stringFinished == false)
	{
		myWindow->waitEvent(event);
		if (event.type == sf::Event::EventType::TextEntered)
		{

			inputString += event.text.unicode;
			sf::Text inputText(inputString, *inputFont, menuTextSize);
			inputText.setPosition(300, (menuTextSize+10) * (LineNumber) + 200);	//Position for strings for announcements and such
			inputText.setFillColor(sf::Color::Black);

			myWindow->clear();
			mywindow->draw(backgroundSprite);
			myWindow->draw(announceText);
			myWindow->draw(inputText);
			myWindow->display();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			stringFinished = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) && inputString.getSize() > 0)
		{
			inputString.erase(inputString.getSize() - 1);
		}


	}
	std::string debugString = inputString;
	return inputString;
}

char playCharInput(sf::RenderWindow* myWindow)
{
	char inputChar = '~';
	sf::Event event;
	bool validInput = false;

	//Clear the event queue while we were doing other things
	while (myWindow->pollEvent(event));	//May be too false-positive-ish as far as clearing out legit user inputs, we'll see

	while (validInput == false)
	{
		myWindow->waitEvent(event);
		if (event.type == sf::Event::EventType::TextEntered)
		{
			validInput = true;
			inputChar = event.text.unicode;
		}
		else
			if (event.type == sf::Event::EventType::MouseButtonPressed)
			{
				validInput = true;
			}
			else
			{
				NULL;
			}


	}

	return inputChar;
}

//mainmenu.cpp changes
mainMenu::mainMenu(sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* cour, std::vector <sf::Texture>* topMenuButtonTextureArray,
	std::vector  <sf::Texture>* inputGameMenuButtonTextureArray, std::vector <sf::Texture>* inputOtherTextureArray, sf::Music* inputIntroMusic)
{
	myTexture = gameTexture;
	introMusic = inputIntroMusic;
	myFont = cour;
	mywindow = myWindow;
	computerPlayerRoster.resize(1);	//Arbitray resize to prevent exceptions.

	//Initialize the clearField sprite used for player string input

	//Assume existence of 2 separate Button vectors within mainMenu already. One for game menu, one for top menu.
	//Overall menu area is:
	int menuTop = 100;
	int menuLeft = 400;


	//See button.h - offset helps keep the enum alligned with button type
	int buttonTypeOffset = 9;
	int buttonPlacement = 0;

	//Offset for first button and between buttons.
	int topMargin = 50;
	int leftMargin = 30;
	int betweenMargin = 20;
	sf::Vector2u textureSize = (*inputGameMenuButtonTextureArray).at(buttonTypeOffset).getSize(); 	//Buttons must all be same height, so use the first button's height
	int buttonHeight = textureSize.y;

	//For each input texture, create new button and push_back.
	for (int i = 0; i < inputGameMenuButtonTextureArray->size(); i++)
	{
		int buttonType = i + buttonTypeOffset;
		int y = 0;

		//For soundOff and speedFast, they have the same y-coord as the previous button, soundOn and speedNormal.
		if (buttonType == soundOff || buttonType == speedFast)
		{
			y = menuTop + topMargin + (buttonHeight + betweenMargin) * (buttonPlacement - 1);
		}
		else
		{
			y = menuTop + topMargin + (buttonHeight + betweenMargin) * buttonPlacement;
			//Only advance button position if it's a "unique" button. Not a sound/speed alternate button.
			buttonPlacement++;
		}

		gameMenuButtons.emplace_back(menuLeft + leftMargin, y, buttonType, &(inputGameMenuButtonTextureArray->at(i)));


	}


	//Top menu button numbers
	//Overall menu area is:
	int TopMenuTop = 200;
	int TopMenuLeft = 470;

	//Offset for first button and between buttons.
	int TopTopMargin = 50;
	int TopLeftMargin = 30;
	int TopBetweenMargin = 60;

	//Top menu buttons are larger, so get new size number.
	sf::Vector2u TopTextureSize = (*inputGameMenuButtonTextureArray).at(0).getSize(); 	//Buttons must all be same height, so use the first button's height
	int TopButtonHeight = TopTextureSize.y;

	//Reset button placement index
	buttonPlacement = 0;



	sf::Vector2u topMenuTextureSize = (*topMenuButtonTextureArray).at(0).getSize();
	int topButtonHeight = topMenuTextureSize.y;

	//Top menu buttons new game, load game, toggle editor mode
	for (int i = 0; i < 4; i++)
	{
		int y = 0;

		//For editModeOn, it has the same y-coord as the previous button, editModeOff.
		if (i == editorModeOn)
		{
			y = TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * (i - 1);
		}
		else
		{
			y = TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * i;
			//Only advance button position if it's a "unique" button. Not a editMode alternate button.
			buttonPlacement++;
		}

		topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin, y, i, &(topMenuButtonTextureArray->at(i)));
	}

	//Top menu buttons - new skirmish, new campaign, go back
	int y = 0;
	//Reset button placement index
	buttonPlacement = 0;
	for (int i = 4; i < 7; i++)
	{
		y = TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * buttonPlacement;
		topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin, y, i, &(topMenuButtonTextureArray->at(i)));
		buttonPlacement++;
	}
	//Take in other required textures
	otherGameTextures = inputOtherTextureArray;

}



//Gameplay note:
//As currently constructed, host for a new scenario must be player 1. Anyone join will play the turn assigned based on their player name.
//Thus host never says "his" player name, because that is the first one inputted!
//Similarly, if you host for a loaded game, you are playing as whose turn it was, so you should only host for whose turn was current when saved.
//Finally, for host, you currently must start with a unique session name, and they will be cleared every day to start.


int mainMenu::setCharacteristics(MasterBoard* LoadBoard)
{
	for (int x = 0; x < LoadBoard->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < LoadBoard->BOARD_HEIGHT; y++)
		{
			LoadBoard->Board[x][y].locationX = x;
			LoadBoard->Board[x][y].locationY = y;


		}
	}

	for (int x = 0; x < LoadBoard->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < LoadBoard->BOARD_HEIGHT; y++)
		{
			LoadBoard->Board[x][y].setCharacterstics(myTexture, LoadBoard);
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

	saveGame << "Mission_Briefing " << std::endl;	//String with mission info
	saveGame << briefingLineNumber << " ";
	saveGame << missionBriefing;

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

	//Then save fog of war status.
	saveGame << "Fog_Of_War:" << std::endl;
	saveGame << boardToPrint->fogOfWar << std::endl;


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
				<< int(boardToPrint->Board[boardToPrint->minionRoster[i]->locationX][boardToPrint->minionRoster[i]->locationY].capturePoints) << " "
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

	briefingLineNumber = 0;
	*saveGame >> ThrowawayString;		//String with mission info
	*saveGame >> briefingLineNumber;

	missionBriefing = "";

	for (int i = 0; i < briefingLineNumber; i++)
	{
		std::string singleLine;
		std::getline(*saveGame, singleLine);
		missionBriefing += singleLine;
		missionBriefing += "\n";
	}

	//End Mission Data Load
	//*********************************************//


	//Unique to save_game vs scenario. Load player names (User names):
	*saveGame >> ThrowawayString;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		int playerType = 0;

		*saveGame >> boardToPrint->playerRoster[i].name;	//For new game this should be ~
		*saveGame >> playerType;
		*saveGame >> boardToPrint->playerRoster[i].stillAlive;
		*saveGame >> boardToPrint->playerRoster[i].treasury;

		if (playerType == 0)
			boardToPrint->playerRoster[i].playerType = humanPlayer;
		else {
			boardToPrint->playerRoster[i].playerType = computerPlayer;
		}

	}

	//Then load the game turn.
	*saveGame >> ThrowawayString;
	*saveGame >> gameTurn;

	//Then load fog of war status - in "new" game this should be overriden by player choice.
	*saveGame >> ThrowawayString;
	*saveGame >> boardToPrint->fogOfWar;

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

	//Once board height and width are loaded, we try to adjust window size to either smaller or larger.
	if (boardToPrint->BOARD_WIDTH < MAX_WINDOW_WIDTH)
	{
		boardToPrint->WINDOW_WIDTH = boardToPrint->BOARD_WIDTH;
	}
	else
	{
		boardToPrint->WINDOW_WIDTH = MAX_WINDOW_WIDTH;
	}
	if (boardToPrint->BOARD_HEIGHT < MAX_WINDOW_HEIGHT)
	{
		boardToPrint->WINDOW_HEIGHT = boardToPrint->BOARD_HEIGHT;
	}
	else
	{
		boardToPrint->WINDOW_HEIGHT = MAX_WINDOW_HEIGHT;
	}

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
	//After team data is loaded, set property characteristics.
	setCharacteristics(boardToPrint);


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

int mainMenu::introScreen(MasterBoard* boardToPlay, inputLayer* InputLayer)
{

	sf::Sprite startWallpaperSprite;
	startWallpaperSprite.setTexture((otherGameTextures->at(0)));

	sf::Sprite startScreenStatementSprite;
	startScreenStatementSprite.setTexture(otherGameTextures->at(2));
	startScreenStatementSprite.setPosition(330, 130);

	mywindow->clear();

	introMusic->play();

	mywindow->draw(startWallpaperSprite);
	mywindow->draw(startScreenStatementSprite);

	mywindow->display();

	sf::Event throwAwayEvent;
	bool validInput = false;
	while (validInput == false)
	{
		mywindow->waitEvent(throwAwayEvent);
		if (throwAwayEvent.type == sf::Event::EventType::KeyPressed || throwAwayEvent.type == sf::Event::EventType::MouseButtonReleased)
		{
			validInput = true;
			skipOneInput = true;
		}
	}
	mywindow->clear();

	introMusic->stop();


	playGame(boardToPlay, InputLayer);


	return 0;
}

int mainMenu::playGame(MasterBoard* boardToPlay, inputLayer* InputLayer)
{


	//Run as long as the user wants. Infinite while loop.
	while (true)
	{
		sf::Event playerInput;
		enum sf::Keyboard::Key Input = sf::Keyboard::F1;

		//This is to skip if we are coming from another context like a new game.
		if (skipOneInput == true)
		{
			skipOneInput = false;
			Input = sf::Keyboard::F1;	//F1 represents "pass-one-round"
		}
		else
		{
			//Keep polling until a legit player input, not just mouse movement.
			mywindow->waitEvent(playerInput);

			if (playerInput.type == sf::Event::KeyPressed)
			{
				Input = playerInput.key.code;
			}
			else
				if (playerInput.type == sf::Event::MouseButtonPressed && playerInput.mouseButton.button == sf::Mouse::Left) //sf::Mouse::isButtonPressed(sf::Mouse::Left)) //
				{
					Input = sf::Keyboard::Quote;	//'`' represents mouseclick placeholder
				}
				else if (playerInput.type == sf::Event::MouseButtonPressed && playerInput.mouseButton.button == sf::Mouse::Right)	//Right click
				{
					Input = sf::Keyboard::Comma;	//This is right click indicator
				}


		}


		//If we're still on top menu, do that instead of game/inputLayer.
		if (menuStatus == topmenu)
		{
			printTopMenu();
			topMenuInput(&Input, boardToPlay, InputLayer);
		}
			else
				if (menuStatus == playingMap)
				{
					//Only call upkeep before play commences if it is a new game AND very first turn
					//And not compie. Compie performs upkeep in its own function.
					if (veryFirstTurn == true && isItSaveGame == false && boardToPlay->playerRoster[boardToPlay->playerFlag].playerType == humanPlayer)
					{
						boardToPlay->upkeep(InputLayer, boardToPlay->playerFlag);
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
					if (boardToPlay->playerRoster[boardToPlay->playerFlag].playerType == computerPlayer && boardToPlay->playerRoster[boardToPlay->playerFlag].stillAlive == true)
					{
						computerPlayerRoster[boardToPlay->playerFlag].takeMyTurn(boardToPlay);

					}

					//This prints the screen AFTER the latest input has taken effect.
					//Is this messing with remote play? Not sure.
					boardToPlay->checkWindow();

					InputLayer->printScreen(boardToPlay, boardToPlay->playerFlag, false);

				}


	}
	return 0;
}

//For later: int mainMenu::topMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
int mainMenu::topMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	//If valid mouse click
	if (*Input == sf::Keyboard::Quote)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

		//Start new game
		bool withinNewButton = (topMenuButtons)[newGame].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinNewButton == true)
		{
			char placeholder = 'a';
			int goBack = topMenuNew(&placeholder, boardToPlay, InputLayer);
			if (goBack == 0)
			{

				InputLayer->status = gameBoard;
			}
			skipOneInput = true;
		}

		//Load game
		bool withinLoadButton = (topMenuButtons)[loadGame].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinLoadButton == true)
		{
			char placeholder = 'a';
			int goBack = topMenuLoad(&placeholder, boardToPlay, InputLayer);
			if (goBack == 0)
			{
				InputLayer->status = gameBoard;
			}
			skipOneInput = true;
		}


		bool withinEditorButton = (topMenuButtons)[editorModeOff].checkWithinButton(mousePosition.x, mousePosition.y);
		//Toggle sound based on current sound output.
		if (withinEditorButton == true && editorMode == true)
		{
			editorMode = false;
		}
		else if (withinEditorButton == true && editorMode == false)
		{
			editorMode = true;
		}

	}
	return 0;
}

int mainMenu::printTopMenu()
{

	sf::Sprite topMenuWallpaperSprite;
	topMenuWallpaperSprite.setTexture(otherGameTextures->at(1));

	sf::Sprite topMenuSprite;
	topMenuSprite.setTexture(otherGameTextures->at(3));

	mywindow->clear();

	mywindow->draw(topMenuWallpaperSprite);

	topMenuSprite.setPosition(450, 150);
	mywindow->draw(topMenuSprite);

	//Draw three buttons for top menu
	for (int i = 0; i < 2; i++)
	{
		mywindow->draw(topMenuButtons.at(i).mySprite);
	}
	if (editorMode == false)
	{
		mywindow->draw(topMenuButtons.at(2).mySprite);
	}
	else {
		mywindow->draw(topMenuButtons.at(3).mySprite);
	}

	mywindow->display();

	return 0;
}

int mainMenu::printWaitingScreen()
{
	mywindow->clear();
	sf::String waitingScreenString("Waiting for other player(s) \n");
	sf::Text text(waitingScreenString, *myFont, 20);
	mywindow->draw(text);
	mywindow->display();
	return 0;
}

int mainMenu::topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	if (computerPlayerRoster.empty() == false)
	{
		computerPlayerRoster.clear();
	}

	sf::Event playerInput;


	//First draw out buttons, then take in input from mouse.
	sf::Sprite topMenuWallpaperSprite;
	topMenuWallpaperSprite.setTexture(otherGameTextures->at(1));

	sf::Sprite topMenuSprite;
	topMenuSprite.setTexture(otherGameTextures->at(3));

	mywindow->clear();

	mywindow->draw(topMenuWallpaperSprite);

	topMenuSprite.setPosition(450, 150);
	mywindow->draw(topMenuSprite);

	//Draw two buttons for top menu new
	for (int i = 4; i < 7; i++)
	{
		mywindow->draw(topMenuButtons.at(i).mySprite);
	}
	mywindow->display();


	//Determine if game is remote or local.
	gameType = unchosen;

	while (gameType == unchosen)
	{

		sf::Event playerInput;
		mywindow->waitEvent(playerInput);

		//Keep polling until a legit player input, not just mouse movement.
		if (playerInput.type == sf::Event::MouseButtonPressed && playerInput.mouseButton.button == sf::Mouse::Left)	//Must be mouse click
		{
			//Get mouse position
			sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

			sf::String nextTopMenuNewString;
			bool withinSkirmishButton = (topMenuButtons)[newSkirmish].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinSkirmishButton == true)
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(6));
				mywindow->draw(backgroundSprite);

				nextTopMenuNewString = "Local skirmish selected. Press any key to continue.\n";
				sf::Text newText(nextTopMenuNewString, *myFont, menuTextSize);
				newText.setPosition(300, 200);
				newText.setFillColor(sf::Color::Black);

				mywindow->draw(newText);
				mywindow->display();
				gameType = localSkirmish;
			}

			bool withinCampaignButton = (topMenuButtons)[newCampaign].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinCampaignButton == true)
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(6));
				mywindow->draw(backgroundSprite);

				nextTopMenuNewString = "Local campaign selected. Press any key to continue.\n";
				sf::Text newText(nextTopMenuNewString, *myFont, menuTextSize);
				newText.setPosition(430, 200);
				newText.setFillColor(sf::Color::Black);

				mywindow->draw(newText);
				mywindow->display();
				gameType = localCampaign;
			}

			bool withinBackButton = (topMenuButtons)[topBack1].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinBackButton == true)
			{
				return 1;
			}
		}
	}

	//Await player input to move past the selected item.
	playCharInput(mywindow);


	//Load the actual map
	std::ifstream newGameMap;
	bool loadsuccessful = false;

	sf::String anotherTopMenuNewString;


	//Necessary to move lines around in the input field screen and avoid overlap.
	int lineOffset = 1;
	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		//Need to print out mission/scenario printout here
		if (gameType == localSkirmish)
		{
			mywindow->clear();
			anotherTopMenuNewString += "Choose which scenario to load (Case sensitive): \n";
			sf::String scenarioName = playerInputString(mywindow, myFont, anotherTopMenuNewString, lineOffset, "new");
			sf::Event event;

			std::string newScenario = scenarioName;
			newGameMap.open(".\\scenarios\\" + newScenario + ".txt");
			if (newGameMap.is_open())
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(6));
				mywindow->draw(backgroundSprite);

				anotherTopMenuNewString = "Successfully loaded!\n";
				sf::Text newText(anotherTopMenuNewString, *myFont, menuTextSize);
				newText.setPosition(300, 200);
				newText.setFillColor(sf::Color::Black);

				mywindow->draw(newText);
				mywindow->display();

				loadsuccessful = true;
			}
			else
			{
				mywindow->clear();
				anotherTopMenuNewString = "Could not load scenario.\nPlease check that it exists and the right spelling was used.\n";
				lineOffset = 3;
				sf::Text newText(anotherTopMenuNewString, *myFont, menuTextSize);
				mywindow->draw(newText);
				mywindow->display();

			}
		}
		else if (gameType == localCampaign)
		{
			//Edited to only have one campaign, this saves the player from having to "decide" when there is only one available for the foreseeable future.
			mywindow->clear();

			sf::String sfCampaignName = "";
			sf::Event event;

			std::string newCampaignName = "Ormosa";
			std::string newMission = "";

			std::ifstream newCampaign;
			newCampaign.open(".\\campaigns\\" + newCampaignName + "\\" + newCampaignName + ".txt");

			if (newCampaign.is_open())
			{
				loadsuccessful = true;
				std::string CampaignBriefing;

				//Load up campaign briefing and print on screen
				missionBriefing = "";
				int briefingLineNumber = 0;
				std::string throwAway;
				newCampaign >> throwAway;
				newCampaign >> briefingLineNumber;

				for (int i = 0; i < briefingLineNumber; i++)
				{
					std::string singleLine;
					std::getline(newCampaign, singleLine);
					CampaignBriefing += singleLine;
					CampaignBriefing += "\n";
				}

				//Draw it out
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(6));
				mywindow->draw(backgroundSprite);

				sf::Text newText(CampaignBriefing, *myFont, menuTextSize);
				newText.setPosition(200, 200);
				newText.setFillColor(sf::Color::Black);
				mywindow->draw(newText);
				mywindow->display();

				//Wait for one input.
				playCharInput(mywindow);



				newCampaign >> newMission;
				newGameMap.open(".\\campaigns\\" + newCampaignName + "\\" + newMission + ".txt");

			}
			else
			{
				mywindow->clear();
				anotherTopMenuNewString = "Could not load campaign.\nPlease check that it exists and the right spelling was used. \n";
				lineOffset = 3;
				sf::Text newText(anotherTopMenuNewString, *myFont, menuTextSize);
				mywindow->draw(newText);
				mywindow->display();

			}
		}
	}
	//Actually load scenario. Initialize board, etc.
	gameLoad(boardToPlay, InputLayer, &newGameMap);
	newGameMap.close();

	//Flush event queue to clear out "Enter" and other rifraf
	sf::Event throwAwayEvent;
	while (mywindow->pollEvent(throwAwayEvent));

	std::ifstream loadSession;
	bool sessionCreationSuccessful = false;
	sf::String topMenuNewString;
	//Determine player names for number of players

	//We added one to the array, just like treasury, for easy access.
	int numberOfCompies = 0;
	sf::String inputName;

	//Resize computer Roster for easy access.
	computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);

	if (gameType == localSkirmish) //Campaign map sticks to names supplied
	{
		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			mywindow->clear();
			char buffer[100];
			snprintf(buffer, 100, "Input Player %d's name: \n", i);
			sf::String announceString = buffer;
			inputName = playerInputString(mywindow, myFont, announceString, 1, "new" );

			boardToPlay->playerRoster[i].name = inputName;

			bool playerTypeDecided = false;


			while (!playerTypeDecided)
			{
				char playerTypeInput = ' ';
				topMenuNewString = "Is this player human (h) or computer (c)? \n";
				sf::Text anotherText(topMenuNewString, *myFont, menuTextSize);
				anotherText.setPosition(300, 200);
				anotherText.setFillColor(sf::Color::Black);

				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(6));
			
				mywindow->draw(backgroundSprite);
				mywindow->draw(anotherText);

				mywindow->display();

				playerTypeInput = playCharInput(mywindow);

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
	else	//If campaign game we just give player 1's name AND initialize all other players as compie
	{
		mywindow->clear();
		topMenuNewString = "Input Player 1's name: \n";

		inputName = playerInputString(mywindow, myFont, topMenuNewString, 1 , "new");
		boardToPlay->playerRoster[1].name = inputName;

		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			if (boardToPlay->playerRoster[i].playerType == computerPlayer)
				computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay);
		}

	}

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

	//Before entering play, make sure to print out briefing.
	InputLayer->printMissionBriefing(boardToPlay);

	return 0;
}

int mainMenu::topMenuLoad(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{	
	mywindow->clear();
	
	//Clear out the old roster. Then we can make new one.
	if (computerPlayerRoster.empty() == false)
	{
		computerPlayerRoster.clear();
	}

	//Load the actual save game
	std::ifstream loadGameSave;

	bool loadsuccessful = false;
	sf::String topMenuNewString;
	int announcementLength = 1;

	topMenuNewString = "Choose which save game to load (Case sensitive. Do not use _save portion of save.): \n";
	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		

		sf::String scenarioName = playerInputString(mywindow, myFont, topMenuNewString, announcementLength, "load");
		std::string saveToLoad = scenarioName;

		saveToLoad += "_save.txt";
		loadGameSave.open(".\\savegames\\" + saveToLoad);
		if (loadGameSave.is_open())
		{
			mywindow->clear();
			topMenuNewString = "Save game successfully loaded! Press any key to continue.\n";
			
			sf::Text newText(topMenuNewString, *myFont, menuTextSize);
			newText.setFillColor(sf::Color::Black);
			newText.setPosition(300, 200);

			sf::Sprite backgroundSprite;
			backgroundSprite.setTexture(otherGameTextures->at(5));

			mywindow->draw(backgroundSprite);
			mywindow->draw(newText);
			mywindow->display();

			loadsuccessful = true;

			playCharInput(mywindow);
		}
		else
		{
			topMenuNewString = "Could not load save game. Please check that it exists and the right spelling was used.\nChoose which save game to load (Case sensitive. Do not use _save portion of save.): \n";
			announcementLength = 2;
		}

	}
	//Actually load scenario. Initialize board, etc.
	gameLoad(boardToPlay, InputLayer, &loadGameSave);
	loadGameSave.close();

	std::ifstream loadSession;
	bool sessionCreationSuccessful = false;

	computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);
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
			computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay);
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
