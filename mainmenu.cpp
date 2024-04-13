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


class inputLayer;
class MasterBoard;


//Because we're having difficulty getting strings to work, we're leaving this for now.
//Linenumber indicates offset for input field.
//If you provide an announcement string with 2 lines, LineNumber should be 3.
sf::String mainMenu::playerInputString(sf::RenderWindow* myWindow, sf::Font* inputFont, sf::String AnnouncementString, int LineNumber, std::string backgroundType, int offsetWidth)
{
	sf::String inputString = "";
	sf::Event event;

	//Flush the event queue to avoid hanging \ns for multiple line entries in a row
	while (myWindow->pollEvent(event));

	bool stringFinished = false;

	myWindow->clear();

	int boxType = 0;
	sf::String titleString;
	if (backgroundType == "load")
	{
		boxType = 5;
		titleString = "LOAD GAME";
	}
	else if (backgroundType == "new")
	{
		boxType = 6;
		titleString = "NEW GAME";
	}
	else if (backgroundType == "save")
	{
		boxType = 7;
		titleString = "SAVE GAME";
	}
	else if (backgroundType == "remote")
	{
		boxType = 14;    //Needs new sprite for remote setup and validly added to array
		titleString = "REMOTE SETUP";
	}

	//Keep top menu background
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(otherGameTextures->at(1));
	mywindow->draw(backgroundSprite);

	//Load or new game box
	sf::Sprite backgroundBoxSprite;
	backgroundBoxSprite.setTexture(otherGameTextures->at(boxType));
	backgroundBoxSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
	mywindow->draw(backgroundBoxSprite);

	//Title text
	sf::Text titleText(titleString, *inputFont, 50);
	titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
	titleText.setFillColor(sf::Color::Black);
	myWindow->draw(titleText);

	//Text filled by player
	sf::Text announceText(AnnouncementString, *inputFont, menuTextSize);
	announceText.setFillColor(sf::Color::Black);
	announceText.setPosition(MM_WIDTH_OFFSET + offsetWidth, MM_HEIGHT_OFFSET + 200);
	myWindow->draw(announceText);

	myWindow->display();

	while (stringFinished == false)
	{
		myWindow->waitEvent(event);
		if (event.type == sf::Event::EventType::TextEntered)
		{

			inputString += event.text.unicode;
			sf::Text inputText(inputString, *inputFont, menuTextSize);
			inputText.setPosition(MM_WIDTH_OFFSET + offsetWidth, MM_HEIGHT_OFFSET + (menuTextSize + 10) * (LineNumber)+200);	//Position for strings for announcements and such
			inputText.setFillColor(sf::Color::Black);

			myWindow->clear();
			mywindow->draw(backgroundSprite);
			mywindow->draw(backgroundBoxSprite);
			myWindow->draw(announceText);
			myWindow->draw(titleText);
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

char getValidPlayerInput(sf::RenderWindow* myWindow)
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

mainMenu::mainMenu(sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* inputFont, sf::Font* boldInputFont, std::vector <sf::Texture>* topMenuButtonTextureArray,
	std::vector  <sf::Texture>* inputGameMenuButtonTextureArray, std::vector <sf::Texture>* inputOtherTextureArray, sf::Music* inputMusicArray,
	std::vector <sf::Texture>* factionButtonTextureArray, std::string inputConfigFileName, std::string inputMapName)
{
	printTimer.restart();

	battleLabScenarioName = inputMapName;
	battleLabConfigFileName = inputConfigFileName;

	myTexture = gameTexture;
	musicArray = inputMusicArray;
	myFont = inputFont;
	myBoldFont = boldInputFont;
	mywindow = myWindow;
	computerPlayerRoster.resize(1);	//Arbitray resize to prevent exceptions.

	//Determine offsets for mainmenu AND for inputLayer
	sf::Vector2u windowSize = myWindow->getSize();
	std::cout << "Window width is: " << windowSize.x << std::endl;
	std::cout << "Window height is: " << windowSize.y << std::endl;

	//Actually needs to be based off the various briefing backgrounds and such, not the game board itself.
	MM_WIDTH_OFFSET = (windowSize.x - inputOtherTextureArray->at(5).getSize().x) / 2;
	MM_HEIGHT_OFFSET = (windowSize.y - inputOtherTextureArray->at(5).getSize().y) / 2;
	//MM_HEIGHT_OFFSET = (windowSize.y - MAX_WINDOW_HEIGHT * 50) / 2;


	//Game menu buttons ///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
			y = MM_HEIGHT_OFFSET + menuTop + topMargin + (buttonHeight + betweenMargin) * (buttonPlacement - 1);
		}
		else
		{
			y = MM_HEIGHT_OFFSET + menuTop + topMargin + (buttonHeight + betweenMargin) * buttonPlacement;
			//Only advance button position if it's a "unique" button. Not a sound/speed alternate button.
			buttonPlacement++;
		}
		gameMenuButtons.emplace_back(menuLeft + leftMargin + MM_WIDTH_OFFSET, y, buttonType, &(inputGameMenuButtonTextureArray->at(i)), "GameMenuButton");
	}
	//Game menu buttons ///////////////////////////////////////////////////////////////////////////////////////////////////////////




	//Top menu buttons ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Top menu button numbers
	//Overall menu area is:
	const int TopMenuTop = 200;
	const int TopMenuLeft = 470;

	//Offset for first button:
	const int TopTopMargin = 20;
	//Margin between buttons:
	int TopBetweenMargin = 60;

	const int TopLeftMargin = 30;


	//Top menu buttons are larger, so get new size number.
	sf::Vector2u TopTextureSize = (*inputGameMenuButtonTextureArray).at(0).getSize(); 	//Buttons must all be same height, so use the first button's height
	int TopButtonHeight = TopTextureSize.y;

	//Reset button placement index
	buttonPlacement = 0;

	//Top menu buttons new game, load game, toggle editor mode (2x buttons)
	int y = 0;
	for (int i = 0; i < 4; i++)
	{
		y = 0;

		//For editModeOn, it has the same y-coord as the previous button, editModeOff.
		if (i == editorModeOn)
		{
			y = MM_HEIGHT_OFFSET + TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * (i - 1);
		}
		else
		{
			y = MM_HEIGHT_OFFSET + TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * i;
			//Only advance button position if it's a "unique" button. Not a editMode alternate button.
			buttonPlacement++;
		}
		topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, i, &(topMenuButtonTextureArray->at(i)), "TopMenuButton");
	}


	//Top menu buttons - new skirmish, new campaign, go back
	y = 0;
	//Reset button placement index
	buttonPlacement = 0;
	for (int i = 4; i < 7; i++)		//Adds texture index 6 first time
	{
		y = TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * buttonPlacement;
		topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y + MM_HEIGHT_OFFSET, i, &(topMenuButtonTextureArray->at(i)), "TopMenuButton");
		buttonPlacement++;
	}

	//Add quit button at end to not mess up menu functions
	y = MM_HEIGHT_OFFSET + TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * 4;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, quitButton, &(topMenuButtonTextureArray->at(7)), "quitButton");

	//Load game buttons - up and down scroll buttons
	int x = MM_WIDTH_OFFSET + 40;
	y = MM_HEIGHT_OFFSET + 40;
	topMenuButtons.emplace_back(x, y, upDownButton, &(topMenuButtonTextureArray->at(8)), "upButton");
	y = MM_HEIGHT_OFFSET + 40 + topMenuButtonTextureArray->at(9).getSize().y + 30;
	topMenuButtons.emplace_back(x, y, upDownButton, &(topMenuButtonTextureArray->at(9)), "downButton");
	y = MM_HEIGHT_OFFSET + 40 + 2 * (topMenuButtonTextureArray->at(10).getSize().y + 30);
	topMenuButtons.emplace_back(x, y, selectButton, &(topMenuButtonTextureArray->at(10)), "selectButton");

	//Human / computer choice buttons
	int additionalMargin = 30;
	y = TopMenuTop + TopTopMargin + MM_HEIGHT_OFFSET + additionalMargin;
	std::cout << "Computer y: " << y << std::endl;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, selectButton, &(topMenuButtonTextureArray->at(11)), "computerPlayerButton");
	y = TopMenuTop + TopTopMargin + TopButtonHeight + TopBetweenMargin + MM_HEIGHT_OFFSET + additionalMargin;
	std::cout << "Human y : " << y << std::endl;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, selectButton, &(topMenuButtonTextureArray->at(12)), "humanPlayerButton");

	//Back button # 2 (Load, save, etc)
	//This needs its own additional back button
	//Causes offset issue as noted in main.cpp
	y = MM_HEIGHT_OFFSET + TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * 3;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, topBack1, &(topMenuButtonTextureArray->at(6)), "backButton");

	//Add remote play buttons
	y = TopMenuTop + TopTopMargin + MM_HEIGHT_OFFSET;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, remoteClientButton, &(topMenuButtonTextureArray->at(13)), "remoteJoinButton");
	y = TopMenuTop + TopTopMargin + TopButtonHeight + TopBetweenMargin + MM_HEIGHT_OFFSET;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, remoteHostButton, &(topMenuButtonTextureArray->at(14)), "remoteHostButton");

	//This button comes before quit, so has 3*offset, while quit has 4*offset
	y = MM_HEIGHT_OFFSET + TopMenuTop + TopTopMargin + (TopButtonHeight + TopBetweenMargin) * 3;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, remotePlayButton, &(topMenuButtonTextureArray->at(15)), "remotePlayButton");

	y = TopMenuTop + TopTopMargin + 2*(TopButtonHeight + TopBetweenMargin) + MM_HEIGHT_OFFSET;
	topMenuButtons.emplace_back(TopMenuLeft + TopLeftMargin + MM_WIDTH_OFFSET, y, selectButton, &(topMenuButtonTextureArray->at(16)), "remoteHumanPlayerButton");


	//Top menu buttons ////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//Faction choice buttons ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Top menu button numbers

	//Find button size
	sf::Vector2u factionButtonSize = (*factionButtonTextureArray).at(0).getSize(); 	//Buttons must all be same height, so use the first button's height
	int factionButtonHeight = factionButtonSize.y;
	int factionButtonWidth = factionButtonSize.x;

	//Overall menu area is:
	int factionButtonsTop = 180;
	int factionButtonsLeft = 350;

	//Offset between buttons
	int factionBetweenMargin = 50;
	int factionNumber = 0;

	//Factionbutton creation
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			int y = i * (factionButtonHeight + factionBetweenMargin) + factionButtonsTop + MM_HEIGHT_OFFSET;
			int x = j * (factionButtonWidth + factionBetweenMargin) + factionButtonsLeft + MM_WIDTH_OFFSET;
			factionChoiceButtons.emplace_back(x, y, factionButton, &(factionButtonTextureArray->at(factionNumber)), "FactionButton");
			factionNumber++;
		}
	}
	//Faction choice buttons ////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Take in other required textures
	otherGameTextures = inputOtherTextureArray;
}

mainMenu::mainMenu(std::string inputConfigFileName, std::string inputMapName)
{
	battleLabScenarioName = inputMapName;
	battleLabConfigFileName = inputConfigFileName;

	myTexture = NULL;
	musicArray = NULL;
	myFont = NULL;
	mywindow = NULL;

	computerPlayerRoster.resize(1);	//Arbitray resize to prevent exceptions.

	//Determine offsets for mainmenu AND for inputLayer

	std::cout << "Window width is unknown" << std::endl;
	std::cout << "Window height is unknown" << std::endl;


}

bool mainMenu::determineGameToLoad(MasterBoard* boardToPlay, inputLayer* InputLayer, std::ifstream* loadGameStream)
{
	bool loadsuccessful = false;
	std::string topMenuLoadString;

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{
		bool scenarioChosen = false;
		std::vector <std::string> listOfSavegames;
		int highlightedName = 0;
		std::string chosenSaveName = "";

		//Find all available save games and add them to the list
		const std::filesystem::path myPath{ ".\\savegames" };
		for (const auto& entry : std::filesystem::directory_iterator(myPath))
		{
			listOfSavegames.push_back(entry.path().string());
			std::cout << entry.path() << std::endl;

		}

		for (int i = 0; i < listOfSavegames.size(); i++)
		{
			listOfSavegames.at(i).erase(0, 12);
			std::cout << listOfSavegames.at(i) << std::endl;
		}

		while (scenarioChosen == false)
		{
			mywindow->clear();
			topMenuLoadString += "Choose which savegame to load: \n";

			//To print list of scenarios, first determine start and end points
			int startPoint = highlightedName - 5;
			if (startPoint < 0)
				startPoint = 0;
			int endPoint = highlightedName + 5;
			if (endPoint > listOfSavegames.size() - 1)
				endPoint = listOfSavegames.size() - 1;

			//Print backgrounds
			sf::Sprite backgroundSprite;
			backgroundSprite.setTexture(otherGameTextures->at(1));
			mywindow->draw(backgroundSprite);

			sf::Sprite menuBackgroundSprite;
			menuBackgroundSprite.setTexture(otherGameTextures->at(6));
			menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
			mywindow->draw(menuBackgroundSprite);

			//Print title text
			sf::Text titleText("Load Game", *myFont, 50);
			titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
			titleText.setFillColor(sf::Color::Black);
			mywindow->draw(titleText);

			//Print text elements and display
			for (int i = startPoint; i < endPoint + 1; i++)
			{
				sf::Text scenarioListText(listOfSavegames.at(i), *myFont, menuTextSize + 4);
				scenarioListText.setPosition(330 + MM_WIDTH_OFFSET, 300 + MM_HEIGHT_OFFSET + 30 * i);

				//Draw White if highlighted
				if (i == highlightedName)
					scenarioListText.setFillColor(sf::Color::Blue);
				else
					scenarioListText.setFillColor(sf::Color::Black);

				mywindow->draw(scenarioListText);
				//Otherwise draw normal
			}

			//Print up and down buttons and display
			for (int i = 8; i < 11; i++)
			{
				mywindow->draw(topMenuButtons.at(i).mySprite);
			}
			//Print back button
			mywindow->draw(topMenuButtons.at(6).mySprite);
			mywindow->display();


			sf::Event playerInput;
			mywindow->waitEvent(playerInput);

			//Keep polling until a legit player input, not just mouse movement.
			if ((playerInput.type == sf::Event::MouseButtonReleased && playerInput.mouseButton.button == sf::Mouse::Left)
				|| playerInput.type == sf::Event::KeyReleased)
				//Must be mouse click
			{
				//Get mouse position
				sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

				//Take user input

				bool withinUpButton = false;
				if (playerInput.type == sf::Event::MouseButtonReleased)
					withinUpButton = topMenuButtons[8].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinUpButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Up))
				{
					highlightedName--;
					if (highlightedName < 0)
						highlightedName = 0;
				}

				bool withinDownButton = false;
				if (playerInput.type == sf::Event::MouseButtonReleased)
					withinDownButton = topMenuButtons[9].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinDownButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Down))
				{
					highlightedName++;
					if (highlightedName > listOfSavegames.size() - 1)
						highlightedName = listOfSavegames.size() - 1;
				}

				bool withinSelectButton = false;
				if (playerInput.type == sf::Event::MouseButtonReleased)
					withinSelectButton = (topMenuButtons)[10].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinSelectButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Enter))
				{
					scenarioChosen = true;
					chosenSaveName = listOfSavegames.at(highlightedName);
				}

				bool withinBackButton = false;
				if (playerInput.type == sf::Event::MouseButtonReleased)
					withinBackButton = (topMenuButtons)[6].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinBackButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Escape))
				{
					return false;
				}


			}

		}


		//After player chooses a save, try to load that save
		loadGameStream->open(".\\savegames\\" + chosenSaveName);
		if (loadGameStream->is_open())
		{
			mywindow->clear();
			topMenuLoadString = "Save game successfully loaded! Press any key to continue.\n";

			sf::Text newText(topMenuLoadString, *myFont, menuTextSize);
			newText.setFillColor(sf::Color::Black);
			newText.setPosition(330 + MM_WIDTH_OFFSET, 200 + MM_HEIGHT_OFFSET);

			sf::Sprite backgroundSprite;
			backgroundSprite.setTexture(otherGameTextures->at(1));
			mywindow->draw(backgroundSprite);

			sf::Sprite menuBackgroundSprite;
			menuBackgroundSprite.setTexture(otherGameTextures->at(5));
			menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
			mywindow->draw(menuBackgroundSprite);


			mywindow->draw(newText);
			mywindow->display();

			loadsuccessful = true;

			getValidPlayerInput(mywindow);
		}

	}

	return true;

}


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

int mainMenu::saveGameData(std::string inputSaveGameName, MasterBoard* boardToPrint)
{
	std::ofstream saveGameStream(inputSaveGameName);

	saveGameStream << "Savegame_YesNo" << std::endl;
	saveGameStream << 1 << std::endl;

	//Unique to save_game vs scenario. First save number of players, and then player names (User names):
	saveGameStream << "Number_of_players_below:" << std::endl;
	saveGameStream << boardToPrint->NUMBEROFPLAYERS << std::endl;


	//*********************************************//
	//Input "Mission load" data here
	saveGameStream << "Mission_On_Off" << std::endl;		//Mission or not
	saveGameStream << boardToPrint->missionFlag << std::endl;

	saveGameStream << "Campaign_Name" << std::endl;		//Mission or not
	saveGameStream << boardToPrint->campaignName << std::endl;

	saveGameStream << "ScenarioOrMissionName" << std::endl;
	saveGameStream << boardToPrint->scenarioOrMissionName << std::endl;	//Must be exactly the same as the txt file's name

	saveGameStream << "TurnLimit_Or_0_ForOff" << std::endl;		//TurnLength - either 0 for no max turn length, or int to indicate length
	saveGameStream << boardToPrint->missionTurnLength << std::endl;

	saveGameStream << "Who_wins_after_turn_limit" << std::endl;		//Indicates who will win if turn length is met.
	saveGameStream << boardToPrint->whoHoldsOut << std::endl;

	saveGameStream << "Name_of_next_mission_(Same_Directory)" << std::endl;		//Name of next mission
	saveGameStream << nextMissionName << std::endl;

	saveGameStream << "Mission_Briefing " << std::endl;	//String with mission info
	saveGameStream << briefingLineNumber << " ";
	saveGameStream << missionBriefing;

	//End Mission Data Load
	//*********************************************//


	saveGameStream << "Player_Data:_Name_PlayerType_StillAlive_Treasury_Faction" << std::endl;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		saveGameStream << boardToPrint->playerRoster[i].name << std::endl;
		saveGameStream << int(boardToPrint->playerRoster[i].playerType) << std::endl;
		saveGameStream << int(boardToPrint->playerRoster[i].stillAlive) << std::endl;
		saveGameStream << boardToPrint->playerRoster[i].treasury << std::endl;
		saveGameStream << boardToPrint->playerRoster[i].playerFaction << std::endl;
	}

	//Then save the game turn.
	saveGameStream << "Game_turn_below:" << std::endl;
	saveGameStream << gameTurn << std::endl;

	//Then save fog of war status.
	saveGameStream << "Fog_Of_War:" << std::endl;
	saveGameStream << boardToPrint->fogOfWar << std::endl;


	//Then save the map size:
	saveGameStream << "Map_width_below:" << std::endl;
	saveGameStream << boardToPrint->BOARD_WIDTH << std::endl;
	saveGameStream << "Map_height_below:" << std::endl;
	saveGameStream << boardToPrint->BOARD_HEIGHT << std::endl;

	//Then save whos turn it is:
	saveGameStream << "Player_whose_turn_it_is_below:" << std::endl;
	saveGameStream << boardToPrint->playerFlag << std::endl;

	//Terrain save:
	//Iterate through board and save the exact symbol.
	//Saving symbol and controller separately for ease of edit and viewing.
	//Only pain point is do mass control-edits, but viewability is protected.
	saveGameStream << "Map_terrain_below:" << std::endl;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			saveGameStream << boardToPrint->Board[x][y].symbol;
		}
		saveGameStream << std::endl;
	}

	//Iterate through board and save the controller.
	saveGameStream << "Property_controller_number_below:" << std::endl;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			saveGameStream << boardToPrint->Board[x][y].controller;

		}
		saveGameStream << std::endl;
	}

	//Save minion ban list. Start with number of banned minion types.
	//Number of banned types
	int numberOfBannedTypes = boardToPrint->banList.size();

	saveGameStream << "Number_Of_Banned_Minions" << std::endl;
	saveGameStream << numberOfBannedTypes << std::endl;

	saveGameStream << "Banned_Minion_List" << std::endl;
	//Then write out ban list
	for (int i = 0; i < numberOfBannedTypes; i++)
	{
		saveGameStream << boardToPrint->banList.at(i) << std::endl;
	}


	//Note the number of minions:
	saveGameStream << "Total_minions_below:" << std::endl;
	saveGameStream << boardToPrint->totalNumberOfMinions << std::endl;

	//Go through entire minionRoster and save each value associated with each minion, one line per minion.
	saveGameStream << "Minion_roster_below_(XCoord,YCoord,Team,Seniority,Status,Health,Veterancy,CapPoints,TransportStatus,Fuel,PriAmmo,SecAmmo):" << std::endl;
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//First cycle through all non transported minions and do them
		if (boardToPrint->minionRoster[i] != NULL && boardToPrint->minionRoster[i]->transporter == NULL)
		{
			int beingTransported = 0;

			saveGameStream << boardToPrint->minionRoster[i]->type << " "
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
			saveGameStream << boardToPrint->minionRoster[i]->type << " "
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
	saveGameStream.close();
	return 1;
}

//Load scenario game and initialize the board with its contents.
int mainMenu::loadGameData(MasterBoard* boardToPrint, inputLayer* InputLayer, std::ifstream* dataStream)
{
	veryFirstTurn = true;

	//Clear board in case scenario load was called by player menu later in game.
	boardToPrint->clearBoard(InputLayer);

	std::string ThrowawayString;

	*dataStream >> ThrowawayString;
	*dataStream >> isItSaveGame;

	//First load number of players from save
	//Remember to have one exta for ease of access (Player "0" is blank)
	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->NUMBEROFPLAYERS;
	boardToPrint->playerRoster.resize(boardToPrint->NUMBEROFPLAYERS + 1);

	//*********************************************//
	//Input "Mission load" data here
	*dataStream >> ThrowawayString;		//Campaign_game
	*dataStream >> boardToPrint->missionFlag;

	//If we still don't know the game type, base it on the save data
	if (gameType == unchosen)
	{
		if (boardToPrint->missionFlag == 1)
			gameType == localCampaign;
		else gameType = localSkirmish;
	}

	*dataStream >> ThrowawayString;		//Campaign Name - used to navigate menu for next level
	*dataStream >> boardToPrint->campaignName;

	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->scenarioOrMissionName;	//Must be exactly the same as the txt file's name

	*dataStream >> ThrowawayString;		//TurnLength - either 0 for no max turn length, or int to indicate length
	*dataStream >> boardToPrint->missionTurnLength;

	*dataStream >> ThrowawayString;		//Indicates who will win if turn length is met.
	*dataStream >> boardToPrint->whoHoldsOut;

	*dataStream >> ThrowawayString;		//Name of next mission
	*dataStream >> nextMissionName;

	briefingLineNumber = 0;
	*dataStream >> ThrowawayString;		//String with mission info
	*dataStream >> briefingLineNumber;

	missionBriefing = "";

	for (int i = 0; i < briefingLineNumber; i++)
	{
		std::string singleLine;
		std::getline(*dataStream, singleLine);
		missionBriefing += singleLine;
		missionBriefing += "\n";
	}

	//End Mission Data Load
	//*********************************************//


	//Unique to save_game vs scenario. Load player names (User names):
	*dataStream >> ThrowawayString;
	for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
	{
		int playerType = 0;
		int factionType = 0;

		*dataStream >> boardToPrint->playerRoster[i].name;	//For new game this should be ~
		*dataStream >> playerType;
		*dataStream >> boardToPrint->playerRoster[i].stillAlive;
		*dataStream >> boardToPrint->playerRoster[i].treasury;
		*dataStream >> factionType;

		switch (factionType)
		{
		case(0):
			boardToPrint->playerRoster[i].playerFaction = NeutralFaction;
			break;
		case(1):
			boardToPrint->playerRoster[i].playerFaction = NorthRedonia;
			break;
		case(2):
			boardToPrint->playerRoster[i].playerFaction = SouthRedonia;
			break;
		case(3):
			boardToPrint->playerRoster[i].playerFaction = Ormosa;
			break;
		case(4):
			boardToPrint->playerRoster[i].playerFaction = Torran;
			break;
		}


		if (playerType == 0)
			boardToPrint->playerRoster[i].playerType = localHumanPlayer;
		else if (playerType == 1)
		{
			boardToPrint->playerRoster[i].playerType = computerPlayer;
		}
		else if (playerType == 2)
		{
			boardToPrint->playerRoster[i].playerType = remoteHumanPlayer;
		}

	}

	//Then load the game turn.
	*dataStream >> ThrowawayString;
	*dataStream >> gameTurn;

	//Then load fog of war status - in "new" game this should be overriden by player choice.
	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->fogOfWar;

	//If this is a new game, clear the treasury.
	//(If going from a game in middle of play, to new game).
	//Otherwise leave the values from the loadGameData portion.
	//Clear treasury not necessary, just use whatever is in the scenario.

	//First load the map size:
	//Ideally we can create new vector or whatever to have different map size:
	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->BOARD_WIDTH;
	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->BOARD_HEIGHT;

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
	boardToPrint->Board.resize(boardToPrint->BOARD_WIDTH);	//+1)
	for (int i = 0; i < boardToPrint->BOARD_WIDTH; i++)
	{
		boardToPrint->Board[i].resize(boardToPrint->BOARD_HEIGHT);
	}

	//Then load player data:
	//This is the current player whos turn it is:
	*dataStream >> ThrowawayString;
	*dataStream >> boardToPrint->playerFlag;

	//Then load map
	*dataStream >> ThrowawayString;

	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			char checkonMe = 'L';
			*dataStream >> checkonMe;
			boardToPrint->Board[x][y].symbol = checkonMe;

			//Also resize for withinVision
			boardToPrint->Board[x][y].withinVision.resize(boardToPrint->NUMBEROFPLAYERS + 1);
			for (int n = 0; n < boardToPrint->NUMBEROFPLAYERS + 1; n++)
				boardToPrint->Board[x][y].withinVision[n] = false;

		}
	}


	//This is for properties.
	char inputProperties;
	*dataStream >> ThrowawayString;
	for (int y = 0; y < boardToPrint->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPrint->BOARD_WIDTH; x++)
		{
			*dataStream >> inputProperties;
			boardToPrint->Board[x][y].controller = (int(inputProperties)) - 48;
		}
	}
	//After team data is loaded, set property characteristics.
	setCharacteristics(boardToPrint);

	//Load minion ban list. Start with number of banned minion types.
	//Number of banned types
	*dataStream >> ThrowawayString;
	int numberOfBannedTypes = 0;
	*dataStream >> numberOfBannedTypes;

	*dataStream >> ThrowawayString;
	//Then create ban list
	for (int i = 0; i < numberOfBannedTypes; i++)
	{
		std::string bannedMinionType;
		*dataStream >> bannedMinionType;
		boardToPrint->banList.push_back(bannedMinionType);
	}


	//Then load minion data:
	*dataStream >> ThrowawayString;
	int numberOfMinions;
	*dataStream >> numberOfMinions;
	int health, locationX, locationY, team, seniority, status, veterancy, capturePoints, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo;
	std::string type;
	*dataStream >> ThrowawayString;

	//Initialize all minions.
	for (int y = 0; y < numberOfMinions; y++)
	{
		*dataStream >> type
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

	//Initialize all compies at this stage if a mission (We are certain who is a compie)
	//Or if it's a load game
	if (boardToPrint->missionFlag == true || isItSaveGame == true)
	{
		computerPlayerRoster.resize(boardToPrint->NUMBEROFPLAYERS + 1);
		for (int i = 1; i <= boardToPrint->NUMBEROFPLAYERS; i++)
		{
			if (boardToPrint->playerRoster[i].playerType == computerPlayer)
				computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPrint, -1, -1, -1, -1, -1);	//Use default behaviors
		}
	}

	dataStream->close();
	return 1;
}

int mainMenu::introScreen(MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	std::ifstream battleLabConfigFile;
	battleLabConfigFile.open(battleLabConfigFileName);
	std::string ThrowawayString = "";
	if (battleLabConfigFile.is_open() == true)
	{
		battleLabConfigFile >> ThrowawayString;
		battleLabConfigFile >> battleLabOn;
	}
	else
	{
		std::cout << "Could not open battle lab config file." << std::endl;
	}


	//If ordered, run battle lab.
	if (battleLabOn == true)
	{
		std::cout << "Running Battle Lab" << std::endl;
		runBattleLab(boardToPlay, InputLayer, &battleLabConfigFile);
		return 0;
	}
	//END_BATTLELAB


	sf::Sprite startWallpaperSprite;
	startWallpaperSprite.setTexture((otherGameTextures->at(0)));

	sf::Sprite startScreenStatementSprite;
	startScreenStatementSprite.setTexture(otherGameTextures->at(2));
	startScreenStatementSprite.setPosition(330 + MM_WIDTH_OFFSET, 130 + MM_HEIGHT_OFFSET);

	mywindow->clear();

	if (InputLayer->soundsOn == true)
		musicArray[0].play();

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



	playGame(boardToPlay, InputLayer);


	return 0;
}

int mainMenu::runBattleLab(MasterBoard* boardToPlay, inputLayer* InputLayer, std::ifstream* configFile)
{
	std::cout << "Entering battle lab." << std::endl;

	//Battle lab expects the following in config file:
	//runBattleLab_0_Off_1_On
	//0
	//Number_Of_Runs
	//100
	//TurnLimit
	//200
	//Scenario_Name
	//midway

	battleLabTurnLimit = 0;
	int battleLabNumberDraws = 0;
	int battleLabnumberPlayerOneWins = 0;
	int battleLabnumberPlayerTwoWins = 0;

	std::string ThrowawayString;
	int numberOfRuns = 0;

	*configFile >> ThrowawayString;
	*configFile >> numberOfRuns;

	*configFile >> ThrowawayString;
	*configFile >> battleLabTurnLimit; //Global within mainMenu.cpp

	std::string outputName = "";
	*configFile >> ThrowawayString;
	*configFile >> outputName;

	std::ofstream outputFile;
	outputFile.open(".\\battlelab\\" + outputName + "_" + battleLabScenarioName + ".txt");

	//Increase speed and turn off sound
	InputLayer->speedFactor = 1000;
	InputLayer->soundsOn = false;

	//Track several characteristics to describe compie's behavior:
	int armySizeFactor[2] = { 1,1 };	//From 1 to 10

	//Run given scenario according to number of runs directed
	for (int i = 0; i < numberOfRuns; i++)
	{
		battleLabReset = false;

		if (computerPlayerRoster.empty() == false)
		{
			computerPlayerRoster.clear();
		}

		gameType = localSkirmish;

		//Load the actual map
		std::ifstream newGameMap;

		//Need to print out mission/scenario printout here
		if (gameType == localSkirmish)
		{
			newGameMap.open(".\\scenarios\\" + battleLabScenarioName);
			if (newGameMap.is_open() == false)
			{
				std::cout << "Could not open scenario. Aborting battle lab." << std::endl;
				return 1;
			}
			else
			{
				std::cout << "Successfully opened scenario " << battleLabScenarioName << std::endl;
			}
		}

		//Actually load scenario. Initialize board, etc.
		loadGameData(boardToPlay, InputLayer, &newGameMap);
		newGameMap.close();

		//Initialize compies if not done by gameLoad (They were initialized if it is a mission or a savegame)
		int variableRepairThreshold = 40;				//10-90 represents when a minion will attempt to repair, on a 100 hp scale. 40 gives best behavior so far.
		int variableMinionAggressionBonus = 0;			//Intended range is -20 - 20 for reasonable behavior. Allows compie minions to attack even when the odds are against them.
		int variableInfantryAttackBonus = 2;			//Intended range is 1-3 for reasonable behavior. Makes attacking infantry more attractive - would be difficult to attack them with expensive minions otherwise
		int variableInfantryBaseline = 5;				//No idea what a reasonable range is, probably between 0 - 10. Describes min. number of infantry required in army.
		int variableArmySizeFactor = 4;					//Reasonable range is 1 - 8. 1 makes larger armies, 8 smaller.

		outputFile << "********** Starting game number: " << i << " ****************" << std::endl;

		if (boardToPlay->missionFlag == false)
		{
			computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);
			for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
			{
				//Randomize input values
				//variableRepairThreshold = 10 + rand() % 81;
				//variableMinionAggressionBonus = -20 + rand() % 41;
				//variableInfantryAttackBonus = 1 + rand() % 3;
				//variableInfantryBaseline = rand() % 11;
				variableArmySizeFactor = 1 + rand() % 8;

				computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay, variableRepairThreshold, variableMinionAggressionBonus,
					variableInfantryAttackBonus, variableInfantryBaseline, variableArmySizeFactor);

				outputFile << "Repair threshold for player " << i << ": " << variableRepairThreshold << std::endl;
				outputFile << "Aggression bonus for player " << i << ": " << variableMinionAggressionBonus << std::endl;
				outputFile << "Willingness to attack infantry for player " << i << ": " << variableInfantryAttackBonus << std::endl;
				outputFile << "Infantry component size for player " << i << ": " << variableInfantryBaseline << std::endl;
				outputFile << "Army size factor for player " << i << ": " << variableArmySizeFactor << std::endl;
			}
		}

		boardToPlay->isItSinglePlayerGame = true;

		menuStatus = playingMap;

		skipOneInput = true;

		//Turn off mission flag so we come back to main menu after the game is resolved.
		boardToPlay->missionFlag = false;

		//After loading scenario for this iteration, play game.
		playGame(boardToPlay, InputLayer);

		if (battleLabWinningPlayer == 1)
			battleLabnumberPlayerOneWins++;
		else if (battleLabWinningPlayer == 2)
			battleLabnumberPlayerTwoWins++;
		else battleLabNumberDraws++;

		//Put out status
		outputFile << "********** Completed game number: " << i << " ****************" << std::endl;
		outputFile << "Winner: Player " << battleLabWinningPlayer << std::endl;
		outputFile << "Number of turns in this game: " << gameTurn << std::endl;
		outputFile << "\tPlayer 1 wins: " << battleLabnumberPlayerOneWins << std::endl;
		outputFile << "\tPlayer 2 wins: " << battleLabnumberPlayerTwoWins << std::endl;
		outputFile << "\tNumber of draws: " << battleLabNumberDraws << std::endl << std::endl << std::endl;

	}

	return 0;
}


int mainMenu::playGame(MasterBoard* boardToPlay, inputLayer* InputLayer)
{

	//Run as long as the user wants. Infinite while loop.
	while (true)
	{
		//If battle lab is running, check for game draw.
		//Check for game reset due to one player winning.
		if (battleLabOn == true)
		{
			if (gameTurn >= battleLabTurnLimit)
			{
				battleLabWinningPlayer = 0;
				battleLabReset = true;
				return 0;
			}
			else
				if (battleLabReset == true)
				{
					return 0;
				}
		}

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
			//Keep polling until any player input, even if mouse movement
			bool playerInputReceived = false;
			while (playerInputReceived == false)
			{
				//See if player did an input
				playerInputReceived = mywindow->pollEvent(playerInput);

				//We never count mouse movement as an event.
				if (playerInput.type == sf::Event::MouseMoved)
					playerInputReceived = false;

				//Only print repeatedly if actually playing the board
				if (menuStatus == playingMap)
				{
					//Advance the printTimer and reset after 1 second of real life time
					if (printTimer.getElapsedTime().asSeconds() > float(1))
					{
						printTimer.restart();

						if (printSecond == true)
							printSecond = false;
						else printSecond = true;
					}

					//Wait 20 ms regardless of input received
					if (playerInputReceived == false)
					{
						sf::Time smallWaitTime;
						while (smallWaitTime.asMilliseconds() < 11)
						{
							smallWaitTime = printTimer.getElapsedTime();
						}
					}

					InputLayer->printScreen(boardToPlay, boardToPlay->playerFlag, false);
				}

			}

			if (playerInput.type == sf::Event::Closed)
			{
				mywindow->close();
				exit(0);
			}
			else
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

				if (musicArray != NULL)
				{
					if (musicArray[0].getStatus() == sf::SoundSource::Playing)
						musicArray[0].stop();
				}

				if (gameType == remoteClient && remoteClientAwaitingFirstUpdate == true)
				{
					//If we are remote client we have to get an update from the host before we even know what the map is, etc.
					remoteClientAwaitingFirstUpdate = false;
				}
				else
				{
					//Only call upkeep before play commences if it is a new game AND very first turn
					//And not compie. Compie performs upkeep in its own function.
					if (veryFirstTurn == true && isItSaveGame == false && boardToPlay->playerRoster[boardToPlay->playerFlag].playerType != computerPlayer)
					{
						boardToPlay->upkeep(InputLayer, boardToPlay->playerFlag);
						veryFirstTurn = false;
					}
					else    //Still want to update vision even if we're not doing upkeep.
					{
						boardToPlay->setVisionField(boardToPlay->playerFlag);
					}
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
				else if (InputLayer->status == waitingForHost)
				{
					InputLayer->waitingScreenRemoteClient(boardToPlay);
				}
				else if (InputLayer->status == waitingForClient)
				{
					InputLayer->waitingScreenRemoteHost(boardToPlay);
				}
				else if (InputLayer->status == insertMinion)
				{
					InputLayer->insertMinionInput(&playerInput, boardToPlay);
				}
				else if (InputLayer->status == insertTile)
				{
					InputLayer->insertTileInput(&Input, boardToPlay);
				}

				//Computer takes turn if it is his turn to do so.
				//Note that this doesn't deal with "status".
				if (boardToPlay->playerRoster[boardToPlay->playerFlag].playerType == computerPlayer && boardToPlay->playerRoster[boardToPlay->playerFlag].stillAlive == true)
				{
					computerPlayerRoster[boardToPlay->playerFlag].takeMyTurn();

				}

				//This prints the screen AFTER the latest input has taken effect.
				//As long as battlelab is not on, print the screen.
				if (battleLabOn == false)
				{
					boardToPlay->checkWindow();

					InputLayer->printScreen(boardToPlay, boardToPlay->playerFlag, false);
				}

			}


	}
	return 0;
}

//New function
int mainMenu::printTopMenuBackground()
{
	sf::Sprite topMenuWallpaperSprite;
	topMenuWallpaperSprite.setTexture(otherGameTextures->at(1));
	sf::Sprite topMenuSprite;
	topMenuSprite.setTexture(otherGameTextures->at(3));
	topMenuSprite.setPosition(450 + MM_WIDTH_OFFSET, 100 + MM_HEIGHT_OFFSET);

	mywindow->clear();
	mywindow->draw(topMenuWallpaperSprite);
	mywindow->draw(topMenuSprite);

	sf::String topmenuString = "MAIN MENU";
	sf::Text topmenuText(topmenuString, *myBoldFont, menuTextSize + 5);
	topmenuText.setPosition(520 + MM_WIDTH_OFFSET, 130 + MM_HEIGHT_OFFSET);
	topmenuText.setFillColor(sf::Color::Black);
	mywindow->draw(topmenuText);

	return 0;
}

//New function
int mainMenu::topMenuRemote(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	char outputBuffer[100];

	mywindow->clear();

	//Print backgrounds
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(otherGameTextures->at(1));
	mywindow->draw(backgroundSprite);

	sf::Sprite menuBackgroundSprite;
	menuBackgroundSprite.setTexture(otherGameTextures->at(6));
	menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
	mywindow->draw(menuBackgroundSprite);

	//Print title text
	sf::Text titleText("Remote Game", *myFont, 50);
	titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
	titleText.setFillColor(sf::Color::Black);
	mywindow->draw(titleText);

	//Draw JOIN and HOST
	mywindow->draw(topMenuButtons.at(14).mySprite);
	mywindow->draw(topMenuButtons.at(15).mySprite);

	mywindow->display();
	

	//First determine if hosting or joining the remote game
	bool remoteTypeChosen = false;
	while (remoteTypeChosen == false)
	{
		sf::Event playerInput;
		mywindow->waitEvent(playerInput);
		//Keep polling until a legit player input, not just mouse movement.
		if (playerInput.type == sf::Event::MouseButtonPressed && playerInput.mouseButton.button == sf::Mouse::Left)	//Must be mouse click
		{
			//Get mouse position
			sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));
			//Print host or join buttons, get input, see which one selected
			int hostRemoteButton = 15;
			bool withinHost = (topMenuButtons)[hostRemoteButton].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinHost == true)
			{
				gameType = remoteHost;
				remoteTypeChosen = true;
			}
			int joinRemoteButton = 14;
			bool withinJoin = (topMenuButtons)[joinRemoteButton].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinJoin == true)
			{
				gameType = remoteClient;
				remoteTypeChosen = true;
			}
		}
	}

	//Prompt for IP
	snprintf(outputBuffer, 100, "Input IP Address of host:\n");
	sf::String announceString = outputBuffer;
	sf::String inputIPAdressString;
	inputIPAdressString = playerInputString(mywindow, myFont, announceString, 1, "remote", 330);
	remoteHostIPAddress = new sf::IpAddress(inputIPAdressString);

	//Prompt for port
	snprintf(outputBuffer, 100, "Input host Port number:\n");
	announceString = outputBuffer;
	sf::String inputPort = playerInputString(mywindow, myFont, announceString, 1, "remote", 330);
	remoteHostPortNumber = std::stoi(std::string(inputPort));
	
	if (gameType == remoteHost)
	{
		//Draw options and take input
		printTopMenuBackground();
		//Draw two buttons for load/new
		for (int i = 0; i < 2; i++)
		{
			mywindow->draw(topMenuButtons.at(i).mySprite);
		}

		mywindow->display();

		//Get input
		//Determine if load or new
		bool loadOrNewChosen = false;
		while (loadOrNewChosen == false)
		{
			sf::Event playerInput;
			mywindow->waitEvent(playerInput);
			//Keep polling until a legit player input, not just mouse movement.
			if (playerInput.type == sf::Event::MouseButtonPressed && playerInput.mouseButton.button == sf::Mouse::Left)	//Must be mouse click
			{
				//Get mouse position
				sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));
				char placeholder = 'a';
				//Print load and new buttons, get input, see which one selected
				bool withinNew = (topMenuButtons)[newGame].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinNew == true)
				{
					loadOrNewChosen = true;

					int goBack = topMenuNew(&placeholder, boardToPlay, InputLayer);
					if (goBack == 0)
					{
						InputLayer->status = gameBoard;
					}
					skipOneInput = true;
				}
				bool withinLoad = (topMenuButtons)[loadGame].checkWithinButton(mousePosition.x, mousePosition.y);
				if (withinLoad == true)
				{
					loadOrNewChosen = true;

					int goBack = topMenuLoad(&placeholder, boardToPlay, InputLayer);
					if (goBack == 0)
					{
						InputLayer->status = gameBoard;
					}
					skipOneInput = true;
				}
			}
		}
		//which should give player number choice and name
	}
	else
		if (gameType == remoteClient)
		{
			//attempt to establish connection with host
			sf::TcpSocket socket;

			sf::Time timeout = sf::seconds(5);
			sf::Socket::Status status = socket.connect(*remoteHostIPAddress, remoteHostPortNumber, timeout);

			if (status == sf::Socket::Status::NotReady || status == sf::Socket::Status::Error)
				std::cout << "Socket error" << std::endl;


			//Prompt for player number - do not ask for name, since that is explicitly asked from host.
			//This means host has to ask the player what they want their name to be.
			snprintf(outputBuffer, 100, "Input your player number:\n");
			sf::String announceString = outputBuffer;
			sf::String inputPlayerNumberString = playerInputString(mywindow, myFont, announceString, 1, "remote", 330);

			myPlayerNumber = std::stoi(std::string(inputPlayerNumberString));
		}


	return 0;
}


int mainMenu::topMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	//Reset game type
	gameType = unchosen;

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

		//Remote play
		const int topRemoteButton = 16;
		bool withinRemoteButton = (topMenuButtons)[topRemoteButton].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinRemoteButton == true)
		{
			char placeholder = 'a';
			int goBack = topMenuRemote(&placeholder, boardToPlay, InputLayer);
			if (goBack == 0)
			{
				//If hosting, either take your turn, or pass to client
				if (gameType == remoteHost)
				{
					//If its host's turn, take turn.
					//If compie's turn, they take their turn.
					if (boardToPlay->playerFlag == myPlayerNumber || boardToPlay->playerRoster[boardToPlay->playerFlag].playerType == computerPlayer)
					{
						InputLayer->status = gameBoard;
						menuStatus = playingMap;
					}
					else
					{
						InputLayer->status = waitingForClient;
						menuStatus = playingMap;
					}
				}
				else
					if (gameType == remoteClient)
					{
						InputLayer->status = waitingForHost;
						menuStatus = playingMap;
						remoteClientAwaitingFirstUpdate = true;
					}

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

		//Quit game
		bool withinQuitButton = (topMenuButtons)[7].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinQuitButton == true)
		{
			mywindow->close();
			exit(0);
		}

	}
	return 0;
}

int mainMenu::printTopMenu()
{

	printTopMenuBackground();

	//Draw new and load
	for (int i = 0; i < 2; i++)
	{
		mywindow->draw(topMenuButtons.at(i).mySprite);
	}

	//Draw editor mode
	if (editorMode == false)
	{
		mywindow->draw(topMenuButtons.at(2).mySprite);
	}
	else 
	{
		mywindow->draw(topMenuButtons.at(3).mySprite);
	}
	
	//Draw quit
	mywindow->draw(topMenuButtons.at(7).mySprite);

	//Draw remote play
	mywindow->draw(topMenuButtons.at(16).mySprite);

	mywindow->display();

	return 0;
}

/*int mainMenu::printWaitingScreen()
{
	mywindow->clear();
	sf::String waitingScreenString("Waiting for other player(s) \n");
	sf::Text text(waitingScreenString, *myFont, 20);
	mywindow->draw(text);
	mywindow->display();
	return 0;
}*/


int mainMenu::topMenuNew(char* Input, MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	if (computerPlayerRoster.empty() == false)
	{
		computerPlayerRoster.clear();
	}

	sf::Event playerInput;

	//First draw out buttons, then take in input from mouse.
	printTopMenuBackground();

	//Draw three buttons for top menu new
	for (int i = 4; i < 7; i++)
	{
		mywindow->draw(topMenuButtons.at(i).mySprite);
	}
	mywindow->display();

	//If remote host, skip this step - gameType already decided
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
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				nextTopMenuNewString = "Skirmish selected. Press any key to continue.\n";
				sf::Text newText(nextTopMenuNewString, *myFont, menuTextSize);
				newText.setPosition(330 + MM_WIDTH_OFFSET, 300 + MM_HEIGHT_OFFSET);
				newText.setFillColor(sf::Color::Black);
				mywindow->draw(newText);
				mywindow->display();

				gameType = localSkirmish;

				//Await player input to move past the selected item.
				getValidPlayerInput(mywindow);
				mywindow->clear();

			}

			bool withinCampaignButton = (topMenuButtons)[newCampaign].checkWithinButton(mousePosition.x, mousePosition.y);
			if (withinCampaignButton == true)
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				nextTopMenuNewString = "Local campaign selected. Press any key to continue.\n";
				sf::Text newText(nextTopMenuNewString, *myFont, menuTextSize + 4);
				newText.setPosition(330 + MM_WIDTH_OFFSET, 300 + MM_HEIGHT_OFFSET);
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
	getValidPlayerInput(mywindow);
	mywindow->clear();

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
		if (gameType == localSkirmish || gameType == remoteHost)
		{
			bool scenarioChosen = false;
			std::string chosenScenarioName = "";
			std::vector <std::string> listOfScenarios;
			int highlightedName = 0;

			const std::filesystem::path myPath{ ".\\scenarios" };
			for (const auto& entry : std::filesystem::directory_iterator(myPath))
			{
				listOfScenarios.push_back(entry.path().string());
				std::cout << entry.path() << std::endl;
			}

			for (int i = 0; i < listOfScenarios.size(); i++)
			{
				listOfScenarios.at(i).erase(0, 12);
				std::cout << listOfScenarios.at(i) << std::endl;
			}

			while (scenarioChosen == false)
			{
				mywindow->clear();
				anotherTopMenuNewString += "Choose which scenario to load (Case sensitive): \n";

				//To print list of scenarios, first determine start and end points
				int startPoint = highlightedName - 5;
				if (startPoint < 0)
					startPoint = 0;
				int endPoint = highlightedName + 5;
				if (endPoint > listOfScenarios.size() - 1)
					endPoint = listOfScenarios.size() - 1;

				//Print backgrounds
				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				//Print title text
				sf::Text titleText("New Game", *myFont, 50);
				titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
				titleText.setFillColor(sf::Color::Black);
				mywindow->draw(titleText);

				//Print text elements and display
				for (int i = startPoint; i < endPoint + 1; i++)
				{
					sf::Text scenarioListText(listOfScenarios.at(i), *myFont, menuTextSize + 4);
					scenarioListText.setPosition(330 + MM_WIDTH_OFFSET, 300 + MM_HEIGHT_OFFSET + 30 * i);

					//Draw White if highlighted
					if (i == highlightedName)
						scenarioListText.setFillColor(sf::Color::Blue);
					else
						scenarioListText.setFillColor(sf::Color::Black);

					mywindow->draw(scenarioListText);
					//Otherwise draw normal
				}

				//Print up and down buttons and display
				for (int i = 8; i < 11; i++)
				{
					mywindow->draw(topMenuButtons.at(i).mySprite);
				}
				mywindow->display();


				sf::Event playerInput;
				mywindow->waitEvent(playerInput);

				//Keep polling until a legit player input, not just mouse movement.
				if ((playerInput.type == sf::Event::MouseButtonReleased && playerInput.mouseButton.button == sf::Mouse::Left)
					|| playerInput.type == sf::Event::KeyReleased)
					//Must be mouse click
				{
					//Get mouse position
					sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

					//Take user input

					bool withinUpButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinUpButton = topMenuButtons[8].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinUpButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Up))
					{
						highlightedName--;
						if (highlightedName < 0)
							highlightedName = 0;
					}

					bool withinDownButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinDownButton = topMenuButtons[9].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinDownButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Down))
					{
						highlightedName++;
						if (highlightedName > listOfScenarios.size() - 1)
							highlightedName = listOfScenarios.size() - 1;
					}

					bool withinSelectButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinSelectButton = (topMenuButtons)[10].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinSelectButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::Enter))
					{
						scenarioChosen = true;
						chosenScenarioName = listOfScenarios.at(highlightedName);
					}
				}

			}


			std::string newScenario = chosenScenarioName;
			newGameMap.open(".\\scenarios\\" + newScenario);
			if (newGameMap.is_open())
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				anotherTopMenuNewString = "Successfully loaded. Press any key to continue.\n";
				sf::Text newText(anotherTopMenuNewString, *myFont, menuTextSize);
				newText.setPosition(330 + MM_WIDTH_OFFSET, 200 + MM_HEIGHT_OFFSET);
				newText.setFillColor(sf::Color::Black);

				mywindow->draw(newText);
				mywindow->display();

				//Wait for one input.
				getValidPlayerInput(mywindow);

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
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				sf::Text newText(CampaignBriefing, *myFont, menuTextSize);
				newText.setPosition(60 + MM_WIDTH_OFFSET, 100 + MM_HEIGHT_OFFSET);
				newText.setFillColor(sf::Color::Black);
				mywindow->draw(newText);
				mywindow->display();

				//Wait for one input.
				getValidPlayerInput(mywindow);

				newCampaign >> newMission;
				newGameMap.open(".\\campaigns\\" + newCampaignName + "\\" + newMission+ ".txt");

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
	loadGameData(boardToPlay, InputLayer, &newGameMap);
	newGameMap.close();

	//Flush event queue to clear out "Enter" and other rifraf
	sf::Event throwAwayEvent;
	while (mywindow->pollEvent(throwAwayEvent));

	//We added one to the array, just like treasury, for easy access.
	int numberOfCompies = 0;
	sf::String inputName;

	//Resize computer Roster for easy access.
	computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);

	if (gameType == localSkirmish || gameType == remoteHost) //Campaign map sticks to names and values supplied
	{
		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			bool playerTypeDecided = false;
			while (!playerTypeDecided)
			{
				mywindow->clear();

				//Draw background
				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				//Print title text
				sf::Text titleText("New Game", *myFont, 50);
				titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
				titleText.setFillColor(sf::Color::Black);
				mywindow->draw(titleText);

				char buffer[100];
				snprintf(buffer, 100, "Player %d: Choose remote/local human or computer.\n", i);
				sf::String topMenuNewString = buffer;
				sf::Text anotherText(topMenuNewString, *myFont, menuTextSize);
				anotherText.setPosition(MM_WIDTH_OFFSET + 330, MM_HEIGHT_OFFSET + 200);
				anotherText.setFillColor(sf::Color::Black);
				mywindow->draw(anotherText);

				//Draw comp/human buttons (If remote, two types of human players - local and remote)
				mywindow->draw(topMenuButtons.at(11).mySprite);
				mywindow->draw(topMenuButtons.at(12).mySprite);
				if(gameType == remoteHost)
					mywindow->draw(topMenuButtons.at(16).mySprite);

				mywindow->display();

				sf::Event playerInput;
				mywindow->waitEvent(playerInput);

				//Keep polling until a legit player input, not just mouse movement.
				if ((playerInput.type == sf::Event::MouseButtonReleased && playerInput.mouseButton.button == sf::Mouse::Left)
					|| playerInput.type == sf::Event::KeyReleased)
					//Must be mouse click
				{
					//Get mouse position
					sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

					//Determine whether they want computer or human for this player
					bool withinCompButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinCompButton = (topMenuButtons)[11].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinCompButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::C))
					{
						boardToPlay->playerRoster[i].playerType = computerPlayer;
						computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay, -1, -1, -1, -1, -1);	//Use default behaviors
						playerTypeDecided = true;
					}

					bool withinLocalHumanButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinLocalHumanButton = (topMenuButtons)[12].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinLocalHumanButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::H))
					{
						boardToPlay->playerRoster[i].playerType = localHumanPlayer;
						playerTypeDecided = true;
					}

					bool withinRemoteHumanButton = false;
					if (playerInput.type == sf::Event::MouseButtonReleased)
						withinRemoteHumanButton = (topMenuButtons)[12].checkWithinButton(mousePosition.x, mousePosition.y);
					if (withinRemoteHumanButton == true || (playerInput.type == sf::Event::KeyReleased && playerInput.key.code == sf::Keyboard::H))
					{
						boardToPlay->playerRoster[i].playerType = remoteHumanPlayer;
						playerTypeDecided = true;
					}
				}

			}

			mywindow->clear();
			char buffer[100];
			snprintf(buffer, 100, "Input Player %d's name: \n", i);
			sf::String announceString = buffer;
			inputName = playerInputString(mywindow, myFont, announceString, 1, "new", 330);

			boardToPlay->playerRoster[i].name = inputName;


			bool factionDecided = false;
			while (!factionDecided)
			{
				mywindow->clear();

				sf::Sprite backgroundSprite;
				backgroundSprite.setTexture(otherGameTextures->at(1));
				mywindow->draw(backgroundSprite);

				sf::Sprite menuBackgroundSprite;
				menuBackgroundSprite.setTexture(otherGameTextures->at(6));
				menuBackgroundSprite.setPosition(MM_WIDTH_OFFSET, MM_HEIGHT_OFFSET);
				mywindow->draw(menuBackgroundSprite);

				for (int i = 0; i < factionChoiceButtons.size(); i++)
					mywindow->draw(factionChoiceButtons.at(i).mySprite);

				//Print title text
				sf::Text titleText("New Game", *myFont, 50);
				titleText.setPosition(MM_WIDTH_OFFSET + 470, MM_HEIGHT_OFFSET + 70);
				titleText.setFillColor(sf::Color::Black);
				mywindow->draw(titleText);

				std::string topMenuNewString = "Choose this player's faction. \n";
				sf::Text factionChooseText(topMenuNewString, *myFont, menuTextSize);
				factionChooseText.setPosition(500 + MM_WIDTH_OFFSET, 140 + MM_HEIGHT_OFFSET);
				factionChooseText.setFillColor(sf::Color::Black);
				mywindow->draw(factionChooseText);

				mywindow->display();

				//Wait for a mouse click, then check if it's within a faction box.
				sf::Event factionChoiceEvent;
				mywindow->waitEvent(factionChoiceEvent);

				//Keep polling until a legit player input, not just mouse movement.
				if (factionChoiceEvent.type == sf::Event::MouseButtonPressed && factionChoiceEvent.mouseButton.button == sf::Mouse::Left)	//Must be mouse click
				{
					//Check if within faction box
					sf::Vector2i mousePosition = sf::Mouse::getPosition(*(mywindow));

					for (int x = 0; x < factionChoiceButtons.size(); x++)
					{
						bool withinFactionButton = (factionChoiceButtons)[x].checkWithinButton(mousePosition.x, mousePosition.y);

						if (withinFactionButton == true)
						{
							//Have to offset for neutral player, since neutral faction type is 0, but [0] in the choice array is N. Redonia.
							boardToPlay->playerRoster[i].playerFaction = factionType(x + 1);
							factionDecided = true;
						}

					}
				}

			}

		}
	}
	else	//If campaign game we just give player 1's name, and compie has already been initialized.
	{
		mywindow->clear();
		std::string topMenuNewString = "Input Player 1's name: \n";

		inputName = playerInputString(mywindow, myFont, topMenuNewString, 1, "new", 400);
		boardToPlay->playerRoster[1].name = inputName;

	}

	//Determines if they print or not.
	int numberOfHumanPlayers = 0;
	for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
	{
		if (boardToPlay->playerRoster[i].playerType == localHumanPlayer || boardToPlay->playerRoster[i].playerType == remoteHumanPlayer)
			numberOfHumanPlayers++;
	}

	if (numberOfHumanPlayers < 2)
	{
		boardToPlay->isItSinglePlayerGame = true;
	}
	else boardToPlay->isItSinglePlayerGame = false;

	menuStatus = playingMap;

	//If we start new game, stop menu music since we're getting briefing music
	if (musicArray[0].getStatus() == sf::SoundSource::Playing)
		musicArray[0].stop();

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
	std::ifstream loadGameStream;

	//Prompt user input and find game to load
	bool didItLoad = determineGameToLoad(boardToPlay, InputLayer, &loadGameStream);

	if (didItLoad == true)
	{
		//Actually load scenario. Initialize board, etc.
		loadGameData(boardToPlay, InputLayer, &loadGameStream);
		loadGameStream.close();

		//Determines if they print or not.
		int numberOfHumanPlayers = 0;
		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			if (boardToPlay->playerRoster[i].playerType == localHumanPlayer || boardToPlay->playerRoster[i].playerType == remoteHumanPlayer)
			{
				numberOfHumanPlayers++;
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
	else
	{
		//Back button
		menuStatus = topmenu;
		return 1;
	}
}

//based on inputLayers waitingForNextLocalPlayer, this function MAY want to be blocking,
//with while loopand all, because once it comes out of this, its going to want to print, and the context may change.
//Blocking function?!?! (We may not want blocking. Maybe just poll for messages and then move on) that waits for the remote player to finish.
//Then either passes the game to the next remote player, or transitions to host/compie control.
//While this function is running, printWaitingForRemoteClient() is printed.
//This function doesn't have while loop built in to allow control to go back to playGame().
int mainMenu::waitingForRemoteClient(MasterBoard* boardToPlay, inputLayer* InputLayer)
{
	sf::TcpListener listener;

	InputLayer->printWaitingScreenRemote(boardToPlay);

	// bind the listener to a port
	if (listener.listen(remoteHostPortNumber) != sf::Socket::Done)
	{
		std::cout << "Could not bind listener to a port" << std::endl;
	}

	while (menuStatus != playingMap)
	{

		// accept a new connection
		//This is blocking
		sf::TcpSocket client;
		if (listener.accept(client) == sf::Socket::Done)
		{
			sf::Packet receivedPacket;
			//First see if client is updating or asking for update
			client.receive(receivedPacket);

			sf::String statusLine;
			receivedPacket >> statusLine;

			if (statusLine == "Request_Update")
			{
				sf::Packet updateToSend;

				updateToSend << "Update_Follows";
				
				saveGameDataToPacket(boardToPlay, &updateToSend , InputLayer);

				client.send(updateToSend);

				client.disconnect();
			}
			else
			if(statusLine == "Update_Follows")
			{
				sf::String nextLine;
				int freshUpdate = loadGameDataFromPacket(boardToPlay, &receivedPacket , InputLayer);

				//update our own internal game data
				//If it's host's turn or compie's turn, take back control
				if (freshUpdate == 0)
				{
					menuStatus = playingMap;
				}

			}
		}
		else
		{
			// error...
		}

	}
	return 0;
}

int mainMenu::waitingForRemoteHost(MasterBoard* boardToPlay , inputLayer* InputLayer)
{
	//Client asking for update:
	sf::TcpSocket socket;
	sf::Packet packetToSend;

	//Bad to print here but it's stuck in a loop so do it anyway
	InputLayer->printWaitingScreenRemote(boardToPlay);

	while (menuStatus != playingMap)
	{

		sf::Time timeout = sf::seconds(5);
		sf::Socket::Status status = socket.connect(*remoteHostIPAddress, remoteHostPortNumber, timeout);
		if (status != sf::Socket::Disconnected)
		{
			packetToSend.clear();
			sf::String message = "Request_Update";
			packetToSend << message;

			//Ask host for an update
			socket.send(packetToSend);

			//Get status of game from host - are we still waiting or is there an update?
			//If update available, get updated game file.
			//If no update available, do nothing.
			sf::Packet receivePacket;
			socket.receive(receivePacket);
			
			sf::String header;
			receivePacket >> header ;
		
			//Always load up the most recent game data.
			//This may cause unecessary sending/receiving game data. We don't care.
			if (header == "Update_Follows")
			{
				int freshUpdate = loadGameDataFromPacket(boardToPlay, &receivePacket , InputLayer);

				//If it's a fresh update (Our NEXT turn) then play, otherwise still waiting
				//Compies are played by host player
				if (freshUpdate == 0)
				{
					menuStatus = playingMap;
				}
			}
			
			//Then close socket.
			socket.disconnect();

		}
		else
		{
			//If can't open socket, try again later
			std::cout << "Couldn't conenct" << std::endl;
			continue;
		}

		//Delay before requesting update again;
		sf::Clock timer;
		timer.restart();
		sf::Time elapsedTime;
		while (elapsedTime.asSeconds() < updateRequestDelay)
		{
			elapsedTime = timer.getElapsedTime();
		}

	}


	return 0;
}

int mainMenu::loadGameDataFromPacket(MasterBoard* boardToPlay, sf::Packet* gamePacket, inputLayer* InputLayer)
{
	std::string ThrowawayString;

	//Then load the game turn.
	int newTurnValue = 0;
	*gamePacket >> ThrowawayString;
	*gamePacket >> newTurnValue;
	
	//This is the current player whos turn it is:
	int nextPlayerFlag = 0;
	*gamePacket >> ThrowawayString;
	*gamePacket >> nextPlayerFlag;

	//Host checks for either compie's next turn, our host's next turn
	if (gameType == remoteHost)
	{
		if ((nextPlayerFlag == myPlayerNumber || boardToPlay->playerRoster[nextPlayerFlag].playerType == computerPlayer)
			&& gameTurn + 1 == newTurnValue)
		{
			gameTurn = newTurnValue;
			boardToPlay->playerFlag = nextPlayerFlag;

		}
		else return 1;
	}
	else
	{
		//Non-host merely checks for our next turn
		if (nextPlayerFlag == myPlayerNumber && gameTurn + 1 == newTurnValue)
		{
			gameTurn = newTurnValue;
			boardToPlay->playerFlag = nextPlayerFlag;
		}
		else return 1;
	}

	boardToPlay->clearBoard(InputLayer);
	   

	//Pretty sure this is never true because loadfrompacket means it's coming after someone took their turn
	//veryFirstTurn = true;
	   
	*gamePacket >> ThrowawayString;
	*gamePacket >> isItSaveGame;

	//First load number of players from packet
	*gamePacket >> ThrowawayString;
	*gamePacket >> boardToPlay->NUMBEROFPLAYERS;
	boardToPlay->playerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);

	//*********************************************//
	//Input "Mission load" data here
	//*gamePacket >> ThrowawayString;			//Campaign_game
	//*gamePacket >> boardToPlay->missionFlag;

	//Must be remote skirmish
	/*if (gameType == unchosen)
	{
		if (boardToPrint->missionFlag == 1)
			gameType == localCampaign;
		else gameType = localSkirmish;
	}*/

	//* dataStream >> ThrowawayString;		//Campaign Name - used to navigate menu for next level
	//*dataStream >> boardToPrint->campaignName;

	*gamePacket >> ThrowawayString;
	*gamePacket >> boardToPlay->scenarioOrMissionName;	//Must be exactly the same as the txt file's name

	//*dataStream >> ThrowawayString;		//TurnLength - either 0 for no max turn length, or int to indicate length
	//*dataStream >> boardToPrint->missionTurnLength;

	//*dataStream >> ThrowawayString;		//Indicates who will win if turn length is met.
	//*dataStream >> boardToPrint->whoHoldsOut;

	//*dataStream >> ThrowawayString;		//Name of next mission
	//*dataStream >> nextMissionName;

	//briefingLineNumber = 0;
	//*dataStream >> ThrowawayString;		//String with mission info
	//*dataStream >> briefingLineNumber;
	//missionBriefing = "";
	/*
	for (int i = 0; i < briefingLineNumber; i++)
	{
		std::string singleLine;
		std::getline(*dataStream, singleLine);
		missionBriefing += singleLine;
		missionBriefing += "\n";
	}*/


	//End Mission Data Load
	//*********************************************//

	//Must verify this is updated by checking game turn and player flag.
	//If not same player flag, AND next game turn, return 1.




	//Unique to save_game vs scenario. Load player names (User names):
	*gamePacket >> ThrowawayString;
	for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
	{
		int playerType = 0;
		int factionType = 0;

		*gamePacket >> boardToPlay->playerRoster[i].name;	//For new game this should be ~
		*gamePacket >> playerType;
		*gamePacket >> boardToPlay->playerRoster[i].stillAlive;
		*gamePacket >> boardToPlay->playerRoster[i].treasury;
		*gamePacket >> factionType;

		switch (factionType)
		{
		case(0):
			boardToPlay->playerRoster[i].playerFaction = NeutralFaction;
			break;
		case(1):
			boardToPlay->playerRoster[i].playerFaction = NorthRedonia;
			break;
		case(2):
			boardToPlay->playerRoster[i].playerFaction = SouthRedonia;
			break;
		case(3):
			boardToPlay->playerRoster[i].playerFaction = Ormosa;
			break;
		case(4):
			boardToPlay->playerRoster[i].playerFaction = Torran;
			break;
		}


		if (playerType == 0)
			boardToPlay->playerRoster[i].playerType = localHumanPlayer;
		else if (playerType == 1)
		{
			boardToPlay->playerRoster[i].playerType = computerPlayer;
		}
		else if (playerType == 2)
		{
			boardToPlay->playerRoster[i].playerType = remoteHumanPlayer;
		}

	}



	//Then load fog of war status - in "new" game this should be overriden by player choice.
	*gamePacket >> ThrowawayString;
	*gamePacket >> boardToPlay->fogOfWar;

	//If this is a new game, clear the treasury.
	//(If going from a game in middle of play, to new game).
	//Otherwise leave the values from the loadGameData portion.
	//Clear treasury not necessary, just use whatever is in the scenario.

	//First load the map size:
	//Ideally we can create new vector or whatever to have different map size:
	*gamePacket >> ThrowawayString;
	*gamePacket >> boardToPlay->BOARD_WIDTH;
	*gamePacket >> ThrowawayString;
	*gamePacket >> boardToPlay->BOARD_HEIGHT;

	//Once board height and width are loaded, we try to adjust window size to either smaller or larger.
	if (boardToPlay->BOARD_WIDTH < MAX_WINDOW_WIDTH)
	{
		boardToPlay->WINDOW_WIDTH = boardToPlay->BOARD_WIDTH;
	}
	else
	{
		boardToPlay->WINDOW_WIDTH = MAX_WINDOW_WIDTH;
	}
	if (boardToPlay->BOARD_HEIGHT < MAX_WINDOW_HEIGHT)
	{
		boardToPlay->WINDOW_HEIGHT = boardToPlay->BOARD_HEIGHT;
	}
	else
	{
		boardToPlay->WINDOW_HEIGHT = MAX_WINDOW_HEIGHT;
	}

	//Resize the map based on new data.
	//Not sure why we have to give an additional line but it segfaulted otherwise
	boardToPlay->Board.resize(boardToPlay->BOARD_WIDTH);	//+1)
	for (int i = 0; i < boardToPlay->BOARD_WIDTH; i++)
	{
		boardToPlay->Board[i].resize(boardToPlay->BOARD_HEIGHT);
	}

	//Then load player data:


	//Then load map
	*gamePacket >> ThrowawayString;

	for (int y = 0; y < boardToPlay->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPlay->BOARD_WIDTH; x++)
		{
			std::string checkonMe = "L";
			*gamePacket >> checkonMe;
			boardToPlay->Board[x][y].symbol = checkonMe.at(0);

			//Also resize for withinVision
			boardToPlay->Board[x][y].withinVision.resize(boardToPlay->NUMBEROFPLAYERS + 1);
			for (int n = 0; n < boardToPlay->NUMBEROFPLAYERS + 1; n++)
				boardToPlay->Board[x][y].withinVision[n] = false;

		}
	}


	//This is for properties.
	int inputProperties;
	*gamePacket >> ThrowawayString;
	for (int y = 0; y < boardToPlay->BOARD_HEIGHT; y++)
	{

		for (int x = 0; x < boardToPlay->BOARD_WIDTH; x++)
		{
			*gamePacket >> inputProperties;
			boardToPlay->Board[x][y].controller = inputProperties;
		}
	}
	//After team data is loaded, set property characteristics.
	setCharacteristics(boardToPlay);

	//Load minion ban list. Start with number of banned minion types.
	//Number of banned types
	*gamePacket >> ThrowawayString;
	int numberOfBannedTypes = 0;
	*gamePacket >> numberOfBannedTypes;

	*gamePacket >> ThrowawayString;
	//Then create ban list
	for (int i = 0; i < numberOfBannedTypes; i++)
	{
		std::string bannedMinionType;
		*gamePacket >> bannedMinionType;
		boardToPlay->banList.push_back(bannedMinionType);
	}


	//Then load minion data:
	*gamePacket >> ThrowawayString;
	int numberOfMinions;
	*gamePacket >> numberOfMinions;
	int health, locationX, locationY, team, seniority, status, veterancy, capturePoints, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo;
	std::string type;
	*gamePacket >> ThrowawayString;

	//Initialize all minions.
	for (int y = 0; y < numberOfMinions; y++)
	{
		*gamePacket >> type
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
		boardToPlay->createMinion(type, locationX, locationY, team, health, status, veterancy, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo);

		//Set capture status.
		if (capturePoints != 20)
		{
			boardToPlay->Board[locationX][locationY].minionOnTop->isCapturing = true;
			boardToPlay->Board[locationX][locationY].capturePoints = capturePoints;
		}


	}

	//Initialize all compies at this stage if a mission (We are certain who is a compie)
	//Or if it's a load game
	if (boardToPlay->missionFlag == true || isItSaveGame == true)
	{
		computerPlayerRoster.resize(boardToPlay->NUMBEROFPLAYERS + 1);
		for (int i = 1; i <= boardToPlay->NUMBEROFPLAYERS; i++)
		{
			if (boardToPlay->playerRoster[i].playerType == computerPlayer)
				computerPlayerRoster[i].initalizeCompie(this, i, InputLayer, boardToPlay, -1, -1, -1, -1, -1);	//Use default behaviors
		}
	}

	gamePacket->clear();


	return 0;
}

int mainMenu::saveGameDataToPacket(MasterBoard* boardToPlay, sf::Packet* gamePacket , inputLayer* InputLayer)
{
	//First element is the header
	*gamePacket << "Update_Follows";

	//Then indicate


	return 0;
}

//Called once by client to send information to remote host
int mainMenu::updateRemoteHost(MasterBoard* boardToPlay , inputLayer* InputLayer)
{
	//Client sending update:
	sf::TcpSocket socket;
	sf::Packet packetToSend;

	bool stillWaiting = true;
	while (stillWaiting == true)
	{
		sf::Time timeout = sf::seconds(5);
		sf::Socket::Status status = socket.connect(*remoteHostIPAddress, remoteHostPortNumber, timeout);
		if (status != sf::Socket::Disconnected)
		{
			packetToSend.clear();

			//Generate my save data and send it
			saveGameDataToPacket(boardToPlay, &packetToSend, InputLayer);

			socket.send(packetToSend);
		}
	}

	return 0;
}
