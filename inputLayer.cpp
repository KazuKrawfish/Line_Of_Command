//Copyright 2022, Supercontinent Software Ltd.
//
//	inputLayer.cpp
//
/*
The inputLayer class is responsible for all player-related and interface activities. It interacts with the masterboard class, sending commands
to move minions and such, while receiving data from the board to print to screen for animations and static display. It also goes back up to mainmenu
when necessary, ie. when game ends or player wants to leave the current game.
*/

#include "Minion.hpp"
#include "MasterBoard.hpp"
#include "inputLayer.hpp"
#include "mainmenu.h"
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <cmath>
#include "compie.hpp"
#include <thread>
#include "button.hpp"

char playCharInput(sf::RenderWindow* myWindow);

inputLayer::inputLayer(mainMenu* inputMainMenu, sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* cour, std::vector <sf::Sound>* inputSoundEffects, std::vector <Button>* inputMenuButtons)
{
	inputLayerTexture = gameTexture;
	inputLayerFont = cour;
	inputLayerWindow = myWindow;
	MainMenu = inputMainMenu;
	soundEffects = inputSoundEffects;
	menuButtons = inputMenuButtons;


	//Create buttons for property menus using the gameTexture

	//Overall property menu area is:
	int menuTop = 100;
	int menuLeft = MAX_WINDOW_WIDTH * 50 + 20;

	//Button dimensions
	int buttonHeight = 50;
	int buttonWidth = 50;
	int numberOfButtonsPerRow = 4;

	int y = 0;
	int x = 0;

	//For each minion that can be purchased, create new button and push_back.
	for (int i = 0; i < factoryOptions.size(); i++)
	{
		factoryButtons.emplace_back(menuLeft + x*50, menuTop + y*50 , factoryButton, gameTexture);
		factoryButtons.at(i).mySprite.setTextureRect(rectArray[0][4]);	//Placeholder image
		factoryButtons.at(i).width = 50;
		factoryButtons.at(i).height = 50;
		x++;
		if (x == numberOfButtonsPerRow)
		{
			x = 0;
			y++;
		}
	}
	factoryButtons.at(0).mySprite.setTextureRect(rectArray[0][4]);	//i
	factoryButtons.at(1).mySprite.setTextureRect(rectArray[1][4]);	//s
	factoryButtons.at(2).mySprite.setTextureRect(rectArray[2][4]);  //c
	factoryButtons.at(3).mySprite.setTextureRect(rectArray[4][4]);  //P
	factoryButtons.at(4).mySprite.setTextureRect(rectArray[5][4]);  //r	
	factoryButtons.at(5).mySprite.setTextureRect(rectArray[3][4]);  //a
	factoryButtons.at(6).mySprite.setTextureRect(rectArray[7][4]);  //A
	factoryButtons.at(7).mySprite.setTextureRect(rectArray[8][4]);  //R
	factoryButtons.at(8).mySprite.setTextureRect(rectArray[6][4]);  //T

	//Airbase buttons
	y = 0;	//Reset counters
	x = 0;
	for (int i = 0; i < airbaseOptions.size(); i++)
	{
		airbaseButtons.emplace_back(menuLeft + x * 50, menuTop + y * 50, airbaseButton, gameTexture);
		airbaseButtons.at(i).mySprite.setTextureRect(rectArray[0][4]);	//Placeholder image
		factoryButtons.at(i).width = 50;
		factoryButtons.at(i).height = 50;
		x++;
		if (x == numberOfButtonsPerRow)
		{
			x = 0;
			y++;
		}
	}
	airbaseButtons.at(0).mySprite.setTextureRect(rectArray[10][4]);  //h
	airbaseButtons.at(1).mySprite.setTextureRect(rectArray[9][4]);  //v
	airbaseButtons.at(2).mySprite.setTextureRect(rectArray[11][4]);  //f
	airbaseButtons.at(3).mySprite.setTextureRect(rectArray[12][4]);  //b

	//Naval buttons
	y = 0;	//Reset counters
	x = 0;
	for (int i = 0; i < portOptions.size(); i++)
	{
		portButtons.emplace_back(menuLeft + x * 50, menuTop + y * 50, portButton, gameTexture);
		portButtons.at(i).mySprite.setTextureRect(rectArray[0][4]);	//Placeholder image
		factoryButtons.at(i).width = 50;
		factoryButtons.at(i).height = 50;
		x++;
		if (x == numberOfButtonsPerRow)
		{
			x = 0;
			y++;
		}
	}
	portButtons.at(0).mySprite.setTextureRect(rectArray[16][4]);  //G
	portButtons.at(1).mySprite.setTextureRect(rectArray[14][4]);  //C
	portButtons.at(2).mySprite.setTextureRect(rectArray[15][4]);  //L
	portButtons.at(3).mySprite.setTextureRect(rectArray[18][4]);  //U
	portButtons.at(4).mySprite.setTextureRect(rectArray[13][4]);  //B
	portButtons.at(5).mySprite.setTextureRect(rectArray[17][4]);  //V

	return;
}

int inputLayer::printSingleTile(int screenX, int screenY, int actualX, int actualY, MasterBoard* boardToPrint, int playerNumber, bool withinAnimation)
{
	tile* tileToPrint = &boardToPrint->Board[actualX][actualY];
	//Initialize effects sprite, even though it may not always be used.
	sf::Sprite effectsSprite;
	effectsSprite.setTexture(*inputLayerTexture);
	effectsSprite.setPosition(screenX * 50, screenY * 50);

	//First print tile, change sprite depending on if withinVision or not
	tileToPrint->mySprite.setPosition(screenX * 50, screenY * 50);
	tileToPrint->myFogSprite.setPosition(screenX * 50, screenY * 50);
	if (tileToPrint->withinVision[playerNumber] == true)
	{
		inputLayerWindow->draw(tileToPrint->mySprite);
	}
	else
	{
		inputLayerWindow->draw(tileToPrint->myFogSprite);
	}

	//Now handle river tiles onto sea and beach
	if (tileToPrint->symbol == '~' || tileToPrint->symbol == '*')
	{
		sf::Sprite riverSprite;
		riverSprite.setTexture(*inputLayerTexture);
		riverSprite.setPosition(screenX * 50, screenY * 50);

		int riverYCoord = 9;
		if (tileToPrint->withinVision[playerNumber] == false)
		{
			riverYCoord = 10;
		}


		if (actualX < boardToPrint->BOARD_WIDTH - 1 && boardToPrint->Board[actualX + 1][actualY].symbol == '-')
		{
			riverSprite.setTextureRect(rectArray[16][riverYCoord]);
			inputLayerWindow->draw(riverSprite);
		}
		if (actualY < boardToPrint->BOARD_HEIGHT - 1 && boardToPrint->Board[actualX][actualY + 1].symbol == '-')
		{
			riverSprite.setTextureRect(rectArray[18][riverYCoord]);
			inputLayerWindow->draw(riverSprite);
		}
		if (actualY > 0 && boardToPrint->Board[actualX][actualY - 1].symbol == '-')
		{
			riverSprite.setTextureRect(rectArray[19][riverYCoord]);
			inputLayerWindow->draw(riverSprite);
		}
		if (actualX > 0 && boardToPrint->Board[actualX - 1][actualY].symbol == '-')
		{
			riverSprite.setTextureRect(rectArray[17][riverYCoord]);
			inputLayerWindow->draw(riverSprite);
		}
	}

	//Now handle shoal onto sea
	if (tileToPrint->symbol == '~')
	{
		sf::Sprite beachSprite;
		beachSprite.setTexture(*inputLayerTexture);
		beachSprite.setPosition(screenX * 50, screenY * 50);

		int beachYCoord = 11;
		if (tileToPrint->withinVision[playerNumber] == false)
		{
			beachYCoord = 12;
		}

		if (actualX < boardToPrint->BOARD_WIDTH - 1 && boardToPrint->Board[actualX + 1][actualY].symbol == '*')
		{
			beachSprite.setTextureRect(rectArray[16][beachYCoord]);
			inputLayerWindow->draw(beachSprite);
		}
		if (actualY < boardToPrint->BOARD_HEIGHT - 1 && boardToPrint->Board[actualX][actualY + 1].symbol == '*')
		{
			beachSprite.setTextureRect(rectArray[18][beachYCoord]);
			inputLayerWindow->draw(beachSprite);
		}
		if (actualY > 0 && boardToPrint->Board[actualX][actualY - 1].symbol == '*')
		{
			beachSprite.setTextureRect(rectArray[19][beachYCoord]);
			inputLayerWindow->draw(beachSprite);
		}
		if (actualX > 0 && boardToPrint->Board[actualX - 1][actualY].symbol == '*')
		{
			beachSprite.setTextureRect(rectArray[17][beachYCoord]);
			inputLayerWindow->draw(beachSprite);
		}
	}

	//We only do minions and associated effects if they are visible
	if (minionVisibleStatus == showMinions)
	{
		//Then print minion if withinVision AND not invisible
		if (tileToPrint->hasMinionOnTop == true && tileToPrint->withinVision[playerNumber] == true
			&& tileToPrint->minionOnTop->invisible == false)
		{
			tileToPrint->minionOnTop->mySprite.setPosition(screenX * 50, screenY * 50);
			inputLayerWindow->draw(tileToPrint->minionOnTop->mySprite);
		}


		//Will only print  "selection" effects if human player's turn, or debug mode 
		//AND if not in the middle of animation - indicated by the invisible flag on minion for movement
		//AND indicated by the withinAnimation flag, for attacks and such.
		if ((boardToPrint->playerFlag == playerNumber || MainMenu->editorMode == true)
			&& boardToPrint->cursor.selectMinionPointer != NULL
			&& boardToPrint->cursor.selectMinionPointer->invisible == false
			&& withinAnimation == false)
		{
			if (boardToPrint->cursor.selectMinionFlag == true
				&& boardToPrint->cursor.selectMinionPointer->locationX == actualX
				&& boardToPrint->cursor.selectMinionPointer->locationY == actualY
				&& tileToPrint->withinVision[playerNumber] == true)
			{
				//If minion is selected here, print the selected square.
				//We'll print selected square regardless of compie or player
				effectsSprite.setTextureRect(rectArray[1][2]);
				inputLayerWindow->draw(effectsSprite);
			}
			else
				if (tileToPrint->withinCursorPath == true)
				{
					//If this tile is on cursor's path
					effectsSprite.setTextureRect(rectArray[6][2]);
					inputLayerWindow->draw(effectsSprite);
				}
				else
					if (tileToPrint->withinApparentRange == true)
					{
						//If this tile is within apparent range.
						effectsSprite.setTextureRect(rectArray[3][2]);
						inputLayerWindow->draw(effectsSprite);
					}
					else if (tileToPrint->withinRange == true		//Attack and transport square 
						&& (boardToPrint->cursor.selectMinionPointer->status == gaveupmovehasntfired
							|| boardToPrint->cursor.selectMinionPointer->status == hasmovedhasntfired)
						&& boardToPrint->cursor.selectMinionPointer->specialtyGroup == transport)
					{
						//If this tile is within range for drop off
						effectsSprite.setTextureRect(rectArray[4][2]);
						inputLayerWindow->draw(effectsSprite);
					}
					else if (tileToPrint->withinRange == true
						&& (boardToPrint->cursor.selectMinionPointer->status == gaveupmovehasntfired
							|| boardToPrint->cursor.selectMinionPointer->status == hasmovedhasntfired))
					{
						//If minion can fire and if this tile is within range.
						effectsSprite.setTextureRect(rectArray[2][2]);
						inputLayerWindow->draw(effectsSprite);
					}


		}
	}


	//Always draw cursor
	if (boardToPrint->cursor.XCoord == actualX && boardToPrint->cursor.YCoord == actualY
		&& (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType != computerPlayer
			|| MainMenu->editorMode == true))
	{
		boardToPrint->cursor.mySprite.setPosition(screenX * 50, screenY * 50);
		inputLayerWindow->draw(boardToPrint->cursor.mySprite);

	}

	Minion* minionToPrint = NULL;
	if (tileToPrint->hasMinionOnTop == true)
		minionToPrint = tileToPrint->minionOnTop;

	//Must be visible to see effects!
	if (tileToPrint->hasMinionOnTop == true && minionToPrint != NULL && tileToPrint->withinVision[playerNumber] == true
		&& minionVisibleStatus == showMinions && minionToPrint->invisible == false)
	{

		//Print if transporting or capturing // Specialty moves
		if (minionToPrint->isCapturing == true)
		{
			effectsSprite.setTextureRect(rectArray[5][3]);
			inputLayerWindow->draw(effectsSprite);
		}
		else if (minionToPrint->minionBeingTransported != NULL)
		{
			effectsSprite.setTextureRect(rectArray[5][2]);
			inputLayerWindow->draw(effectsSprite);
		}

		//If minion has done all possible moves, black out triangle at top.
		if ((minionToPrint->status == hasfired || (minionToPrint->rangeType == rangedFire && minionToPrint->status == hasmovedhasntfired)))
		{
			effectsSprite.setTextureRect(rectArray[4][3]);
			inputLayerWindow->draw(effectsSprite);
		}

		//If minion has only moved/held position and can still attack/capture, grey triangle at top
		if ((minionToPrint->status == hasmovedhasntfired  && minionToPrint->rangeType != rangedFire) || minionToPrint->status == gaveupmovehasntfired)
		{
			effectsSprite.setTextureRect(rectArray[5][0]);
			inputLayerWindow->draw(effectsSprite);
		}

		//Minion exists and is below 1/3 fuel
		if ((minionToPrint->currentFuel == 0 || (double(minionToPrint->maxFuel) / double(minionToPrint->currentFuel)) >= 3))
		{
			effectsSprite.setTextureRect(rectArray[1][3]);
			inputLayerWindow->draw(effectsSprite);
		}

		//Minion exists, has a gun (Max != -1), and it is below 1/3 ammo AND isn't infinite ammo (Max != 0)
		//If either weapon is low on ammo, then print low ammo
		if ((minionToPrint->maxPriAmmo != 0 && minionToPrint->maxPriAmmo != -1 && (minionToPrint->currentPriAmmo == 0 || (double(minionToPrint->maxPriAmmo) / double(minionToPrint->currentPriAmmo)) >= 3)) ||
			(minionToPrint->maxSecAmmo != 0 && minionToPrint->maxSecAmmo != -1 && (minionToPrint->currentSecAmmo == 0 || (double(minionToPrint->maxSecAmmo) / double(minionToPrint->currentSecAmmo)) >= 3)))
		{
			effectsSprite.setTextureRect(rectArray[0][3]);
			inputLayerWindow->draw(effectsSprite);
		}



		if (minionToPrint->health < 95)
		{
			if (minionToPrint->health < 95 && minionToPrint->health >= 85)
				effectsSprite.setTextureRect(rectArray[6][0]);
			if (minionToPrint->health < 85 && minionToPrint->health >= 75)
				effectsSprite.setTextureRect(rectArray[6][1]);
			if (minionToPrint->health < 75 && minionToPrint->health >= 65)
				effectsSprite.setTextureRect(rectArray[7][2]);
			if (minionToPrint->health < 65 && minionToPrint->health >= 55)
				effectsSprite.setTextureRect(rectArray[8][2]);
			if (minionToPrint->health < 55 && minionToPrint->health >= 45)
				effectsSprite.setTextureRect(rectArray[8][3]);
			if (minionToPrint->health < 45 && minionToPrint->health >= 35)
				effectsSprite.setTextureRect(rectArray[7][3]);
			if (minionToPrint->health < 35 && minionToPrint->health >= 25)
				effectsSprite.setTextureRect(rectArray[6][3]);
			if (minionToPrint->health < 25 && minionToPrint->health >= 15)
				effectsSprite.setTextureRect(rectArray[3][3]);
			if (minionToPrint->health < 15 && minionToPrint->health >= 5)
				effectsSprite.setTextureRect(rectArray[2][3]);

			inputLayerWindow->draw(effectsSprite);

		}

		//Or print veterency status
		if (minionToPrint->veterancy > 0)
		{
			if (minionToPrint->veterancy == 3)
			{
				effectsSprite.setTextureRect(rectArray[4][0]);
				inputLayerWindow->draw(effectsSprite);
			}
			else if (minionToPrint->veterancy == 2)
			{
				effectsSprite.setTextureRect(rectArray[5][1]);
				inputLayerWindow->draw(effectsSprite);
			}
			else //vet 1
			{
				effectsSprite.setTextureRect(rectArray[4][1]);
				inputLayerWindow->draw(effectsSprite);
			}
		}


	}

	if (tileToPrint->animationSprite != NULL)
	{
		//If there is some additional animation, print that too, on top of everything else
		//It must be set by previous function
		tileToPrint->animationSprite->setPosition(screenX * 50, screenY * 50);
		inputLayerWindow->draw(*(tileToPrint->animationSprite));


	}

	return 1;
}

int inputLayer::printStatus(MasterBoard* boardToPrint, int observerNumber)
{
	int spacingConstant = 4;
	tile* currentTile = &boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	//Need string pointer since addstr giving grief about printing strings, and same with snprintf.
	char pointerToPrint[100];
	char* descriptionPointer = &currentTile->description[0];
	char* playerName = &(boardToPrint->playerRoster[currentTile->controller].name[0]);
	sf::String tileDescription;

	if (observerNumber == boardToPrint->playerFlag)
	{
		if (currentTile->controller != 0)
		{
			tileDescription = playerName;
			tileDescription += "'s ";
			tileDescription += descriptionPointer;
		}
		else
		{
			tileDescription = descriptionPointer;
		}

		if (MainMenu->editorMode == true)
		{
			if (boardToPrint->cursor.selectMinionPointer != NULL)
			{
				snprintf(pointerToPrint, 100, "\nTrue MP/Apparent/Terrain: %d/%d/%d, \nCursor X/Y: %d/%d\n",
					boardToPrint->cursor.selectMinionPointer->truePathMap[currentTile->locationX][currentTile->locationY].distanceFromMinion,
					boardToPrint->cursor.selectMinionPointer->apparentPathMap[currentTile->locationX][currentTile->locationY].distanceFromMinion,
					boardToPrint->cursor.selectMinionPointer->terrainOnlyPathMap[currentTile->locationX][currentTile->locationY].distanceFromMinion,
					boardToPrint->cursor.XCoord,
					boardToPrint->cursor.YCoord);

			}
			else
			{
				snprintf(pointerToPrint, 100, "\nCursor X/Y: %d/%d\n",
					boardToPrint->cursor.XCoord,
					boardToPrint->cursor.YCoord);

			}

			tileDescription += pointerToPrint;
			sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
			newText.setPosition(MAX_WINDOW_WIDTH * 52, (menuLineTracker * MainMenu->menuTextSize) + spacingConstant);
			newText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(newText);
			if (MainMenu->editorMode == true)
				menuLineTracker += 4;
			else menuLineTracker += 2;

		}
		else
		{
			sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
			newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant);
			newText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(newText);
			menuLineTracker += 1;
		}




		//If tile is undergoing capture, let us know.
		if (currentTile->capturePoints != 20)
		{
			snprintf(pointerToPrint, 100, " Capture Points Left: %d ", currentTile->capturePoints);

			sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
			newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant);
			newText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(newText);
			menuLineTracker += 1;
		}

		if (currentTile->hasMinionOnTop == true && currentTile->withinVision[observerNumber])
		{
			Minion* currentMinion = currentTile->minionOnTop;

			//Print out basic minion status.
			sf::String minionDescription = &(boardToPrint->playerRoster[currentMinion->team].name[0]);
			minionDescription += "'s ";
			minionDescription += &currentMinion->description[0];

			sf::Text newText(minionDescription, *inputLayerFont, MainMenu->menuTextSize);
			newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant);
			newText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(newText);
			menuLineTracker += 1;

			if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo > 0)		//Both pri/sec have ammo
			{
				if (MainMenu->editorMode == true)
					snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/%d\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo, currentMinion->currentSecAmmo);
				else
					snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/%d\n", currentMinion->currentFuel, currentMinion->currentPriAmmo, currentMinion->currentSecAmmo);
			}
			else if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo == 0)		//Infinte sec ammo
			{
				if (MainMenu->editorMode == true)
					snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/Infinite\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo);
				else
					snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/Infinite\n", currentMinion->currentFuel, currentMinion->currentPriAmmo);
			}
			else if (currentMinion->maxSecAmmo == 0 && currentMinion->maxPriAmmo == -1)	//No pri, infinite sec.
			{
				if (MainMenu->editorMode == true)
					snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): None/Infinite\n", int(currentMinion->health), currentMinion->currentFuel);
				else
					snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): None/Infinite\n", currentMinion->currentFuel);
			}
			else if (currentMinion->maxSecAmmo == -1 && currentMinion->maxPriAmmo > 0)	//Primary with no sec.
			{
				if (MainMenu->editorMode == true)
					snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/None\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo);
				else
					snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/None\n", currentMinion->currentFuel, currentMinion->currentPriAmmo);
			}
			else //No weapons. Used as catch all for debug.
			{
				if (MainMenu->editorMode == true)
					snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nNo weapons\n", int(currentMinion->health), currentMinion->currentFuel);
				else
					snprintf(pointerToPrint, 100, "Fuel: %d \nNo weapons\n", currentMinion->currentFuel);
			}

			sf::String minionStatus = pointerToPrint;
			sf::Text minionStatusText(minionStatus, *inputLayerFont, MainMenu->menuTextSize);
			minionStatusText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant);
			minionStatusText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(minionStatusText);
			menuLineTracker += 2;

			if (currentMinion->status == gaveupmovehasntfired)
			{
				minionStatus = "Holding position. Ready to attack.\n";

			}
			else
				if (currentMinion->status == hasmovedhasntfired)
				{
					minionStatus = "Has already moved this turn. ";

					if (currentMinion->rangeType == rangedFire)
					{
						minionStatus += "Cannot attack after moving. \n";

					}
					if (currentMinion->rangeType == directFire)
					{
						minionStatus += "Ready to attack.\n";

					}
				}
				else
					if (currentMinion->status == hasfired)
					{
						minionStatus = "Has already moved this turn. Has attacked this turn. \n";
					}
					else
						if (currentMinion->status == hasntmovedorfired)
						{
							minionStatus = "Ready to move. Ready to attack. \n";
						}

			sf::Text secondMinionStatusText(minionStatus, *inputLayerFont, MainMenu->menuTextSize);
			secondMinionStatusText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant*2);
			secondMinionStatusText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(secondMinionStatusText);
			menuLineTracker += 1;

		}


	}


	//Print current turn.

	sf::String playerStatus = &(boardToPrint->playerRoster[boardToPrint->playerFlag].name[0]);
	snprintf(pointerToPrint, 100, "'s turn.\nTreasury Total: %d\n", boardToPrint->playerRoster[boardToPrint->playerFlag].treasury);
	playerStatus += pointerToPrint;
	playerStatus += &eventText[0];
	sf::Text playerStatuText(playerStatus, *inputLayerFont, MainMenu->menuTextSize);
	playerStatuText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + spacingConstant*2);
	playerStatuText.setFillColor(sf::Color::Black);
	inputLayerWindow->draw(playerStatuText);
	menuLineTracker += 3;

	eventText = "";

	return 0;
}

int inputLayer::printMinionMenu(MasterBoard* boardToPrint) {

	if (boardToPrint->cursor.selectMinionPointer == NULL)
		return 0;

	minionStatus mystatus = boardToPrint->cursor.selectMinionPointer->status;

	sf::String boardMessage;

	if (mystatus == hasntmovedorfired)
	{
		boardMessage = "Move cursor(WASD) | Move minion (m)\nDeselect minion(t) | Capture move(c)\n";
	}

	if (mystatus == hasmovedhasntfired || mystatus == gaveupmovehasntfired)
	{
		boardMessage = "Move cursor(WASD) | Attack (r)\nDeselect minion (t) | Capture (c)\n";
	}

	if (mystatus == hasfired)
	{
		boardMessage = "No further action possible";

	}

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + 2);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printBoardMenu(MasterBoard* boardToPrint) {

	sf::String boardMessage = "Move cursor (WASD) | Menu (m)\nSelect minion/property (t)\n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;
}

int	inputLayer::printPropertyMenu(MasterBoard* boardToPrint) {

	tile* myTile = &boardToPrint->Board[boardToPrint->cursor.XCoord][boardToPrint->cursor.YCoord];
	
	//Black border box
	sf::Sprite effectsSprite;
	effectsSprite.setTexture(*inputLayerTexture);
	effectsSprite.setTextureRect(rectArray[25][4]);

	if (myTile->symbol == 'h')
	{
		for (int i = 0 ; i < factoryButtons.size(); i++ )
		{
			inputLayerWindow->draw(factoryButtons.at(i).mySprite);
			effectsSprite.setPosition(factoryButtons.at(i).mySprite.getPosition());
			inputLayerWindow->draw(effectsSprite);

		}
	}

	if (myTile->symbol == 'P')
	{
		for (int i = 0; i < portButtons.size(); i++)
		{
			inputLayerWindow->draw(portButtons.at(i).mySprite);
			effectsSprite.setPosition(portButtons.at(i).mySprite.getPosition());
			inputLayerWindow->draw(effectsSprite);
		}
	}

	if (myTile->symbol == 'A')
	{
		for (int i = 0; i < airbaseButtons.size(); i++)
		{
			inputLayerWindow->draw(airbaseButtons.at(i).mySprite);
			effectsSprite.setPosition(airbaseButtons.at(i).mySprite.getPosition());
			inputLayerWindow->draw(effectsSprite);
		}
	}

	return 0;
}

int inputLayer::printMenu(MasterBoard* boardToPrint)
{

	//Draw each button at its offset.
	//Button must be active, so no sound means soundOn is not drawn
	for (int i = 0; i < menuButtons->size(); i++)
	{
		bool printThisButton = false;

		switch ((menuButtons->at(i)).myType)
		{
		case (soundOn):
		{	
			if (soundsOn == true)
				printThisButton = true;
			break;
		}
		case (soundOff):
		{
			if (soundsOn == false)
				printThisButton = true;
			break;
		}
		case (speedNormal):
		{
			if (speedFactor == 1)
				printThisButton = true;
			break;
		}
		case(speedFast):
		{
			if (speedFactor != 1)
				printThisButton = true;
			break;
		}
		default:
			printThisButton = true;
		}

		if(printThisButton == true)
			inputLayerWindow->draw((*menuButtons)[i].mySprite);
	}

	return 0;

}

int inputLayer::printLowerScreen(MasterBoard* boardToPrint, int observerNumber) {

	//First draw the background for status area
	sf::Sprite statusBox;
	statusBox.setTexture(MainMenu->otherGameTextures->at(4));
	statusBox.setPosition(MAX_WINDOW_WIDTH * 50, 0);
	inputLayerWindow->draw(statusBox);

	//Then write whatever status message
	if (boardToPrint->playerFlag == observerNumber)
	{
		if (status == gameBoard)
		{
			printBoardMenu(boardToPrint);
		}
		else

			if (status == menu)
			{
				printMenu(boardToPrint);
			}
			else

				if (status == minionAction)
				{
					printMinionMenu(boardToPrint);
				}
				else

					if (status == propertyAction)
					{
						printPropertyMenu(boardToPrint);
					}
					else
						if (status == insertMinion)
						{
							printInsertMinion(boardToPrint);
						}
						else
							if (status == insertTile)
							{
								printInsertTile(boardToPrint);
							}


	}

	if (status != menu)
		printStatus(boardToPrint, observerNumber);

	return 0;

}

int inputLayer::printInsertMinion(MasterBoard* boardToPrint)
{

	sf::String boardMessage = "Insert a minion by typing its symbol\nExit insert minion menu(x) \n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printInsertTile(MasterBoard* boardToPrint)
{
	sf::String boardMessage = "Insert a tile by typing its symbol\nExit insert tile menu(x) \n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printMissionBriefing(MasterBoard* boardToInput)
{
	inputLayerWindow->clear();
	sf::String boardMessage;

	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(MainMenu->otherGameTextures->at(9));
	inputLayerWindow->draw(backgroundSprite);

	if (boardToInput->missionFlag == true)
	{
		boardMessage = boardToInput->campaignName;
		boardMessage += ": ";
	}

	boardMessage += boardToInput->scenarioOrMissionName;
	boardMessage += "\n\n";

	boardMessage += MainMenu->missionBriefing;
	boardMessage += "\n\nPress any key to continue.";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);
	newText.setPosition(250, 200);
	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	playCharInput(inputLayerWindow);

	inputLayerWindow->pollEvent(event);

	return 0;

}

int inputLayer::printUpperScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation) {
	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocationY;
	int windowX = boardToPrint->windowLocationX;

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (int i = windowY; i < (windowY + boardToPrint->WINDOW_HEIGHT); i++)
	{
		for (int j = windowX; j < (windowX + boardToPrint->WINDOW_WIDTH); j++)
		{
			printSingleTile((j - windowX), (i - windowY), j, i, boardToPrint, observerNumber, withinAnimation);
		}

	}

	return 0;
}

int inputLayer::printWaitingScreen(MasterBoard* boardToPrint)
{
	if (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType == humanPlayer)
	{
		inputLayerWindow->clear();
		char buffer[100];

		sf::Sprite backgroundSprite;
		backgroundSprite.setTexture(MainMenu->otherGameTextures->at(8));
		inputLayerWindow->draw(backgroundSprite);

		sf::String announceString = boardToPrint->playerRoster[boardToPrint->playerFlag].name;
		announceString += "'s turn. Press any key to begin.  \n";
		sf::Text newText(announceString, *inputLayerFont, 20);
		newText.setPosition(300, 200);
		newText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(newText);
		inputLayerWindow->display();
	}
	return 0;

}

//movementGraphics is called for every tile as the path is verified. If that tile is within vision OR this is player turn,
//we will print movement "animation" i.e. the minion sprite flashing on that tile for 200 ms.
int inputLayer::movementGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToMove, int locationX, int locationY)
{
	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	//Sound effects
	if (soundsOn == true)
	{
		(*soundEffects)[minionToMove->myMoveSound].play();
	}


	//If player controlled, tile the minion moves through will always be visible.
	if (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType == humanPlayer)
	{
		boardToPrint->Board[locationX][locationY].withinVision[observerNumber] = true;
	}

	//If within vision, change specific tile's animationSprite to match the movement graphics.
	if (boardToPrint->Board[locationX][locationY].withinVision[observerNumber] == true)
	{
		boardToPrint->Board[locationX][locationY].animationSprite = &(minionToMove->mySprite);
	}

	//Temporarily make minion invisible, so it disappears from its starting point.
	minionToMove->invisible = true;

	//Use usual print method
	bool withinAnimation = true;
	printScreen(boardToPrint, observerNumber, withinAnimation);


	//Delay after printing;
	std::this_thread::sleep_for(std::chrono::milliseconds(140 / speedFactor));

	//Reset invisibilty status for minion selected
	minionToMove->invisible = false;

	//Reset animation sprite
	boardToPrint->Board[locationX][locationY].animationSprite = NULL;

	if (soundsOn == true)
	{
		(*soundEffects)[minionToMove->myMoveSound].stop();
	}

	return 0;
}

int inputLayer::combatGraphics(MasterBoard* boardToPrint, int observerNumber, tile* tileAttacking, tile* tileBeingAttacked)
{
	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (tileAttacking == NULL || tileBeingAttacked == NULL)
	{
		std::cout << "Could not animate combat, one of the tiles was NULL" << std::endl;
		return -1;
	}

	if (soundsOn == true)
	{
		//Provide sound regardless of vision
		(*soundEffects)[tileAttacking->minionOnTop->myAttackSound].play();
	}


	//If within vision, we will watch attackerVisible
	if (tileAttacking->withinVision[observerNumber] == true)
	{

		//Create new sprite for animation
		tileAttacking->animationSprite = new sf::Sprite;

		//Set texture
		tileAttacking->animationSprite->setTexture(*inputLayerTexture);

		//Now do animation sequence. Custom for combat.
		//Remember the last tile which is blank, to "clear" the effect
		for (int i = 0; i < 5; i++)
		{

			tileAttacking->animationSprite->setTextureRect(rectArray[i][13]);
			bool withinAnimation = true;
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(70 / speedFactor));
		}

		//Clean up afterwards if necessary
		if (tileAttacking->animationSprite != NULL)
		{
			delete tileAttacking->animationSprite;
			tileAttacking->animationSprite = NULL;
		}
	}

	//If within vision, we will watch attackerVisible
	if (tileBeingAttacked->withinVision[observerNumber] == true)
	{

		//Create new sprite for animation
		tileBeingAttacked->animationSprite = new sf::Sprite;

		//Set texture
		tileBeingAttacked->animationSprite->setTexture(*inputLayerTexture);

		//Now do animation sequence. Custom for combat.
		//Remember the last tile which is blank, to "clear" the effect
		for (int i = 0; i < 5; i++)
		{
			tileBeingAttacked->animationSprite->setTextureRect(rectArray[i][14]);
			bool withinAnimation = true;
			printScreen(boardToPrint, observerNumber, withinAnimation);

			std::this_thread::sleep_for(std::chrono::milliseconds(70 / speedFactor));
		}

		//Clean up afterwards if necessary
		if (tileBeingAttacked->animationSprite != NULL)
		{
			delete tileBeingAttacked->animationSprite;
			tileBeingAttacked->animationSprite = NULL;
		}


	}
	if (soundsOn == true)
	{
		(*soundEffects)[tileAttacking->minionOnTop->myAttackSound].stop();
	}
	return 0;
}

int inputLayer::captureGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToCapture, int locationX, int locationY)
{

	//Should be checking for bad locX but will add later
	tile* myTile = &boardToPrint->Board[locationX][locationY];

	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (minionToCapture == NULL)
	{
		std::cout << "Could not animate capture, the tile was NULL" << std::endl;
		return -1;
	}

	if (soundsOn == true)
	{
		(*soundEffects)[capture].play();
	}

	//If within vision, we will watch capture occur
	if (myTile->withinVision[observerNumber] == true)
	{
		//Create new sprite for animation
		myTile->animationSprite = new sf::Sprite;

		//Set texture
		myTile->animationSprite->setTexture(*inputLayerTexture);


		for (int i = 0; i < 2; i++)
		{
			bool withinAnimation = true;
			myTile->animationSprite->setTextureRect(rectArray[5][14]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5 + minionToCapture->team][14]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	if (soundsOn == true)
	{
		(*soundEffects)[capture].stop();
	}
}

int inputLayer::supplyGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToSupply, int locationX, int locationY)
{
	//Should be checking for bad locX but will add later
	tile* myTile = &boardToPrint->Board[locationX][locationY];

	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (minionToSupply == NULL)
	{
		std::cout << "Could not animate supply, the tile was NULL" << std::endl;
		return -1;
	}

	if (soundsOn == true)
	{
		(*soundEffects)[resupply].play();
	}

	//If within vision, we will watch capture occur
	if (myTile->withinVision[observerNumber] == true)
	{
		//Create new sprite for animation
		myTile->animationSprite = new sf::Sprite;

		//Set texture
		myTile->animationSprite->setTexture(*inputLayerTexture);

		for (int i = 0; i < 3; i++)
		{
			bool withinAnimation = true;
			myTile->animationSprite->setTextureRect(rectArray[6][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	if (soundsOn == true)
	{
		(*soundEffects)[resupply].stop();
	}

}

int inputLayer::trapGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionTrapped, int locationX, int locationY)
{
	//Should be checking for bad locX but will add later
	tile* myTile = &boardToPrint->Board[locationX][locationY];

	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (minionTrapped == NULL)
	{
		std::cout << "Could not animate trap, the tile was NULL" << std::endl;
		return -1;
	}

	if (soundsOn == true)
	{
		(*soundEffects)[trapped].play();
	}

	//If within vision, we will watch trap occur
	if (myTile->withinVision[observerNumber] == true)
	{
		//Create new sprite for animation
		myTile->animationSprite = new sf::Sprite;

		//Set texture
		myTile->animationSprite->setTexture(*inputLayerTexture);


		bool withinAnimation = true;

		myTile->animationSprite->setTextureRect(rectArray[8][13]);
		printScreen(boardToPrint, observerNumber, withinAnimation);
		std::this_thread::sleep_for(std::chrono::milliseconds(350 / speedFactor));

		myTile->animationSprite->setTextureRect(rectArray[5][13]);
		printScreen(boardToPrint, observerNumber, withinAnimation);


		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	if (soundsOn == true)
	{
		(*soundEffects)[trapped].stop();
	}


}

int inputLayer::repairGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToSupply, int locationX, int locationY)
{

	//Should be checking for bad locX but will add later
	tile* myTile = &boardToPrint->Board[locationX][locationY];

	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (minionToSupply == NULL)
	{
		std::cout << "Could not animate repair, the tile was NULL" << std::endl;
		return -1;
	}

	std::cout << "Repairing " << minionToSupply->description << std::endl;

	if (soundsOn == true)
	{
		(*soundEffects)[repair].play();

	}

	//If within vision, we will watch repair occur
	if (myTile->withinVision[observerNumber] == true)
	{
		//Create new sprite for animation
		myTile->animationSprite = new sf::Sprite;

		//Set texture
		myTile->animationSprite->setTexture(*inputLayerTexture);

		for (int i = 0; i < 3; i++)
		{
			bool withinAnimation = true;
			myTile->animationSprite->setTextureRect(rectArray[7][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	if (soundsOn == true)
	{
		(*soundEffects)[repair].stop();
	}
}

int inputLayer::printScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation)
{
	if (status != waitingForNextLocalPlayer)
	{
		inputLayerWindow->clear();
		printUpperScreen(boardToPrint, observerNumber, withinAnimation);
		printLowerScreen(boardToPrint, observerNumber);
		inputLayerWindow->display();

	}
	else printWaitingScreen(boardToPrint);

	//Reset line tracker after each print.
	menuLineTracker = 1;

	return 0;
}

int inputLayer::waitingScreenInput(MasterBoard* boardToInput)
{
	playCharInput(inputLayerWindow);

	//Only lasts one input.
	status = gameBoard;

	if (boardToInput->playerRoster[boardToInput->playerFlag].playerType == humanPlayer) {
		if (MainMenu->editorMode == true)	//If debug, see from 0's perspective.
		{
			boardToInput->upkeep(this, 0);
		}
		else  if (boardToInput->isItSinglePlayerGame == true)	//If it's single player, then we see from player 1's perspective. 
		{
			boardToInput->upkeep(this, 1);
		}
		else if (boardToInput->isItSinglePlayerGame == false && boardToInput->playerRoster[boardToInput->playerFlag].playerType == computerPlayer)	//If compie during multiplayer, no sight
		{
			boardToInput->upkeep(this, -1);
		}
		else
		{
			boardToInput->upkeep(this, boardToInput->playerFlag);
		}
	}
	//Do upkeep here for all human players

	return 0;
}

int inputLayer::insertMinionInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput)
{
	Cursor* myCursor = &boardToInput->cursor;
	tile* myTile = &boardToInput->Board[myCursor->XCoord][myCursor->YCoord];

	//Shift must be used like Caps Lock
	if (*Input == sf::Keyboard::Key::LShift || *Input == sf::Keyboard::Key::RShift)
	{
		if (capsLockOn == false)
			capsLockOn = true;
		else if (capsLockOn == true)
			capsLockOn = false;

		return 1;
	}


	//Return to gameBoard if player presses 'x'.
	if (*Input == sf::Keyboard::Key::X)
	{
		status = gameBoard;
		return 1;
	}


	//Convert valid keyboard input to char
	char convertedInput = '~';

	if (capsLockOn == false)
	{
		//Lower case letters
		switch (*Input)
		{
			case sf::Keyboard::I:
			{
				convertedInput = 'i';
				break;
			}

			case(sf::Keyboard::S):
			{
				convertedInput = 's';
				break;
			}

			case(sf::Keyboard::A):
			{
				convertedInput = 'a';
				break;
			}

			case(sf::Keyboard::T):
			{
				convertedInput = 'T';
				break;
			}

			case(sf::Keyboard::R):
			{
				convertedInput = 'r';
				break;
			}

			case(sf::Keyboard::C):
			{
				convertedInput = 'c';
				break;
			}

			case(sf::Keyboard::K):
			{
				convertedInput = 'K';
				break;
			}

			case(sf::Keyboard::V):
			{
				convertedInput = 'v';
				break;
			}

			case(sf::Keyboard::H):
			{
				convertedInput = 'h';
				break;
			}

			case(sf::Keyboard::P):
			{
				convertedInput = 'P';
				break;
			}

			case(sf::Keyboard::F):
			{
				convertedInput = 'f';
				break;
			}

			case(sf::Keyboard::B):
			{
				convertedInput = 'b';
				break;
			}

			case(sf::Keyboard::G):
			{
				convertedInput = 'G';
				break;
			}

			case(sf::Keyboard::L):
			{
				convertedInput = 'L';
				break;
			}

			case(sf::Keyboard::U):
			{
				convertedInput = 'U';
				break;
			}

		}
	}
	else //Shift-required characters
	{
		switch (*Input)
		{
			case(sf::Keyboard::A):
			{
				convertedInput = 'A';
				break;
			}
			case(sf::Keyboard::V):
			{	
				convertedInput = 'V';
				break;
			}

			case(sf::Keyboard::R):
			{
				convertedInput = 'R';
				break;
			}
			case(sf::Keyboard::B):
			{
			
				convertedInput = 'B';
				break;
			}
			case(sf::Keyboard::C):
			{	
				convertedInput = 'C';
				break;
			}
			case(sf::Keyboard::S):	
			{
				convertedInput = 'S';
				break;
			}
		}
	}

	//Prevent minion insertion on top of another, and prevent insertion somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true || myTile->consultMovementChart(convertedInput, myTile->symbol) == 99)
		return 1;

	int requestedUnitPrice = boardToInput->consultMinionCostChart(convertedInput, '~');

	//If it is real minion, then price > 0
	if (requestedUnitPrice > 0)
	{
		boardToInput->createMinion(convertedInput, myCursor->getX(), myCursor->getY(), boardToInput->playerFlag, 100, 0, 0, 0, -1, -1, -1);
		status = gameBoard;
		return 0;
	}

	return 1;

}

//If there is a minion underneath, destroy it regardless of any status.
//Activate with debug mode and "z" input.
int inputLayer::deleteMinionInput(MasterBoard* boardToInput)
{
	Cursor* myCursor = &boardToInput->cursor;
	tile* myTile = &boardToInput->Board[myCursor->XCoord][myCursor->YCoord];


	if (myTile->hasMinionOnTop != true)
	{
		status = gameBoard;
		return 1;
	}
	else
	{
		boardToInput->destroyMinion(myTile->minionOnTop, false, this, true);
		status = gameBoard;
		return 0;
	}



}

int inputLayer::insertTileInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput)
{
	Cursor* myCursor = &boardToInput->cursor;
	tile* myTile = &boardToInput->Board[myCursor->XCoord][myCursor->YCoord];

	//Return to gameBoard if player presses 'q'.
	if (*Input == sf::Keyboard::Key::Q)
	{
		status = gameBoard;
		return 1;
	}

	//Shift must be used like Caps Lock
	if (*Input == sf::Keyboard::Key::LShift || *Input == sf::Keyboard::Key::RShift)
	{
		if (capsLockOn == false)
			capsLockOn = true;
		else if (capsLockOn == true)
			capsLockOn = false;

		return 1;
	}

	
	//Convert to char before analyzing
	char inputChar = '!';

	if (capsLockOn == false)
	{
		//Lower case letters
		switch (*Input)
		{
			case sf::Keyboard::Period:
				inputChar = '.';
				break;
			case(sf::Keyboard::Add):
				inputChar = '+';
				break; 
			case(sf::Keyboard::M):
				inputChar = 'm';
				break;
			case(sf::Keyboard::N):
				inputChar = 'n';
				break;
			case(sf::Keyboard::H):
				inputChar = 'h';
				break;
			case(sf::Keyboard::Equal):
				inputChar = '=';
				break;
			case(sf::Keyboard::Dash):
				inputChar = '-';
				break;
		}
	}
	else 
	{
		//Upper case letters
		switch (*Input)
		{
			case(sf::Keyboard::Add):
			case(sf::Keyboard::Equal):		//Test okay
				inputChar = '+';
				break;
			case(sf::Keyboard::Num6):
				inputChar = '^';
				break;
			case(sf::Keyboard::M):
				inputChar = 'M';
				break;
			case(sf::Keyboard::H):
				inputChar = 'H';
				break;
			case(sf::Keyboard::A):
				inputChar = 'A';
				break;
			case(sf::Keyboard::P):
				inputChar = 'P';
				break;
			case(sf::Keyboard::Num8):
			case(sf::Keyboard::Multiply):		//Test okay
				inputChar = '*';
				break;
			case(sf::Keyboard::Subtract):	//Test okay
				inputChar = '-';
				break;
			case(sf::Keyboard::Q):
				inputChar = 'Q';
				break;			
			case(sf::Keyboard::Tilde):	//Test needed
				inputChar = '~';
				break;
		}
	}

	//If input tile symbol is invalid, return 1.
	if (myTile->consultMovementChart('i', inputChar) == -1)
		return 1;

	//Prevent terrain from being somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true && myTile->consultMovementChart(myTile->minionOnTop->type, inputChar) == 99)
		return 1;

	//If it is real tile, change the underlying tile.
	myTile->symbol = inputChar;	
	myTile->capturePoints = 20;

	if (myTile->checkForProperty(myTile->symbol) == true)
	{
		myTile->controller = boardToInput->playerFlag;
	}
	else
	{
		myTile->controller = 0;
	}

	myTile->setCharacterstics(inputLayerTexture, boardToInput);

	status = gameBoard;
	return 0;

}

int inputLayer::gameBoardInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput)
{
	//Must be mouse click

	if (*Input == sf::Keyboard::Quote)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));
	
		//If mouse click is within map, 

		int windowX = mousePosition.x / 50;
		int windowY = mousePosition.y / 50;
		int tileX = windowX + boardToInput->windowLocationX;
		int tileY = windowY + boardToInput->windowLocationY;

		if (tileX < boardToInput->BOARD_WIDTH && tileX >= 0 && tileY < boardToInput->BOARD_HEIGHT && tileY >= 0)
		{
			//And mouse click not within cursor, move cursor there.
			if (tileX != boardToInput->cursor.XCoord || tileY != boardToInput->cursor.YCoord) 
			{
				boardToInput->cursor.relocate(tileX, tileY);	//Move cursor to where mouse click occurred.
			}
			else
			//If mouse click IS within cursor, change input to "select" tile.
			if (tileX == boardToInput->cursor.XCoord && tileY == boardToInput->cursor.YCoord)
			{
				*Input = sf::Keyboard::Key::T; //Select that tile.
			}
		}	
	
	}


	//The below inputs will also occur based on mouse clicks setting the input to such.

	if (*Input == sf::Keyboard::Key::A || *Input == sf::Keyboard::Key::D || *Input == sf::Keyboard::Key::S || *Input == sf::Keyboard::Key::W)
	{
		boardToInput->cursor.move(Input);
	}

	if (*Input == sf::Keyboard::Key::X && MainMenu->editorMode == true)
	{
		status = insertMinion;
	}

	if (*Input == sf::Keyboard::Key::Z && MainMenu->editorMode == true)
	{
		deleteMinionInput(boardToInput);
	}

	if (*Input == sf::Keyboard::Key::Q && MainMenu->editorMode == true)
	{
		status = insertTile;
	}

	//Need char for shift
	if (*Input == sf::Keyboard::Key::Num0)
	{
		if (minionVisibleStatus == hideMinions)
		{
			minionVisibleStatus = showMinions;
		}
		else if (minionVisibleStatus == showMinions)
		{
			minionVisibleStatus = hideMinions;
		}
	}

	//Select minion or property.
	if (*Input == sf::Keyboard::Key::T)
	{
		//If minion is not selected, select it.Must be successful to set flag.
		if (boardToInput->cursor.selectMinionFlag == false //This is probably not needed since it's always true/false in conj. with inputLayer.
			&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
		{
			if (boardToInput->selectMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
			{
				status = minionAction;
			}
		}	//Else if empty property, select it. No minion on top, right team, must be factory to select.
		else
			if ((boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'h' ||
				boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'A' ||
				boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'P')
				&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].controller == boardToInput->playerFlag
				&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false)
			{
				status = propertyAction;
			}

	}

	if (*Input == sf::Keyboard::Key::M || * Input == sf::Keyboard::Key::Comma)
	{
		status = menu;
	}

	return 0;
}

int inputLayer::minionInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput) {

	//This tracks who may lose after an action. Only one player can lose per action, so only need one number.
	int playerPotentiallyDefeated = 0;

	if (*Input == sf::Keyboard::Quote)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));

		//Get mouse click information
		int windowX = mousePosition.x / 50;
		int windowY = mousePosition.y / 50;
		int tileX = windowX + boardToInput->windowLocationX;
		int tileY = windowY + boardToInput->windowLocationY;

		//If mouse click is within map, 
		if (tileX < boardToInput->BOARD_WIDTH && tileX > 0 && tileY < boardToInput->BOARD_HEIGHT && tileY > 0)
		{
			//And mouse click not within cursor, move cursor there.
			if (tileX != boardToInput->cursor.XCoord || tileY != boardToInput->cursor.YCoord)
			{
				boardToInput->cursor.relocate(tileX, tileY);	//Move cursor to where mouse click occurred.
			}
			else
				//If mouse click IS within cursor 
				if (tileX == boardToInput->cursor.XCoord && tileY == boardToInput->cursor.YCoord)
				{
					Minion* myMinion = boardToInput->cursor.selectMinionPointer;
					tile* targetedTile = &boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()];

					//If cursor is on top of selected minion,
					if(boardToInput->cursor.XCoord == myMinion->locationX &&
						boardToInput->cursor.YCoord == myMinion->locationY)
					{
						//If minion is infantry that has already moved, attempt to capture
						if(myMinion->specialtyGroup == infantry &&
							(myMinion->status == hasmovedhasntfired ||	myMinion->status == gaveupmovehasntfired)   )
							*Input = sf::Keyboard::Key::C; 
						else
							//If minion is transport that already moved, attempt to supply.
							if (myMinion->specialtyGroup == transport &&
								(myMinion->status == hasmovedhasntfired ||	myMinion->status == gaveupmovehasntfired))
							{
								*Input = sf::Keyboard::Key::I;
							}
							else 
							{
								*Input = sf::Keyboard::Key::M;	//Otherwise attempt to move there.
							}
					}
					else //If we are direct/hybrid attack, and have already moved, attempt to attack there. Must be in range and visible.
						 //Or if we are indirect/hybrid attack that held position, attempt to attack there. Must be in range and visible.
						if (targetedTile->hasMinionOnTop == true &&	targetedTile->minionOnTop->team != boardToInput->playerFlag && 
							targetedTile->withinRange == true && targetedTile->withinVision[myMinion->team] == true &&
							( ((myMinion->rangeType == directFire || myMinion->rangeType == hybridRange) &&
								(myMinion->status == hasmovedhasntfired || myMinion->status == gaveupmovehasntfired))
								|| ((myMinion->rangeType == rangedFire || myMinion->rangeType == hybridRange) &&
								( myMinion->status == gaveupmovehasntfired)) )
							)

						{
							*Input = sf::Keyboard::Key::R;
						}
						//If empty space and this is a transport that already moved, attempt to drop there.
						else	if (boardToInput->cursor.selectMinionPointer->specialtyGroup == transport &&
								(boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired ||
									boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)) 
									{
										*Input = sf::Keyboard::Key::O;
									}
									else  //Otherwise attempt to move there.
										{
											*Input = sf::Keyboard::Key::M;
										}
				}
		}

	}

	//If right click occurred, deselect
	if (*Input == sf::Keyboard::Comma)
	{
		*Input = sf::Keyboard::Key::T;
	}

	if (*Input == sf::Keyboard::Key::A || *Input == sf::Keyboard::Key::D || *Input == sf::Keyboard::Key::S || *Input == sf::Keyboard::Key::W)
	{
		boardToInput->cursor.move(Input);
	}

	//Need char for shift
	if (*Input == sf::Keyboard::Key::Num0)
	{
		if (minionVisibleStatus == hideMinions)
		{
			minionVisibleStatus = showMinions;
		}
		else if (minionVisibleStatus == showMinions)
		{
			minionVisibleStatus = hideMinions;
		}
	}

	//Deselect
	if (*Input == sf::Keyboard::Key::T)
	{
		if (boardToInput->cursor.selectMinionFlag == true)
		{
			boardToInput->deselectMinion();
			status = gameBoard;
		}
	}

	//Move minion command
	//If minion selected and hasn't moved or fired, attempt to move.
	//The moveMinion function will check if we are on top of ourselves or another minion.
	if (*Input == sf::Keyboard::Key::M && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->status == hasntmovedorfired)
	{
		//If there is a minion on top and that minion is visible to the minion's owner
		if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true
			&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinVision[boardToInput->cursor.selectMinionPointer->team] == true)
		{	
			//If minion on top is this minion, do hold position movement.
			if (boardToInput->cursor.selectMinionPointer->locationX == boardToInput->cursor.getX()
				&& boardToInput->cursor.selectMinionPointer->locationY == boardToInput->cursor.getY())
			{
				boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this, boardToInput->playerFlag);
				status = gameBoard;
			}
			else
			//If that minion is an empty transport and is same team
			if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->specialtyGroup == transport &&
				boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->minionBeingTransported == NULL
				&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team == boardToInput->cursor.selectMinionPointer->team)
			{
				status = gameBoard;	//Regardless of pickup outcome - either successful or trap, we are deselecting minion.
									//If we do not set status to gameboard, it will segfault when it prints minion status during move, during validatePath.
				boardToInput->pickUpMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this, boardToInput->playerFlag);
				//Need new validatePath function that accounts for pickup graphics. Right now it thinks the minion is getting trapped and does graphics accordingly.

			}
		}
		else //If no minion on top attempt to move there
			if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this, boardToInput->playerFlag) == 0)
			{
				//Change status appropriately for successful movement.
				status = gameBoard;
			}

	}

	//'i' is supply
	//Must have minion selected.
	//Must be APC, hasn't taken second action, cursor is on minion, and regardless of transport status.
	if (*Input == sf::Keyboard::Key::I && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->type == 'P'
		&& (boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired || boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
		&& boardToInput->cursor.getX() == boardToInput->cursor.selectMinionPointer->locationX
		&& boardToInput->cursor.getY() == boardToInput->cursor.selectMinionPointer->locationY)
	{
		//May not be successful, so not necessarily return 0
		if (boardToInput->individualResupply(boardToInput->cursor.selectMinionPointer, false, this, boardToInput->playerFlag) == 0)
			status = gameBoard;
	}

	//O is drop
	//Must have minion selected.
	//Must be transport, hasn't taken second action, has a minion to drop, and tile within range, and not blocked by another minion.
	//Also must not be impassable.
	if (*Input == sf::Keyboard::Key::O && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->specialtyGroup == transport
		&& ((boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired ||
			boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
			&& boardToInput->cursor.selectMinionPointer->minionBeingTransported != NULL)
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].consultMovementChart(boardToInput->cursor.selectMinionPointer->minionBeingTransported->type, boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol) != 99)
	{
		if (boardToInput->dropOffMinion() == 0)
			status = gameBoard;
	}

	bool lastMinionDestroyed = false;
	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours. Must also not be transport type.
	if (*Input == sf::Keyboard::Key::R && boardToInput->cursor.selectMinionFlag == true && boardToInput->cursor.selectMinionPointer->type != transport)
		if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
			if ((boardToInput->cursor.getX() != boardToInput->cursor.selectMinionPointer->locationX) || (boardToInput->cursor.getY() != boardToInput->cursor.selectMinionPointer->locationY))//Can attack if minion is selected
				if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team)//And it's enemy team's.
					if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true)	//In range
					{
						//This is the actual attack portion. Return of 0 indicates successful attack.
						//Note minion's owner so if they lose we know who lost.
						playerPotentiallyDefeated = boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team;
						bool attackSuccess = boardToInput->attackMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this, boardToInput->playerFlag);
						if (attackSuccess == 0)
						{
							status = gameBoard;
						}
					}

	//Press 'c' to capture property minion is currently on.
	//First, minion must be available.
	//Also, must be infantry type.

	if (*Input == sf::Keyboard::Key::C && boardToInput->cursor.selectMinionFlag == true)
		if ((boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired
			|| boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
			&& boardToInput->cursor.selectMinionPointer->specialtyGroup == infantry)
		{
			tile* tileToCheck = &boardToInput->Board[boardToInput->cursor.selectMinionPointer->locationX][boardToInput->cursor.selectMinionPointer->locationY];
			playerPotentiallyDefeated = tileToCheck->controller;
			//Must be property and must not be the current player's property (Could be neutral).
			if (tileToCheck->checkForProperty(tileToCheck->symbol) && tileToCheck->controller != boardToInput->playerFlag)
			{

				eventText = boardToInput->captureProperty(tileToCheck, boardToInput->cursor.selectMinionPointer, this, boardToInput->playerFlag);
				boardToInput->deselectMinion();
				status = gameBoard;
			}
		}


	return 0;
}

int inputLayer::printPlayerDefeat(int playerDefeated, MasterBoard* boardToPrint)
{

	inputLayerWindow->clear();
	sf::String topMenuJoinString = boardToPrint->playerRoster[playerDefeated].name;
	topMenuJoinString += " was defeated. Press any key to continue.  \n";
	sf::Text newText(topMenuJoinString, *inputLayerFont, MainMenu->menuTextSize);
	MainMenu->mywindow->draw(newText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	playCharInput(inputLayerWindow);


	return 0;
}

int inputLayer::printPlayerVictory(int playerVictorious, MasterBoard* boardToPrint)
{
	inputLayerWindow->clear();
	sf::String topMenuJoinString = boardToPrint->playerRoster[playerVictorious].name;
	topMenuJoinString += " was victorious! Press any key to continue.  \n";
	sf::Text newText(topMenuJoinString, *inputLayerFont, MainMenu->menuTextSize);
	MainMenu->mywindow->draw(newText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	playCharInput(inputLayerWindow);

	return 0;
}

//Checks mouse input against various buttons.
//Buttons are currently hard coded, since array is too small to matter.
int inputLayer::menuInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput)
{
	//Must be mouse click
	if (*Input == sf::Keyboard::Quote)
	{

		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));


		bool withinMainMenuButton = (*menuButtons)[0].checkWithinButton(mousePosition.x, mousePosition.y);
		//Exit to main menu
		if (withinMainMenuButton == true)
		{
			MainMenu->gameSave(".\\savegames\\Auto_save.txt", boardToInput);
			exitToMainMenu(boardToInput);

		}


		bool withinSaveGameButton = (*menuButtons)[1].checkWithinButton(mousePosition.x, mousePosition.y);
		//Prompt user and save game.
		if (withinSaveGameButton == true)
		{
			int lineOffset = 1;
			inputLayerWindow->clear();
			sf::String savePrompt = "Choose a name to save your game.\n";
			sf::String saveGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, savePrompt, lineOffset , "save");

			std::string stdSaveGameName = ".\\savegames\\";
			stdSaveGameName += saveGameName;
			stdSaveGameName += "_save.txt";

			MainMenu->gameSave(stdSaveGameName, boardToInput);

			inputLayerWindow->clear();

			sf::Sprite backgroundSprite;
			backgroundSprite.setTexture(MainMenu->otherGameTextures->at(7));
			inputLayerWindow->draw(backgroundSprite);

			sf::String successSave = "Game saved. Press any key to continue.\n";
			sf::Text newText(successSave, *inputLayerFont, MainMenu->menuTextSize);
			newText.setFillColor(sf::Color::Black);
			newText.setPosition(300, 200);
			inputLayerWindow->draw(newText);
			inputLayerWindow->display();

			//Flush event queue to clear out "Enter" and other rifraf
			sf::Event throwAwayEvent;
			while (inputLayerWindow->pollEvent(throwAwayEvent));

			playCharInput(inputLayerWindow);

			status = gameBoard;

		}


		bool withinLoadGameButton = (*menuButtons)[2].checkWithinButton(mousePosition.x, mousePosition.y);
		//Load a game
		if (withinLoadGameButton == true)
		{
			//Load the actual save game
			std::ifstream loadGameSave;
			bool loadsuccessful = false;

			//Prompt user and load scenario
			int lineOffset = 1;
			while (loadsuccessful == false)
			{

				sf::String loadPrompt = "Choose which save game to load (Case sensitive): \n";
				sf::String loadGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, loadPrompt, lineOffset , "load");
				sf::Event event;

				std::string stdloadGameName = loadGameName;
				loadGameSave.open(".\\savegames\\" + stdloadGameName + "_save.txt");
				if (loadGameSave.is_open())
				{
					inputLayerWindow->clear();
					sf::String successful = "Successfully loaded! Press any key to continue.\n";
					
					sf::Sprite backgroundSprite;
					backgroundSprite.setTexture(MainMenu->otherGameTextures->at(5));
					inputLayerWindow->draw(backgroundSprite);

					sf::Text newText(successful, *inputLayerFont, MainMenu->menuTextSize);
					newText.setPosition(300, 200);
					newText.setFillColor(sf::Color::Black);
					inputLayerWindow->draw(newText);
					inputLayerWindow->display();

					loadsuccessful = true;
				}
				else
				{
					sf::String loadPrompt = "Could not load save game. Please check that it exists and the right spelling was used.\nChoose which save game to load (Case sensitive. Do not use _save portion of save.): \n";
					lineOffset = 2;

				}
			}
			//Actually load scenario. Initialize board, etc.
			MainMenu->gameLoad(boardToInput, this, &loadGameSave);
			//Flush event queue to clear out "Enter" and other rifraf
			sf::Event throwAwayEvent;
			while (inputLayerWindow->pollEvent(throwAwayEvent));
			//Give player a chance to click.
			playCharInput(inputLayerWindow);

			status = gameBoard;
		}


		bool withinRestartButton = (*menuButtons)[3].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinRestartButton == true)
			//Restart current mission/scenario
		{
			restartGame(boardToInput);
		}


		bool withinSoundButton = (*menuButtons)[4].checkWithinButton(mousePosition.x, mousePosition.y);
		//Toggle sound based on current sound output.
		if (withinSoundButton == true && soundsOn == true)
		{
			soundsOn = false;
		}
		else if (withinSoundButton == true && soundsOn == false)
		{
			soundsOn = true;
		}

		//Toggle speed based on current speed factor.
		bool withinToggleSpeedButton = (*menuButtons)[6].checkWithinButton(mousePosition.x, mousePosition.y);
		if (withinToggleSpeedButton == true && speedFactor == 1)	 
		{
			speedFactor = 3;
		}
		else if (withinToggleSpeedButton == true && speedFactor == 3)
		{
			speedFactor = 1;
		}

		bool withinEndTurnButton = (*menuButtons)[8].checkWithinButton(mousePosition.x, mousePosition.y);
		//Ends the turn and passes it to the next player.
		//Autosave every turn.
		if (withinEndTurnButton == true)
		{
			if (boardToInput->cursor.selectMinionFlag == true)
				boardToInput->deselectMinion();
			int incrementGameTurn = boardToInput->endTurn(this);
			//If we advanced a gameTurn, mainMenu will keep track of it.
			MainMenu->gameTurn += incrementGameTurn;

			//Have to always keep an autosave!
			MainMenu->gameSave(".\\savegames\\Auto_save.txt", boardToInput);
		}

		bool withinReturnToGameButton = (*menuButtons)[9].checkWithinButton(mousePosition.x, mousePosition.y);
		//Exit menu
		if (withinReturnToGameButton == true)
		{
			status = gameBoard;

		}

	}

	//Right click to exit menu
	if (*Input == sf::Keyboard::Comma)
	{
		status = gameBoard;
	}

	return 0;
}

//Because this is only ordered by player, we don't have to worry about cleaning up compie behavior afterwards. So we can just re-load and go back to gameboard.
int inputLayer::restartGame(MasterBoard* boardToInput)
{

	//Load the actual save game
	std::ifstream loadGame;
	bool loadsuccessful = false;

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{

		std::string gameToLoad = boardToInput->scenarioOrMissionName;

		if (boardToInput->missionFlag == true)
		{

			loadGame.open(".\\campaigns\\" + boardToInput->campaignName + "\\" + gameToLoad + ".txt");
		}
		else
		{
			loadGame.open(".\\scenarios\\" + gameToLoad + ".txt");
		}

		if (loadGame.is_open())
		{
			std::cout << "Scenario/mission successfully loaded!\n";
			loadsuccessful = true;
		}
		else
		{
			std::cout << "Could not load scenario or mission. Please check that it exists and the right spelling was used.\n";

		}

	}
	//Actually load scenario. Initialize board, etc.
	MainMenu->gameLoad(boardToInput, this, &loadGame);

	//We don't always print mission briefing but we do here
	printMissionBriefing(boardToInput);

	status = gameBoard;
	return 0;

}

int inputLayer::NextMission(MasterBoard* boardToInput)
{
	//Load the actual save game
	std::ifstream loadGame;
	bool loadsuccessful = false;

	//Prompt user and load scenario
	while (loadsuccessful == false)
	{

		std::string gameToLoad = MainMenu->nextMissionName;

		loadGame.open(".\\campaigns\\" + boardToInput->campaignName + "\\" + gameToLoad + ".txt");


		if (loadGame.is_open())
		{
			std::cout << "Scenario/mission successfully loaded!\n";
			loadsuccessful = true;
		}
		else
		{
			std::cout << "Could not load scenario or mission. Please check that it exists and the right spelling was used.\n";

		}

	}
	std::string playerName = boardToInput->playerRoster[1].name;
	//Actually load scenario. Initialize board, etc.
	MainMenu->gameLoad(boardToInput, this, &loadGame);
	boardToInput->playerRoster[1].name = playerName;
	//NEED to transfer player information -  just the name for now.

	//We don't always print mission briefing but we do here
	printMissionBriefing(boardToInput);



	status = gameBoard;

	MainMenu->playGame(boardToInput, this);

	return 0;

}

int inputLayer::propertyMenuInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput) {

	tile* myTile = &boardToInput->Board[boardToInput->cursor.XCoord][boardToInput->cursor.YCoord];
	char requestedPurchase = '~';
	bool purchaseSuccess = false;
	int treasury = boardToInput->playerRoster[boardToInput->playerFlag].treasury;

	//Need char for shift
	if (*Input == '0')
	{
		if (minionVisibleStatus == hideMinions)
		{
			minionVisibleStatus = showMinions;
		}
		else if (minionVisibleStatus == showMinions)
		{
			minionVisibleStatus = hideMinions;
		}
		return 0;
	}

	int buttonNumber = -1;

	if (*Input == sf::Keyboard::Quote)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));


		if (myTile->symbol == 'h') 
		{

			for (int i = 0; i < factoryButtons.size(); i++)
			{
				bool withinButton = factoryButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);
				//Prompt user and save game.
				if (withinButton == true)
				{
					buttonNumber = i;
				}
			}

			switch (buttonNumber)
			{
			case 0:
				requestedPurchase = 'i';
				break;
			case 1:
				requestedPurchase = 's';
				break;
			case 2:
				requestedPurchase = 'c';
				break;
			case 3:
				requestedPurchase = 'P';
				break;
			case 4:
				requestedPurchase = 'r';
				break;
			case 5:
				requestedPurchase = 'a';
				break;
			case 6:
				requestedPurchase = 'A';
				break;
			case 7:
				requestedPurchase = 'R';
				break;
			case 8:
				requestedPurchase = 'T';
				break;
			}
		}
	
	if(myTile->symbol == 'P')
	{
		for (int i = 0; i < portButtons.size(); i++)
		{
			bool withinButton = portButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);
			//Prompt user and save game.
			if (withinButton == true)
			{
				buttonNumber = i;
			}
		}

		if (buttonNumber == 0)
			requestedPurchase = 'G';
		if (buttonNumber == 1)
			requestedPurchase = 'C';
		if (buttonNumber == 2)
			requestedPurchase = 'L';
		if (buttonNumber == 3)
			requestedPurchase = 'U';
		if (buttonNumber == 4)
			requestedPurchase = 'B';
		if (buttonNumber == 5)
			requestedPurchase = 'V';

	}
	
	if( myTile->symbol == 'A')
	{
		for (int i = 0; i < airbaseButtons.size(); i++)
		{
			bool withinButton = airbaseButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);
			//Prompt user and save game.
			if (withinButton == true)
			{
				buttonNumber = i;
			}
		}

		if (buttonNumber == 0)
			requestedPurchase = 'h';
		if (buttonNumber == 1)
			requestedPurchase = 'v';
		if (buttonNumber == 2)
			requestedPurchase = 'f';
		if (buttonNumber == 3)
			requestedPurchase = 'b';


	}
	
	}


	//Consult cost table:
	int requestedUnitPrice = boardToInput->consultMinionCostChart(requestedPurchase, myTile->symbol);

	//If it is a real unit we are trying to purchase
	//Aka unitPrica is not -1 aka non-error
	if (requestedUnitPrice > 0)
	{
		//Can we afford it
		if (requestedUnitPrice <= treasury)
		{
			//Confirm purchase
			boardToInput->attemptPurchaseMinion(requestedPurchase, boardToInput->cursor.getX(), boardToInput->cursor.getY(), boardToInput->playerFlag);
			status = gameBoard;
		}
	}

	//Right click to exit menu
	if (*Input == sf::Keyboard::Comma)
	{
		status = gameBoard;
	}


	return 0;
}


int inputLayer::exitToMainMenu(MasterBoard* boardToInput)
{

	//Clear up resources we used to avoid memory leak.
	boardToInput->clearBoard(this);

	MainMenu->menuStatus = topmenu;
	MainMenu->skipOneInput = true;


	//Leave it all behind and start again.
	//Do you... really think we could?
	MainMenu->playGame(boardToInput, this);

	return 0;
}