#include "Minion.hpp"
#include "MasterBoard.hpp"
#include "inputLayer.hpp"
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include "compie.hpp"
#include <curses.h>



int scrambleMap(MasterBoard* LoadBoard, inputLayer* InputLayer);
int scenarioSave(std::string saveGameName, MasterBoard* boardToPrint);
int scenarioLoad(MasterBoard* boardToPrint, inputLayer* InputLayer, compie* ComputerPlayer);

int inputLayer::printSingleTile(int inputX, int inputY, std::string inputString, int teamNumber) 
{
	move(inputX*3, inputY*3);
	addch(inputString[0] + COLOR_PAIR(teamNumber));
	addch(inputString[1] + COLOR_PAIR(teamNumber));
	addch(inputString[2] + COLOR_PAIR(teamNumber));

	move(inputX * 3 + 1, inputY * 3);
	addch(inputString[3] + COLOR_PAIR(teamNumber));
	addch(inputString[4] + COLOR_PAIR(teamNumber));
	addch(inputString[5] + COLOR_PAIR(teamNumber));

	move(inputX * 3 + 2, inputY * 3);
	addch(inputString[6] + COLOR_PAIR(teamNumber));
	addch(inputString[7] + COLOR_PAIR(teamNumber));
	addch(inputString[8] + COLOR_PAIR(teamNumber));

	return 1;
}

int inputLayer::printStatus(MasterBoard* boardToPrint)
{
	tile* currentTile = &boardToPrint->Board[boardToPrint->cursor.getX()][boardToPrint->cursor.getY()];

	//Need string pointer since addstr giving grief about printing strings, and same with snprintf.
	char pointerToPrint[100];
	char* stringPointer = &currentTile->description[0];
	
	if (currentTile->controller != 0)
	{
		snprintf(pointerToPrint, 100, "Player %d's ", currentTile->controller);
		addstr(pointerToPrint);
		addstr(stringPointer);
	}
	else
	{
		
		addstr(stringPointer);
	}

	//If tile is undergoing capture, let us know.
	if (currentTile->capturePoints != 20)
	{
		snprintf(pointerToPrint, 100, " Capture Points Left: %d ", currentTile->capturePoints);
		addstr(pointerToPrint);
	}

	if (currentTile->hasMinionOnTop == true)
	{
		Minion* currentMinion = currentTile->minionOnTop;
	
		//Print out basic minion status.
		snprintf(pointerToPrint, 100, " Player %d's ", currentMinion->team);
		stringPointer = &currentMinion->description[0];
		addstr(pointerToPrint);
		addstr(stringPointer);
		snprintf(pointerToPrint, 100, ": %d Health Left. \n", int(currentMinion->health));
		addstr(pointerToPrint);

		if (currentMinion->status == gaveupmovehasntfired)
		{
			addstr("Holding position. Ready to attack.\n");
					
		}

		if (currentMinion->status == hasmovedhasntfired)
		{
			addstr("Has already moved this turn. ");
			
			if (currentMinion->rangeType == rangedFire)
			{
				addstr("Cannot attack after moving. \n");
				
			}
			if (currentMinion->rangeType == directFire)
			{
				addstr("Ready to attack.\n");
				
			}
		}
		if (currentMinion->status == hasfired)
		{
			addstr("Has already moved this turn. Has attacked this turn. \n");
		}
		if (currentMinion->status == hasntmovedorfired)
		{
			addstr("Ready to move. Ready to attack. \n");
		}
	}
	else
	{
		addstr("\n\n");
	}

	//Print current turn.
	snprintf(pointerToPrint, 100, "Player %d's turn. Treasury Total: %d\n", boardToPrint->playerFlag, boardToPrint->treasury[boardToPrint->playerFlag]);
	addstr(pointerToPrint);
	
	char* eventTextToPrint = &eventText[0];
	addstr(eventTextToPrint);
	//Need to figure out string to char *.
	//std::cout << eventText<<std::endl; // << "Cursor Location: " << boardToPrint->cursor.getX() << boardToPrint->cursor.getY() << std::endl;
	//std::cout << boardToPrint->windowLocation << std::endl;
	eventText = "";

	return 0;
}

int inputLayer::printMinionMenu(MasterBoard* boardToPrint) {

	minionStatus mystatus = boardToPrint->cursor.selectMinionPointer->status;

	if (mystatus == hasntmovedorfired)
	{
		addstr("Move cursor(WASD) | Move minion (m)\n");
		addstr("Deselect minion(t) | Capture move(c)\n" );
		//std::cout << "Move cursor(WASD) | Move minion (m)" << std::endl;
		//std::cout << "Deselect minion (t) | Capture move (c)" << std::endl;
	}

	if (mystatus == hasmovedhasntfired || mystatus == gaveupmovehasntfired)
	{
		addstr("Move cursor(WASD) | Attack (r)\n");
		addstr("Deselect minion (t) | Capture (c)\n");
		//std::cout << "Move cursor(WASD) | Attack (r)" << std::endl;
		//std::cout << "Deselect minion (t) | Capture (c)" << std::endl;
	}
	if (mystatus == hasfired)
	{
		addstr("\n\n");
		//std::cout << std::endl;
		//std::cout << std::endl;
	}
	return 0;

}

int inputLayer::printBoardMenu() {
	addstr("Move cursor (WASD) | Menu (m)\n");
	addstr( "Select minion/property (t)\n");
	return 0;
}

int	inputLayer::printPropertyMenu() {

	//If this is not the second valid purchase input
	if (requestedMinionToBuy == '\n')
	{
		addstr("Input Minion to Buy\n");
		addstr("Deselect Property (P)\n");
		
	}
	else if (requestedMinionToBuy != '\n')
	{
		if (requestedMinionToBuy == '!')
		{
			addstr("Can't afford, try another symbol.\n");
			addstr("Input Minion to Buy | Deselect Property (P)\n");
		}
		else if (requestedMinionToBuy == '?')
		{
			addstr("Invalid input. Try another symbol.\n");
			addstr("Input Minion to Buy | Deselect Property (P)\n");
			
		}
		else
		{
			char toOutput[100];
			snprintf(toOutput, 100, "Requested unit costs: %d. \n", unitPrice);
			
			
			addstr(toOutput);
			addstr("Confirm (C) | Cancel (P)\n");
			
		}
	}

	return 0;

}

int inputLayer::printMenu() {
	addstr("Save game (s) | Load new game (L)\n");
	addstr("End turn (t) | Exit menu (m)\n");
	return 0;
}

int inputLayer::printLowerScreen(MasterBoard* boardToPrint) {

	mvaddstr(WINDOW_HEIGHT*3 + 1, 0, "                                                 \n");
	addstr("                                                 \n");
	addstr("                                                 \n");
	addstr("                                                 \n");
	addstr("                                                 \n");
	move(WINDOW_HEIGHT*3 + 1, 0 );

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
					printPropertyMenu();
				}

	printStatus(boardToPrint);

	return 0;

}

int inputLayer::printUpperScreen(MasterBoard* boardToPrint) {
	//windowLocation is a single scalar representing x and y.
	//We do some basic math to break it into the two values for the function.
	//Need to convert windowLocation into a better two part variable.
	int windowY = boardToPrint->windowLocation / BOARD_WIDTH;
	int windowX = boardToPrint->windowLocation % BOARD_WIDTH;
	

	//Go through the whole "board", staying within the bounds of window's x and y coordinates.
	for (int i = windowY; i < (windowY + WINDOW_HEIGHT); i++)
	{
		for (int j = windowX; j < (windowX + WINDOW_WIDTH); j++)
		{
			//First do lower priority set which is tiles and minions. These can be ovveridden by cursor/in range.
			//Is there a minion there? Do we have minions toggled on as visible? Is the minion within vision?
			if (boardToPrint->Board[j][i].hasMinionOnTop == true && minionVisibleStatus == showMinions && boardToPrint->Board[j][i].withinVision == true)
			{
					printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].minionOnTop->Image, boardToPrint->Board[j][i].minionOnTop->team + 1);
			}
			//If no minion print map piece.
			else 
			{
				if(boardToPrint->Board[j][i].withinVision == true)
					printSingleTile((i - windowY) , (j - windowX), boardToPrint->Board[j][i].Image, boardToPrint->Board[j][i].controller + 1);
				else
					printSingleTile((i - windowY), (j - windowX), boardToPrint->Board[j][i].Image, boardToPrint->Board[j][i].controller + 7);
			}
			
			//Higher "priority set" is cursor vs in range, thus it goes after the "base" portion of minions and tiles.
			//If there is a cursor there, it takes priority for printing.
			if (i == boardToPrint->cursor.getY() && j == boardToPrint->cursor.getX())
			{
				mvaddch((i - windowY) * 3 + 2, (j - windowX) * 3 + 1, '*' + COLOR_PAIR(0));
			}
			else
			if (boardToPrint->Board[j][i].withinRange == true && minionVisibleStatus == showMinions)
			{
				mvaddch((i - windowY) * 3 + 2, (j - windowX) * 3 + 1, ':' + COLOR_PAIR(0));
			}

		}
		
	}

	return 0;
}

int inputLayer::printScreen(MasterBoard* boardToPrint)
{
	clear();
	printUpperScreen(boardToPrint);
	printLowerScreen(boardToPrint);
	refresh();
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
				//DEBUG
				//computerPlayer->determineMinionTasks(boardToInput);
				//DEBUG
				status = minionAction;
			}
		}	//Else if empty property, select it. No minion on top, right team, must be factory to select.
		else
			if (boardToInput->Board[boardToInput->cursor.getX()][boardToInput->cursor.getY()].symbol == 'h'
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
		if (boardToInput->moveMinion(boardToInput->cursor.getX(), boardToInput->cursor.getY()) == 0)
		{	//Change status appropriately for successful movement.
			//boardToInput->cursor.selectMinionPointer->status = hasmovedhasntfired;
			//boardToInput->deselectMinion();
			status = gameBoard;
		}
	}

	//Attack command. Pre-reqs: must be in range, must be enemy team and not yours.
	if (*Input == 'r' && boardToInput->cursor.selectMinionFlag == true)
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
		scrambleMap(boardToInput, this);	//This needs to be cleaned up to deal with minions.
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
	if (*Input == 't')
	{
		if (boardToInput->cursor.selectMinionFlag == true)
			boardToInput->deselectMinion();
		boardToInput->endTurn();
		scenarioSave("Autosave", boardToInput);
		status = gameBoard;
	}

	if (*Input == 'l')
	{
		//DEBUG- this specially load a certain compie but instead loads the same exact one.
		std::cout << "DISABLED DISABLED DISABLED" << std::endl;
		scenarioLoad(boardToInput, this, computerPlayer);
		status = gameBoard;
		//Load new map
	}

	//Prompt user and save scenario.
	if (*Input == 's')
	{
		std::string saveName = "";
		std::cout << "Choose where to save your game:" << std::endl;
		std::cin >> saveName;
		scenarioSave(saveName, boardToInput);
		status = gameBoard;
	}

	//Exit menu
	if (*Input == 'm')
	{
		status = gameBoard;
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

	//Player treasury, cost of unit initialized to -1 to show bad inputs.
	unitPrice = -1;
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
			return 0;
		}
		//Consult cost table:
		unitPrice = boardToInput->consultMinionCostChart(*Input);

		//If it is a real unit we are trying to purchase
		//Aka unitPrica is not -1 aka non-error
		if (unitPrice > 0)
		{
			requestedMinionToBuy = *Input;

			//If we can't afford, use special ! character
			if (unitPrice > treasury)
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
		if (*Input == 'c')
		{
			//Confirm purchase
			boardToInput->createMinion(requestedMinionToBuy, boardToInput->cursor.getX(), boardToInput->cursor.getY(), boardToInput->playerFlag, 100, hasfired);
			boardToInput->treasury[boardToInput->playerFlag] -= unitPrice;
			status = gameBoard;
			requestedMinionToBuy = '\n';
		}
	}

	return 0;
}

