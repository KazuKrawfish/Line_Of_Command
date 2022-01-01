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



char playCharInput(sf::RenderWindow* myWindow); 

inputLayer::inputLayer(mainMenu* inputMainMenu, sf::RenderWindow* myWindow, sf::Texture* gameTexture, sf::Font* cour, std::vector <sf::Sound>* inputSoundEffects)
{
	inputLayerTexture = gameTexture;
	inputLayerFont = cour;
	inputLayerWindow = myWindow;
	MainMenu = inputMainMenu;
	soundEffects = inputSoundEffects;
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


		if (actualX < boardToPrint->BOARD_WIDTH - 1 &&  boardToPrint->Board[actualX+1][actualY].symbol == '-') 
		{
			riverSprite.setTextureRect(rectArray[16][riverYCoord]);
			inputLayerWindow->draw(riverSprite);
		}
		if (actualY < boardToPrint->BOARD_HEIGHT - 1 && boardToPrint->Board[actualX ][actualY+1].symbol == '-')
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
		if ((boardToPrint->playerFlag == playerNumber || MainMenu->debugMode == true)
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
			if (tileToPrint->withinCursorPath == true  )
			{
				//If this tile is on cursor's path
				effectsSprite.setTextureRect(rectArray[6][2]);
				inputLayerWindow->draw(effectsSprite);
			}
			else
				if (tileToPrint->withinApparentRange == true )
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
		|| MainMenu->debugMode == true)	)
	{
		boardToPrint->cursor.mySprite.setPosition(screenX * 50, screenY * 50);
		inputLayerWindow->draw(boardToPrint->cursor.mySprite);

	}

	Minion* minionToPrint = NULL;
	if(tileToPrint->hasMinionOnTop == true)
		minionToPrint = tileToPrint->minionOnTop;
	
	//Must be visible to see effects!
	if (tileToPrint->hasMinionOnTop == true && minionToPrint != NULL && tileToPrint->withinVision[playerNumber] == true
		&& minionVisibleStatus == showMinions && minionToPrint->invisible == false)
	{

		//Print if transporting or capturing // Specialty moves
		if ( minionToPrint->isCapturing == true)
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
	
		//If minion has only movement and can still attack, grey triangle at top
		if(minionToPrint->status == hasmovedhasntfired &&   minionToPrint->rangeType != rangedFire)
		{
			effectsSprite.setTextureRect(rectArray[5][0]);
			inputLayerWindow->draw(effectsSprite);
		}

		//Minion exists and is below 1/3 fuel
		if ( (minionToPrint->currentFuel == 0 || (double(minionToPrint->maxFuel) / double(minionToPrint->currentFuel)) >= 3))
		{
			effectsSprite.setTextureRect(rectArray[1][3]);
			inputLayerWindow->draw(effectsSprite);
		}		
		
		//Minion exists, has a gun (Max != -1), and it is below 1/3 ammo AND isn't infinite ammo (Max != 0)
		//If either weapon is low on ammo, then print low ammo
		if (( minionToPrint->maxPriAmmo != 0 && minionToPrint->maxPriAmmo != -1 && (minionToPrint->currentPriAmmo == 0 || (double(minionToPrint->maxPriAmmo) / double(minionToPrint->currentPriAmmo)) >= 3)) ||
			( minionToPrint->maxSecAmmo != 0 && minionToPrint->maxSecAmmo != -1 && (minionToPrint->currentSecAmmo == 0 || (double(minionToPrint->maxSecAmmo) / double(minionToPrint->currentSecAmmo)) >= 3)))
		{
			effectsSprite.setTextureRect(rectArray[0][3]);
			inputLayerWindow->draw(effectsSprite);
		}



		if ( minionToPrint->health < 95)
		{
			if(minionToPrint->health < 95 && minionToPrint->health >= 85)
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
	tile* currentTile = &boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	//Need string pointer since addstr giving grief about printing strings, and same with snprintf.
	char pointerToPrint[100];
	char* descriptionPointer = &currentTile->description[0];
	char* playerName = &(boardToPrint->playerRoster[currentTile->controller].name[0]);
	sf::String tileDescription; 

if( observerNumber == boardToPrint->playerFlag)
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

	if (MainMenu->debugMode == true) 
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
		newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, (menuLineTracker * MainMenu->menuTextSize) + 4 );
		inputLayerWindow->draw(newText);
		if (MainMenu->debugMode == true)
			menuLineTracker += 4;
			else menuLineTracker += 2;

	}
	else 
	{
		sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
		newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + 4);
		inputLayerWindow->draw(newText);
		menuLineTracker += 1;
	}

	


	//If tile is undergoing capture, let us know.
	if (currentTile->capturePoints != 20)
	{
		snprintf(pointerToPrint, 100, " Capture Points Left: %d ", currentTile->capturePoints);

		sf::Text newText(tileDescription, *inputLayerFont, MainMenu->menuTextSize);
		newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + 4);
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
		newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + 4);
		inputLayerWindow->draw(newText);
		menuLineTracker += 1;

		if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo > 0)		//Both pri/sec have ammo
		{
			if(MainMenu->debugMode == true	)
				snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/%d\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo, currentMinion->currentSecAmmo);
			else
				snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/%d\n", currentMinion->currentFuel, currentMinion->currentPriAmmo, currentMinion->currentSecAmmo);
		}
		else if (currentMinion->maxPriAmmo > 0 && currentMinion->maxSecAmmo == 0)		//Infinte sec ammo
		{
			if (MainMenu->debugMode == true)
				snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/Infinite\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo);
			else 
				snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/Infinite\n",currentMinion->currentFuel, currentMinion->currentPriAmmo);
		} 
		else if (currentMinion->maxSecAmmo == 0 && currentMinion->maxPriAmmo == -1)	//No pri, infinite sec.
		{
			if (MainMenu->debugMode == true)
				snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): None/Infinite\n", int(currentMinion->health), currentMinion->currentFuel);
			else
				snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): None/Infinite\n", currentMinion->currentFuel);
		}
		else if (currentMinion->maxSecAmmo == -1 && currentMinion->maxPriAmmo > 0)	//Primary with no sec.
		{
			if (MainMenu->debugMode == true)
				snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nAmmo(P/S): %d/None\n", int(currentMinion->health), currentMinion->currentFuel, currentMinion->currentPriAmmo);
			else
				snprintf(pointerToPrint, 100, "Fuel: %d \nAmmo(P/S): %d/None\n", currentMinion->currentFuel, currentMinion->currentPriAmmo);
		}
		else //if (currentMinion->maxSecAmmo == -1 && currentMinion->maxPriAmmo == -1)		//No weapons. Used as catch all for debug.
		{
			if (MainMenu->debugMode == true)
				snprintf(pointerToPrint, 100, "Health: %d | Fuel: %d \nNo weapons\n", int(currentMinion->health), currentMinion->currentFuel);
			else 
				snprintf(pointerToPrint, 100, "Fuel: %d \nNo weapons\n", currentMinion->currentFuel);
		}
		
		sf::String minionStatus = pointerToPrint;
		sf::Text minionStatusText(minionStatus, *inputLayerFont, MainMenu->menuTextSize);
		minionStatusText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker* MainMenu->menuTextSize + 4);
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
		secondMinionStatusText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker* MainMenu->menuTextSize + 8);
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
	playerStatuText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker* MainMenu->menuTextSize + 8);
	inputLayerWindow->draw(playerStatuText);
	menuLineTracker += 3;

	eventText = "";
	
	return 0;
}

int inputLayer::printMinionMenu(MasterBoard* boardToPrint) {

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
	
	if(mystatus == hasfired)
	{
		boardMessage = "No further action possible";
		
	}
	   	
	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize + 2);

	inputLayerWindow->draw(newText);
	
	menuLineTracker += 2;

	return 0;

}

int inputLayer::printBoardMenu(MasterBoard* boardToPrint) {
		
	sf::String boardMessage = "Move cursor (WASD) | Menu (m)\nSelect minion/property (t)\n";
	
	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);
	
	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker*MainMenu->menuTextSize);

	inputLayerWindow->draw(newText);
	
	menuLineTracker += 2;
	
	return 0;
}

int	inputLayer::printPropertyMenu(MasterBoard* boardToPrint) {
	
	tile* myTile = &boardToPrint->Board[boardToPrint->cursor.XCoord][boardToPrint->cursor.YCoord];



	//First determine size of property menu we are using.
	int propertyMenuSize = 0;
	if (myTile->symbol == 'h')
		propertyMenuSize = factoryOptions.size();
	if (myTile->symbol == 'P')
		propertyMenuSize = portOptions.size();
	if (myTile->symbol == 'A')
		propertyMenuSize = airbaseOptions.size();


	//MenuCrawler
	std::string boardMessage = "";

	for (int i = 0; i < propertyMenuSize; i++)
	{
		if (menuCursor == i)
			boardMessage += "*";
		else boardMessage += " ";

		if(myTile->symbol == 'P')
			boardMessage += portOptions[i];
		if (myTile->symbol == 'A')
			boardMessage += airbaseOptions[i];
		if (myTile->symbol == 'h')
			boardMessage += factoryOptions[i];

		boardMessage += "\n";
	}
	boardMessage += "Press (p) to deselect property.";

	sf::String sfBoardMessage = boardMessage;
	sf::Text newText(sfBoardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	inputLayerWindow->draw(newText);

	menuLineTracker += propertyMenuSize + 2;

	return 0;
}

int inputLayer::printMenu(MasterBoard* boardToPrint) {

	//MenuCrawler
	std::string boardMessage = "";

	for (int i = 0; i < menuOptions.size(); i++)
	{
		if (menuCursor == i)
			boardMessage += "*";
		else boardMessage += " ";
		boardMessage += menuOptions[i];
		boardMessage += "\n";
	}


	sf::String sfBoardMessage = boardMessage;
	sf::Text newText(sfBoardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	inputLayerWindow->draw(newText);

	menuLineTracker += menuOptions.size()+1;

	return 0;

}

int inputLayer::printLowerScreen(MasterBoard* boardToPrint, int observerNumber) {
	
	if (boardToPrint->playerFlag  == observerNumber)
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

	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printInsertTile(MasterBoard* boardToPrint)
{
	sf::String boardMessage = "Insert a tile by typing its symbol\nExit insert tile menu(x) \n";

	sf::Text newText(boardMessage, *inputLayerFont, MainMenu->menuTextSize);

	newText.setPosition(boardToPrint->WINDOW_WIDTH * 52, menuLineTracker * MainMenu->menuTextSize);

	inputLayerWindow->draw(newText);

	menuLineTracker += 2;

	return 0;

}

int inputLayer::printMissionBriefing(MasterBoard* boardToInput)
{
	inputLayerWindow->clear();
	sf::String boardMessage;

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


	inputLayerWindow->draw(newText);
	inputLayerWindow->display();

	sf::Event event;
	inputLayerWindow->pollEvent(event);

	//Wait for one input.
	playCharInput(inputLayerWindow);

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
			printSingleTile((j - windowX),  (i - windowY), j , i , boardToPrint, observerNumber, withinAnimation);
		}
		
	}

	return 0;
}

int inputLayer::printWaitingScreen(MasterBoard* boardToPrint) 
{	
	
	inputLayerWindow->clear();
	char buffer[100];
	sf::String announceString = boardToPrint->playerRoster[boardToPrint->playerFlag].name;
	announceString += "'s turn. Press any key to begin.  \n";
	sf::Text newText(announceString, *inputLayerFont, 20);
	inputLayerWindow->draw(newText);
	inputLayerWindow->display();
	
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
	(*soundEffects)[minionToMove->myMoveSound].play();


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
		std::this_thread::sleep_for(std::chrono::milliseconds(140));

	//Reset invisibilty status for minion selected
	minionToMove->invisible = false;

	//Reset animation sprite
	boardToPrint->Board[locationX][locationY].animationSprite = NULL;


	(*soundEffects)[minionToMove->myMoveSound].stop();


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
	
	//Provide sound regardless of vision
	(*soundEffects)[tileAttacking->minionOnTop->myAttackSound].play();



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
			std::this_thread::sleep_for(std::chrono::milliseconds(70));
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

			std::this_thread::sleep_for(std::chrono::milliseconds(70));
		}
	
		//Clean up afterwards if necessary
		if (tileBeingAttacked->animationSprite != NULL)
		{
			delete tileBeingAttacked->animationSprite;
			tileBeingAttacked->animationSprite = NULL;
		}

	
	}
	
	(*soundEffects)[tileAttacking->minionOnTop->myAttackSound].stop();

	return 0;
}

int inputLayer::captureGraphics(MasterBoard* boardToPrint, int observerNumber, Minion* minionToCapture, int locationX, int locationY)
{

	//Should be checking for bad locX but will add later
	tile* myTile = & boardToPrint->Board[locationX][locationY];

	//-1 Observer indicates this is compie playing, during non-single player, so we do not print any graphics.
	if (observerNumber == -1)
		return -1;

	if (minionToCapture == NULL )
	{
		std::cout << "Could not animate capture, the tile was NULL" << std::endl;
		return -1;
	}

	(*soundEffects)[capture].play();


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
		std::this_thread::sleep_for(std::chrono::milliseconds(180));

		myTile->animationSprite->setTextureRect(rectArray[5+ minionToCapture->team][14]);
		printScreen(boardToPrint, observerNumber, withinAnimation);
		std::this_thread::sleep_for(std::chrono::milliseconds(180));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	(*soundEffects)[capture].stop();

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

	(*soundEffects)[resupply].play();

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
			std::this_thread::sleep_for(std::chrono::milliseconds(180));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	(*soundEffects)[resupply].stop();

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

	(*soundEffects)[trapped].play();

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
		std::this_thread::sleep_for(std::chrono::milliseconds(350));

		myTile->animationSprite->setTextureRect(rectArray[5][13]);
		printScreen(boardToPrint, observerNumber, withinAnimation);


		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	(*soundEffects)[trapped].stop();



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

	(*soundEffects)[repair].play();

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
			std::this_thread::sleep_for(std::chrono::milliseconds(180));

			myTile->animationSprite->setTextureRect(rectArray[5][13]);
			printScreen(boardToPrint, observerNumber, withinAnimation);
			std::this_thread::sleep_for(std::chrono::milliseconds(180));

		}

		//Clean up afterwards if necessary
		if (myTile->animationSprite != NULL)
		{
			delete myTile->animationSprite;
			myTile->animationSprite = NULL;
		}

	}

	(*soundEffects)[repair].stop();

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
	menuLineTracker = 0;
	
	return 0;
}

int inputLayer::waitingScreenInput(MasterBoard* boardToInput) 
{
	//Only lasts one input.
	status = gameBoard;

	if (MainMenu->debugMode == true)	//If debug, see from 0's perspective.
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
	//Do upkeep here for all players

	return 0;
}

int inputLayer::insertMinionInput(char* Input, MasterBoard* boardToInput)
{
	Cursor * myCursor = & boardToInput->cursor;
	tile* myTile = & boardToInput->Board[myCursor->XCoord][myCursor->YCoord];


	//Return to gameBoard if player presses 'x'.
	if (*Input == 'x' )
	{
		status = gameBoard;
		return 1;
	}

	//Prevent minion insertion on top of another, and prevent insertion somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true 	|| myTile->consultMovementChart(*Input, myTile->symbol) == 99)
		return 1;
												
	int requestedUnitPrice = boardToInput->consultMinionCostChart(*Input, '~');

	//If it is real minion, then price > 0
	if (requestedUnitPrice > 0)
	{
		boardToInput->createMinion(*Input, myCursor->getX(), myCursor->getY(), boardToInput->playerFlag, 100, 0, 0, 0, -1, -1, -1);
		status = gameBoard;
		return 0;
	}

	return 1;

}

//If there is a minion underneath, destroy it regardless of any status.
//Activate with debug mode and "z" input.
int inputLayer::deleteMinionInput( MasterBoard* boardToInput)
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
		boardToInput->destroyMinion(myTile->minionOnTop, false,this, true);
		status = gameBoard;
		return 0;
	}		
		
	

}

int inputLayer::insertTileInput(char* Input, MasterBoard* boardToInput) 
{
	Cursor* myCursor = &boardToInput->cursor;
	tile* myTile = &boardToInput->Board[myCursor->XCoord][myCursor->YCoord];
	
	//Return to gameBoard if player presses 'q'.
	if (*Input == 'q')
	{
		status = gameBoard;
		return 1;
	}

	//If input tile symbol is invalid, return 1.
	if (myTile->consultMovementChart('i', *Input) == -1)
		return 1;

	//Prevent minion insertion on top of another, and prevent insertion somewhere that minion couldn't actually move.
	if (myTile->hasMinionOnTop == true && myTile->consultMovementChart(myTile->minionOnTop->type ,  *Input ) == 99)
		return 1;

	//If it is real tile, change the underlying tile.
	//May break things....
	myTile->capturePoints = 20;
	myTile->symbol = *Input;
	
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

int inputLayer::gameBoardInput(char* Input, MasterBoard* boardToInput)
{
	if (*Input == 'a' || *Input == 'd' || *Input == 's' || *Input == 'w')
	{
		boardToInput->cursor.move(Input);
	}

	if (*Input == 'x' && MainMenu->debugMode == true)
	{
		status = insertMinion;
	}	

	if (*Input == 'z' && MainMenu->debugMode == true)
	{
		deleteMinionInput(boardToInput);
	}
	
	if (*Input == 'q' && MainMenu->debugMode == true)
	{
		status = insertTile;
	}

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
	}

	//Select minion or property.
	if (*Input == 't')
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

	if (*Input == 'm')
	{
		status = menu;
	}

	return 0;
}

int inputLayer::minionInput(char* Input, MasterBoard* boardToInput) {

	//This tracks who may lose after an action. Only one player can lose per action, so only need one number.
	int playerPotentiallyDefeated = 0;

	if (*Input == 'a' || *Input == 'd' || *Input == 's' || *Input == 'w')
	{
		boardToInput->cursor.move(Input);
	}
	
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
	}

	//Deselect
	if (*Input == 't')
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
	if (*Input == 'm' && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->status == hasntmovedorfired)
	{
		if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->minionBeingTransported == NULL	
		&&	boardToInput->pickUpMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{
			//Change status if successful pickup occurred
			status = gameBoard;
		}
		else
		if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this, boardToInput->playerFlag) == 0)
		{	
			//Change status appropriately for successful movement.
			status = gameBoard;
		}
	}

	//'i' is supply
	//Must have minion selected.
	//Must be APC, hasn't taken second action, cursor is on minion, and regardless of transport status.
	if (*Input == 'i' && boardToInput->cursor.selectMinionFlag == true
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
	if (*Input == 'o' && boardToInput->cursor.selectMinionFlag == true
		&& boardToInput->cursor.selectMinionPointer->specialtyGroup == transport
		&&	( (boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired || 
		 boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
		&& boardToInput->cursor.selectMinionPointer->minionBeingTransported != NULL)
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == false
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true
		&& boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].consultMovementChart(boardToInput->cursor.selectMinionPointer->minionBeingTransported->type, boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol) != 99)
	{
		if(	boardToInput->dropOffMinion() == 0)
			status = gameBoard;
	}

	bool lastMinionDestroyed = false;
	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours. Must also not be transport type.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true && boardToInput->cursor.selectMinionPointer->type != transport)
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

	if (*Input == 'c' && boardToInput->cursor.selectMinionFlag == true )
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
	topMenuJoinString	+= " was defeated. Press any key to continue.  \n";
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


int inputLayer::menuInput(char* Input, MasterBoard* boardToInput) {

	//This is a working key.
	if (*Input == 'g' && MainMenu->debugMode == true)
	{
		MainMenu->scrambleMap(boardToInput, this);	//This needs to be cleaned up to deal with minions.
	}

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
	}

	if (*Input == 's') 
	{
		menuCursor++;

		if (menuCursor >= menuOptions.size())
			menuCursor = 0;
	}
	else
	if (*Input == 'w')
	{
		menuCursor --;
		if (menuCursor < 0)
			menuCursor = menuOptions.size() - 1;
	}
	else
	//Ends the turn and passes it to the next player.
	//Autosave every turn.
	if (menuCursor == 0 && *Input == 't')
	{
		menuCursor = 0;

		if (boardToInput->cursor.selectMinionFlag == true)
			boardToInput->deselectMinion();
		int incrementGameTurn = boardToInput->endTurn(this);
		//If we advanced a gameTurn, mainMenu will keep track of it.
		MainMenu->gameTurn += incrementGameTurn;
		//Have to always keep an autosave!
		MainMenu->gameSave(".\\savegames\\Auto_save.txt", boardToInput);
		
		//If multiplayer, push to remote server and queue "waiting"
		//mainmenu's playGame will keep the player waiting.
		if (MainMenu->gameType == remote)
		{
			MainMenu->skipOneInput = true;
			MainMenu->multiplayerPushSaveGame(boardToInput);
			MainMenu->menuStatus = waitingForRemotePlayer;
		}

		//status = gameBoard;

	}
	else
	//Restart current mission/scenario
	if (menuCursor == 4 &&  *Input == 't')
	{
		menuCursor = 0;
		restartGame(boardToInput);
	}
	else
	//Load a game
	if (menuCursor == 3 &&  *Input == 't')
	{	
		//Load the actual save game
		std::ifstream loadGameSave;
		bool loadsuccessful = false;

		//Prompt user and load scenario
		int lineOffset = 1;
		while (loadsuccessful == false)
		{
			inputLayerWindow->clear();
			sf::String loadPrompt = "Choose which save game to load (Case sensitive): \n";
			sf::String loadGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, loadPrompt, lineOffset);
			sf::Event event;

			std::string stdloadGameName = loadGameName;
			loadGameSave.open(".\\savegames\\" + stdloadGameName + "_save.txt");
			if (loadGameSave.is_open())
			{
				inputLayerWindow->clear();
				sf::String successful = "Successfully loaded! Press any key to continue.\n";
				sf::Text newText(successful, *inputLayerFont, MainMenu->menuTextSize);
				inputLayerWindow->draw(newText);
				inputLayerWindow->display();

				loadsuccessful = true;
			}
			else
			{
				inputLayerWindow->clear();
				sf::String loadPrompt = "Could not load scenario.\nPlease check that it exists and the right spelling was used.\n";
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
		menuCursor = 0;
		
	}
	else
	//Prompt user and save game.
	if (menuCursor == 1 &&  *Input == 't')
	{
		int lineOffset = 1;
		inputLayerWindow->clear();
		sf::String savePrompt = "Choose a name to save your game.\n";
		sf::String saveGameName = MainMenu->playerInputString(inputLayerWindow, inputLayerFont, savePrompt, lineOffset);

		std::string stdSaveGameName = ".\\savegames\\";
		stdSaveGameName += saveGameName;
		stdSaveGameName += "_save.txt";

		MainMenu->gameSave(stdSaveGameName, boardToInput);
		
		inputLayerWindow->clear();

		sf::String successSave = "Game saved. Press any key to continue.\n";
		sf::Text newText(successSave, *inputLayerFont, MainMenu->menuTextSize);
		inputLayerWindow->draw(newText);
		inputLayerWindow->display();

		//Flush event queue to clear out "Enter" and other rifraf
		sf::Event throwAwayEvent;
		while (inputLayerWindow->pollEvent(throwAwayEvent));

		playCharInput(inputLayerWindow);
		
		status = gameBoard;
		menuCursor = 0;
	}

	//Exit menu
	if (menuCursor == 5 && *Input == 't')
	{
		status = gameBoard;
		menuCursor = 0;
	}

	//Exit to main menu
	if (menuCursor == 2 &&  *Input == 't')
	{
		MainMenu->gameSave(".\\savegames\\Auto_save.txt", boardToInput);
		menuCursor = 0;
		exitToMainMenu(boardToInput);
		
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

			loadGame.open(".\\campaigns\\" + boardToInput->campaignName +"\\" + gameToLoad + ".txt");
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
	printMissionBriefing(boardToInput );

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
	printMissionBriefing(boardToInput );



	status = gameBoard;
	
	MainMenu->playGame(boardToInput, this);

	return 0;

}

int inputLayer::propertyMenuInput(char* Input, MasterBoard* boardToInput) {

	tile* myTile =  & boardToInput->Board[boardToInput->cursor.XCoord][boardToInput->cursor.YCoord];
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


	//First determine size of property menu we are using.
	int propertyMenuSize = 0;
	if( myTile->symbol == 'h') 
		propertyMenuSize = factoryOptions.size();
	if (myTile->symbol == 'P')
		propertyMenuSize = portOptions.size();
	if (myTile->symbol == 'A')
		propertyMenuSize = airbaseOptions.size();

	//Move cursor up/down
	if (*Input == 's')
	{
		menuCursor++;

		if (menuCursor >= propertyMenuSize)
			menuCursor = 0;
	}
	else
		if (*Input == 'w')
		{
			menuCursor--;
			if (menuCursor < 0)
				menuCursor = propertyMenuSize - 1;
		}
		else
			if (*Input == 'p') 
			{
				menuCursor = 0;
				status = gameBoard;
				return 0;
			}

	//Attempted purchase of minion
	//Factory first
	if (*Input == 't' && myTile->symbol == 'h')
	{


		if (menuCursor == 0)
			requestedPurchase = 'i';
		if (menuCursor == 1)
			requestedPurchase = 's';
		if (menuCursor == 2)
			requestedPurchase = 'c';
		if (menuCursor == 3)
			requestedPurchase = 'P';
		if (menuCursor == 4)
			requestedPurchase = 'r';
		if (menuCursor == 5)
			requestedPurchase = 'a';
		if (menuCursor == 6)
			requestedPurchase = 'A';
		if (menuCursor == 7)
			requestedPurchase = 'R';
		if (menuCursor == 8)
			requestedPurchase = 'T';




	}

	if (*Input == 't' && myTile->symbol == 'P')
	{

		if (menuCursor == 0)
			requestedPurchase = 'G';
		if (menuCursor == 1)
			requestedPurchase = 'C';
		if (menuCursor == 2)
			requestedPurchase = 'L';
		if (menuCursor == 3)
			requestedPurchase = 'U';
		if (menuCursor == 4)
			requestedPurchase = 'B';
		if (menuCursor == 5)
			requestedPurchase = 'V';


	}

	if (*Input == 't' && myTile->symbol == 'A')
	{
		
		if (menuCursor == 0)
			requestedPurchase = 'h';
		if (menuCursor == 1)
			requestedPurchase = 'v';
		if (menuCursor == 2)
			requestedPurchase = 'f';
		if (menuCursor == 3)
			requestedPurchase = 'b';



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
			menuCursor = 0;
		}
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
