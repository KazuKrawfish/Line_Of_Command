//Copyright 2022, Supercontinent Software Ltd.
//
//	button.hpp
//
//Class for button interface. Handles printing and taking input.
//It knows its size and location for taking mouse clicks.
//It contains its own sprite for easy printing.

#ifndef BUTTON_HPP__
#define BUTTON_HPP__

//Button.cpp and Button.hpp
#include <string.h>
#include <SFML/Graphics.hpp>



//change in button.h:19
enum buttonType {
	newGame, loadGame, editorModeOff, editorModeOn, newCampaign, newSkirmish, topBack1, loadCampaign, loadSkirmish,
	exitToMainMenu, saveGame, loadGameFromGame, restart, soundOn, soundOff, speedNormal, speedFast, endTurn, resumePlay,
	factoryButton, airbaseButton, portButton, statusButton, factionButton, quitButton, upDownButton, selectButton, 
	remoteClientButton, remoteHostButton , remotePlayButton};




class Button
{
public:
	int xCoord = 0;
	int yCoord = 0;
	int width = 0;
	int height = 0;


public:
	buttonType myType;
	std::string myName = "~";

	//GRAPHICS ///////////////////////////
	//Each button has its own individual texture loaded separately from the texture array
	sf::Sprite mySprite;
	sf::Texture* myTexture;
	//GRAPHICS ///////////////////////////

	Button(int inputXCoord, int inputYCoord, int inputButtonType, sf::Texture* inputTexture, std::string inputName)
	{
		//Assign values to members
		xCoord = inputXCoord;
		yCoord = inputYCoord;

		myType = buttonType(inputButtonType);
		myTexture = inputTexture;
		mySprite.setTexture(*myTexture);
		mySprite.setPosition(xCoord, yCoord);

		sf::Vector2u dimensions = myTexture->getSize();
		width = dimensions.x;
		height = dimensions.y;

		myName = inputName;

	}

	bool checkWithinButton(int inputXCoord, int inputYCoord)
	{
		if (inputXCoord < xCoord + width && inputXCoord > xCoord)
			if (inputYCoord < yCoord + height && inputYCoord > yCoord)
				return true;

		return false;
	}

};



#endif /* BUTTON_HPP__ */