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
#include "SFML/System/Time.hpp"
#include "SFML/System/Clock.hpp"

char getValidPlayerInput(sf::RenderWindow* myWindow);

inputLayer::inputLayer(mainMenu* inputMainMenu, sf::RenderWindow* myWindow, sf::Texture* gameTexture,
	sf::Font* cour, std::vector <sf::Sound>* inputSoundEffects, std::vector <Button>* inputMenuButtons,
	std::vector <sf::Texture>* statusTextures,  sf::Music * inputGameMusic)
{
	inputLayerTexture = gameTexture;
	inputLayerFont = cour;
	inputLayerWindow = myWindow;
	MainMenu = inputMainMenu;
	soundEffects = inputSoundEffects;
	menuButtons = inputMenuButtons;
	showRangeStatus = hideRange;
	gameMusic = inputGameMusic;

	IL_WIDTH_OFFSET = inputMainMenu->MM_WIDTH_OFFSET;
	IL_HEIGHT_OFFSET = inputMainMenu->MM_HEIGHT_OFFSET;

	std::cout << "Width offset is: " << IL_WIDTH_OFFSET << std::endl;
	std::cout << "Height offset is: " << IL_HEIGHT_OFFSET << std::endl;

	//Create buttons for property menus using the gameTexture
	//Overall property menu area is:
	int menuTop = 150 ;
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
		factoryButtons.emplace_back(menuLeft + x * 50 + IL_WIDTH_OFFSET, menuTop + y * 50 + IL_HEIGHT_OFFSET, factoryButton, gameTexture, factoryOptions.at(i));
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
	factoryButtons.at(0).mySprite.setTextureRect(rectArray[0][4]);	    //Infantry
	factoryButtons.at(1).mySprite.setTextureRect(rectArray[30][4]);	    //Insurgent
	factoryButtons.at(2).mySprite.setTextureRect(rectArray[29][4]);	    //Operative
	factoryButtons.at(3).mySprite.setTextureRect(rectArray[1][4]);	    //Specialist
	factoryButtons.at(4).mySprite.setTextureRect(rectArray[2][4]);      //Recon
	factoryButtons.at(5).mySprite.setTextureRect(rectArray[31][4]);     //Technical
	factoryButtons.at(6).mySprite.setTextureRect(rectArray[4][4]);      //APC
	factoryButtons.at(7).mySprite.setTextureRect(rectArray[25][4]);     //IFV
	factoryButtons.at(8).mySprite.setTextureRect(rectArray[5][4]);      //Artillery	
	factoryButtons.at(9).mySprite.setTextureRect(rectArray[3][4]);      //Armor
	factoryButtons.at(10).mySprite.setTextureRect(rectArray[28][4]);     //Assault gun
	factoryButtons.at(11).mySprite.setTextureRect(rectArray[27][4]);      //Upgunned armor
	factoryButtons.at(12).mySprite.setTextureRect(rectArray[7][4]);      //Anti-Aircraft
	factoryButtons.at(13).mySprite.setTextureRect(rectArray[8][4]);      //Rocket
	factoryButtons.at(14).mySprite.setTextureRect(rectArray[6][4]);      //Heavy Armor
	factoryButtons.at(15).mySprite.setTextureRect(rectArray[26][4]);      //Modern Armor
	factoryButtons.at(16).mySprite.setTextureRect(rectArray[34][4]);      //Super Heavy Armor
	factoryButtons.at(17).mySprite.setTextureRect(rectArray[33][4]);      //Victory Launcher
	factoryButtons.at(18).mySprite.setTextureRect(rectArray[35][4]);      //Cavalry


	//Airbase buttons
	y = 0;	//Reset counters
	x = 0;
	for (int i = 0; i < airbaseOptions.size(); i++)
	{
		airbaseButtons.emplace_back(menuLeft + x * 50 + IL_WIDTH_OFFSET, menuTop + y * 50 + IL_HEIGHT_OFFSET, airbaseButton, gameTexture, airbaseOptions.at(i));
		airbaseButtons.at(i).mySprite.setTextureRect(rectArray[0][4]);	//Placeholder image
		airbaseButtons.at(i).width = 50;
		airbaseButtons.at(i).height = 50;
		x++;
		if (x == numberOfButtonsPerRow)
		{
			x = 0;
			y++;
		}
	}
	airbaseButtons.at(0).mySprite.setTextureRect(rectArray[10][4]);  //Transport_Copter
	airbaseButtons.at(1).mySprite.setTextureRect(rectArray[9][4]);   //Attack_Copter
	airbaseButtons.at(2).mySprite.setTextureRect(rectArray[11][4]);  //Interceptor
	airbaseButtons.at(3).mySprite.setTextureRect(rectArray[24][4]);  //Advanced_Fighter
	airbaseButtons.at(4).mySprite.setTextureRect(rectArray[20][4]);  //Multirole
	airbaseButtons.at(5).mySprite.setTextureRect(rectArray[12][4]);  //Bomber

	//Naval buttons
	y = 0;	//Reset counters
	x = 0;
	for (int i = 0; i < portOptions.size(); i++)
	{
		portButtons.emplace_back(menuLeft + x * 50 + IL_WIDTH_OFFSET, menuTop + y * 50 + IL_HEIGHT_OFFSET, portButton, gameTexture, portOptions.at(i));
		portButtons.at(i).mySprite.setTextureRect(rectArray[0][4]);	//Placeholder image
		portButtons.at(i).width = 50;
		portButtons.at(i).height = 50;
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

	//Create status indicator buttons
	//Overall status area is same as above
	//Button dimensions
	sf::Vector2u statusHeightWidth = statusTextures->at(0).getSize();
	int statusButtonHeight = statusHeightWidth.y;
	int statusButtonWidth = statusHeightWidth.x;
	numberOfButtonsPerRow = 3;

	y = 0;
	x = 0;

	//For each status element, create new button and push back.
	for (int i = 0; i < statusTextures->size(); i++)
	{
		statusButtons.emplace_back(menuLeft + (x * (statusButtonWidth + 10) ) + IL_WIDTH_OFFSET , menuTop + ( y * (statusButtonHeight + 10) ) + IL_HEIGHT_OFFSET , int(statusButton), &(statusTextures->at(i)), "StatusButton");

		//Put certain number of buttons on each row, then go to next row.
		x++;
		if (x == numberOfButtonsPerRow)
		{
			x = 0;
			y++;
		}
	}


	return;
}

int inputLayer::printSingleTile(int screenX, int screenY, int actualX, int actualY, MasterBoard* boardToPrint, int playerNumber, bool withinAnimation)
{
	tile* tileToPrint = &boardToPrint->Board[actualX][actualY];
	//Initialize effects sprite, even though it may not always be used.
	sf::Sprite effectsSprite;
	effectsSprite.setTexture(*inputLayerTexture);
	effectsSprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);

	//First print tile, change sprite depending on if withinVision or not
	tileToPrint->mySprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);
	tileToPrint->myFogSprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);
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
		riverSprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);

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
		beachSprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);

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

	//See if there are adjacent minions that can see a stealth minion
	bool adjacentObservers = boardToPrint->hasAdjacentMinion(actualX, actualY, playerNumber);

	//We only do minions and associated effects if they are visible
	if (minionVisibleStatus == showMinions)
	{
		//Then print minion if withinVision AND not invisible AND not hidden
		if (tileToPrint->hasMinionOnTop == true && tileToPrint->withinVision[playerNumber] == true
			&& tileToPrint->minionOnTop->invisible == false)
		{
			//Last check - must not be stealth minion and unseen
			if (tileToPrint->minionOnTop->stealthMode == false || adjacentObservers == true || tileToPrint->minionOnTop->team == playerNumber)
			{
				tileToPrint->minionOnTop->mySprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);
				inputLayerWindow->draw(tileToPrint->minionOnTop->mySprite);
			}
		}

		//Get mouse position to see if it's hovering over a potential action tile
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));
		int windowX = ( mousePosition.x - IL_WIDTH_OFFSET ) / 50 ;
		int windowY = ( mousePosition.y - IL_HEIGHT_OFFSET) / 50 ;
		int mouseTileX = windowX + boardToPrint->windowLocationX;       //May take out windowLocationX if this isn't working
		int mouseTileY = windowY + boardToPrint->windowLocationY;       //May take out windowLocationY if this isn't working
		bool mouseHovering = false;

		//See if mouse is over this current tile
		if (actualX == mouseTileX && actualY == mouseTileY)
		{
			mouseHovering = true;
		}

		//Will only print  "selection" effects if human player's turn, or debug mode 
		//AND if not in the middle of animation - indicated by the invisible flag on minion for movement
		//AND indicated by the withinAnimation flag, for attacks and such.
		//Don't need to check for stealth, because would already be on compie's turn
		if ((boardToPrint->playerFlag == playerNumber || MainMenu->editorMode == true)
			&& boardToPrint->cursor.selectMinionPointer != NULL
			&& boardToPrint->cursor.selectMinionPointer->invisible == false
			&& withinAnimation == false)
		{
			//All the various selection effect squares like within movement, attack, and drop range
			if (boardToPrint->cursor.selectMinionFlag == true
				&& boardToPrint->cursor.selectMinionPointer->locationX == actualX
				&& boardToPrint->cursor.selectMinionPointer->locationY == actualY
				&& tileToPrint->withinVision[playerNumber] == true)
			{
				//If minion is selected here, print the "selected" square.
				//We'll print selected square regardless of compie or player
				effectsSprite.setTextureRect(rectArray[1][2]);
				inputLayerWindow->draw(effectsSprite);

				//If minion can capture and is on top of capturable property, show capturable flag
				if (mouseHovering == true && boardToPrint->cursor.selectMinionPointer->captureCapable == true
					&& tileToPrint->checkForProperty() == true
					&& tileToPrint->controller != boardToPrint->playerFlag)
				{
					effectsSprite.setTextureRect(rectArray[6][15]); //Draw little flag
					inputLayerWindow->draw(effectsSprite);
				}
				else
					if (mouseHovering == true && boardToPrint->cursor.selectMinionPointer->type == "APC")
					{
						effectsSprite.setTextureRect(rectArray[8][15]); //Draw supply image
						inputLayerWindow->draw(effectsSprite);
					}
					else if (mouseHovering == true && boardToPrint->cursor.selectMinionPointer->type == "Submarine")
					{
						effectsSprite.setTextureRect(rectArray[10][15]); //Draw periscope
						inputLayerWindow->draw(effectsSprite);
					}
			}
			else
				if (tileToPrint->withinCursorPath == true)
				{
					//If this tile is on cursor's path
					effectsSprite.setTextureRect(rectArray[6][2]);
					inputLayerWindow->draw(effectsSprite);

					if (mouseHovering == true)
					{
						effectsSprite.setTextureRect(rectArray[9][15]); //Draw boot
						inputLayerWindow->draw(effectsSprite);
					}
				}
				else
					if (tileToPrint->withinApparentRange == true)		//This should apply to movement only I believe
					{
						//If this tile is within apparent range.
						effectsSprite.setTextureRect(rectArray[3][2]);
						inputLayerWindow->draw(effectsSprite);

						if (mouseHovering == true)
						{
							effectsSprite.setTextureRect(rectArray[9][15]); //Draw boot
							inputLayerWindow->draw(effectsSprite);
						}

					}
					else if (tileToPrint->withinRange == true		//Attack and transport/landmine square 
						&& (boardToPrint->cursor.selectMinionPointer->status == gaveupmovehasntfired
							|| boardToPrint->cursor.selectMinionPointer->status == hasmovedhasntfired)
						&& (boardToPrint->cursor.selectMinionPointer->specialtyGroup == smallTransport
							|| boardToPrint->cursor.selectMinionPointer->specialtyGroup == largeTransport
							|| boardToPrint->cursor.selectMinionPointer->specialtyGroup == aircraftCarrier
							|| boardToPrint->cursor.selectMinionPointer->type == "Operative")
						&& (tileToPrint->hasMinionOnTop == false || tileToPrint->minionOnTop->team == boardToPrint->playerFlag))
					{
						//If this tile is within range for drop off AND doesn't have enemy minion
						//Can also indicate that operative can deploy landmine here.
						effectsSprite.setTextureRect(rectArray[4][2]);
						inputLayerWindow->draw(effectsSprite);

						if (mouseHovering == true)
						{
							//Differentiate between operative and transport for potential action
							if (boardToPrint->cursor.selectMinionPointer->type == "Operative")
								effectsSprite.setTextureRect(rectArray[4][15]); //Draw landmine drop
							else effectsSprite.setTextureRect(rectArray[7][15]); //Draw bag drop

							inputLayerWindow->draw(effectsSprite);
						}

					}
					else if (tileToPrint->withinRange == true && boardToPrint->cursor.selectMinionPointer->attackRange != 0
						&& (boardToPrint->cursor.selectMinionPointer->status == gaveupmovehasntfired
							|| boardToPrint->cursor.selectMinionPointer->status == hasmovedhasntfired))
					{
						//If minion can fire and if this tile is within range.
						effectsSprite.setTextureRect(rectArray[2][2]);
						inputLayerWindow->draw(effectsSprite);

						if (mouseHovering == true)
						{
							effectsSprite.setTextureRect(rectArray[3][15]); //Draw crosshairs
							inputLayerWindow->draw(effectsSprite);
						}
					}

		}
		else   //If cursor is hovering (no one selected), and showRange is toggled
			if (showRangeStatus == showRange && boardToPrint->cursor.selectMinionFlag == false)
			{

				Minion* minionBeneathCursor = boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()].minionOnTop;
				if (minionBeneathCursor != NULL)
				{
					int rangeBetweenTileAndCursor = boardToPrint->computeDistance(minionBeneathCursor->locationX, tileToPrint->locationX, minionBeneathCursor->locationY, tileToPrint->locationY);

					if (rangeBetweenTileAndCursor <= minionBeneathCursor->attackRange && rangeBetweenTileAndCursor > minionBeneathCursor->minAttackRange)
					{
						//If this tile is within range of minion weapon, Print the withinAttackRange box.
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
			boardToPrint->cursor.mySprite.setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);
			inputLayerWindow->draw(boardToPrint->cursor.mySprite);

		}

		Minion* minionToPrint = NULL;
		if (tileToPrint->hasMinionOnTop == true)
			minionToPrint = tileToPrint->minionOnTop;

		//Must be visible to see effects!
		if (tileToPrint->hasMinionOnTop == true && minionToPrint != NULL && tileToPrint->withinVision[playerNumber] == true
			&& minionVisibleStatus == showMinions && minionToPrint->invisible == false
			&& (tileToPrint->minionOnTop->stealthMode == false || adjacentObservers == true || tileToPrint->minionOnTop->team == playerNumber))
		{

			//Print if transporting or capturing // Specialty moves
			if (minionToPrint->isCapturing == true)
			{
				effectsSprite.setTextureRect(rectArray[5][3]);
				inputLayerWindow->draw(effectsSprite);
			}
			else if (minionToPrint->firstMinionBeingTransported != NULL || minionToPrint->secondMinionBeingTransported != NULL)
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
			if ((minionToPrint->status == hasmovedhasntfired && minionToPrint->rangeType != rangedFire) || minionToPrint->status == gaveupmovehasntfired)
			{
				effectsSprite.setTextureRect(rectArray[5][0]);
				inputLayerWindow->draw(effectsSprite);
			}

			//Minion exists and is below 1/3 fuel
			if (((minionToPrint->currentFuel == 0 && minionToPrint->maxFuel != 0) || (double(minionToPrint->maxFuel) / double(minionToPrint->currentFuel)) >= 3))
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
			tileToPrint->animationSprite->setPosition(screenX * 50 + IL_WIDTH_OFFSET, screenY * 50 + IL_HEIGHT_OFFSET);
			inputLayerWindow->draw(*(tileToPrint->animationSprite));


		}

		return 1;
	}
}


//New function:
int inputLayer::printStatusDialogBox(MasterBoard* boardToPrint)
{
	
	//Always print dialog box below text
	int xCoord = 240;
	int yCoord = 200;
	sf::Sprite dialogBoxSprite;
	dialogBoxSprite.setTexture(MainMenu->otherGameTextures->at(11));
	dialogBoxSprite.setPosition(xCoord + IL_WIDTH_OFFSET, yCoord + IL_HEIGHT_OFFSET);
	inputLayerWindow->draw(dialogBoxSprite);

	int explainTextPositionX = 265 + IL_WIDTH_OFFSET;
	int explainTextPositionY = 220 + IL_HEIGHT_OFFSET;

	//Then print text based on which dialog box it is
	switch (dialogBoxOpen)
	{
	case(0):
	{
		//Print defense explain:
		sf::String defenseExplainString = "Each tile provides a defense bonus from 0 to 5.\nEach point reduces damage by 5 % in combat.";
		sf::Text defenseExplainText(defenseExplainString, *inputLayerFont, MainMenu->menuTextSize);
		defenseExplainText.setPosition(explainTextPositionX , explainTextPositionY);
		defenseExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(defenseExplainText);
		break;
	}
	case(1):
	{
		//Print production explain:
		sf::String productionExplainString = "Friendly properties produce gold to support the\nwar effort. Most produce 1000 per turn, but some\nproduce more.";
		sf::Text productionExplainText(productionExplainString, *inputLayerFont, MainMenu->menuTextSize);
		productionExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		productionExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(productionExplainText);
		break;
	}
	case(2):
	{
		//Print capture explain:
		sf::String capExplainString = "Capture points. Infantry and technicals can capture\nenemy properties at 1 point per health per turn.\n20 points are needed to capture a property.";
		sf::Text capExplainText(capExplainString, *inputLayerFont, MainMenu->menuTextSize);
		capExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		capExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(capExplainText);
		break;
	}
	case(3):
	{
		//Print health explain:
		sf::String healthExplainString = "Minion hit points. All minions start with 100 and\ndie at 0. Minions repair 20 hit points per turn on\na friendly property. Attack and capture is\nproportional to hit points.";
		sf::Text healthExplainText(healthExplainString, *inputLayerFont, MainMenu->menuTextSize);
		healthExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		healthExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(healthExplainText);
		break;
	}
	case(4):
	{
		//Print movement/fuel explain:
		sf::String fuelExplainString = "Fuel left and maximum fuel. Minions use fuel based\non their type and the terrain they cross. Also, aircraft\nuse 5 fuel and ships use 2 fuel during upkeep.\nIf they run out, they die.";
		sf::Text fuelExplainText(fuelExplainString, *inputLayerFont, MainMenu->menuTextSize);
		fuelExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		fuelExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(fuelExplainText);
		break;
	}
	case(5):
	{
		//Print ammo explain:
		sf::String ammoExplainString = "Minions require ammo to attack with each weapon.\nIndicates current and maximum ammo.\nPrimary ammo is on top, secondary ammo on bottom.\nINF indicates infinite ammo for that weapon.";
		sf::Text ammoExplainText(ammoExplainString, *inputLayerFont, MainMenu->menuTextSize);
		ammoExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		ammoExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(ammoExplainText);
		break;
	}
	case(6):
	{
		//Print move-state explain:
		sf::String moveExplainString = "Green arrow - Can move and then attack/other action.\nYellow pause - Can now attack, unload, supply, etc.\nRed stop - Has performed all possible moves this turn.";
		sf::Text moveExplainText(moveExplainString, *inputLayerFont, MainMenu->menuTextSize);
		moveExplainText.setPosition(explainTextPositionX, explainTextPositionY);
		moveExplainText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(moveExplainText);
		break;
	}

	}

	return 0;
}



//Updated printStatus
int inputLayer::printStatus(MasterBoard* boardToPrint, int observerNumber)
{
	//InputLayerStatus is not working, so we check if they're computer player here
	if (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType == computerPlayer)
		menuLineTracker += 2;

	int spacingConstant = 25;
	tile* currentTile = &boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	//Need string pointer since addstr giving grief about printing strings, and same with snprintf.
	char pointerToPrint[100];
	char* descriptionPointer = &currentTile->description[0];
	char* playerName = &(boardToPrint->playerRoster[currentTile->controller].name[0]);
	sf::String tileDescription;

	//Print current player, with treasury and potential event text.
	sf::String playerStatus = &(boardToPrint->playerRoster[boardToPrint->playerFlag].name[0]);
	snprintf(pointerToPrint, 100, "'s turn.\nTreasury Total: %d\n", boardToPrint->playerRoster[boardToPrint->playerFlag].treasury);
	//snprintf(pointerToPrint, 100, "'s turn.\nTreasury Total: %d\n", boardToPrint->playerRoster[boardToPrint->playerFlag].treasury);
	playerStatus += pointerToPrint;
	playerStatus += &eventText[0];

	//Convert and print.
	sf::Text playerStatusText(playerStatus, *inputLayerFont, MainMenu->menuTextSize);
	playerStatusText.setPosition(MAX_WINDOW_WIDTH * 52 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + spacingConstant + IL_HEIGHT_OFFSET);
	playerStatusText.setFillColor(sf::Color::Black);
	inputLayerWindow->draw(playerStatusText);

	//Reset event text
	eventText = "";

	//Do not print out status boxes for property menu
	//Do not print status for compie moves
	if (status != propertyAction && boardToPrint->playerRoster[boardToPrint->playerFlag].playerType != computerPlayer)
	{
		//Draw boxes first
		for (int i = 0; i < statusButtons.size(); i++)
			inputLayerWindow->draw(statusButtons.at(i).mySprite);

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

		}

		//See if any adjacent friendly minions to this square
		bool friendlyAdjacentMinion = boardToPrint->hasAdjacentMinion(currentTile->locationX, currentTile->locationY, boardToPrint->playerFlag);

		//See if enemy minion here.
		bool stealthEnemyHere = false;
		if (currentTile->hasMinionOnTop == true && currentTile->minionOnTop->team != boardToPrint->playerFlag && currentTile->minionOnTop->stealthMode == true)
			stealthEnemyHere = true;

		bool unseenStealthEnemyHere = false;
		if (stealthEnemyHere == true && friendlyAdjacentMinion == false)
			unseenStealthEnemyHere = true;

		if (currentTile->hasMinionOnTop == true && currentTile->withinVision[observerNumber] && unseenStealthEnemyHere == false)
		{


			//If tile is undergoing capture, let us know.
			if (currentTile->capturePoints != 20)
			{
				//First draw box for capture status
				snprintf(pointerToPrint, 100, "%d", currentTile->capturePoints);
				sf::Text newText(pointerToPrint, *inputLayerFont, MainMenu->menuTextSize + 6);
				newText.setPosition(statusButtons.at(2).xCoord + 78 + IL_WIDTH_OFFSET, statusButtons.at(2).yCoord + 18 + IL_HEIGHT_OFFSET);
				newText.setFillColor(sf::Color::Black);
				inputLayerWindow->draw(newText);
			}

			Minion* currentMinion = currentTile->minionOnTop;

			//Print out basic minion status.
			tileDescription += "\n";
			tileDescription += &(boardToPrint->playerRoster[currentMinion->team].name[0]);
			tileDescription += "'s ";
			tileDescription += &currentMinion->description[0];


			//Print health:
			//Then print actual number
			snprintf(pointerToPrint, 100, "%d", int(currentMinion->health));
			sf::String healthNumberString = pointerToPrint;
			sf::Text healthNumberText(healthNumberString, *inputLayerFont, MainMenu->menuTextSize);
			healthNumberText.setPosition(statusButtons.at(3).xCoord + 78, statusButtons.at(3).yCoord + 23);
			healthNumberText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(healthNumberText);

			//Print out fuel status
			//Then print actual number
			snprintf(pointerToPrint, 100, "%d/%d", currentMinion->currentFuel, currentMinion->maxFuel);
			sf::String fuelNumberString = pointerToPrint;
			sf::Text fuelNumberText(fuelNumberString, *inputLayerFont, MainMenu->menuTextSize);
			fuelNumberText.setPosition(statusButtons.at(4).xCoord + 70, statusButtons.at(4).yCoord + 23);
			fuelNumberText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(fuelNumberText);

			//Print ammo box:
			//Then determine combo of pri/sec ammo:
			if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo > 0)		//Both pri/sec have ammo
				snprintf(pointerToPrint, 100, "%d/%d\n%d/%d", currentMinion->currentPriAmmo, currentMinion->maxPriAmmo, currentMinion->currentSecAmmo, currentMinion->maxSecAmmo);
			else if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo == 0)		//Infinte sec ammo
				snprintf(pointerToPrint, 100, "%d/%d\nINF", currentMinion->currentPriAmmo, currentMinion->maxPriAmmo);
			else if (currentMinion->maxSecAmmo == 0 && currentMinion->maxPriAmmo == -1)	//No pri, infinite sec. 
				snprintf(pointerToPrint, 100, "N/A\nINF");
			else if (currentMinion->maxSecAmmo == -1 && currentMinion->maxPriAmmo > 0)	//Primary with no sec.
				snprintf(pointerToPrint, 100, "%d/%d\nN/A", currentMinion->currentPriAmmo, currentMinion->maxPriAmmo);
			else //No weapons.
				snprintf(pointerToPrint, 100, "N/A\nN/A");

			//Then print out actual values of ammo
			sf::String ammoNumberString = pointerToPrint;
			sf::Text ammoNumberText(ammoNumberString, *inputLayerFont, MainMenu->menuTextSize);
			ammoNumberText.setPosition(statusButtons.at(5).xCoord + 80 + IL_WIDTH_OFFSET, statusButtons.at(5).yCoord + 15 + IL_HEIGHT_OFFSET);
			ammoNumberText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(ammoNumberText);

			//Print move state of minion
			sf::Sprite effectsSprite;
			effectsSprite.setTexture(*inputLayerTexture);
			effectsSprite.setPosition(statusButtons.at(6).xCoord + 35 + IL_WIDTH_OFFSET, statusButtons.at(6).yCoord + 10 + IL_HEIGHT_OFFSET);
			if (currentMinion->status == gaveupmovehasntfired)
			{
				//Print yellow pause image
				effectsSprite.setTextureRect(rectArray[1][15]);
				inputLayerWindow->draw(effectsSprite);
			}
			else
				if (currentMinion->status == hasmovedhasntfired)
				{
					if (currentMinion->rangeType == rangedFire)
					{
						//Print red octagon -- 15, 2
						effectsSprite.setTextureRect(rectArray[2][15]);
						inputLayerWindow->draw(effectsSprite);

					}
					if (currentMinion->rangeType == directFire || currentMinion->rangeType == hybridRange)
					{
						//Print yellow pause image -- 15, 1
						effectsSprite.setTextureRect(rectArray[1][15]);
						inputLayerWindow->draw(effectsSprite);

					}
				}
				else
					if (currentMinion->status == hasfired)
					{
						//Print red octagon -- 15, 2
						effectsSprite.setTextureRect(rectArray[2][15]);
						inputLayerWindow->draw(effectsSprite);
					}
					else
						if (currentMinion->status == hasntmovedorfired)
						{
							//Print green triangle -- 15,0
							effectsSprite.setTextureRect(rectArray[0][15]);
							inputLayerWindow->draw(effectsSprite);
						}

		}

		//No longer need debug printouts on editMode.
		sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
		newText.setPosition(MAX_WINDOW_WIDTH * 52 + 200 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + spacingConstant + IL_HEIGHT_OFFSET);
		newText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(newText);

		//Printout defense bonus status for tile
		//Then print actual number
		snprintf(pointerToPrint, 100, "%d", int(round((currentTile->defenseFactor - 1.0) * 10)));
		sf::String defenseBonusNumber = pointerToPrint;
		sf::Text defenseBonusText(defenseBonusNumber, *inputLayerFont, MainMenu->menuTextSize + 6);
		defenseBonusText.setPosition(statusButtons.at(0).xCoord + 80 + IL_WIDTH_OFFSET,  statusButtons.at(0).yCoord + 18 + IL_HEIGHT_OFFSET);
		defenseBonusText.setFillColor(sf::Color::Black);
		inputLayerWindow->draw(defenseBonusText);

		//Printout production status for tile if it exists
		if (currentTile->production > 0)
		{
			//Print out production status
			//Then print actual number
			snprintf(pointerToPrint, 100, "%d", currentTile->production);
			sf::String productionNumberString = pointerToPrint;
			sf::Text productionNumberText(productionNumberString, *inputLayerFont, MainMenu->menuTextSize);
			productionNumberText.setPosition(statusButtons.at(1).xCoord + 70 + IL_WIDTH_OFFSET, statusButtons.at(1).yCoord + 23 + IL_HEIGHT_OFFSET) ; 
			productionNumberText.setFillColor(sf::Color::Black);
			inputLayerWindow->draw(productionNumberText);
		}
	}


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

	newText.setPosition(MAX_WINDOW_WIDTH * 52 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + 2 + IL_HEIGHT_OFFSET);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printBoardMenu(MasterBoard* boardToPrint) {

	sf::String boardMessage = "Move cursor (WASD) | Menu (m)\nSelect minion/property (t)\n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + IL_HEIGHT_OFFSET);

	newText.setFillColor(sf::Color::Black);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;
}

int	inputLayer::printPropertyMenu(MasterBoard* boardToPrint)
{
	//Get mouse position for checking buttons later
	sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));

	//Create string, but do not yet print. Still need to add hovered minion name.
	sf::String boardMessage = "Purchase minion (L-Click). Deselect (R-Click/T-Key)\n";
	char hoveredMinionName[100];
	snprintf(hoveredMinionName, 100, " ");

	tile* myTile = &boardToPrint->Board[boardToPrint->cursor.XCoord][boardToPrint->cursor.YCoord];

	//Black border box
	sf::Sprite effectsSprite;
	effectsSprite.setTexture(*inputLayerTexture);
	effectsSprite.setTextureRect(rectArray[10][13]);

	sf::Sprite grayBoxSprite;
	grayBoxSprite.setTexture(*inputLayerTexture);
	grayBoxSprite.setTextureRect(rectArray[9][13]);
	grayBoxSprite.setColor(sf::Color(255, 255, 255, 128));

	if (myTile->symbol == 'h')
	{
		for (int i = 0; i < factoryButtons.size(); i++)
		{
			//Check if minion type is on ban-list
			//Must NOT be on ban list in order to print
			bool withinFaction = boardToPrint->checkFactionAvailability(factoryButtons.at(i).myName, boardToPrint->playerFlag);

			if (std::find(boardToPrint->banList.begin(), boardToPrint->banList.end(), factoryButtons.at(i).myName) == boardToPrint->banList.end() && withinFaction == true)
			{
				inputLayerWindow->draw(factoryButtons.at(i).mySprite);
				effectsSprite.setPosition(factoryButtons.at(i).mySprite.getPosition());
				inputLayerWindow->draw(effectsSprite);

				//Check if player can afford each particular minion, and if not, then draw over a gray box effect.
				int minionPrice = boardToPrint->consultMinionCostChart(factoryButtons.at(i).myName, '~');

				//If it is a real and non-banned minion, BUT is not affordable, print gray box effect.
				if (minionPrice > 0 && minionPrice > boardToPrint->playerRoster[boardToPrint->playerFlag].treasury)
				{
					grayBoxSprite.setPosition(factoryButtons.at(i).mySprite.getPosition());
					inputLayerWindow->draw(grayBoxSprite);
				}


				//Also, if not on ban list, see if mouse is hovering over this button, and if so, add that string to the status print.
				bool withinButton = factoryButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);

				if (withinButton == true)
				{
					snprintf(hoveredMinionName, 100, "%s: costs %d", &(factoryButtons.at(i).myName[0]), minionPrice);
				}

			}

		}
	}

	if (myTile->symbol == 'P')
	{
		for (int i = 0; i < portButtons.size(); i++)
		{
			//Check if minion type is on ban-list
			//Must NOT be on ban list in order to print

			bool withinFaction = boardToPrint->checkFactionAvailability(portButtons.at(i).myName, boardToPrint->playerFlag);

			if (std::find(boardToPrint->banList.begin(), boardToPrint->banList.end(), portButtons.at(i).myName) == boardToPrint->banList.end() && withinFaction == true)
			{
				inputLayerWindow->draw(portButtons.at(i).mySprite);
				effectsSprite.setPosition(portButtons.at(i).mySprite.getPosition());
				inputLayerWindow->draw(effectsSprite);

				//Check if player can afford each particular minion, and if not, then draw over a gray box effect.
				int minionPrice = boardToPrint->consultMinionCostChart(portButtons.at(i).myName, '~');
				//If it is a real and non-banned minion, BUT is not affordable, print gray box effect.
				if (minionPrice > 0 && minionPrice > boardToPrint->playerRoster[boardToPrint->playerFlag].treasury)
				{
					grayBoxSprite.setPosition(portButtons.at(i).mySprite.getPosition());
					inputLayerWindow->draw(grayBoxSprite);
				}



				//Also, if not on ban list, see if mouse is hovering over this button, and if so, add that string to the status print.

				bool withinButton = portButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);

				if (withinButton == true)
				{

					snprintf(hoveredMinionName, 100, "%s: costs %d", &(portButtons.at(i).myName[0]), minionPrice);
				}

			}
		}
	}

	if (myTile->symbol == 'A')
	{
		for (int i = 0; i < airbaseButtons.size(); i++)
		{
			//Check if minion type is on ban-list
			//Must NOT be on ban list in order to print

			bool withinFaction = boardToPrint->checkFactionAvailability(airbaseButtons.at(i).myName, boardToPrint->playerFlag);

			if (std::find(boardToPrint->banList.begin(), boardToPrint->banList.end(), airbaseButtons.at(i).myName) == boardToPrint->banList.end() && withinFaction == true)
			{
				inputLayerWindow->draw(airbaseButtons.at(i).mySprite);
				effectsSprite.setPosition(airbaseButtons.at(i).mySprite.getPosition());
				inputLayerWindow->draw(effectsSprite);

				//Check if player can afford each particular minion, and if not, then draw over a gray box effect.
				int minionPrice = boardToPrint->consultMinionCostChart(airbaseButtons.at(i).myName, '~');
				//If it is a real and non-banned minion, BUT is not affordable, print gray box effect.
				if (minionPrice > 0 && minionPrice > boardToPrint->playerRoster[boardToPrint->playerFlag].treasury)
				{
					grayBoxSprite.setPosition(airbaseButtons.at(i).mySprite.getPosition());
					inputLayerWindow->draw(grayBoxSprite);
				}



				//Also, if not on ban list, see if mouse is hovering over this button, and if so, add that string to the status print.

				bool withinButton = airbaseButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);

				if (withinButton == true)
				{
					snprintf(hoveredMinionName, 100, "%s: costs %d", &(airbaseButtons.at(i).myName[0]), minionPrice);
				}

			}
		}
	}

	//Add final newline at end regardless
	//if(unitToShow != " ") I think this line is useless, since unitToShow is always assigned some value since this is the property menu printer.
	boardMessage += hoveredMinionName;
	boardMessage += "\n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);
	newText.setPosition(MAX_WINDOW_WIDTH * 52 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + IL_HEIGHT_OFFSET);
	newText.setFillColor(sf::Color::Black);
	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

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

		if (printThisButton == true)
			inputLayerWindow->draw((*menuButtons)[i].mySprite);
	}

	return 0;

}

int inputLayer::printLowerScreen(MasterBoard* boardToPrint, int observerNumber) {

	//First draw the background for status area
	sf::Sprite statusBox;
	statusBox.setTexture(MainMenu->otherGameTextures->at(4));
	statusBox.setPosition(MAX_WINDOW_WIDTH * 50 + IL_WIDTH_OFFSET, 0 + IL_HEIGHT_OFFSET);
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

//Use trigraph instead of single letter. Means invoke getString instead of getChar.
//Assumes existance of savedInsertMinionInput within inputLayer.hpp, which is string.

int inputLayer::printInsertMinion(MasterBoard* boardToPrint)
{

	sf::String boardMessage = "Insert a minion by typing its first three letters.\nExit (ESC)    Current input: ";
	boardMessage += savedInsertMinionInput;

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(MAX_WINDOW_WIDTH * 52 + IL_WIDTH_OFFSET, menuLineTracker * MainMenu->menuTextSize + IL_HEIGHT_OFFSET);

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

	//If not playing music already, play music
	if (soundsOn == true && gameMusic[1].getStatus() != sf::SoundSource::Status::Playing)
	{
		gameMusic[1].play();
	}

	sf::String boardMessage;

	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(MainMenu->otherGameTextures->at(9));
	backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
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
	getValidPlayerInput(inputLayerWindow);

	inputLayerWindow->pollEvent(event);

	//Turn off music
	gameMusic[1].stop();


	return 0;

}

int inputLayer::printUpperScreen(MasterBoard* boardToPrint, int observerNumber, bool withinAnimation) {
	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocationY;
	int windowX = boardToPrint->windowLocationX;

	//Print background
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(MainMenu->otherGameTextures->at(12));
	inputLayerWindow->draw(backgroundSprite);

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (int i = windowY; i < (windowY + boardToPrint->WINDOW_HEIGHT); i++)
	{
		for (int j = windowX; j < (windowX + boardToPrint->WINDOW_WIDTH); j++)
		{
			printSingleTile((j - windowX), (i - windowY), j, i, boardToPrint, observerNumber, withinAnimation);
		}

	}

	//After printing out whole board, we may print a dialogbox over that for status.
	if (dialogBoxOpen != -1)
	{
		printStatusDialogBox(boardToPrint);
	}

	return 0;
}

int inputLayer::printWaitingScreen(MasterBoard* boardToPrint)
{
	if (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType == humanPlayer)
	{
		inputLayerWindow->clear();
		
		sf::Sprite backgroundSprite;
		backgroundSprite.setTexture(MainMenu->otherGameTextures->at(8));
		backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
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
		if ((*soundEffects)[minionToMove->myMoveSound].getStatus() == sf::SoundSource::Stopped)
			(*soundEffects)[minionToMove->myMoveSound].play();
	}

	//If player controlled, tile the minion moves through will always be visible.
	if (boardToPrint->playerRoster[boardToPrint->playerFlag].playerType == humanPlayer)
	{
		boardToPrint->Board[locationX][locationY].withinVision[observerNumber] = true;
	}

	//If within vision, change specific tile's animationSprite to match the movement graphics.
	//Also, must not be a stealth type minion if not our turn
	if (boardToPrint->Board[locationX][locationY].withinVision[observerNumber] == true
		&& (minionToMove->team == observerNumber || minionToMove->stealthMode == false))
	{
		boardToPrint->Board[locationX][locationY].animationSprite = &(minionToMove->mySprite);
	}

	//Temporarily make minion invisible, so it disappears from its starting point.
	minionToMove->invisible = true;

	//Use usual print method
	bool withinAnimation = true;
	printScreen(boardToPrint, observerNumber, withinAnimation);


	//Delay after printing;
	sf::Clock timer;

	timer.restart();
	sf::Time elapsedTime;

	while (elapsedTime.asSeconds() < float(0.140 / speedFactor))
	{
		elapsedTime = timer.getElapsedTime();
	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(140 / speedFactor));

	//Reset invisibilty status for minion selected
	minionToMove->invisible = false;

	//Reset animation sprite
	boardToPrint->Board[locationX][locationY].animationSprite = NULL;

	if (soundsOn == true && minionToMove->myMoveSound != aircraftMove)
	{
		(*soundEffects)[minionToMove->myMoveSound].stop();
	}

	return 0;
}

int inputLayer::combatGraphics(MasterBoard* boardToPrint, int observerNumber, tile* tileAttacking, tile* tileBeingAttacked, bool splashGraphicsOn)
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

	//Determine if this is landmine attack, if it is, we are not showing attacker graphics.
	bool landmineAttack = false;
	if (tileAttacking == tileBeingAttacked)
		landmineAttack = true;

	//If within vision, we will watch attackerVisible
	if (tileAttacking->withinVision[observerNumber] == true && landmineAttack == false)
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
			//Delay after printing;
			sf::Clock timer;

			timer.restart();
			sf::Time elapsedTime;

			while (elapsedTime.asSeconds() < float(0.07 / speedFactor))
			{
				elapsedTime = timer.getElapsedTime();
			}
			//std::this_thread::sleep_for(std::chrono::milliseconds(70 / speedFactor));
		}

		//Clean up afterwards if necessary
		if (tileAttacking->animationSprite != NULL)
		{
			delete tileAttacking->animationSprite;
			tileAttacking->animationSprite = NULL;
		}
	}


	//Animations for defending tile(s) has slightly reversed order - it checks withinVision for each individual tile, and so
	//Iterates through the 5 steps before checking, instead of above which is opposite.

	//Create new sprite for animation
	tileBeingAttacked->animationSprite = new sf::Sprite;
	//Set texture
	tileBeingAttacked->animationSprite->setTexture(*inputLayerTexture);

	if (splashGraphicsOn == true)
	{
		//Create sprite and assign texture for any adjacent legal tile, if splash is on
		if (tileBeingAttacked->locationX > 0)
		{
			boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite = new sf::Sprite;
			boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite->setTexture(*inputLayerTexture);
		}
		if (tileBeingAttacked->locationY > 0)
		{
			boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite = new sf::Sprite;
			boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite->setTexture(*inputLayerTexture);
		}
		if (tileBeingAttacked->locationX < boardToPrint->BOARD_WIDTH - 1)
		{
			boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite = new sf::Sprite;
			boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite->setTexture(*inputLayerTexture);
		}
		if (tileBeingAttacked->locationY < boardToPrint->BOARD_HEIGHT - 1)
		{
			boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite = new sf::Sprite;
			boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite->setTexture(*inputLayerTexture);
		}

	}

	//Now do animation sequence. Custom for combat.
	for (int i = 0; i < 5; i++)
	{
		//If within vision, we will watch tile being attacked
		if (tileBeingAttacked->withinVision[observerNumber] == true)
		{
			//Set center tile
			tileBeingAttacked->animationSprite->setTextureRect(rectArray[i][14]);
		}

		//Potentially set adjacent tiles sprites for explosions
		//They must be legal squares and also within vision.
		//You may see part of a splash damage if some tiles not visible.
		if (splashGraphicsOn == true)
		{
			//For each adjacent square, check if it has a minion on it, and deal damage to that minion, regardless of team.
			if (tileBeingAttacked->locationX > 0 && boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].withinVision[observerNumber] == true)
				boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite->setTextureRect(rectArray[i][14]);

			if (tileBeingAttacked->locationY > 0 && boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].withinVision[observerNumber] == true)
				boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite->setTextureRect(rectArray[i][14]);

			if (tileBeingAttacked->locationX < boardToPrint->BOARD_WIDTH - 1 && boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].withinVision[observerNumber] == true)
				boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite->setTextureRect(rectArray[i][14]);

			if (tileBeingAttacked->locationY < boardToPrint->BOARD_HEIGHT - 1 && boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].withinVision[observerNumber] == true)
				boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite->setTextureRect(rectArray[i][14]);
		}

		bool withinAnimation = true;
		printScreen(boardToPrint, observerNumber, withinAnimation);

		//Delay after printing;
		sf::Clock timer;
		timer.restart();
		sf::Time elapsedTime;
		while (elapsedTime.asSeconds() < float(0.07 / speedFactor))
		{
			elapsedTime = timer.getElapsedTime();
		}
	}

	//Clean up afterwards if necessary
	if (tileBeingAttacked->animationSprite != NULL)
	{
		delete tileBeingAttacked->animationSprite;
		tileBeingAttacked->animationSprite = NULL;
	}

	//Clean up splash tiles as necessary
	if (tileBeingAttacked->locationX > 0 && boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite != NULL)
	{
		delete boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite;
		boardToPrint->Board[tileBeingAttacked->locationX - 1][tileBeingAttacked->locationY].animationSprite = NULL;
	}
	if (tileBeingAttacked->locationY > 0 && boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite != NULL)
	{
		delete boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite;
		boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY - 1].animationSprite = NULL;
	}
	if (tileBeingAttacked->locationX < boardToPrint->BOARD_WIDTH - 1 && boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite != NULL)
	{
		delete boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite;
		boardToPrint->Board[tileBeingAttacked->locationX + 1][tileBeingAttacked->locationY].animationSprite = NULL;
	}
	if (tileBeingAttacked->locationY < boardToPrint->BOARD_HEIGHT - 1 && boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite != NULL)
	{
		delete boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite;
		boardToPrint->Board[tileBeingAttacked->locationX][tileBeingAttacked->locationY + 1].animationSprite = NULL;
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

			//Delay after printing;
			sf::Clock timer;

			timer.restart();
			sf::Time elapsedTime;

			while (elapsedTime.asSeconds() < float(0.180 / speedFactor))
			{
				elapsedTime = timer.getElapsedTime();
			}
			//	std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5 + minionToCapture->team][14]);
			printScreen(boardToPrint, observerNumber, withinAnimation);

			//Delay after printing;
			timer.restart();
			sf::Time secondElapsedTime;

			while (secondElapsedTime.asSeconds() < float(0.140 / speedFactor))
			{
				secondElapsedTime = timer.getElapsedTime();
			}
			//	std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

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

			//Delay after printing;
			sf::Clock timer;

			timer.restart();
			sf::Time elapsedTime;

			while (elapsedTime.asSeconds() < float(0.180 / speedFactor))
			{
				elapsedTime = timer.getElapsedTime();
			}
			//std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);

			timer.restart();
			sf::Time anotherElapsedTime;

			while (anotherElapsedTime.asSeconds() < float(0.180 / speedFactor))
			{
				anotherElapsedTime = timer.getElapsedTime();
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

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

		//Delay after printing;
		sf::Clock timer;

		timer.restart();
		sf::Time elapsedTime;

		while (elapsedTime.asSeconds() < float(0.350 / speedFactor))
		{
			elapsedTime = timer.getElapsedTime();
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(350 / speedFactor));

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
			//Delay after printing;
			sf::Clock timer;

			timer.restart();
			sf::Time elapsedTime;

			while (elapsedTime.asSeconds() < float(0.180 / speedFactor))
			{
				elapsedTime = timer.getElapsedTime();
			}
			//std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);

			//Delay
			timer.restart();
			elapsedTime = timer.getElapsedTime();;

			while (elapsedTime.asSeconds() < float(0.180 / speedFactor))
			{
				elapsedTime = timer.getElapsedTime();
			}
			//	std::this_thread::sleep_for(std::chrono::milliseconds(180 / speedFactor));

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


	//If not playing music already, play music for that player's faction
	if (soundsOn == true)
	{
		int factionMusicOffset = int(boardToPrint->playerRoster[boardToPrint->playerFlag].playerFaction) + 2; //Number of songs before faction themes

		//Crude method to ensure we stopped playing previous faction music if it was different
		for (int i = 3; i < 7; i++)
		{
			if (factionMusicOffset != i)
				gameMusic[i].stop();
		}

		//Then play the actual appropriate music for this faction
		if (gameMusic[factionMusicOffset].getStatus() != sf::SoundSource::Status::Playing)
			gameMusic[factionMusicOffset].play();

	}
	//If sound is off, stop whatever music was playing.
	else if (soundsOn == false)
	{
		for (unsigned int i = 0; i < 7; i++)
		{
			gameMusic[i].stop();
		}
	}

	//Reset line tracker after each print.
	menuLineTracker = 1;

	return 0;
}

int inputLayer::waitingScreenInput(MasterBoard* boardToInput)
{
	getValidPlayerInput(inputLayerWindow);

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

int inputLayer::insertMinionInput(sf::Event* Input, MasterBoard* boardToInput)
{
	//Return to gameBoard if player presses "ESC"".
	//Clear out saved input.
	if (Input->key.code == sf::Keyboard::Key::Escape)
	{
		status = gameBoard;
		savedInsertMinionInput.clear();
		return 1;
	}

	//This clear may need to happen at the end of the function.
	//If at three or more chars in saved string, clear it to make way for the new input.
	if (savedInsertMinionInput.getSize() >= 3 || savedInsertMinionInput == "x")
		savedInsertMinionInput.clear();

	if (Input->type == sf::Event::EventType::TextEntered)
		savedInsertMinionInput += Input->text.unicode;
	else return 1;

	//Convert valid keyboard input to char
	std::string convertedInput = "~";

	//Standard minions
	if (savedInsertMinionInput == "inf" || savedInsertMinionInput == "Inf")
	{
		convertedInput = "Infantry";
	}
	else
		if (savedInsertMinionInput == "spe" || savedInsertMinionInput == "Spe")
		{
			convertedInput = "Specialist";
		}
		else
			if (savedInsertMinionInput == "arm" || savedInsertMinionInput == "Arm")
			{
				convertedInput = "Armor";
			}
			else
				if (savedInsertMinionInput == "hea" || savedInsertMinionInput == "Hea")
				{
					convertedInput = "Heavy_Armor";
				}
				else
					if (savedInsertMinionInput == "art" || savedInsertMinionInput == "Art")
					{
						convertedInput = "Artillery";
					}
					else
						if (savedInsertMinionInput == "rec" || savedInsertMinionInput == "Rec")
						{
							convertedInput = "Recon";
						}
						else
							if (savedInsertMinionInput == "Emp" || savedInsertMinionInput == "emp")
							{
								convertedInput = "Artillery_Emplacement";
							}
							else
								if (savedInsertMinionInput == "att" || savedInsertMinionInput == "Att")
								{
									convertedInput = "Attack_Copter";
								}
								else
									if (savedInsertMinionInput == "tra" || savedInsertMinionInput == "Tra")
									{
										convertedInput = "Transport_Copter";
									}
									else
										if (savedInsertMinionInput == "APC" || savedInsertMinionInput == "apc")
										{
											convertedInput = "APC";
										}
										else
											if (savedInsertMinionInput == "int" || savedInsertMinionInput == "Int")
											{
												convertedInput = "Interceptor";
											}
											else
												if (savedInsertMinionInput == "bom" || savedInsertMinionInput == "Bom")
												{
													convertedInput = "Bomber";
												}
												else
													if (savedInsertMinionInput == "Gun" || savedInsertMinionInput == "gun")
													{
														convertedInput = "Gunboat";
													}
													else
														if (savedInsertMinionInput == "lan" || savedInsertMinionInput == "Lan")
														{
															convertedInput = "Lander";
														}
														else
															if (savedInsertMinionInput == "sub" || savedInsertMinionInput == "Sub")
															{
																convertedInput = "Submarine";
															}
															else
																if (savedInsertMinionInput == "ant" || savedInsertMinionInput == "Ant")
																{
																	convertedInput = "Anti-Aircraft";
																}
																else
																	if (savedInsertMinionInput == "air" || savedInsertMinionInput == "Air")
																	{
																		convertedInput = "Aircraft_Carrier";
																	}
																	else
																		if (savedInsertMinionInput == "roc" || savedInsertMinionInput == "Roc")
																		{
																			convertedInput = "Rocket_Artillery";
																		}
																		else
																			if (savedInsertMinionInput == "bat" || savedInsertMinionInput == "Bat")
																			{
																				convertedInput = "Battleship";
																			}
																			else
																				if (savedInsertMinionInput == "cru" || savedInsertMinionInput == "Cru")
																				{
																					convertedInput = "Cruiser";
																				}
																				else
																					if (savedInsertMinionInput == "SAM" || savedInsertMinionInput == "sam")
																					{
																						convertedInput = "SAM_Site";
																					}
																					else
																						if (savedInsertMinionInput == "min" || savedInsertMinionInput == "Min")
																						{
																							convertedInput = "Landmine";
																						}

	//Faction-specific minions
																						else    //ARNR
																							if (savedInsertMinionInput == "upg" || savedInsertMinionInput == "Upg")
																							{
																								convertedInput = "Upgunned_Armor";
																							}
																							else
																								if (savedInsertMinionInput == "ass" || savedInsertMinionInput == "Ass")
																								{
																									convertedInput = "Assault_Gun";
																								}
																								else
																									if (savedInsertMinionInput == "mul" || savedInsertMinionInput == "Mul")
																									{
																										convertedInput = "Multirole";
																									}
																									else    //South Redonia
																										if (savedInsertMinionInput == "mod" || savedInsertMinionInput == "Mod")
																										{
																											convertedInput = "Modern_Armor";
																										}
																										else
																											if (savedInsertMinionInput == "adv" || savedInsertMinionInput == "Adv")
																											{
																												convertedInput = "Advanced_Fighter";
																											}
																											else
																												if (savedInsertMinionInput == "ifv" || savedInsertMinionInput == "IFV")
																												{
																													convertedInput = "IFV";
																												}
																												else    //Ormosa
																													if (savedInsertMinionInput == "ins" || savedInsertMinionInput == "Ins")
																													{
																														convertedInput = "Insurgent";
																													}
																													else
																														if (savedInsertMinionInput == "ope" || savedInsertMinionInput == "Ope")
																														{
																															convertedInput = "Operative";
																														}
																														else
																															if (savedInsertMinionInput == "Tec" || savedInsertMinionInput == "tec")
																															{
																																convertedInput = "Technical";
																															}
																															else    //Torran
																																if (savedInsertMinionInput == "sup" || savedInsertMinionInput == "Sup")
																																{
																																	convertedInput = "Super_Heavy_Armor";
																																}
																																else
																																	if (savedInsertMinionInput == "vic" || savedInsertMinionInput == "Vic")
																																	{
																																		convertedInput = "Victory_Launcher";
																																	}
																																	else
																																		if (savedInsertMinionInput == "cav" || savedInsertMinionInput == "Cav")
																																		{
																																			convertedInput = "Cavalry";
																																		}


	Cursor* myCursor = &boardToInput->cursor;
	tile* myTile = &boardToInput->Board[myCursor->XCoord][myCursor->YCoord];

	//Prevent minion insertion on top of another, and prevent insertion somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true || myTile->consultMovementChart(convertedInput) == 99)
	{
		savedInsertMinionInput.clear();
		return 1;
	}

	int requestedUnitPrice = boardToInput->consultMinionCostChart(convertedInput, '~');

	//If it is real minion, then price > 0
	if (requestedUnitPrice > 0)
	{
		boardToInput->createMinion(convertedInput, myCursor->getX(), myCursor->getY(), boardToInput->playerFlag, 100, 0, 0, 0, -1, -1, -1);
		status = gameBoard;
		savedInsertMinionInput.clear();
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
	if (myTile->consultMovementChart("Infantry") == -1)
		return 1;

	//Prevent terrain from being somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true && myTile->consultMovementChart(myTile->minionOnTop->type) == 99)
		return 1;

	//If it is real tile, change the underlying tile.
	myTile->symbol = inputChar;
	myTile->capturePoints = 20;

	if (myTile->checkForProperty() == true)
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
	//If we had a dialog box open, one click/key closes it, and no other action is taken.
	if (dialogBoxOpen != -1)
	{
		getValidPlayerInput(inputLayerWindow);
		dialogBoxOpen = -1;
		return 0;
	}

	//Quote indicates mouse click
	if (*Input == sf::Keyboard::Quote)
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));

		//If mouse click is within map, 
		int windowX = (mousePosition.x - IL_WIDTH_OFFSET) / 50;
		int windowY = (mousePosition.y - IL_HEIGHT_OFFSET) / 50;
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
		else	//Check if mouse clicked within status buttons:
		{
			//If within a status button, assign dialogBoxOpen that value:
			for (int i = 0; i < statusButtons.size(); i++)
			{
				if ((statusButtons)[i].checkWithinButton(mousePosition.x, mousePosition.y) == true)
				{
					if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
					{
						dialogBoxOpen = i;
					}
				}
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

	//Add'l advance to next game via loadNextGame for inputLayer:
	if (*Input == sf::Keyboard::Key::V && MainMenu->editorMode == true)
	{
		NextMission(boardToInput);
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

	if (*Input == sf::Keyboard::Key::Num9)
	{
		if (showRangeStatus == showRange)
		{
			showRangeStatus = hideRange;
		}
		else if (showRangeStatus == hideRange)
		{
			showRangeStatus = showRange;
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

	if (*Input == sf::Keyboard::Key::M || *Input == sf::Keyboard::Key::Comma)
	{
		status = menu;
	}

	return 0;
}

int inputLayer::minionInput(sf::Keyboard::Key* Input, MasterBoard* boardToInput) {

	//If we had a dialog box open, one click/key closes it, and no other action is taken.
	if (dialogBoxOpen != -1)
	{
		getValidPlayerInput(inputLayerWindow);
		dialogBoxOpen = -1;
		return 0;
	}

	//This tracks who may lose after an action. Only one player can lose per action, so only need one number.
	int playerPotentiallyDefeated = 0;

	sf::Vector2i mousePosition = sf::Mouse::getPosition(*(inputLayerWindow));

	//Get mouse click information
	int windowX = (mousePosition.x -IL_WIDTH_OFFSET) / 50 ;
	int windowY = (mousePosition.y - IL_HEIGHT_OFFSET) / 50 ;
	int tileX = windowX + boardToInput->windowLocationX;
	int tileY = windowY + boardToInput->windowLocationY;

	//If right click occurred, may move, may not move.
	//This is to avoid accidentally moving in place after selecting a minion.
	//Requires left, then right click, to move in place.
	if (*Input == sf::Keyboard::Comma)
	{

		//If mouse click is within map, 
		if (tileX < boardToInput->BOARD_WIDTH && tileX >= 0 && tileY < boardToInput->BOARD_HEIGHT && tileY >= 0)
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
					if (boardToInput->cursor.XCoord == myMinion->locationX &&
						boardToInput->cursor.YCoord == myMinion->locationY)
					{
						//If minion is capture-capable that has already moved, attempt to capture
						if (myMinion->captureCapable == true &&
							(myMinion->status == hasmovedhasntfired || myMinion->status == gaveupmovehasntfired))
							* Input = sf::Keyboard::Key::C;
						else
							//If minion is transport that already moved, attempt to supply if there is friendly minion there.
							if ((myMinion->specialtyGroup == smallTransport || myMinion->specialtyGroup == largeTransport || myMinion->specialtyGroup == aircraftCarrier) &&
								(myMinion->status == hasmovedhasntfired || myMinion->status == gaveupmovehasntfired))
							{
								*Input = sf::Keyboard::Key::I;
							}
							else if (myMinion->type == "Submarine")
							{
								*Input = sf::Keyboard::Key::U;	//If submarine, either dive or surface.
							}
							else
							{
								*Input = sf::Keyboard::Key::M;	//Otherwise attempt to move there.
							}
					}
					else //If we are direct/hybrid attack, and have already moved, attempt to attack there. Must be in range and visible.
						 //Or if we are indirect/hybrid attack that held position, attempt to attack there. Must be in range and visible.
						if (targetedTile->hasMinionOnTop == true && targetedTile->minionOnTop->team != boardToInput->playerFlag &&
							targetedTile->withinRange == true && targetedTile->withinVision[myMinion->team] == true &&
							(((myMinion->rangeType == directFire || myMinion->rangeType == hybridRange) &&
							(myMinion->status == hasmovedhasntfired || myMinion->status == gaveupmovehasntfired))
								|| ((myMinion->rangeType == rangedFire || myMinion->rangeType == hybridRange) &&
								(myMinion->status == gaveupmovehasntfired)))
							)
						{
							*Input = sf::Keyboard::Key::R;
						}
					//If empty space and this is a transport that already moved, attempt to drop there.
						else	if ((boardToInput->cursor.selectMinionPointer->specialtyGroup == smallTransport || boardToInput->cursor.selectMinionPointer->specialtyGroup == largeTransport || boardToInput->cursor.selectMinionPointer->specialtyGroup == aircraftCarrier) &&
							(boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired ||
								boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired))
						{
							*Input = sf::Keyboard::Key::O;
						}
						else  //If this is empty space and is operative that already moved/gave up move
							if (boardToInput->cursor.selectMinionPointer->type == "Operative" &&
								(boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired ||
									boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired))
							{
								*Input = sf::Keyboard::Key::B;      //B for build. Either operative dropping a landmine, or engineer building something.
							}
							else  //Otherwise attempt to move there.
							{
								*Input = sf::Keyboard::Key::M;
							}
				}
		}
	}
	else
		//If left click, deselect.	
		//Must also be in map.
		if (*Input == sf::Keyboard::Quote)
		{
			if (tileX < boardToInput->BOARD_WIDTH && tileX >= 0 && tileY < boardToInput->BOARD_HEIGHT && tileY >= 0)
				* Input = sf::Keyboard::Key::T;
			else
				//If within a status button, assign dialogBoxOpen that value:
				for (int i = 0; i < statusButtons.size(); i++)
				{
					if ((statusButtons)[i].checkWithinButton(mousePosition.x, mousePosition.y) == true)
					{
						if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
						{
							dialogBoxOpen = i;
						}
					}
				}
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

	if (*Input == sf::Keyboard::Key::Num9)
	{
		if (showRangeStatus == showRange)
		{
			showRangeStatus = hideRange;
		}
		else if (showRangeStatus == hideRange)
		{
			showRangeStatus = showRange;
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

	int cursorX = boardToInput->cursor.getX();
	int cursorY = boardToInput->cursor.getY();

	//See if any adjacent friendly minions to this square
	bool friendlyAdjacentMinion = boardToInput->hasAdjacentMinion(cursorX, cursorY, boardToInput->playerFlag);

	//See if enemy minion here.
	bool stealthEnemyHere = false;
	if (boardToInput->Board[cursorX][cursorY].hasMinionOnTop == true && boardToInput->Board[cursorX][cursorY].minionOnTop->team != boardToInput->playerFlag
		&& boardToInput->Board[cursorX][cursorY].minionOnTop->stealthMode == true)
		stealthEnemyHere = true;

	bool unseenStealthEnemyHere = false;
	if (stealthEnemyHere == true && friendlyAdjacentMinion == false)
		unseenStealthEnemyHere = true;

	//Move minion command
	//If minion selected and hasn't moved or fired, attempt to move.
	//The moveMinion function will check if we are on top of ourselves or another minion.
	if (*Input == sf::Keyboard::Key::M && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->status == hasntmovedorfired)
	{

		//If there is a minion on top and that minion is visible to the minion's owner
		if (boardToInput->Board[cursorX][cursorY].hasMinionOnTop == true
			&& boardToInput->Board[cursorX][cursorY].withinVision[boardToInput->cursor.selectMinionPointer->team] == true
			&& unseenStealthEnemyHere == false)
		{
			//If minion on top is this minion, do hold position movement.
			if (boardToInput->cursor.selectMinionPointer->locationX == cursorX && boardToInput->cursor.selectMinionPointer->locationY == cursorY)
			{
				boardToInput->moveMinion(cursorX, cursorY, this, boardToInput->playerFlag);
				status = gameBoard;
			}
			else
				//If that minion is a ground transport and is same team
				//OR If that minion is aircraft carrier and same team
				if (((boardToInput->Board[cursorX][cursorY].minionOnTop->specialtyGroup == smallTransport && boardToInput->cursor.selectMinionPointer->domain == land) ||
					(boardToInput->Board[cursorX][cursorY].minionOnTop->specialtyGroup == largeTransport && boardToInput->cursor.selectMinionPointer->domain == land) ||
					(boardToInput->Board[cursorX][cursorY].minionOnTop->specialtyGroup == aircraftCarrier && boardToInput->cursor.selectMinionPointer->domain == air))
					&& boardToInput->Board[cursorX][cursorY].minionOnTop->team == boardToInput->cursor.selectMinionPointer->team)
				{
					status = gameBoard;	//Set status to gameboard to make it through pickupMinion
										//If we do not set status to gameboard, it will segfault when it prints minion status during move, during validatePath.
					int success = boardToInput->pickUpMinion(cursorX, cursorY, this, boardToInput->playerFlag);

					//If failed to actually move at all (invalid move attempt), set back to minion action, since we didn't deselect within pickupMinion.
					if (success == 1)
						status = minionAction;
				}
		}
		else //To attempt to move on tile, must either not have a stealther, or at least doesn't have someone adjacent to see them
			if (stealthEnemyHere == false || friendlyAdjacentMinion == false)
			{
				if (boardToInput->moveMinion(cursorX, cursorY, this, boardToInput->playerFlag) == 0)
				{
					//Change status appropriately for successful movement.
					status = gameBoard;
				}
			}

	}

	//'i' is supply
	//Must have minion selected.
	//Must be APC/AC Carrier, hasn't taken second action, cursor is on minion, and regardless of transport status.
	if (*Input == sf::Keyboard::Key::I && boardToInput->cursor.selectMinionFlag == true
		&& (boardToInput->cursor.selectMinionPointer->type == "APC" || boardToInput->cursor.selectMinionPointer->type == "Aircraft_Carrier"
			&& (boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired || boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
			&& cursorX == boardToInput->cursor.selectMinionPointer->locationX
			&& cursorY == boardToInput->cursor.selectMinionPointer->locationY) )
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
		&& (boardToInput->cursor.selectMinionPointer->specialtyGroup == smallTransport || boardToInput->cursor.selectMinionPointer->specialtyGroup == largeTransport || boardToInput->cursor.selectMinionPointer->specialtyGroup == aircraftCarrier)
		&& ((boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired || boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
			&& boardToInput->cursor.selectMinionPointer->firstMinionBeingTransported != NULL)
		&& boardToInput->Board[cursorX][cursorY].hasMinionOnTop == false
		&& boardToInput->Board[cursorX][cursorY].withinRange == true
		&& boardToInput->Board[cursorX][cursorY].consultMovementChart(boardToInput->cursor.selectMinionPointer->firstMinionBeingTransported->type) != 99)
	{
		if (boardToInput->dropOffMinion() == 0)
			status = gameBoard;
	}

	//Must be operative, must have moved already.
	if (*Input == sf::Keyboard::Key::B && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->type == "Operative"
		&& (boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired || boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired))
	{
		if (boardToInput->deployLandmine(this, boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{
			status = gameBoard;
		}
	}

	//'U' - If submarine, attempt to surface or dive
	//Cursor must be on top of selected minion.
	if (*Input == sf::Keyboard::Key::U && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->type == "Submarine"
		&& cursorX == boardToInput->cursor.selectMinionPointer->locationX
		&& cursorY == boardToInput->cursor.selectMinionPointer->locationY)
	{
		boardToInput->diveOrSurfaceSub();
		status = gameBoard;
	}

	bool lastMinionDestroyed = false;
	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours. Must also not be transport type.
	//Also, must not be stealth if we have no one adjacent.


	if (*Input == sf::Keyboard::Key::R && boardToInput->cursor.selectMinionFlag == true)
		if (boardToInput->Board[cursorX][cursorY].hasMinionOnTop == true)
			if ((cursorX != boardToInput->cursor.selectMinionPointer->locationX) || (cursorY != boardToInput->cursor.selectMinionPointer->locationY))//Can attack if minion is selected
				if (boardToInput->Board[cursorX][cursorY].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team)//And it's enemy team's.
					if (boardToInput->Board[cursorX][cursorY].withinRange == true)	//In range
						if (stealthEnemyHere == false || friendlyAdjacentMinion == true)	//Either not a stealther, or we can see them with adjacent minion
						{
							//This is the actual attack portion. Return of 0 indicates successful attack.
							//Note minion's owner so if they lose we know who lost.
							playerPotentiallyDefeated = boardToInput->Board[cursorX][cursorY].minionOnTop->team;
							bool attackSuccess = boardToInput->attackMinion(cursorX, cursorY, this, boardToInput->playerFlag);
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
			&& boardToInput->cursor.selectMinionPointer->captureCapable == true)
		{
			tile* tileToCheck = &boardToInput->Board[boardToInput->cursor.selectMinionPointer->locationX][boardToInput->cursor.selectMinionPointer->locationY];
			playerPotentiallyDefeated = tileToCheck->controller;
			//Must be property and must not be the current player's property (Could be neutral).
			if (tileToCheck->checkForProperty() && tileToCheck->controller != boardToInput->playerFlag)
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
	sf::String boardMessage;

	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(MainMenu->otherGameTextures->at(10));
	backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
	inputLayerWindow->draw(backgroundSprite);


	sf::String defeatMessage = boardToPrint->playerRoster[playerDefeated].name;
	defeatMessage += " was defeated. Press any key to continue.  \n";

	sf::Text defeatText(defeatMessage, *inputLayerFont, MainMenu->menuTextSize + 5);
	defeatText.setPosition(380, 100);
	defeatText.setFillColor(sf::Color::Black);
	MainMenu->mywindow->draw(defeatText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	getValidPlayerInput(inputLayerWindow);


	return 0;
}

int inputLayer::printPlayerVictory(int playerVictorious, MasterBoard* boardToPrint)
{
	inputLayerWindow->clear();

	//If not playing music already, play music
	if (soundsOn == true && gameMusic[2].getStatus() != sf::SoundSource::Status::Playing)
	{
		gameMusic[2].play();
	}

	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(MainMenu->otherGameTextures->at(10));
	backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
	inputLayerWindow->draw(backgroundSprite);

	sf::String victoryMessage = boardToPrint->playerRoster[playerVictorious].name;
	victoryMessage += " was victorious! Press any key to continue.  \n";

	sf::Text victoryText(victoryMessage, *inputLayerFont, MainMenu->menuTextSize + 5);
	victoryText.setPosition(380, 100);
	victoryText.setFillColor(sf::Color::Black);
	MainMenu->mywindow->draw(victoryText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	getValidPlayerInput(inputLayerWindow);

	gameMusic[2].stop();

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
			sf::String saveGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, savePrompt, lineOffset, "save");

			std::string stdSaveGameName = ".\\savegames\\";
			stdSaveGameName += saveGameName;
			stdSaveGameName += "_save.txt";

			MainMenu->gameSave(stdSaveGameName, boardToInput);

			inputLayerWindow->clear();

			sf::Sprite backgroundSprite;
			backgroundSprite.setTexture(MainMenu->otherGameTextures->at(7));
			backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
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

			getValidPlayerInput(inputLayerWindow);

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
				sf::String loadGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, loadPrompt, lineOffset, "load");
				
				std::string stdloadGameName = loadGameName;
				loadGameSave.open(".\\savegames\\" + stdloadGameName + "_save.txt");
				if (loadGameSave.is_open())
				{
					inputLayerWindow->clear();
					sf::String successful = "Successfully loaded! Press any key to continue.\n";

					sf::Sprite backgroundSprite;
					backgroundSprite.setTexture(MainMenu->otherGameTextures->at(5));
					backgroundSprite.setPosition(IL_WIDTH_OFFSET, IL_HEIGHT_OFFSET);
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
			getValidPlayerInput(inputLayerWindow);

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
	std::string requestedPurchase = "NOBODY";
	bool purchaseSuccess = false;
	int treasury = boardToInput->playerRoster[boardToInput->playerFlag].treasury;

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
		return 0;
	}

	if (*Input == sf::Keyboard::Key::Num9)
	{
		if (showRangeStatus == showRange)
		{
			showRangeStatus = hideRange;
		}
		else if (showRangeStatus == hideRange)
		{
			showRangeStatus = showRange;
		}
	}

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
					requestedPurchase = factoryButtons.at(i).myName;
				}
			}

		}

		if (myTile->symbol == 'P')
		{
			for (int i = 0; i < portButtons.size(); i++)
			{
				bool withinButton = portButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);
				//Prompt user and save game.
				if (withinButton == true)
				{
					requestedPurchase = portButtons.at(i).myName;
				}
			}

		}

		if (myTile->symbol == 'A')
		{
			for (int i = 0; i < airbaseButtons.size(); i++)
			{
				bool withinButton = airbaseButtons.at(i).checkWithinButton(mousePosition.x, mousePosition.y);
				//Prompt user and save game.
				if (withinButton == true)
				{
					requestedPurchase = airbaseButtons.at(i).myName;
				}
			}

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
			int result = boardToInput->attemptPurchaseMinion(requestedPurchase, boardToInput->cursor.getX(), boardToInput->cursor.getY(), boardToInput->playerFlag);

			//Only leave if successfuly bought minion
			if (result == 0)
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


	//Stop whatever music was happening before.
	if (soundsOn == true)
	{

		//Turn off all music playing
		for (int i = 0; i < 7; i++)
		{
			gameMusic[i].stop();
		}

		//Play main menu music again.
		if (gameMusic[0].getStatus() != sf::SoundSource::Playing)
			gameMusic[0].play();

	}

	//If battle lab is on, we need to get back to that instance of playGame()
	if (MainMenu->battleLabOn == false)
	{
		MainMenu->playGame(boardToInput, this);
	}
	else

	return 0;
}
