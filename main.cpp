//Crisis Front (Project X)
//Or maybe call it Line of Command
//Copyright 2021, Park Family Software Laboratory (ParkLab)


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
const int rectArrayWidth = 32;
const int rectArrayHeight = 11;
std::vector <std::vector<sf::IntRect>> rectArray;


int main()
{

	sf::RenderWindow mainWindow(sf::VideoMode(1300, 700), "Line of Command");

	//Load textures
	sf::Texture mainTexture;
	sf::Image mainImage;
	sf::Texture globeTexture;
	sf::Image globeImage;

		
	//Initialize intRect grid
	rectArray.resize(rectArrayWidth+1);
	for (int i = 0; i < rectArrayWidth; i++)
	{
		rectArray[i].resize(rectArrayHeight+1);
		for (int j = 0; j < rectArrayHeight; j++)
		{

			rectArray[i][j].left = i * 52 + 1;
			rectArray[i][j].top = j * 52 + 1;
			rectArray[i][j].height = 50;
			rectArray[i][j].width = 50;
		}
	}
	
	sf::Font cour;
	//Load up image and use to initiate texture
	//Also set white to transparent
	if (!mainImage.loadFromFile("tilesAndUnits.png"))
	{
		std::cout << "Couldn't load image!" << std::endl;
	}
	if (!globeImage.loadFromFile("globe.png"))
	{
		std::cout << "Couldn't load image!" << std::endl;
	}
	sf::Color colorWhite;
	globeTexture.loadFromImage(globeImage);
	mainImage.createMaskFromColor(colorWhite.White);
	mainTexture.loadFromImage(mainImage);

	if (!cour.loadFromFile("cour.ttf"))
	{
		std::cout << "Couldn't load fonts!" << std::endl;
	}
	
	mainMenu MainMenu(&mainWindow, &mainTexture, &cour, &globeTexture);
	
	inputLayer InputLayer(&MainMenu, &mainWindow , &mainTexture, &cour);
	MasterBoard GameBoard(&mainTexture);

	MainMenu.introScreen(&GameBoard, &InputLayer);

}


