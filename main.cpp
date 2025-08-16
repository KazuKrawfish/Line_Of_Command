//Copyright 2025, Supercontinent Software Ltd.
//
//	main.cpp
//
//Line of Command - Tactical turn based wargame. Inspired by the classic Gameboy Advance game Advance Wars.
//Built with SFML library.


#include <iostream>
#include <string_view>
#include <ctype.h>
#include <fstream>
#include "MasterBoard.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include "compie.hpp"
#include "mainmenu.h"
#include <thread>

std::vector <std::vector<sf::IntRect>> rectArray;

int initializeTextureArray(std::string directory, const std::vector <std::string>& imageNameList,  std::vector <sf::Texture>& buttonTextureArray)
{
	//Need longer list of actual buttons

	for (int i = 0; i < buttonTextureArray.size(); i++)
	{
		if (!buttonTextureArray[i].loadFromFile(directory + "/" + imageNameList[i] + ".png"))
		{
			std::cout << "Couldn't load button: " << imageNameList[i] << std::endl;

		}
	}
	return 0;
}

//Need to make custom constructors for NULL window for each of these items.
int buildThread (std::string inputConfigFileName, std::string inputMapListName)
{
	mainMenu MainMenu(inputConfigFileName, inputMapListName);
	inputLayer InputLayer(&MainMenu);
	MasterBoard GameBoard(NULL, NULL);

	MainMenu.introScreen(&GameBoard, &InputLayer);

	return 1;
}

int main()
{
	//BATLELAB
	//First open battle lab config and determine if we are in battle lab mode.
	//Normal play is 0, 1 is battle lab   
	std::ifstream battleLabConfigFile;
	std::string battleLabConfigFileName = ".\\battleLab\\battleLabConfig.txt";
	battleLabConfigFile.open(battleLabConfigFileName);
	bool mainBattleLabOn = false;
	std::string ThrowawayString;

	if (battleLabConfigFile.is_open() == true)
	{
		battleLabConfigFile >> ThrowawayString;
		battleLabConfigFile >> mainBattleLabOn;
	}
	else
	{
		std::cout << "Could not open battle lab config file." << std::endl;
	}
	battleLabConfigFile.close();

	//Insert a new job each for a series of maps, if we're running battle lab.
	//Config file has all configurations INCLUDING whether or not battle lab is being run.
	//Map list simply contains list of maps to run battle lab on.
	std::ifstream mapListFile;
	std::string mapListName = ".\\battleLab\\mapList.txt";
	mapListFile.open(mapListName);
	std::vector <std::string> mapList;
	std::string mapName;
	if (mapListFile.is_open() == true)
	{
		while (mapListFile >> mapName)
		{
			mapList.push_back(mapName);
		}
	
	}
	else 
	{
		std::cout << "Could not open map list for battle lab" << std::endl;
	}
		
	sf::Color colorWhite;
	//Determine size of desktop for window
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();


	//Load topMenuButton textures
	//NOTE: There is one less texture than total top menu buttons, because top_Back is used twice. So every button after back #2 uses 
	//the index of that button's particular image + 1.
	std::vector <std::string> imageList = { "top_New_Game", "top_Load_Game", "top_Editor_Mode_Off",  "top_Editor_Mode_On",  "top_New_Campaign", 
											"top_New_Skirmish", "top_Back" , "top_Quit", "upButton", "downButton", "Select", "top_Computer", 
											"top_Human", "top_Remote_Join", "top_Remote_Host", "top_Remote_Game", "top_Remote_Human"};
	std::vector <sf::Texture> topMenuButtonTextureArray;
	topMenuButtonTextureArray.resize(imageList.size());
	initializeTextureArray("topMenuButtons", imageList, topMenuButtonTextureArray);

	//Load gameMenuButton textures
	std::vector <std::string> gameMenuButtonImageList = { "exit_To_Main_Menu", "save_Game", "load_Game" , "restart", "sound_On", "sound_Off", "speed_Normal", "speed_Fast",  "end_Turn", "resume_Play", };
	std::vector <sf::Texture> gameMenuButtonTextureArray;
	gameMenuButtonTextureArray.resize(gameMenuButtonImageList.size());
	initializeTextureArray("menuButtons", gameMenuButtonImageList, gameMenuButtonTextureArray);

	//Load other textures
	std::vector <std::string> otherImagesList = { "startScreenBackground", "topMenuBackground", "startScreenStatement", "topMenuBox", "Sidebar", "loadGameBackground",
													"newGameBackground","saveGameBackground", "nextPlayerBackground", "missionBriefingBackground", "victoryDefeatBackground", 
													"dialogBox" , "gameplayBackground", "inGameMenu" , "remoteGameBackground"};
	std::vector <sf::Texture> otherTextureArray;
	otherTextureArray.resize(otherImagesList.size());
	initializeTextureArray("otherImages", otherImagesList, otherTextureArray);

	//Load status indicators
	std::vector <std::string> statusTextureList = { "statusBar_DefenseBonusBox", "statusBar_ProductionBox", "statusBar_CaptureBox", "statusBar_HealthBox", "statusBar_FuelBox", "statusBar_AmmoBox",  "statusBar_MoveState" };
	std::vector <sf::Texture> statusTexturesArray;
	statusTexturesArray.resize(statusTextureList.size());
	initializeTextureArray("otherImages", statusTextureList, statusTexturesArray);

	//Load flags
	std::vector <std::string> factionTexturesList = { "NorthRedoniaFlag" , "SouthRedoniaFlag", "OrmosaFlag" , "TorranFlag" };
	std::vector <sf::Texture> factionTexturesArray;
	factionTexturesArray.resize(factionTexturesList.size());
	initializeTextureArray("otherImages/Flags", factionTexturesList, factionTexturesArray);

	std::map<char, std::string> symbols = { {'.',"plains.png"},{'+',"forest.png"},{'-',"river.png"},{'n',"settlement.png"} };		//'~','n','^','f','H'}
	std::map<char, sf::Texture> terrainTextures;
	for (std::map<char, std::string>::iterator i = symbols.begin(); i != symbols.end(); i++)
	{
		sf::Texture loader;

		std::cout << i->first << " ; " << i->second << std::endl;
		std::string fileName = "otherImages/Terrain/" + i->second;
		if (!loader.loadFromFile(fileName))
		{
			std::cout << "Couldn't load: " << fileName<< std::endl;

		}
		terrainTextures.emplace(i->first,loader) ;
	}



	//Load music

	std::vector <std::string> gameMusicList = { "introTheme", "briefingTheme" , "victoryTheme" , "ARNRTheme", "southRedoniaTheme", "ormosaTheme", "torranTheme" };
	sf::Music gameMusicArray[7];
#if USE_MUSIC
	for (int i = 0; i < 7; i++)
	{
		if (!gameMusicArray[i].openFromFile("music/" + gameMusicList[i] + ".wav"))
		{
			std::cout << "Couldn't load music: " << gameMusicList[i] << std::endl;

		}
	}
#endif

	sf::Texture mainTexture;
	sf::Texture secondMainTexture;
	sf::Image mainImage;
	sf::Image secondMainImage;

	//Initialize Sounds Array
	const int numberOfSoundEffects = 20;
	std::vector <std::string> soundEffectNames = { "machineGun", "rpg", "cannon", "antiAircraftCannon", "infantryMove", "vehicleMove", "aircraftMove", "buildUnit", "capture", "resupply", "repair", "trapped" };
	std::vector <sf::SoundBuffer > soundEffectBuffers;
	std::vector <sf::Sound> soundEffects;
	soundEffects.resize(numberOfSoundEffects + 1);

	const int rectArrayWidth = 41;
	const int rectArrayHeight = 16;

	//Initialize intRect grid
	rectArray.resize(rectArrayWidth + 1);
	for (int i = 0; i < rectArrayWidth; i++)
	{
		rectArray[i].resize(rectArrayHeight + 1);
		for (int j = 0; j < rectArrayHeight; j++)
		{
			rectArray[i][j].left = i * 52 + 1;
			rectArray[i][j].top = j * 52 + 1;
			rectArray[i][j].height = 50;
			rectArray[i][j].width = 50;
		}
	}

	sf::Font gameFont;
	sf::Font boldGameFont;
	//Load up image and use to initiate texture
	//Also set white to transparent
	if (!mainImage.loadFromFile("tilesAndUnits.png"))
	{
		std::cout << "Couldn't load image!" << std::endl;
	}

	if (!secondMainImage.loadFromFile("tilesAndUnitsAlternate.png"))
	{
		std::cout << "Couldn't load image!" << std::endl;
	}

	//Init sound effect buffer and sound arrays
	soundEffectBuffers.resize(soundEffectNames.size() + 1);
	for (int i = 0; i < soundEffectNames.size(); i++)
	{
		std::string fileName = "soundEffects\\" + soundEffectNames[i] + ".wav";
		if (!soundEffectBuffers[i].loadFromFile(fileName))
		{
			std::cout << "Couldn't load" << soundEffectNames[i] << std::endl;
		}
		//Transfer Sound Buffer to Sounds

		soundEffects[i].setBuffer(soundEffectBuffers[i]);
	}


	//Transfer Images to Textures
	mainImage.createMaskFromColor(colorWhite.White);
	mainTexture.loadFromImage(mainImage);

	secondMainImage.createMaskFromColor(colorWhite.White);
	secondMainTexture.loadFromImage(secondMainImage);

	if (!gameFont.loadFromFile("Orbitron-Medium.ttf"))
	{
		std::cout << "Couldn't load fonts!" << std::endl;
	}
	
	if (!boldGameFont.loadFromFile("Orbitron-Black.ttf"))
	{
		std::cout << "Couldn't load fonts!" << std::endl;
	}

	//If battle lab is on, create threads to handle various maps.
	if (mainBattleLabOn == true)
	{
		//Close window to prevent freezing
		std::vector <std::thread> threadList; 
		std::cout << "Total maps:" << mapList.size() << std::endl;
		for (int i = 0; i < mapList.size(); i++)
		{
			threadList.push_back(std::thread(buildThread, battleLabConfigFileName, mapList.at(i) ));
			std::cout << "Thread created" << std::endl;
		}

		//Rejoin all threads before proceeding.
		for (int i = 0; i < mapList.size(); i++)
		{
			threadList.at(i).join();
			std::cout << "Thread joined" << std::endl;

		}

	}
	else //Otherwise proceed with just one set of instances 
	{
		sf::RenderWindow mainWindow(desktopMode, "Line of Command", sf::Style::Fullscreen);
		//sf::RenderWindow mainWindow(desktopMode, "Line of Command");	


		mainMenu MainMenu(&mainWindow, &mainTexture, &gameFont, &boldGameFont, &topMenuButtonTextureArray, &gameMenuButtonTextureArray, &otherTextureArray, &(gameMusicArray[0]), &factionTexturesArray, battleLabConfigFileName, mapListName);

		inputLayer InputLayer(&MainMenu, &mainWindow, &mainTexture, &secondMainTexture, &gameFont, &boldGameFont, &soundEffects, & MainMenu.gameMenuButtons, & statusTexturesArray, & (gameMusicArray[0]) , &terrainTextures);
		MasterBoard GameBoard(&mainTexture , &secondMainTexture);

		MainMenu.introScreen(&GameBoard, &InputLayer);
	}


	std::cout << "Complete" << std::endl;
	return 0;
}


