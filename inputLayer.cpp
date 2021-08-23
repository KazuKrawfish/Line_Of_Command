#include "Minion.hpp"
#include "MasterBoard.hpp"
#include "inputLayer.hpp"
#include "mainmenu.h"
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <cmath>
#include "compie.hpp"

inputLayer::inputLayer(mainMenu* inputMainMenu) 
{
	MainMenu = inputMainMenu;
}

int inputLayer::printSingleTile(int inputX, int inputY, std::string inputString, int teamNumber, Minion* minionToPrint)
{
	wmove(MainMenu->mywindow, inputX*3, inputY*3);
	waddch(MainMenu->mywindow, inputString[0] + COLOR_PAIR(teamNumber));
	waddch(MainMenu->mywindow, inputString[1] + COLOR_PAIR(teamNumber));
	waddch(MainMenu->mywindow, inputString[2] + COLOR_PAIR(teamNumber));

	wmove(MainMenu->mywindow, inputX * 3 + 1, inputY * 3);
	waddch(MainMenu->mywindow, inputString[3] + COLOR_PAIR(teamNumber));
	waddch(MainMenu->mywindow, inputString[4] + COLOR_PAIR(teamNumber));

	//Print single tile needs access to board, it's not working currently
	if (minionToPrint != NULL && minionToPrint->isCapturing == true)
	{
		waddch(MainMenu->mywindow, 'C' + COLOR_PAIR(teamNumber));
	}
	else if (minionToPrint != NULL && minionToPrint->minionBeingTransported != NULL) 
	{
		waddch(MainMenu->mywindow, 'T' + COLOR_PAIR(teamNumber));
	}
	else waddch(MainMenu->mywindow, inputString[5] + COLOR_PAIR(teamNumber));
	
	//If minion has done all possible moves, black out the bottom row.
	if (minionToPrint != NULL && minionToPrint->status == hasfired)
	{
		teamNumber += 24;
	}

	wmove(MainMenu->mywindow, inputX * 3 + 2, inputY * 3);
	//Potentially add veterancy if Level 1 - 3:
	if (minionToPrint != NULL && minionToPrint->veterancy > 0)
	{
		if (minionToPrint->veterancy == 3) 
		{
			waddch(MainMenu->mywindow, '+' + COLOR_PAIR(teamNumber));
		}
		else if (minionToPrint->veterancy == 2)
		{
			waddch(MainMenu->mywindow, '=' + COLOR_PAIR(teamNumber));
		} else  waddch(MainMenu->mywindow, '-' + COLOR_PAIR(teamNumber));
	}
	else waddch(MainMenu->mywindow, inputString[6] + COLOR_PAIR(teamNumber));
	
	waddch(MainMenu->mywindow, inputString[7] + COLOR_PAIR(teamNumber));

	//If minion is damaged indicate the health level on bottom right, otherwise print symbol
	if (minionToPrint != NULL && minionToPrint->health <= 94)
	{
		waddch(MainMenu->mywindow, char (int(std::round(minionToPrint->health /10)) + 48) + COLOR_PAIR(teamNumber));
	}
	else waddch(MainMenu->mywindow, inputString[8] + COLOR_PAIR(teamNumber));
	
	return 1;
}

int inputLayer::printStatus(MasterBoard* boardToPrint)
{
	tile* currentTile = &boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	//Need string pointer since addstr giving grief about printing strings, and same with snprintf.
	char pointerToPrint[100];
	char* descriptionPointer = &currentTile->description[0];
	char* playerName = &(MainMenu->playerNames[currentTile->controller])[0];

	if (currentTile->controller != 0)
	{
		waddstr(MainMenu->mywindow, "Player ");
		waddstr(MainMenu->mywindow, playerName);
		waddstr(MainMenu->mywindow, "'s ");
		waddstr(MainMenu->mywindow, descriptionPointer);
	}
	else
	{
		
		waddstr(MainMenu->mywindow, descriptionPointer);
	}

	//If tile is undergoing capture, let us know.
	if (currentTile->capturePoints != 20)
	{
		snprintf(pointerToPrint, 100, " Capture Points Left: %d ", currentTile->capturePoints);
		waddstr(MainMenu->mywindow, pointerToPrint);
	}

	if (currentTile->hasMinionOnTop == true && currentTile->withinVision)
	{
		Minion* currentMinion = currentTile->minionOnTop;
		
		//Print out basic minion status.
		waddstr(MainMenu->mywindow, " Player ");
		waddstr(MainMenu->mywindow, &(MainMenu->playerNames[currentMinion->team])[0]);
		waddstr(MainMenu->mywindow, "'s ");
		waddstr(MainMenu->mywindow, &currentMinion->description[0]);
		snprintf(pointerToPrint, 100, ": %d Health Left. \n", int(currentMinion->health));		//int(round(currentMinion->health/10)));
		waddstr(MainMenu->mywindow, pointerToPrint);

		if (currentMinion->status == gaveupmovehasntfired)
		{
			waddstr(MainMenu->mywindow, "Holding position. Ready to attack.\n");
					
		}

		if (currentMinion->status == hasmovedhasntfired)
		{
			waddstr(MainMenu->mywindow, "Has already moved this turn. ");
			
			if (currentMinion->rangeType == rangedFire)
			{
				waddstr(MainMenu->mywindow, "Cannot attack after moving. \n");
				
			}
			if (currentMinion->rangeType == directFire)
			{
				waddstr(MainMenu->mywindow, "Ready to attack.\n");
				
			}
		}
		if (currentMinion->status == hasfired)
		{
			waddstr(MainMenu->mywindow, "Has already moved this turn. Has attacked this turn. \n");
		}
		if (currentMinion->status == hasntmovedorfired)
		{
			waddstr(MainMenu->mywindow, "Ready to move. Ready to attack. \n");
		}
	}
	else
	{
		waddstr(MainMenu->mywindow, "\n\n");
	}

	//Print current turn.
	snprintf(pointerToPrint, 100, "Player %d's turn. Treasury Total: %d\n", boardToPrint->playerFlag, boardToPrint->treasury[boardToPrint->playerFlag]);
	waddstr(MainMenu->mywindow, pointerToPrint);
	
	char* eventTextToPrint = &eventText[0];
	waddstr(MainMenu->mywindow, eventTextToPrint);
	eventText = "";

	return 0;
}

int inputLayer::printMinionMenu(MasterBoard* boardToPrint) {

	minionStatus mystatus = boardToPrint->cursor.selectMinionPointer->status;

	if (mystatus == hasntmovedorfired)
	{
		waddstr(MainMenu->mywindow, "Move cursor(WASD) | Move minion (m)\n");
		waddstr(MainMenu->mywindow, "Deselect minion(t) | Capture move(c)\n" );
	}

	if (mystatus == hasmovedhasntfired || mystatus == gaveupmovehasntfired)
	{
		waddstr(MainMenu->mywindow, "Move cursor(WASD) | Attack (r)\n");
		waddstr(MainMenu->mywindow, "Deselect minion (t) | Capture (c)\n");
	}
	if (mystatus == hasfired)
	{
		waddstr(MainMenu->mywindow, "\n\n");
	}
	return 0;

}

int inputLayer::printBoardMenu() {
	waddstr(MainMenu->mywindow, "Move cursor (WASD) | Menu (m)\n");
	waddstr(MainMenu->mywindow, "Select minion/property (t)\n");
	return 0;
}

int	inputLayer::printPropertyMenu(MasterBoard* boardToPrint) {

	//If this is not the second valid purchase input
	if (requestedMinionToBuy == '\n')
	{
		if (boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()].symbol == 'h')
		{
			waddstr(MainMenu->mywindow, "Input Minion to Buy(i,s,a,r,c,R,T,A):\n");
			waddstr(MainMenu->mywindow, "Deselect Property (P)\n");
		}
		if (boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()].symbol == 'A')
		{
			waddstr(MainMenu->mywindow, "Input Minion to Buy(v,h):\n");
			waddstr(MainMenu->mywindow, "Deselect Property (P)\n");
		}
	
	}
	else if (requestedMinionToBuy != '\n')
	{
		if (requestedMinionToBuy == '!')
		{
			waddstr(MainMenu->mywindow, "Can't afford, try another symbol.\n");
			waddstr(MainMenu->mywindow, "Input Minion to Buy | Deselect Property (P)\n");
		}
		else if (requestedMinionToBuy == '?')
		{
			waddstr(MainMenu->mywindow, "Invalid input. Try another symbol.\n");
			waddstr(MainMenu->mywindow, "Input Minion to Buy | Deselect Property (P)\n");
			
		}
		else
		{
			char toOutput[100];
			snprintf(toOutput, 100, "Requested unit costs: %d. \n", requestedUnitPrice);
			
			
			waddstr(MainMenu->mywindow, toOutput);
			waddstr(MainMenu->mywindow, "Confirm (z) | Cancel (p)\n");
			
		}
	}

	return 0;

}

int inputLayer::printMenu() {
	waddstr(MainMenu->mywindow, "Save game (s) | Go to main menu (x) | Load save game (L) \n");
	waddstr(MainMenu->mywindow, "End turn (e) | Exit menu (m) \n");
	return 0;
}

int inputLayer::printLowerScreen(MasterBoard* boardToPrint) {

	mvwaddstr(MainMenu->mywindow, WINDOW_HEIGHT*3 + 1, 0, "                                                 \n");
	waddstr(MainMenu->mywindow, "                                                 \n");
	waddstr(MainMenu->mywindow, "                                                 \n");
	waddstr(MainMenu->mywindow, "                                                 \n");
	waddstr(MainMenu->mywindow, "                                                 \n");
	wmove(MainMenu->mywindow, WINDOW_HEIGHT*3 + 1, 0 );

	if (status == gameBoard)
	{
		printBoardMenu();
	}
	else

		if (status == menu)
		{
			printMenu();
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

	printStatus(boardToPrint);

	return 0;

}

int inputLayer::printUpperScreen(MasterBoard* boardToPrint) {
	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocation / boardToPrint->BOARD_WIDTH;
	int windowX = boardToPrint->windowLocation % boardToPrint->BOARD_WIDTH;
	

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (int i = windowY; i < (windowY + WINDOW_HEIGHT); i++)
	{
		for (int j = windowX; j < (windowX + WINDOW_WIDTH); j++)
		{
			//First do lower priority set which is tiles and minions. These can be ovveridden by cursor/in range.
			//Is there a minion there? Do we have minions toggled on as visible? Is the minion within vision?
			if (boardToPrint->Board[j][i].hasMinionOnTop == true && minionVisibleStatus == showMinions && boardToPrint->Board[j][i].withinVision == true)
			{
					printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].minionOnTop->Image, boardToPrint->Board[j][i].minionOnTop->team + 3, boardToPrint->Board[j][i].minionOnTop);
			}
			//If no minion print map piece.
			else 
			{
				//If player controlled (Must be property), must always be visible
				if (boardToPrint->Board[j][i].controller == boardToPrint->playerFlag)
				{
					printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, boardToPrint->Board[j][i].controller + 9, NULL);
				}
				//Not player controlled, either neutral/enemy property, or non-property
				else
				{
					//If it's property
					if (boardToPrint->Board[j][i].checkForProperty() == true)
					{
						//Within vision
						if (boardToPrint->Board[j][i].withinVision == true)
						{
							//If neutral property
							if (boardToPrint->Board[j][i].controller == 0)
							{
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, neutralTile, NULL);
							}
							else
							{	//Player controlled inside vision
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, boardToPrint->Board[j][i].controller + 9, NULL);
							}
						}
						//Outside vision
						else
						{
							//If neutral property
							if (boardToPrint->Board[j][i].controller == 0)
							{
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, fogNeutralTile, NULL);
							}
							else
							{	//Player controlled outside vision
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, boardToPrint->Board[j][i].controller + 15, NULL);
							}
						}
					}
					//It's not property
					else
					{
						//Within vision
						if (boardToPrint->Board[j][i].withinVision == true)
						{
							if (boardToPrint->Board[j][i].symbol == '~' || boardToPrint->Board[j][i].symbol == '-')
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, waterTile, NULL);
							else
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, landTile, NULL);
						}
						//Outside vision
						else
						{
							if (boardToPrint->Board[j][i].symbol == '~' || boardToPrint->Board[j][i].symbol == '-')
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, fogWaterTile, NULL);
							else
								printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, fogLandTile, NULL);
						}
					}
				}
			}
			//Higher "priority set" is cursor vs in range, thus it goes after the "base" portion of minions and tiles.
			//If there is a cursor there, it takes priority for printing.
			if (i == boardToPrint->cursor.getY() && j == boardToPrint->cursor.getX())
			{
				mvwaddch(MainMenu->mywindow, (i - windowY) * 3 + 2, (j - windowX) * 3 + 1, 'X' + COLOR_PAIR(cursorSymbol));
			}
			else
			if (boardToPrint->Board[j][i].withinRange == true && minionVisibleStatus == showMinions)
			{
				mvwaddch(MainMenu->mywindow, (i - windowY) * 3 + 2, (j - windowX) * 3 + 1, ' ' + COLOR_PAIR(moveRangeSymbol));
			}

		}
		
	}

	return 0;
}

int inputLayer::printWaitingScreen(MasterBoard* boardToPrint) 
{
	wclear(MainMenu->mywindow);
	char* playerName = &(MainMenu->playerNames[boardToPrint->playerFlag])[0];
	waddstr(MainMenu->mywindow, "Player ");
	waddstr(MainMenu->mywindow, playerName);
	waddstr(MainMenu->mywindow, "'s ");
	waddstr(MainMenu->mywindow, " Turn. Press any key to begin.  \n");
	move(WINDOW_HEIGHT * 3 + 1, 0);
	wrefresh(MainMenu->mywindow);
	return 0;

}

int inputLayer::printScreen(MasterBoard* boardToPrint)
{
	if (status != waitingForNextLocalPlayer) 
	{
	wclear(MainMenu->mywindow);
	printUpperScreen(boardToPrint);
	printLowerScreen(boardToPrint);
	wrefresh(MainMenu->mywindow);
	
	}
	else printWaitingScreen(boardToPrint);

	return 0;
}

int inputLayer::waitingScreenInput(MasterBoard* boardToInput) 
{
	//Only lasts one input.
	status = gameBoard;

	return 0;
}

int inputLayer::gameBoardInput(char* Input, MasterBoard* boardToInput)
{
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
				boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'A' )
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
		if (boardToInput->pickUpMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0) 
		{
			//Change status if successful pickup occurred
			status = gameBoard;
		}
		else
		if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{	
			//Change status appropriately for successful movement.
			status = gameBoard;
		}
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

	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours. Must also not be transport type.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true && boardToInput->cursor.selectMinionPointer->type != transport)
		if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].hasMinionOnTop == true)
			if ((boardToInput->cursor.getX() != boardToInput->cursor.selectMinionPointer->locationX) || (boardToInput->cursor.getY() != boardToInput->cursor.selectMinionPointer->locationY))//Can attack if minion is selected
				if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].minionOnTop->team != boardToInput->cursor.selectMinionPointer->team)//And it's enemy team's.
					if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].withinRange == true)	//In range
					{
						//This is the actual attack portion. Return of 0 indicates successful attack.
						bool attackSuccess = boardToInput->attackMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY(), this);
						if (attackSuccess == 0)
						{
							status = gameBoard;
						}
					}

	//Press 'c' to capture property minion is currently on.
	//First, minion must be available.
	//Also, must be infantry type. (Cavalry can capture too).
	if (*Input == 'c' && boardToInput->cursor.selectMinionFlag == true )
		if ((boardToInput->cursor.selectMinionPointer->status == hasmovedhasntfired
		|| boardToInput->cursor.selectMinionPointer->status == gaveupmovehasntfired)
		&& boardToInput->cursor.selectMinionPointer->specialtyGroup == infantry)
	{
		tile* tileToCheck = &boardToInput->Board[boardToInput->cursor.selectMinionPointer->locationX][boardToInput->cursor.selectMinionPointer->locationY];
		
		//Must be property and must not be the current player's property (Could be neutral).
		if (tileToCheck->checkForProperty() && tileToCheck->controller != boardToInput->playerFlag)
		{
			eventText = boardToInput->captureProperty(tileToCheck, boardToInput->cursor.selectMinionPointer);
			boardToInput->deselectMinion();
			status = gameBoard;
		}
	}
	

	return 0;
}

int inputLayer::menuInput(char* Input, MasterBoard* boardToInput) {

	//This is a working key.
	if (*Input == 'g')
	{
		MainMenu->scrambleMap(boardToInput, this);	//This needs to be cleaned up to deal with minions.
	}

	//Another working key for compie
	if (*Input == 'c') 
	{
		computerPlayer->moveMinions(boardToInput);
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


	//Ends the turn and passes it to the next player.
	//Autosave every turn.
	if (*Input == 'e')
	{
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

	}
	
	//Below disabled, you have to load via the main menu for the moment, since you have to set up a new game with players and such.
	
	if (*Input == 'l')
	{
		//Load the actual save game
		std::ifstream loadGameSave;
		bool loadsuccessful = false;

		//Prompt user and load scenario
		while (loadsuccessful == false)
		{
			waddstr(MainMenu->mywindow, "Choose which save game to load (Case sensitive): \n");
			std::string gameToLoad;
			char inputChars[100];
			wgetstr( MainMenu->mywindow,&inputChars[0]);
			gameToLoad = inputChars;

			loadGameSave.open(".\\savegames\\" + gameToLoad + "_save.txt");
			if (loadGameSave.is_open())
			{
				waddstr(MainMenu->mywindow, "Save game successfully loaded!\n");
				loadsuccessful = true;
			}
			else
			{
				waddstr(MainMenu->mywindow, "Could not load save game. Please check that it exists and the right spelling was used.\n");

			}

		}
		//Actually load scenario. Initialize board, etc.
		MainMenu->gameLoad(boardToInput, this, computerPlayer, &loadGameSave);
		status = gameBoard;
	}

	//Prompt user and save game.
	if (*Input == 's')
	{
		//This is a mess but it's just using getstr instead of cin, which requires a little footwork.
		char gameToSave[100];

		//Ensure game is saved into the right directory.
		std::string saveName = ".\\savegames\\";

		waddstr(MainMenu->mywindow, "Choose file name to save your game:\n");
		wgetstr( MainMenu->mywindow,&gameToSave[0]);
		saveName += gameToSave;
		saveName += "_save.txt";


		MainMenu->gameSave(saveName, boardToInput);
		status = gameBoard;
	}

	//Exit menu
	if (*Input == 'm')
	{
		status = gameBoard;
	}

	//Exit to main menu
	if (*Input == 'x')
	{
		MainMenu->gameSave(".\\savegames\\Auto_save.txt", boardToInput);
		exitToMainMenu();
	}

	return 0;
}

int inputLayer::propertyMenuInput(char* Input, MasterBoard* boardToInput) {

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

	int treasury = boardToInput->treasury[boardToInput->playerFlag];

	//If this is NOT the second valid purchase input
	//IE we have not yet gotten any valid input for propertyLayer.
	//Thus one of the three states (prestate, too expensive, not a real unit).
	if (requestedMinionToBuy == '\n' || requestedMinionToBuy == '!' || requestedMinionToBuy == '?')
	{
		if (*Input == 'p')
		{
			//Deselect
			status = gameBoard;
			requestedMinionToBuy = '\n';
			return 0;
		}
		//Consult cost table:
		requestedUnitPrice = boardToInput->consultMinionCostChart(*Input, boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol);

		//If it is a real unit we are trying to purchase
		//Aka unitPrica is not -1 aka non-error
		if (requestedUnitPrice > 0)
		{
			requestedMinionToBuy = *Input;

			//If we can't afford, use special ! character
			if (requestedUnitPrice > treasury)
			{
				//Indicates you can't afford
				requestedMinionToBuy = '!';
			}
		}
		else
		{
			requestedMinionToBuy = '?';
		}

	}

	//This is the second valid input.
	else
	{
		if (*Input == 'p')
		{
			//Cancel purchase
			requestedMinionToBuy = '\n';
		}
		if (*Input == 'z')
		{
			//Confirm purchase
			boardToInput->attemptPurchaseMinion(requestedMinionToBuy, boardToInput->cursor.getX(), boardToInput->cursor.getY(), boardToInput->playerFlag);
			status = gameBoard;
			requestedMinionToBuy = '\n';
			requestedUnitPrice = -1;
		}
	}

	return 0;
}

int inputLayer::exitToMainMenu() 
{
	this->MainMenu->menuStatus = topmenu;
	MainMenu->skipOneInput = true;
	return 0; 
}
