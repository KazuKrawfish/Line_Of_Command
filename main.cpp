//Copyright 2022, Supercontinent Software Ltd.
//
//	main.cpp
//
//Line of Command - Tactical turn based wargame. Inspired by the classic Gameboy Advance game Advance Wars.
//Built with SFML library.


#include <iostream>
#include <ctype.h>
#include <fstream>
#include "MasterBoard.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"
#include "compie.hpp"
#include "mainmenu.h"


/*
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#elif defined(__unix__)
#include <iostream>
#include <stdio.h>
#define SetConsoleTextAttribute(X,ATTR)
#define GetStdHandle(x)
#define STD_OUTPUT_HANDLE
#define _getch() getc(stdin)
#else
#error "Unknown build target!"
#endif
*/



//Global for the moment to support faster production
//Everyone needs access to this at all times so it seems reasonable
const int rectArrayWidth = 38;
const int rectArrayHeight = 16;
std::vector <std::vector<sf::IntRect>> rectArray;



int initializeTextureArray(std::string directory, std::vector <std::string> imageNameList, std::vector <sf::Texture>& buttonTextureArray ) //, std::vector <sf::Image> & imageList)
{
	//Need longer list of actual buttons

	for (int i = 0; i < buttonTextureArray.size(); i++)
	{
		if (!buttonTextureArray[i].loadFromFile(directory + "/" + imageNameList[i] + ".png"))
		{
			std::cout << "Couldn't load button: "<< imageNameList[i] << std::endl;

		}
	}
	return 0;
}

int main()
{

		sf::RenderWindow mainWindow(sf::VideoMode(1300, 700), "Line of Command");

		//Load topMenuButton textures
		std::vector <std::string> imageList = { "top_New_Game", "top_Load_Game", "top_Editor_Mode_Off", "top_Editor_Mode_On", "top_New_Campaign", "top_New_Skirmish", "top_Back", "top_Load_Campaign"};
		std::vector <sf::Texture> topMenuButtonTextureArray;
		topMenuButtonTextureArray.resize(imageList.size());
		initializeTextureArray("topMenuButtons", imageList, topMenuButtonTextureArray);

		//Load gameMenuButton textures
		std::vector <std::string> gameMenuButtonImageList = { "exit_To_Main_Menu", "save_Game", "load_Game" , "restart", "sound_On", "sound_Off", "speed_Normal", "speed_Fast",  "end_Turn", "resume_Play", };
		std::vector <sf::Texture> gameMenuButtonTextureArray;
		gameMenuButtonTextureArray.resize(gameMenuButtonImageList.size());
		initializeTextureArray("menuButtons", gameMenuButtonImageList, gameMenuButtonTextureArray);

		//Load other textures
		std::vector <std::string> otherImagesList = { "startScreenBackground", "topMenuBackground", "startScreenStatement", "topMenuBox", "Sidebar", "loadGameBackground", "newGameBackground","saveGameBackground", "nextPlayerBackground", "missionBriefingBackground" };
		std::vector <sf::Texture> otherTextureArray;
		otherTextureArray.resize(otherImagesList.size());
		initializeTextureArray("otherImages", otherImagesList, otherTextureArray);

		//Load status indicators
		std::vector <std::string> statusTextureList = { "statusBar_HealthBox", "statusBar_FuelBox", "statusBar_AmmoBox", "statusBar_CaptureBox", "statusBar_DefenseBonusBox", "statusBar_ProductionBox", "statusBar_MoveState" };
		std::vector <sf::Texture> statusTexturesArray;
		statusTexturesArray.resize(statusTextureList.size());
		initializeTextureArray("otherImages", statusTextureList, statusTexturesArray);


		sf::Texture mainTexture;
		sf::Image mainImage;

		//Load Sounds/Music
		sf::Music introMusic;

		//Initialize Sounds Array
		const int numberOfSoundEffects = 20;
		std::vector <std::string> soundEffectNames = { "machineGun", "rpg", "cannon", "antiAircraftCannon", "infantryMove", "vehicleMove", "buildUnit", "capture", "resupply", "repair", "trapped" };
		std::vector <sf::SoundBuffer > soundEffectBuffers;
		std::vector <sf::Sound> soundEffects;
		soundEffects.resize(numberOfSoundEffects + 1);


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
	//Load up image and use to initiate texture
	//Also set white to transparent
	if (!mainImage.loadFromFile("tilesAndUnits.png"))
	{
		std::cout << "Couldn't load image!" << std::endl;
	}
	if (!introMusic.openFromFile("soundEffects\\introTheme.wav"))
	{
		std::cout << "Couldn't load intro theme!" << std::endl;
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

	sf::Color colorWhite;

	//Transfer Images to Textures
	mainImage.createMaskFromColor(colorWhite.White);
	mainTexture.loadFromImage(mainImage);

	if (!gameFont.loadFromFile("times.ttf"))
	{
		std::cout << "Couldn't load fonts!" << std::endl;
	}
	
	mainMenu MainMenu(&mainWindow, &mainTexture, &gameFont, &topMenuButtonTextureArray, &gameMenuButtonTextureArray, &otherTextureArray, &introMusic);

	inputLayer InputLayer(&MainMenu, &mainWindow, &mainTexture, &gameFont, &soundEffects, & MainMenu.gameMenuButtons, &statusTexturesArray);
	MasterBoard GameBoard(&mainTexture);

	MainMenu.introScreen(&GameBoard, &InputLayer);
	   
}


