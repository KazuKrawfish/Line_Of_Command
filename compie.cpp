//Copyright 2022, Supercontinent Software Ltd.
//
//	compie.cpp
//

#include "compie.hpp"
#include <thread>
#include <iostream>
#include "mainmenu.h"

bool isAdjacent(int inputX1, int inputX2, int inputY1, int inputY2);

//Compie calculates its closest objective by using buildPath and finding potential objective that has the best distance.
//Then it builds a compie pathMap again on that objective
//SelectMinion now builds an apparentPathMap that overrides the initial one. Shouldn't matter for strategic considerations.


//Find closest airbase to minion
//If none exist, return NULL tile
tile* compie::findClosestAirbase(Minion* inputMinion)
{
	tile* closestAirBase = NULL;
	int oldDistance = -1;


	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
			{
				if (masterBoardPointer->Board[x][y].symbol == 'A')
				{
					if (closestAirBase == NULL)
					{
						closestAirBase = &masterBoardPointer->Board[x][y];
						oldDistance = masterBoardPointer->computeDistance(masterBoardPointer->Board[x][y].locationX, inputMinion->locationX, masterBoardPointer->Board[x][y].locationY, inputMinion->locationY);
					}
					else
					{
						int newDistance = masterBoardPointer->computeDistance(masterBoardPointer->Board[x][y].locationX, inputMinion->locationX, masterBoardPointer->Board[x][y].locationY, inputMinion->locationY);

						if (newDistance < oldDistance)
						{
							oldDistance = newDistance;
							closestAirBase = &masterBoardPointer->Board[x][y];
						}
					}
				}
			}
		}

	}
	return closestAirBase;
}

int compie::statusUpdate()
{
	double totalPropertiesCompieOwns = 0;
	double totalFactoriesCompieOwns = 0;

	//Clear all land masses of infantry/army size amounts
	for (unsigned int i = 1; i < compieLandMassMap.roster.size(); i++)
	{
		compieLandMassMap.roster[i].armySizeHere = 0;
		compieLandMassMap.roster[i].onlyInfantryArmySizeHere = 0;
	}

	buildCompieMinionRoster();

	//Now find factories and properties totals int total- we calculate the turn's armamentFactor as a global rate, not individual land masses.
	//Otherwise the compie would randomly skew certain areas to high-mech. and they'd never build units if it remained actually poor.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
			{
				if (masterBoardPointer->Board[x][y].symbol == 'h')
				{
					totalFactoriesCompieOwns++;
				}

				totalPropertiesCompieOwns++;

			}
		}
	}

	//More factories means larger more infantry heavy army
	double armamentFactor = totalFactoriesCompieOwns / totalPropertiesCompieOwns;
	for (unsigned int i = 1; i < compieLandMassMap.roster.size(); i++)
	{
		compieLandMassMap.roster[i].infantryMaxHere = armamentFactor * compieLandMassMap.roster[i].maxArmySize + compieInfantryBaseline;
	}

	return 0;

}


//Builds land mass map for compie. Recursive since it has to build contiguous territories.
//Should return the number used. Which should only matter if this was the first one called.
int compie::buildLandMassMap(int x, int y, int nextNumber)
{
	int myNumber = compieLandMassMap.grid[x][y].landMassNumber;
	bool firstSquare = false;

	//If this territory is impassible, go no further.
	//Indicates this square is impassible and thus doesn't belong to any landMass.
	if (myNumber == -1)	//Previously assigned based on 99 == -1
		return -1;

	//If not impassable
	if (myNumber >= 0)
	{

		//This is the first square we're analyzing (We weren't reached previously)
		//That means this is part of a new landmass.
		//Otherwise we were assigned a value before this function was called.
		if (compieLandMassMap.grid[x][y].landMassNumber == 0)
		{
			compieLandMassMap.grid[x][y].landMassNumber = nextNumber;
			firstSquare = true;

		}


		//Check each neighbor. If they have a number/are impassible skip them. Otherwise they get your number.
		//Then call buildLandMasMap on them.
		if (x > 0)
		{
			if (compieLandMassMap.grid[x - 1][y].landMassNumber == 0)
			{
				compieLandMassMap.grid[x - 1][y].landMassNumber = myNumber;
				buildLandMassMap(x - 1, y, nextNumber);
			}
		}
		if (y > 0)
		{
			if (compieLandMassMap.grid[x][y - 1].landMassNumber == 0)
			{
				compieLandMassMap.grid[x][y - 1].landMassNumber = myNumber;
				buildLandMassMap(x, y - 1, nextNumber);
			}
		}
		if (y < masterBoardPointer->BOARD_HEIGHT - 1)
		{
			if (compieLandMassMap.grid[x][y + 1].landMassNumber == 0)
			{
				compieLandMassMap.grid[x][y + 1].landMassNumber = myNumber;
				buildLandMassMap(x, y + 1, nextNumber);
			}
		}
		if (x < masterBoardPointer->BOARD_WIDTH - 1)
		{
			if (compieLandMassMap.grid[x + 1][y].landMassNumber == 0)
			{
				compieLandMassMap.grid[x + 1][y].landMassNumber = myNumber;
				buildLandMassMap(x + 1, y, nextNumber);
			}
		}


		//If this was first call, return next number iterated.
		//If not the first call, return nextNumber without iteration.
		if (firstSquare == true)
			return nextNumber + 1;
		else return nextNumber;

	}

	return -1;
}

int compie::analyzeSingleLandMass(int landMassNumber)
{

	int maxX = 0;
	int minX = 0;
	int minY = 0;
	int maxY = 0;

	double landToLandBorders = 0;
	double landToImpassBorders = 0;
	double totalLandSquares = 0;

	bool firstSquare = true;

	//First find borders of single land mass.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			if (compieLandMassMap.grid[x][y].landMassNumber == landMassNumber)
			{
				if (x < minX || firstSquare == true)
				{
					minX = x;

				}
				if (x > maxX || firstSquare == true)
				{
					maxX = x;

				}

				if (y < minY || firstSquare == true)
				{
					minY = y;

				}
				if (y > maxY || firstSquare == true)
				{
					maxY = y;

				}

				if (firstSquare == true)
					firstSquare = false;

			}
		}
	}

	//Count borders of individual squares
	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			if (compieLandMassMap.grid[x][y].landMassNumber == landMassNumber)
			{
				totalLandSquares++;
			}

			//If not on rightmost edge of board, check right border
			//Must be the same land mass to be counted
			if (x < masterBoardPointer->BOARD_WIDTH - 1 && x < maxX)
			{
				if (compieLandMassMap.grid[x][y].landMassNumber == landMassNumber)
				{

					if (compieLandMassMap.grid[x][y].landMassNumber == compieLandMassMap.grid[x + 1][y].landMassNumber)
					{
						landToLandBorders++;
					}
					else	//Otherwise it must be impassible
					{
						landToImpassBorders++;
					}
				}
				else
				{	//If it's not a land/passable square, all borders by def. must be impass borders.
					landToImpassBorders++;
				}
			}
			//Then check bottom border of this square, if we're not at board bottom.
			if (y < masterBoardPointer->BOARD_HEIGHT - 1 && y < maxY)
			{
				if (compieLandMassMap.grid[x][y].landMassNumber == landMassNumber)
				{

					if (compieLandMassMap.grid[x][y].landMassNumber == compieLandMassMap.grid[x][y + 1].landMassNumber)
					{
						landToLandBorders++;
					}
					else	//Otherwise it must be impassible
					{
						landToImpassBorders++;
					}
				}
				else
				{	//If it's not a land/passable square, all borders by def. must be impass borders.
					landToImpassBorders++;
				}

			}



		}
	}

	//Compute edginessFactor
	compieLandMassMap.roster[landMassNumber].edginessFactor = landToLandBorders / (landToLandBorders + landToImpassBorders);
	compieLandMassMap.roster[landMassNumber].landEdges = landToLandBorders;
	compieLandMassMap.roster[landMassNumber].impassEdges = landToImpassBorders;


	//Set borders for final product
	compieLandMassMap.roster[landMassNumber].leftX = minX;
	compieLandMassMap.roster[landMassNumber].rightX = maxX;
	compieLandMassMap.roster[landMassNumber].topY = minY;
	compieLandMassMap.roster[landMassNumber].bottomY = maxY;

	//Set land count for final product
	compieLandMassMap.roster[landMassNumber].totalLandSquareCount = totalLandSquares;

	//Set area for final product
	compieLandMassMap.roster[landMassNumber].totalArea = (maxX - minX + 1) * (maxY - minY + 1);

	//Compute landFactor and set for final product
	compieLandMassMap.roster[landMassNumber].landinessFactor = totalLandSquares / (compieLandMassMap.roster[landMassNumber].totalArea);

	compieLandMassMap.roster[landMassNumber].initialized = true;

	return 0;

}

//Helps compie determine semi-constant map elements.
int compie::analyzeMap()
{
	std::cout << "Analyzing map for a compie player" << std::endl;

	//Go through each square on the board
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{

			//Find headquarters:
			if (masterBoardPointer->Board[x][y].symbol == 'Q' && masterBoardPointer->Board[x][y].controller == compiePlayerFlag)
			{
				headquartersX = x;
				headquartersY = y;
			}

			//Count number of properties on map
			if (masterBoardPointer->Board[x][y].checkForProperty() == true)
				totalPropertiesOnMap++;

			//Here we find which properties are impassible to an infantry, which helps determine the boundaries for compie's land mass map.
			int movementCostHere = masterBoardPointer->Board[x][y].consultMovementChart("Infantry");
			if (movementCostHere == -1)
			{
				std::cout << "Error while analyzing map, incorrect symbol" << std::endl;
			}
			else if (movementCostHere == 99)
			{
				compieLandMassMap.grid[x][y].landMassNumber = -1;
			}


		}

	}

	int nextLandMassNumber = 1;
	//Now go through board again, and build land mass map.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			//If this landMassSquare is passable, call recursive builder.
			if (compieLandMassMap.grid[x][y].landMassNumber != -1)
				nextLandMassNumber = buildLandMassMap(x, y, nextLandMassNumber);
		}
	}

	//One for each land mass, one for 0 index, and one extra because it weirdly fails on me.
	compieLandMassMap.roster.resize(nextLandMassNumber + 1);

	//Iterate through board and analyze any landmasses that have not been analyzed.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{

			if (compieLandMassMap.grid[x][y].landMassNumber != -1)
			{
				if (compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].initialized == false)
				{
					analyzeSingleLandMass(compieLandMassMap.grid[x][y].landMassNumber);
				}
			}

		}
	}



	for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
		{
			std::cout << " " << compieLandMassMap.grid[x][y].landMassNumber;
		}
		std::cout << "\n";

	}

	for (unsigned int y = 1; y < compieLandMassMap.roster.size(); y++)
	{
		///Baseline for compieArmySizeFactor: (4 == 2*2) <-- 2: Half of landMass belongs to compie, 2: To provide room for maneuver and repair
		double normalizedArea = compieLandMassMap.roster[y].totalLandSquareCount * compieLandMassMap.roster[y].landinessFactor;
		compieLandMassMap.roster[y].maxArmySize = normalizedArea * compieLandMassMap.roster[y].edginessFactor / (compieArmySizeFactor)+2;

		//Minimum army size per landmass
		if (compieLandMassMap.roster[y].maxArmySize < 4)
		{
			compieLandMassMap.roster[y].maxArmySize = 4;
		}

		std::cout << "Land Mass: " << y << std::endl;
		std::cout << "Total Area: " << compieLandMassMap.roster[y].totalArea << std::endl;
		std::cout << "Total Land Squares: " << compieLandMassMap.roster[y].totalLandSquareCount << std::endl;
		std::cout << "Edge Factor: " << compieLandMassMap.roster[y].edginessFactor << " (Land Edges: " << compieLandMassMap.roster[y].landEdges << ") (Impass. Edges: " << compieLandMassMap.roster[y].impassEdges << ")" << std::endl;
		std::cout << "Land Factor: " << compieLandMassMap.roster[y].landinessFactor << std::endl;
		std::cout << "Max Army Size here: " << compieLandMassMap.roster[y].maxArmySize << std::endl;
		std::cout << "----------" << std::endl;
	}

	return 0;
}

//Assumes minion is selected since we're going to check for withinRange.
int compie::defendHeadquarters(compieMinionRecord* selectedMinionRecord)
{

	//For now artillery doesn't do this
	if (selectedMinionRecord->recordedMinion->rangeType == rangedFire)
		return 0;

	Cursor* myCursor = &masterBoardPointer->cursor;

	//If headquarters under attack by enemy
	if (masterBoardPointer->Board[headquartersX][headquartersY].hasMinionOnTop && masterBoardPointer->Board[headquartersX][headquartersY].minionOnTop->team != masterBoardPointer->playerFlag)
	{
		//Check each adjacent tile to see if you can move there, and then set tasking to do so.
		if (headquartersX > 0 && masterBoardPointer->Board[headquartersX - 1][headquartersY].withinRange == true &&
			(masterBoardPointer->Board[headquartersX - 1][headquartersY].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY && myCursor->selectMinionPointer->locationX == headquartersX - 1)))
		{
			selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[headquartersX - 1][headquartersY];
			selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersX < masterBoardPointer->BOARD_WIDTH - 1 && masterBoardPointer->Board[headquartersX + 1][headquartersY].withinRange == true &&
			(masterBoardPointer->Board[headquartersX + 1][headquartersY].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY && myCursor->selectMinionPointer->locationX == headquartersX + 1)))
		{
			selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[headquartersX + 1][headquartersY];
			selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersY > 0 && masterBoardPointer->Board[headquartersX][headquartersY - 1].withinRange == true &&
			(masterBoardPointer->Board[headquartersX][headquartersY - 1].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY - 1 && myCursor->selectMinionPointer->locationX == headquartersX)))
		{
			selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[headquartersX][headquartersY - 1];
			selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersY < masterBoardPointer->BOARD_HEIGHT - 1 && masterBoardPointer->Board[headquartersX][headquartersY + 1].withinRange == true &&
			(masterBoardPointer->Board[headquartersX][headquartersY + 1].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY + 1 && myCursor->selectMinionPointer->locationX == headquartersX)))
		{
			selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[headquartersX][headquartersY + 1];
			selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}


	}


	return 0;
}

//Assumes minion is selected since we're going to check for withinRange.
int compie::defendProperty(compieMinionRecord* selectedMinionRecord)
{
	//For now artillery doesn't do this
	//Transport will never do this
	if (selectedMinionRecord->recordedMinion->rangeType == rangedFire
		|| selectedMinionRecord->recordedMinion->specialtyGroup == smallTransport
		|| selectedMinionRecord->recordedMinion->specialtyGroup == largeTransport)
		return 0;

	Cursor* myCursor = &masterBoardPointer->cursor;

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > masterBoardPointer->BOARD_WIDTH)
	{
		maxX = masterBoardPointer->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > masterBoardPointer->BOARD_HEIGHT)
	{
		maxY = masterBoardPointer->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//If property under attack by enemy
			if (masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag && masterBoardPointer->Board[x][y].hasMinionOnTop && masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag)
			{
				//Check each adjacent tile to see if you can move there, and then set tasking to do so.
				if (x > 0 && masterBoardPointer->Board[x - 1][y].withinRange == true &&
					(masterBoardPointer->Board[x - 1][y].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x - 1)))
				{
					selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[x - 1][y];
					selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[x][y];
					return 1;
				}
				else if (x < masterBoardPointer->BOARD_WIDTH - 1 && masterBoardPointer->Board[x + 1][y].withinRange == true &&
					(masterBoardPointer->Board[x + 1][y].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x + 1)))
				{
					selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[x + 1][y];
					selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[x][y];
					return 1;
				}
				else if (y > 0 && masterBoardPointer->Board[x][y - 1].withinRange == true &&
					(masterBoardPointer->Board[x][y - 1].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y - 1 && myCursor->selectMinionPointer->locationX == x)))
				{
					selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[x][y - 1];
					selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[x][y];
					return 1;
				}
				else if (y < masterBoardPointer->BOARD_HEIGHT - 1 && masterBoardPointer->Board[x][y + 1].withinRange == true &&
					(masterBoardPointer->Board[x][y + 1].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y + 1 && myCursor->selectMinionPointer->locationX == x)))
				{
					selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[x][y + 1];
					selectedMinionRecord->potentialAttackTile = &masterBoardPointer->Board[x][y];
					return 1;
				}


			}

		}
	}

	return 0;
}


compie::compie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer)
{
	InputLayer = providedInputLayer;
	menuPointer = inputMenu;
	compiePlayerFlag = inputPlayerFlag;
	compieMinionRoster.resize(GLOBALSUPPLYCAP);
	return;
}

int compie::initalizeCompie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer, MasterBoard* boardToUse, int inputRepairThreshold, int inputMinionAggressionBonus, int inputInfantryAttackBonus, int inputInfantryBaseline, int inputArmySizeFactor)
{
	InputLayer = providedInputLayer;
	menuPointer = inputMenu;
	masterBoardPointer = boardToUse;
	compiePlayerFlag = inputPlayerFlag;
	compieMinionRoster.resize(GLOBALSUPPLYCAP);

	//Battlelab inputs
	//Only gets used if not -1. -1 indicates to use defaults.
	if (inputRepairThreshold != -1)
		repairThreshold = inputRepairThreshold;
	if (inputMinionAggressionBonus != -1)
		compieMinionAggressionBonus = inputMinionAggressionBonus;
	if (inputInfantryAttackBonus != -1)
		compieInfantryAttackBonus = inputInfantryAttackBonus;
	if (inputInfantryBaseline != -1)
		compieInfantryBaseline = inputInfantryBaseline;
	if (inputArmySizeFactor != -1)
		compieArmySizeFactor = inputArmySizeFactor;


	compieLandMassMap.grid.clear();
	compieLandMassMap.roster.clear();

	compieLandMassMap.grid.resize(masterBoardPointer->BOARD_WIDTH + 1);

	for (int i = 0; i < masterBoardPointer->BOARD_WIDTH; i++)
	{
		compieLandMassMap.grid[i].resize(masterBoardPointer->BOARD_HEIGHT);

	}

	analyzeMap();

	return 0;
}

//Modified - Edited air/land transport tile search to hopefully fix it.
//MODIFIED: Also, was checking if infantry COULDN'T reach that tile, so changed from == to != - hopefully that's the major fix needed
//Land/air transport - Look for closest factory. Move within 2 squares of that factory.
//Sea transport - Look for closest beach/port which is on same landmass as a friendly factory.
int compie::transportSearchForPickup(compieMinionRecord* selectedMinionRecord)
{
	int distanceFromMinionToObjective = 999;
	tile* objectiveFactoryTile = NULL;

	if (masterBoardPointer->cursor.selectMinionPointer == NULL)
		return -1;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;

	if (myMinion->specialtyGroup != largeTransport && myMinion->specialtyGroup != smallTransport)
	{
		std::cout << "Non transport selected" << std::endl;
		return -1;
	}

	//If air or land transport
	if (myMinion->domain == land || myMinion->domain == air)
	{
		//Now go through the whole board and look for closest factory tile
		for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
		{
			for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
			{
				//Must be factory tile and ours
				if (masterBoardPointer->Board[x][y].symbol == 'h' && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
				{
					int rangeToProp = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeToProp != -1 && rangeToProp < distanceFromMinionToObjective)
					{
						//If it's the closest factory so far, set this as our factory objective tile.
						distanceFromMinionToObjective = rangeToProp;
						objectiveFactoryTile = &(masterBoardPointer->Board[x][y]);
						//selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
					}
				}

			}

		}

		//After determining closest factory tile, find tile closest to minion which is also within 2 tiles of that factory.
		//Reset objective distance number
		distanceFromMinionToObjective = 999;
		if (objectiveFactoryTile != NULL)
		{
			for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
			{
				for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
				{
					//Is it within 2 of objectiveFactoryTile, but not directly on top?
					//Must also be unoccupied
					//Also, an infantry must be able to access that tile.
					int distance = masterBoardPointer->computeDistance(objectiveFactoryTile->locationX, x, objectiveFactoryTile->locationY, y);
					if (distance <= 2 && distance > 0 && (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop == myMinion)
						&& masterBoardPointer->Board[x][y].consultMovementChart("Infantry") != 99)
					{
						//Can we get there? Is it closer than previous distance?
						int rangeToTile = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToTile != -1 && rangeToTile < distanceFromMinionToObjective)
						{
							//Set this is as our actual objective tile
							distanceFromMinionToObjective = rangeToTile;   //This may be supposed to be rangeToTile, not distance
							selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
						}
					}
				}
			}
		}
		else
		{
			//If there is no valid objective, just sit still
			selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[myMinion->locationX][myMinion->locationY]);
			distanceFromMinionToObjective = 0;
		}

	}
	else    //Sea transport, must search based on closest beach/port
	{
		for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
		{
			for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
			{
				//Is it a beach or port tile?
				//Must also be unoccupied.
				if ((masterBoardPointer->Board[x][y].symbol == '*' || masterBoardPointer->Board[x][y].symbol == 'P')
					&& (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop == myMinion))
				{
					int rangeToBeach = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeToBeach != -1 && rangeToBeach < distanceFromMinionToObjective)
					{
						//Search board for friendly factory on this landmass
						bool factoryHere = false;
						for (int i = 0; i < masterBoardPointer->BOARD_WIDTH; i++)
						{
							if (factoryHere == true)
								break;

							for (int j = 0; j < masterBoardPointer->BOARD_HEIGHT; j++)
							{
								//Friendly factory on same landmass
								if (masterBoardPointer->Board[i][j].symbol == 'h' && masterBoardPointer->Board[i][j].controller == masterBoardPointer->playerFlag
									&& compieLandMassMap.grid[i][j].landMassNumber == compieLandMassMap.grid[x][y].landMassNumber)

								{
									factoryHere = true;
									break;
								}
							}
						}

						//Does it share a landmass with a factory?
						if (factoryHere == true)
						{
							//Set this is as our actual objective tile
							distanceFromMinionToObjective = rangeToBeach;
							selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);

						}
					}
				}
			}
		}
	}

	//Call the short term pathfinder and return its result. 999 indicates trouble.
	return findClosestTileToObjective(selectedMinionRecord);

}

//Assuming a minion has a valid objective tile, finds the closest tile to that objective, and sets the potentialMoveTile to that tile.
//Checks for ranged, transport, etc, to keep them away from enemies.
int compie::findClosestTileToObjective(compieMinionRecord* selectedMinionRecord)
{
	int bestDistanceTileToObjective = 999;

	//If we couldn't find an objective, return with -1.
	if (selectedMinionRecord->objectiveTile == NULL || selectedMinionRecord->recordedMinion->locationX == -1 || selectedMinionRecord->recordedMinion->locationY == -1)
	{
		return -1;
	}


	//On that objective tile, and search for the closest tile in our local area, that is within range.
	//Currently this means compie will not get "trapped". Because it will use true pathmap builder, buildPath.
	masterBoardPointer->buildTerrainOnlyPathMap(true, selectedMinionRecord->objectiveTile->locationX, selectedMinionRecord->objectiveTile->locationY, masterBoardPointer->cursor.selectMinionPointer);

	//This offsets by adding the cost to move to the objective tile, and decreases by the amount to get to that interm. square.
	//Turning compiePathMatrix into cost from interm. to objective tile.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			masterBoardPointer->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion += selectedMinionRecord->objectiveTile->consultMovementChart(masterBoardPointer->cursor.selectMinionPointer->type);
			masterBoardPointer->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion -= masterBoardPointer->Board[x][y].consultMovementChart(masterBoardPointer->cursor.selectMinionPointer->type);
		}
	}

	//Now we set a new search box that is much smaller. 
	//This is for actual tiles we can move to, that are closest to the enemy tile we're targeting.

	Cursor* myCursor = &(masterBoardPointer->cursor);
	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > masterBoardPointer->BOARD_WIDTH)
	{
		maxX = masterBoardPointer->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > masterBoardPointer->BOARD_HEIGHT)
	{
		maxY = masterBoardPointer->BOARD_HEIGHT;
	}


	bool transportFound = false;
	for (int x = minX; x < maxX && transportFound == false; x++)
	{
		for (int y = minY; y < maxY && transportFound == false; y++)
		{

			//First a minion can try to load into a transport that is on the x,y square, which has room.
			//Must be infantry for small transport too.		
			//Must be same team, too
			if (masterBoardPointer->Board[x][y].hasMinionOnTop == true
				&& masterBoardPointer->Board[x][y].minionOnTop->team == masterBoardPointer->playerFlag
				&& masterBoardPointer->Board[x][y].withinRange == true
				&& selectedMinionRecord->recordedMinion->domain == land
				&& ((masterBoardPointer->Board[x][y].minionOnTop->specialtyGroup == smallTransport && masterBoardPointer->Board[x][y].minionOnTop->firstMinionBeingTransported == NULL && selectedMinionRecord->recordedMinion->specialtyGroup == infantry)
					|| (masterBoardPointer->Board[x][y].minionOnTop->specialtyGroup == largeTransport && (masterBoardPointer->Board[x][y].minionOnTop->firstMinionBeingTransported == NULL || masterBoardPointer->Board[x][y].minionOnTop->secondMinionBeingTransported == NULL))))
			{
				//Now check that transport is closer to objective than minion currently is - don't want to move away from objective
				int distanceFromTransportToObjective = masterBoardPointer->computeDistance(x, selectedMinionRecord->objectiveTile->locationX, y, selectedMinionRecord->objectiveTile->locationY);
				int distanceFromMinionToObjective = masterBoardPointer->cursor.selectMinionPointer->terrainOnlyPathMap[selectedMinionRecord->recordedMinion->locationX][selectedMinionRecord->recordedMinion->locationY].distanceFromMinion;

				//Give transport a little boost distance-wise, since it's faster.
				//Can only do "tactical" pickup with air/land, for sea it has to be 20 away
				if (distanceFromTransportToObjective - 20 <= distanceFromMinionToObjective || (distanceFromTransportToObjective - 2 <= distanceFromMinionToObjective && masterBoardPointer->Board[x][y].minionOnTop->domain != sea))
				{
					selectedMinionRecord->potentialMoveTile = &(masterBoardPointer->Board[x][y]);
					bestDistanceTileToObjective = distanceFromTransportToObjective;
					transportFound = true;	//Stop searching within
				}
			}
			else
				//Tile must be free of minions and within range
				//Currently prevents compie from getting trapped on strat. move.
				if (masterBoardPointer->Board[x][y].hasMinionOnTop == false && masterBoardPointer->Board[x][y].withinRange == true)
				{
					//If this is a transport or ranged, tile must be free of minions and within range   
					//Either we are on the board's edge, or the adjacent tile has a friendly minion, or no minion
					if ((selectedMinionRecord->recordedMinion->rangeType != rangedFire && selectedMinionRecord->recordedMinion->specialtyGroup != largeTransport && selectedMinionRecord->recordedMinion->specialtyGroup != smallTransport)
						|| (
						(x < 1 || (masterBoardPointer->Board[x - 1][y].hasMinionOnTop == false || masterBoardPointer->Board[x - 1][y].minionOnTop->team == compiePlayerFlag))
							&& (y < 1 || (masterBoardPointer->Board[x][y - 1].hasMinionOnTop == false || masterBoardPointer->Board[x][y - 1].minionOnTop->team == compiePlayerFlag))
							&& (x == masterBoardPointer->BOARD_WIDTH - 1 || (masterBoardPointer->Board[x + 1][y].hasMinionOnTop == false || masterBoardPointer->Board[x + 1][y].minionOnTop->team == compiePlayerFlag))
							&& (y == masterBoardPointer->BOARD_HEIGHT - 1 || (masterBoardPointer->Board[x][y + 1].hasMinionOnTop == false || masterBoardPointer->Board[x][y + 1].minionOnTop->team == compiePlayerFlag))
							)
						)
					{
						//If the current tile is closest to the potentialObjectiveTile (NOT THE MINION!).
						int rangeBetweenTileAndObjective = masterBoardPointer->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (((selectedMinionRecord->recordedMinion->specialtyGroup == largeTransport || selectedMinionRecord->recordedMinion->specialtyGroup == smallTransport)
							&& (rangeBetweenTileAndObjective >= 0 && rangeBetweenTileAndObjective < bestDistanceTileToObjective))
							|| (rangeBetweenTileAndObjective > 0 && rangeBetweenTileAndObjective < bestDistanceTileToObjective))
						{
							//If transport, the objective tile is the drop point, so range can (And ideally should be) zero
							selectedMinionRecord->potentialMoveTile = &(masterBoardPointer->Board[x][y]);
							bestDistanceTileToObjective = rangeBetweenTileAndObjective;
						}

					}
				}
		}
	}

	//If the minion cannot move for some reason (Too many friendly units, etc) This should return 999
	return bestDistanceTileToObjective;

}

//Land transport - Look for closest enemy property/minion on this landmass. Move torwards that.
//Air transport - Look for closest enemy property, regardless of landmass. Move towards that.
//Sea transport - look for closest beach/port which is on same landmass as enemy property, which is not this landmass.
int compie::transportSearchForDrop(compieMinionRecord* selectedMinionRecord)
{
	int distanceFromMinionToObjective = 999;

	if (masterBoardPointer->cursor.selectMinionPointer == NULL || masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported == NULL)
		return -1;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;

	//If land transport, search for closest enemy minion or property.
	if (myMinion->domain == land)
	{
		//Now go through the whole board.
		for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
		{
			for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
			{
				bool throwaway = false;
				int throwAwayInt = 0;
				bool TestRun = true;

				//If the current tile contains a non-friendly minion, see if transported minion can attack them (damagedealt > 0)
				//This allows us to bypass air units, for instance.
				if (masterBoardPointer->Board[x][y].hasMinionOnTop == true && masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag
					&& masterBoardPointer->calculateDamageDealt(masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported, masterBoardPointer->Board[x][y].minionOnTop, throwaway, throwAwayInt, TestRun) > 0)
				{
					//If it is closer than what we previously had targeted, set it as the objective.
					//Must also be valid, ie. not -1. Can actually get there!
					int rangeToEnemy = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeToEnemy != -1 && rangeToEnemy < distanceFromMinionToObjective)
					{
						distanceFromMinionToObjective = rangeToEnemy;
						selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
					}
				}
				else
					//Other option is it's a property tile, not ours, and not being capped by our units (No one friendly on top).
					//Also, we must be transporting infantry for this option.
					if (masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported->specialtyGroup == infantry && masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != masterBoardPointer->playerFlag
						&& (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag))
					{
						int rangeToProp = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToProp != -1 && rangeToProp < distanceFromMinionToObjective)
						{
							distanceFromMinionToObjective = rangeToProp;
							selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
						}
					}

			}
		}
	}
	else
		if (myMinion->domain == air)
		{
			//Now go through the whole board.
			for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
			{
				for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
				{
					//If it's a property tile, not ours, and not being capped by our units (No one friendly on top).
					if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != masterBoardPointer->playerFlag
						&& (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag))
					{
						int rangeToProp = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToProp != -1 && rangeToProp < distanceFromMinionToObjective)
						{
							distanceFromMinionToObjective = rangeToProp;
							selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
						}
					}

				}
			}

		}
		else    //Must be sea transport
		{
			//Now go through the whole board.
			for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
			{
				for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
				{
					//If it's a beach/port that shares a landmass with at least one enemy property
					//Must also be unoccupied, or is this minion.
					if ((masterBoardPointer->Board[x][y].symbol == '*' || masterBoardPointer->Board[x][y].symbol == 'P')
						&& (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop == myMinion))
					{
						int rangeToBeach = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToBeach != -1 && rangeToBeach < distanceFromMinionToObjective)
						{
							//Search board for factory on this landmass
							bool factoryHere = false;
							for (int i = 0; i < masterBoardPointer->BOARD_WIDTH; i++)
							{
								if (factoryHere == true)
									break;

								for (int j = 0; j < masterBoardPointer->BOARD_HEIGHT; j++)
								{
									//Enemy controlled property on same landmass
									if (masterBoardPointer->Board[i][j].checkForProperty() == true && masterBoardPointer->Board[i][j].controller != masterBoardPointer->playerFlag
										&& compieLandMassMap.grid[i][j].landMassNumber == compieLandMassMap.grid[x][y].landMassNumber)

									{
										factoryHere = true;
										break;
									}
								}
							}

							//Does it share a landmass with a factory?
							if (factoryHere == true)
							{
								//Set this is as our actual objective tile
								distanceFromMinionToObjective = rangeToBeach;
								selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);

							}
						}
					}

				}
			}
		}


	//Call the short term pathfinder and return its result. 999 indicates trouble.
	return findClosestTileToObjective(selectedMinionRecord);

}

//Modified - clean up with lambda
//Also edited what looks like small bug - was previously checking objectiveTile - 1 , but should have been just objectiveTile, for computeDistance().
//Assumes valid objective and potential move tile.
int compie::transportAttemptDropOff(compieMinionRecord* selectedMinionRecord)
{
	//Check for valid objective and valid selected minion, and must be currently transporting minion
	if (selectedMinionRecord->objectiveTile == NULL || masterBoardPointer->cursor.selectMinionPointer == NULL
		|| masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported == NULL)
		return -1;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;
	selectedMinionRecord->firstDropTile = NULL;
	selectedMinionRecord->secondDropTile = NULL;

	//Search for viable drop point in four adjacent tiles
	int distanceBetweenAdjacentAndObjective = 999;

	//Lambda for each adjacent tile
	auto checkTileForPotentialDropPoint = [&](int inputX, int inputY)
	{
		if (myMinion->domain != sea) //If not sea transport, look for tile within 2 of objective
		{
			//Must also be passable to transported minion.
			int potentialDistance = masterBoardPointer->computeDistance(selectedMinionRecord->objectiveTile->locationX, inputX, selectedMinionRecord->objectiveTile->locationY, inputY);
			if (potentialDistance < 3 && potentialDistance < distanceBetweenAdjacentAndObjective
				&& masterBoardPointer->Board[inputX][inputY].consultMovementChart(masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported->type) < 99)
			{
				distanceBetweenAdjacentAndObjective = potentialDistance;
				selectedMinionRecord->firstDropTile = &masterBoardPointer->Board[inputX][inputY];
			}
		}
		else    //Must be sea transport
		{
			//Is transport on a beach, and shares the landmass with the objective, and the adjacent tile is passable
			if ((masterBoardPointer->Board[myMinion->locationX][myMinion->locationY].symbol == '*' || masterBoardPointer->Board[myMinion->locationX][myMinion->locationY].symbol == 'P')
				&& compieLandMassMap.grid[inputX][inputY].landMassNumber == compieLandMassMap.grid[selectedMinionRecord->objectiveTile->locationX][selectedMinionRecord->objectiveTile->locationY].landMassNumber
				&& masterBoardPointer->Board[inputX][inputY].consultMovementChart(masterBoardPointer->cursor.selectMinionPointer->firstMinionBeingTransported->type) < 99)
			{
				if (selectedMinionRecord->firstDropTile == NULL)
					selectedMinionRecord->firstDropTile = &masterBoardPointer->Board[inputX][inputY];
				else
					if (selectedMinionRecord->secondDropTile == NULL && myMinion->secondMinionBeingTransported != NULL)
						selectedMinionRecord->secondDropTile = &masterBoardPointer->Board[inputX][inputY];

			}
		}
	};

	if (myMinion->locationX > 0)
	{
		checkTileForPotentialDropPoint(myMinion->locationX - 1, myMinion->locationY);
	}
	//... other tile locations
	if (myMinion->locationY > 0)
	{
		checkTileForPotentialDropPoint(myMinion->locationX, myMinion->locationY - 1);
	}

	if (myMinion->locationX < masterBoardPointer->BOARD_WIDTH - 1)
	{
		checkTileForPotentialDropPoint(myMinion->locationX + 1, myMinion->locationY);
	}
	//... other tile locations
	if (myMinion->locationY < masterBoardPointer->BOARD_HEIGHT - 1)
	{
		checkTileForPotentialDropPoint(myMinion->locationX, myMinion->locationY + 1);
	}

	return distanceBetweenAdjacentAndObjective;

}

//MODIFIED - Added check to give higher priority to advancing on its own, vs taking transport
//Depends on minion health. If <5, looks for repair location.
//Otherwise, searches the map, first checking for an enemy minion to attack, and then checking for enemy property to capture, if infantry type.
//Now assumes a pathMap was built for this- ie minion was selected.
int compie::strategicAdvance(compieMinionRecord* selectedMinionRecord)
{
	int distanceFromMinionToObjective = 999;
	bool currentObjectiveIsTransport = false;

	if (masterBoardPointer->cursor.selectMinionPointer == NULL)
		return -1;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;

	//Now go through the whole board.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{

			bool throwaway = false;
			int throwAwayInt = 0;
			bool TestRun = true;
			//If the current tile contains a non-friendly minion this minion can attack them (damagedealt > 0)
			//This allows us to bypass air units, for instance.
			if (masterBoardPointer->Board[x][y].hasMinionOnTop == true && masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag
				&& masterBoardPointer->calculateDamageDealt(masterBoardPointer->cursor.selectMinionPointer, masterBoardPointer->Board[x][y].minionOnTop, throwaway, throwAwayInt, TestRun) > 0)
			{
				//If it is closer than what we previously had targeted, set it as the objective.
				//Must also be valid, ie. not -1. Can actually get there!
				//Also checks if current objective is transport - will always choose to advance towards an objective it can get to on its own, rather than take a transport there.
				int rangeToEnemy = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
				if (rangeToEnemy != -1 && (rangeToEnemy < distanceFromMinionToObjective || currentObjectiveIsTransport == true))
				{
					distanceFromMinionToObjective = rangeToEnemy;
					currentObjectiveIsTransport = false;
					selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
				}
			}
			else
				//Other option is it's a property tile, not ours, and not being capped by our units (No one friendly on top).
				//Also, we must be infantry for this option.
				//Also checks if current objective is transport - will always choose to advance towards an objective it can get to on its own, rather than take a transport there.
				if (masterBoardPointer->cursor.selectMinionPointer->specialtyGroup == infantry && masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != masterBoardPointer->playerFlag
					&& (masterBoardPointer->Board[x][y].hasMinionOnTop == false || masterBoardPointer->Board[x][y].minionOnTop->team != masterBoardPointer->playerFlag))
				{
					int rangeToProp = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeToProp != -1 && (rangeToProp < distanceFromMinionToObjective || currentObjectiveIsTransport == true))
					{
						distanceFromMinionToObjective = rangeToProp;
						currentObjectiveIsTransport = false;
						selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
					}
				}
				else //Finally, the other option is a friendly large transport within valid range, that has room.
						//Can't check if it has the right flag, because too hard to check minionRoster from the minion itself.
						//This should always be lower priority to prevent minions from re-loading onto a transport that just dropped them off.
					if (masterBoardPointer->Board[x][y].hasMinionOnTop == true && masterBoardPointer->Board[x][y].minionOnTop->team == masterBoardPointer->playerFlag
						&& masterBoardPointer->Board[x][y].minionOnTop->specialtyGroup == largeTransport
						&& (masterBoardPointer->Board[x][y].minionOnTop->firstMinionBeingTransported == NULL || masterBoardPointer->Board[x][y].minionOnTop->secondMinionBeingTransported == NULL))
					{
						int rangeToTransport = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToTransport != -1 && rangeToTransport < distanceFromMinionToObjective)
						{
							distanceFromMinionToObjective = rangeToTransport;
							currentObjectiveIsTransport = true;
							selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
						}
					}

		}
	}

	//Call the short term pathfinder and return its result. 999 indicates trouble.
	return findClosestTileToObjective(selectedMinionRecord);

}

//If minion is damaged or air and needs fuel, looks for friendly property capable of supporting, and either moves onto it, or moving towards it.
int compie::strategicWithdraw(compieMinionRecord* selectedMinionRecord)
{
	int distancefromMinionToObjective = 999;
	int bestDistanceTileToObjective = 999;

	if (masterBoardPointer->cursor.selectMinionPointer == NULL)
		return -1;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;

	//Now go through the whole board.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			//If this is friendly controlled property capable of repairing//supplying our unit
			//We do care if occupied currently
			if (masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag
				&& masterBoardPointer->consultMinionCostChart(myMinion->type, masterBoardPointer->Board[x][y].symbol) != -1
				&& masterBoardPointer->Board[x][y].hasMinionOnTop == false)
			{	//Need to make sure not factory = this is jamming up production
				if (masterBoardPointer->Board[x][y].symbol != 'h') {
					//If this is the closest objective so far, set as objective.
					int rangeBetweenMinionAndTile = masterBoardPointer->cursor.selectMinionPointer->apparentPathMap[x][y].distanceFromMinion;
					if (rangeBetweenMinionAndTile != -1 && distancefromMinionToObjective > rangeBetweenMinionAndTile)
					{
						selectedMinionRecord->objectiveTile = &masterBoardPointer->Board[x][y];
						distancefromMinionToObjective = rangeBetweenMinionAndTile;
					}
				}
			}

		}
	}


	//Call the short term pathfinder and return its result. 999 indicates trouble.
	return findClosestTileToObjective(selectedMinionRecord);

}

//Checks surrounding tiles for best valued enemy.
//This function WILL immediately replace whatever the previous objective was as long as its suitability score is higher than what was passed in.
int compie::checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord)
{
	double attackerDamageDealt = 0;
	double defenderCounterAttackDamageDealt = 0;

	//Within each of four adjacent tiles:
	//Calculate the damage our minion would do to them, as well as the counterattack damage:


	checkSingleTileForCombatValue(currentX, currentY, currentX - 1, currentY, myCursor, relativeSuitabilityScore, selectedMinionRecord);

	checkSingleTileForCombatValue(currentX, currentY, currentX + 1, currentY, myCursor, relativeSuitabilityScore, selectedMinionRecord);

	checkSingleTileForCombatValue(currentX, currentY, currentX, currentY - 1, myCursor, relativeSuitabilityScore, selectedMinionRecord);

	checkSingleTileForCombatValue(currentX, currentY, currentX, currentY + 1, myCursor, relativeSuitabilityScore, selectedMinionRecord);

	return 0;
}

int compie::checkSingleTileForCombatValue(int attackerX, int attackerY, int defenderX, int defenderY, Cursor* myCursor, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord)
{

	bool willAmmoBeUsed = false;
	bool ignoreLimitations = true;

	//Track primary vs secondary weapon used and use to determine if this single tile combo can actually work. Direct attack must only use 2, while ranged uses 1, for hybrid.
	int attackerWeaponUsed = 0;
	int defenderWeaponUsed = 0;

	//Any return of 1 will not have touched relativeSuitability or the compieRoster objective tiles so we're okay to just return.
	if (attackerX < 0 || defenderX < 0 || attackerY < 0 || defenderY < 0)
	{
		return 1;
	}

	if (attackerX >= masterBoardPointer->BOARD_WIDTH || defenderX >= masterBoardPointer->BOARD_WIDTH || attackerY >= masterBoardPointer->BOARD_HEIGHT || defenderY >= masterBoardPointer->BOARD_HEIGHT)
	{
		return 1;
	}

	if (masterBoardPointer->Board[defenderX][defenderY].hasMinionOnTop == false || masterBoardPointer->Board[defenderX][defenderY].withinVision[myCursor->selectMinionPointer->team] != true)
	{
		return 1;
	}

	if (masterBoardPointer->Board[defenderX][defenderY].minionOnTop->team == selectedMinionRecord->recordedMinion->team)
	{
		return 1;
	}



	int attackerCost = 0;
	int defenderCost = 0;

	//Determine minion costs based on types:
	//Uses ~ which gets cost regardless of base below us.
	attackerCost = masterBoardPointer->consultMinionCostChart(myCursor->selectMinionPointer->type, '~');
	defenderCost = masterBoardPointer->consultMinionCostChart(masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type, '~');

	//For infantry, double the defenderCost to double overall score for damage dealt to it.
	if (masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type == "Infantry" || masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type == "Specialist"
		|| masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type == "Cavalry" || masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type == "Insurgent"
		|| masterBoardPointer->Board[defenderX][defenderY].minionOnTop->type == "Engineer")
		defenderCost *= compieInfantryAttackBonus;



	int attackerDamageDealt = masterBoardPointer->calculateDamageDealt(myCursor->selectMinionPointer, masterBoardPointer->Board[defenderX][defenderY].minionOnTop, willAmmoBeUsed, attackerWeaponUsed, ignoreLimitations);
	int defenderCounterAttackDamageDealt = masterBoardPointer->calculateDamageDealt(masterBoardPointer->Board[defenderX][defenderY].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed, defenderWeaponUsed, ignoreLimitations)
		* (masterBoardPointer->Board[defenderX][defenderY].minionOnTop->health - attackerDamageDealt) / 100;

	if (defenderCounterAttackDamageDealt < 0)
		defenderCounterAttackDamageDealt = 0;

	//If ranged unit, defender  cannot counterattack.
	//If defender is ranged unit, also cannot counterattack
	//If attacker is hybrid but standing in place for ranged attack (weaponUsed = 1), defender will not counterattack.
	if (myCursor->selectMinionPointer->rangeType == rangedFire
		|| (myCursor->selectMinionPointer->rangeType == hybridRange && attackerWeaponUsed == 1)
		|| masterBoardPointer->Board[defenderX][defenderY].minionOnTop->rangeType == rangedFire)
		defenderCounterAttackDamageDealt = 0;

	//This is the potential "value added" from combat, based on what we might lose vs gain.
	double newRelativeSuitabilityScore = ((attackerDamageDealt * defenderCost) + compieMinionAggressionBonus - (defenderCounterAttackDamageDealt * attackerCost)) / 100;

	//If it's a better score, switch targets.
	if (newRelativeSuitabilityScore > * relativeSuitabilityScore)
	{
		//If either non-hybrid, OR hybrid AND uses the right weapon based on the range (IE primary weapon for ranged attack)
		if (myCursor->selectMinionPointer->rangeType != hybridRange
			|| (myCursor->selectMinionPointer->rangeType == hybridRange && attackerWeaponUsed == 1 && false == isAdjacent(attackerX, defenderX, attackerY, defenderY))
			|| (myCursor->selectMinionPointer->rangeType == hybridRange && attackerWeaponUsed == 2 && true == isAdjacent(attackerX, defenderX, attackerY, defenderY)))
			//Also, defender's damage dealt must be less than 50
			if (defenderCounterAttackDamageDealt < 50)
			{
				*relativeSuitabilityScore = newRelativeSuitabilityScore;
				selectedMinionRecord->potentialMoveTile = &(masterBoardPointer->Board[attackerX][attackerY]);
				selectedMinionRecord->potentialAttackTile = &(masterBoardPointer->Board[defenderX][defenderY]);
			}
	}

	return 0;




}

double compie::findBestValuedEnemyWithinLocalArea(compieMinionRecord* selectedMinionRecord)
{
	Cursor* myCursor = &(masterBoardPointer->cursor);

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;

	//Then determine the search box based on range or movement, based on fire type.
	//For hybrid, use whatever is larger, the move range or the fire range.
	if (myCursor->selectMinionPointer->rangeType == directFire
		|| (myCursor->selectMinionPointer->rangeType == hybridRange && myCursor->selectMinionPointer->movementRange > myCursor->selectMinionPointer->attackRange))
	{
		minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
		maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;

		minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
		maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;

	}
	else
		if (myCursor->selectMinionPointer->rangeType == rangedFire || myCursor->selectMinionPointer->rangeType == hybridRange)
		{
			minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->attackRange;
			maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->attackRange;

			minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->attackRange;
			maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->attackRange;
		}



	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > masterBoardPointer->BOARD_WIDTH)
	{
		maxX = masterBoardPointer->BOARD_WIDTH;
	}
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > masterBoardPointer->BOARD_HEIGHT)
	{
		maxY = masterBoardPointer->BOARD_HEIGHT;
	}

	double relativeSuitabilityScore = 0;
	//Now go through the search area.
	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)
		{
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//Also must be within range
			//Direct fire only
			if ((selectedMinionRecord->recordedMinion->rangeType == directFire || selectedMinionRecord->recordedMinion->rangeType == hybridRange) &&
				(masterBoardPointer->Board[x][y].hasMinionOnTop == false ||
				(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x))
				&& masterBoardPointer->Board[x][y].withinRange == true)
			{
				checkAdjacentTilesForBestValuedEnemy(x, y, myCursor, &relativeSuitabilityScore, selectedMinionRecord);
				//Already checked for closeness in the function itself so no need to do anything else
			}
			//Check each tile if suitable for artillery fire
			//Must be visible
			if (selectedMinionRecord->recordedMinion->rangeType == rangedFire || selectedMinionRecord->recordedMinion->rangeType == hybridRange)
				if (masterBoardPointer->Board[x][y].hasMinionOnTop == true
					&& masterBoardPointer->Board[x][y].withinVision[myCursor->selectMinionPointer->team] == true
					&& myCursor->selectMinionPointer->team != masterBoardPointer->Board[x][y].minionOnTop->team)
				{
					//Must be within range but outside minimum range
					if (myCursor->selectMinionPointer->minAttackRange < masterBoardPointer->computeDistance(myCursor->selectMinionPointer->locationX, x, myCursor->selectMinionPointer->locationY, y)
						&& myCursor->selectMinionPointer->attackRange >= masterBoardPointer->computeDistance(myCursor->selectMinionPointer->locationX, x, myCursor->selectMinionPointer->locationY, y))
						checkSingleTileForCombatValue(myCursor->selectMinionPointer->locationX, myCursor->selectMinionPointer->locationY, x, y, myCursor, &relativeSuitabilityScore, selectedMinionRecord);
				}
		}
	}

	return relativeSuitabilityScore;
}

int compie::findPropertyWithinLocalArea(int* returnX, int* returnY, compieMinionRecord* selectedMinionRecord)
{
	Cursor* myCursor = &(masterBoardPointer->cursor);
	//These variables hold the information of the closest enemy property and range.
	//Start with NULL and 999 to indicate no valid.
	int distanceToClosestEnemyProperty = 999;

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > masterBoardPointer->BOARD_WIDTH)
	{
		maxX = masterBoardPointer->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > masterBoardPointer->BOARD_HEIGHT)
	{
		maxY = masterBoardPointer->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//std::cout << x << " " << y <<std::endl;
			//If the tile is property and is enemy controlled.
			if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != masterBoardPointer->playerFlag)
			{
				//If the current tile DOES not have minion on top (IE we will be able to move there)
				if (masterBoardPointer->Board[x][y].hasMinionOnTop == false)
				{
					//If the current tile is within movement range AND is less distant than current closestEnemyProp.
					//This means the minion has to be "selected", to turn on withinRange flags.
					int rangeToProp = masterBoardPointer->computeDistance(x, myCursor->selectMinionPointer->locationX, y, myCursor->selectMinionPointer->locationY);
					if (masterBoardPointer->Board[x][y].withinRange == true && rangeToProp < distanceToClosestEnemyProperty)
					{
						distanceToClosestEnemyProperty = rangeToProp;
						selectedMinionRecord->potentialMoveTile = &(masterBoardPointer->Board[x][y]);
						selectedMinionRecord->objectiveTile = &(masterBoardPointer->Board[x][y]);
						*returnY = y;
						*returnX = x;
					}

				}
				//Just need to check if property is ours or theirs, and then take it.
			}
		}
	}

	return distanceToClosestEnemyProperty;
}

//Determine whether the minion will attack local enemies, capture property, or move strategically.
//This function assumes the minion is already selected.
int compie::determinePotentialMinionTasking(compieMinionRecord* selectedMinionRecord)
{
	bool tasked = false;

	if (selectedMinionRecord->recordedMinion == NULL || masterBoardPointer->cursor.selectMinionPointer == NULL)
		return 1;

	std::cout << "Determining task for " << selectedMinionRecord->recordedMinion->description << " " << selectedMinionRecord->recordedMinion->seniority << std::endl;

	int returnX = 0;
	int returnY = 0;

	Minion* myMinion = masterBoardPointer->cursor.selectMinionPointer;

	//Reset defaults.
	selectedMinionRecord->tasking = holdPosition;
	selectedMinionRecord->taskingStatus = immediateExecute;

	if (selectedMinionRecord->recordedMinion->type == "Lander")
		std::cout << "Hello" << std::endl;

	//Check for minion status since we're now calling this function from multiple locations
	if (myMinion->status == hasntmovedorfired)
	{

		//If already repairing, keep repairing
		//As long as consultCost doesn't return -1, the minion CAN be repaired there
		//We may still attack guys next to us
		if (myMinion->health < 100
			&& masterBoardPointer->Board[myMinion->locationX][myMinion->locationY].controller == compiePlayerFlag
			&& masterBoardPointer->consultMinionCostChart(myMinion->type, masterBoardPointer->Board[myMinion->locationX][myMinion->locationY].symbol) != -1)
		{
			selectedMinionRecord->tasking = holdPosition;
			selectedMinionRecord->taskingStatus = immediateExecute;
		}
		else
		{
			int distanceToAirbase = -1;
			//Air minions need to accurately assess their chance of reaching a fuel point.
			if (myMinion->domain == air)
			{
				tile* closestAirbase = findClosestAirbase(myMinion);
				if (closestAirbase != NULL)
				{
					distanceToAirbase = masterBoardPointer->computeDistance(closestAirbase->locationX, myMinion->locationX, closestAirbase->locationY, myMinion->locationY);

				}

			}

			//If highly damaged or Air and far from base/needs to refuel soon / out of ammo
			//Far from base means - current fuel less than distance to closest base, including 2 turns of fuel burn (+10) and a fuel margin (+10) to actually make it.
			if (myMinion->health < repairThreshold ||
				(myMinion->domain == air &&
				(myMinion->currentFuel < distanceToAirbase + 20 || myMinion->currentFuel < (myMinion->maxFuel / 3)
					|| (myMinion->currentPriAmmo == 0 && myMinion->maxPriAmmo != -1)  //Actually has pri weapon, and that weapon is out of ammo
					|| (myMinion->currentSecAmmo == 0 && myMinion->maxSecAmmo != -1))))	//Actually has secondary weapon which is out of ammo 
			{
				int distance = strategicWithdraw(selectedMinionRecord);
				if (distance < 999 && distance >= 0)	//Error code is -1
				{
					selectedMinionRecord->tasking = withdraw;
					selectedMinionRecord->taskingStatus = immediateExecute;
					tasked = true;

				}
			}
			else    //If transport
				if (myMinion->specialtyGroup == smallTransport || myMinion->specialtyGroup == largeTransport)
				{
					//If transport who already has a minion, look for suitable drop point.
					if (myMinion->firstMinionBeingTransported != NULL)
					{
						int distance = transportSearchForDrop(selectedMinionRecord);
						if (distance < 999 && distance >= 0)
						{
							selectedMinionRecord->tasking = dropOff;
							selectedMinionRecord->taskingStatus = immediateExecute;
							tasked = true;
						}
					}
					else    //Otherwise, need to try to pickup a new minion.
					{
						int distance = transportSearchForPickup(selectedMinionRecord);
						if (distance < 999 && distance >= 0)
						{
							selectedMinionRecord->tasking = pickUp;
							selectedMinionRecord->taskingStatus = immediateExecute;
							tasked = true;
						}
					}
				}
				else
					//Otherwise
					//If already capturing continue that regardless.
					if (myMinion->isCapturing == true)
					{
						selectedMinionRecord->potentialMoveTile = &masterBoardPointer->Board[myMinion->locationX][myMinion->locationY];
						selectedMinionRecord->objectiveTile = &masterBoardPointer->Board[myMinion->locationX][myMinion->locationY];
						selectedMinionRecord->tasking = captureLocalProperty;
						selectedMinionRecord->taskingStatus = immediateExecute;
						tasked = true;
					}
					else
						if (defendHeadquarters(selectedMinionRecord) == 1)
						{	//Not sure if this is actually happening, may need to add tasking and taskingStatus.
							std::cout << "Headquarters under attack!" << std::endl;

						}
						else		//DefendProperty ensures that defending prop has higher priority than attacking a local minion regardless of cost
							if (defendProperty(selectedMinionRecord) == 1)
							{
								selectedMinionRecord->tasking = defendProp;
								selectedMinionRecord->taskingStatus = immediateExecute;
								tasked = true;
							}
							else
							{
								//Otherwise see if there are enemies in local area within range and suitable to attack.
								double relativeSuitabilityScore = findBestValuedEnemyWithinLocalArea(selectedMinionRecord);
								if (relativeSuitabilityScore > 0 && selectedMinionRecord->potentialAttackTile != NULL)
								{
									selectedMinionRecord->tasking = attackLocalMinion;
									selectedMinionRecord->taskingStatus = immediateExecute;
									tasked = true;
								}
								else
								{
									//If capable of capping properties, attempt to do so.
									int distance = findPropertyWithinLocalArea(&returnX, &returnY, selectedMinionRecord);
									if (distance < 999 && selectedMinionRecord->potentialMoveTile != NULL
										&& masterBoardPointer->cursor.selectMinionPointer->captureCapable == true)
									{
										selectedMinionRecord->tasking = captureLocalProperty;
										selectedMinionRecord->taskingStatus = immediateExecute;
										tasked = true;

									}
									else
									{
										int distance = strategicAdvance(selectedMinionRecord);
										if (distance < 999 && distance >= 0)	//Error code is -1
										{
											selectedMinionRecord->tasking = advance;
											selectedMinionRecord->taskingStatus = immediateExecute;
											tasked = true;
										}
									}
								}
							}


		}

	}

	//If we have a valid task from the above checks, return for execution.
	if (tasked == true)
		return 1;

	//If we didn't get a valid task, or if this function was called for a minion that has already moved, we will potentially do some "post-move" tasks.

	//If carrying a minion, check for potential drop off.
	if (selectedMinionRecord->recordedMinion->firstMinionBeingTransported != NULL)
	{
		int rangeToObjective = transportAttemptDropOff(selectedMinionRecord);

		//As long as at least one minion is to be dropped, set tasking to such.
		//We will attempt to drop minions on two tiles within the actual execution, if possible.
		if (selectedMinionRecord->firstDropTile != NULL)
		{
			selectedMinionRecord->tasking = dropOffNow;
			selectedMinionRecord->taskingStatus = immediateExecute;
		}

	}
	else	//Not a transport
	{
		//Check adjacent tiles if we want to attack them, even if we stood still.
		//This also will get checked if we strategic moved, since determineTasking is now called after strat move.
		double suitabilityScore = 0;

		checkAdjacentTilesForBestValuedEnemy(selectedMinionRecord->recordedMinion->locationX, selectedMinionRecord->recordedMinion->locationY,
			&masterBoardPointer->cursor, &suitabilityScore, selectedMinionRecord);

		if (selectedMinionRecord->potentialAttackTile != NULL)
		{
			selectedMinionRecord->tasking = attackLocalMinion;
			selectedMinionRecord->taskingStatus = immediateExecute;
		}
		else
		{
			//If we somehow got here, just sit still.
			selectedMinionRecord->tasking = holdPosition;
			selectedMinionRecord->taskingStatus = immediateExecute;
		}
	}


	return 0;
}


int compie::executeMinionTasks(compieMinionRecord* selectedMinionRecord)
{
	int gameEndCode = 0;

	if (selectedMinionRecord->recordedMinion == NULL || masterBoardPointer->cursor.selectMinionPointer == NULL)
		return 1;

	std::cout << "Moving " << selectedMinionRecord->recordedMinion->description << " " << selectedMinionRecord->recordedMinion->seniority << std::endl;

	int whoIsWatching = -1;		//-1 Is "default" meaning in a standard multiplayer game, nothing is printed during compie turn. It's hidden from view.

	//If we're in debug mode, give player vision over everything.
	if (menuPointer->editorMode == true)
	{
		whoIsWatching = 0;

	}
	else
		//If single player make sure player 1 can see, and print for them.
		if (masterBoardPointer->isItSinglePlayerGame == true)
		{
			whoIsWatching = 1;

		}

	if (selectedMinionRecord->tasking == attackLocalMinion || selectedMinionRecord->tasking == defendHQ || selectedMinionRecord->tasking == defendProp)
	{
		//Move cursor
		if (masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY) == 1)
		{
			std::cout << "fail move" << std::endl;
		}


		//moveMinion needs to contain all the status elements too.
		int moveResult = masterBoardPointer->moveMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord, InputLayer, whoIsWatching);

		if (moveResult == 0)
		{
			//The move automatically deselects. Thus reselect.
			masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());
			//Place cursor on tile being attacked.
			masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialAttackTile->locationX, selectedMinionRecord->potentialAttackTile->locationY);
			gameEndCode = masterBoardPointer->attackMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY(), InputLayer, whoIsWatching);

		}

	}

	if (selectedMinionRecord->tasking == captureLocalProperty)
	{
		//Move cursor
		if (masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY) == 1)
		{
			std::cout << "fail move" << std::endl;
		}

		//moveMinion needs to contain all the status elements too.
		int moveResult = masterBoardPointer->moveMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord, InputLayer, whoIsWatching);

		//Capture enemy property
		//The move automatically deselects. Thus reselect.
		if (moveResult == 0)
		{
			masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());
			(void)masterBoardPointer->captureProperty(selectedMinionRecord->objectiveTile, masterBoardPointer->cursor.selectMinionPointer, InputLayer, whoIsWatching);

		}

	}

	if (selectedMinionRecord->tasking == advance && selectedMinionRecord->potentialMoveTile->hasMinionOnTop == true
		&& (selectedMinionRecord->potentialMoveTile->minionOnTop->specialtyGroup == smallTransport || selectedMinionRecord->potentialMoveTile->minionOnTop->specialtyGroup == largeTransport)
		)
	{
		//Move cursor
		masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY);

		//Attempt to pick up on that square
		masterBoardPointer->pickUpMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord, InputLayer, whoIsWatching);
	}
	else
		if (selectedMinionRecord->tasking == advance || selectedMinionRecord->tasking == withdraw
			|| selectedMinionRecord->tasking == pickUp || selectedMinionRecord->tasking == dropOff)
		{
			//Move cursor
			masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY);

			//moveMinion needs to contain all the status elements too.
			masterBoardPointer->moveMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord, InputLayer, whoIsWatching);

		}

	if (selectedMinionRecord->tasking == holdPosition)
	{
		//Do nothing
	}


	//If advance or withdraw, stay selected and re-determine tasking to check for adjacent enemy to attack.
	//Also attempts to find drop point if minion is transport and carrying a guy
	if (selectedMinionRecord->tasking == advance || selectedMinionRecord->tasking == withdraw || selectedMinionRecord->tasking == dropOff)
	{
		masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());
		determinePotentialMinionTasking(selectedMinionRecord);
	}

	//Then deselect and maybe carry out orders.
	masterBoardPointer->deselectMinion();

	//Attempt to drop if the time has come for that.
	//This means not just drop off, but also has legit tile to drop.
	if (selectedMinionRecord->tasking == dropOffNow && selectedMinionRecord->firstDropTile != NULL)
	{
		//Reselect
		masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());

		//Move cursor to objective tile
		masterBoardPointer->cursor.relocate(selectedMinionRecord->firstDropTile->locationX, selectedMinionRecord->firstDropTile->locationY);

		//Attempt drop off
		masterBoardPointer->dropOffMinion();

		//Now check if there is another minion to drop
		if (selectedMinionRecord->secondDropTile != NULL)
		{
			//Move cursor back to transport
			masterBoardPointer->cursor.relocate(selectedMinionRecord->recordedMinion->locationX, selectedMinionRecord->recordedMinion->locationY);

			//Reselect
			masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());

			//Move to next drop point
			masterBoardPointer->cursor.relocate(selectedMinionRecord->secondDropTile->locationX, selectedMinionRecord->secondDropTile->locationY);

			//Attempt drop off
			masterBoardPointer->dropOffMinion();
		}
	}
	else
		//Check for adjacent minion to attack
		if (selectedMinionRecord->tasking == attackLocalMinion)
		{
			masterBoardPointer->selectMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY());
			//Place cursor on tile being attacked.
			masterBoardPointer->cursor.relocate(selectedMinionRecord->potentialAttackTile->locationX, selectedMinionRecord->potentialAttackTile->locationY);
			if (gameEndCode != 999)
				masterBoardPointer->attackMinion(masterBoardPointer->cursor.getX(), masterBoardPointer->cursor.getY(), InputLayer, whoIsWatching);
		}

	masterBoardPointer->setVisionField(whoIsWatching);
	masterBoardPointer->checkWindow();
	InputLayer->printScreen(masterBoardPointer, whoIsWatching, false);

	//Delay after printing;
	sf::Clock timer;

	timer.restart();
	sf::Time elapsedTime;

	while (elapsedTime.asSeconds() < float(0.5 / InputLayer->speedFactor))
	{
		elapsedTime = timer.getElapsedTime();
	}

	//Attempt to stop game freezing by polling for one event.
	//Flush event queue to clear out "Enter" and other rifraf
	if (InputLayer->inputLayerWindow != NULL)
	{
		sf::Event throwAwayEvent;
		while (InputLayer->inputLayerWindow->pollEvent(throwAwayEvent));
	}

	//Regardless of the tasking, it has now been executed.
	selectedMinionRecord->taskingStatus = taskingExecuted;


	if (gameEndCode == 999)
	{
		return gameEndCode;
	}
	else
	{
		return 1;
	}

}

int compie::buildMinionScoreMap(compieMinionRecord* selectedMinionRecord)
{




	return 0;		//Success
}


int compie::takeMyTurn()
{
	std::cout << "Compie player " << masterBoardPointer->playerFlag << "'s " << InputLayer->MainMenu->gameTurn << " turn" << std::endl;
	int whoIsWatching = -1;		//-1 Is "default" meaning in a standard multiplayer game, nothing is printed during compie turn. It's hidden from view.

	//If we're in debug mode, give player vision over everything.
	if (menuPointer->editorMode == true)
	{
		whoIsWatching = 0;

	}
	else
		//If single player make sure player 1 can see, and print for them.
		if (masterBoardPointer->isItSinglePlayerGame == true)
		{
			whoIsWatching = 1;

		}

	masterBoardPointer->upkeep(InputLayer, whoIsWatching);

	statusUpdate();

	bool allMinionsHaveMoved = false;
	int gameEndCode = 0;

	//Very first move: Artillery fire at targets of opportunity.
	for (int i = 0; i < compieMinionRoster.size(); i++)
	{
		if (compieMinionRoster[i] != NULL && compieMinionRoster[i]->recordedMinion->rangeType == rangedFire && compieMinionRoster[i]->taskingStatus == (awaitingTasking))
		{
			//Move cursor, then select minion, then determine tasks
			masterBoardPointer->cursor.relocate(compieMinionRoster[i]->recordedMinion->locationX, compieMinionRoster[i]->recordedMinion->locationY);
			masterBoardPointer->selectMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord);
			determinePotentialMinionTasking(compieMinionRoster[i]);

			if (compieMinionRoster[i]->tasking == attackLocalMinion)
			{
				gameEndCode = executeMinionTasks(compieMinionRoster[i]);
			}
			else //If we couldn't find someone to shoot, reset to awaiting tasking, and move on.
			{
				compieMinionRoster[i]->taskingStatus = awaitingTasking;
				compieMinionRoster[i]->potentialMoveTile = NULL;
			}
			masterBoardPointer->deselectMinion();
		}

		if (gameEndCode == 999)
			return 999;
	}

	int waveIterator = 0;		//Need to eliminate this

	//Go through minionRecord and determine tasking.
	while (allMinionsHaveMoved == false && waveIterator < 4)
	{
		//For a single iterationa assume all minions moved until proven otherwise
		allMinionsHaveMoved = true;

		//Still need to defendHQ.

		for (int i = 0; i < compieMinionRoster.size(); i++)
		{
			//Make sure minion hasn't already done tasking, and also that it's not a landmine.
			if (compieMinionRoster[i] != NULL && compieMinionRoster[i]->taskingStatus != taskingExecuted && compieMinionRoster[i]->recordedMinion->type != "Landmine")

			{
				//Move cursor, then select minion
				masterBoardPointer->cursor.relocate(compieMinionRoster[i]->recordedMinion->locationX, compieMinionRoster[i]->recordedMinion->locationY);

				masterBoardPointer->selectMinion(masterBoardPointer->cursor.XCoord, masterBoardPointer->cursor.YCoord);


				//If awaitingTasking, determine tasking.
				if (compieMinionRoster[i]->taskingStatus == (awaitingTasking))
				{
					determinePotentialMinionTasking(compieMinionRoster[i]);
				}

				//If tasking is immediateExecute, do it now.
				//Should already be selected
				if (compieMinionRoster[i]->taskingStatus == (immediateExecute))
				{
					gameEndCode = executeMinionTasks(compieMinionRoster[i]);
				}
				if (gameEndCode == 999)
					return 999;

				//If a single minion has not finished its tasking, set to false
				if (compieMinionRoster[i]->taskingStatus != taskingExecuted)
				{
					allMinionsHaveMoved = false;
				}

				//If we are on Wave 3, execute, regardless of priority.
				if (waveIterator == 3)
				{
					gameEndCode = executeMinionTasks(compieMinionRoster[i]);
				}
				if (gameEndCode == 999)
					return 999;

				//masterBoardPointer->deselectMinion(); Don't think this is needed

			}
		}


		waveIterator++;
	}


	//Still need to implement non-immediateExecute taskingStatus

	if (masterBoardPointer->playerRoster[compiePlayerFlag].stillAlive == true)
	{

		newDetermineProduction();

		//Have to always keep an autosave!
		menuPointer->saveGameData(".\\savegames\\Auto_save", masterBoardPointer);


		int incrementGameTurn = masterBoardPointer->endTurn(InputLayer);

		//This is convoluted but if only compies are playing for us to view, we want to skip past the "waiting for next player" screen
		//This is overriding the endTurn function.
		bool allComputerPlayers = true;
		for (int i = 1; i < masterBoardPointer->NUMBEROFPLAYERS + 1; i++)
		{
			if (masterBoardPointer->playerRoster[i].playerType != computerPlayer)
				allComputerPlayers = false;
		}
		if (allComputerPlayers == false)
		{
			InputLayer->status = waitingForNextLocalPlayer;
		}
		else
		{
			InputLayer->status = gameBoard;
			menuPointer->skipOneInput = true;
		}
		//End workaround here	///////////


		//If we advanced a gameTurn, mainMenu will keep track of it.
		menuPointer->gameTurn += incrementGameTurn;

	}

	//Regardless of if this was first turn, set to false. Now it's not!
	menuPointer->veryFirstTurn = false;

	return 1;
}

//Modified: Added lambdas to clean up purchase method
//Right now this is actually buying units too.
int compie::determineProduction()
{

	int totalFactoriesLeft = 0;
	int numberOfPorts = 0;
	int numberOfTanks = 0;
	int numberOfInfantry = 0;
	int numberOfCavalry = 0;
	int numberOfSpecialists = 0;
	int numberOfRanged = 0;
	int numberOfBombers = 0;
	int numberOfAA = 0;
	int numberOfSeaCombatants = 0;
	int numberOfEnemyAirUnits = 0;
	int numberOfInfantryTransports = 0;
	int numberOfLanders = 0;

	//Go through map and find all of our factories for totalFactories
	//Also count ports for naval construction
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			//If the current tile is our factory
			if (masterBoardPointer->Board[x][y].symbol == 'h' && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
			{
				totalFactoriesLeft++;
			}
			//If the current tile is our port
			if (masterBoardPointer->Board[x][y].symbol == 'P' && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
			{
				numberOfPorts++;
			}
		}
	}

	//Go through minion Roster. Find various unit totals.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{

		if (masterBoardPointer->minionRoster[i] != NULL && masterBoardPointer->minionRoster[i]->team == masterBoardPointer->playerFlag)
		{

			if (masterBoardPointer->minionRoster[i]->type == "Infantry")
			{
				numberOfInfantry++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Armor" || masterBoardPointer->minionRoster[i]->type == "Heavy_Armor")
			{
				numberOfTanks++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Recon")
			{
				numberOfCavalry++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Specialist" || masterBoardPointer->minionRoster[i]->type == "Engineer")
			{
				numberOfSpecialists++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Artillery" || masterBoardPointer->minionRoster[i]->type == "Rocket_Artillery")
			{
				numberOfRanged++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Attack_Copter" || masterBoardPointer->minionRoster[i]->type == "Bomber")
			{
				numberOfBombers++;

			}
			if (masterBoardPointer->minionRoster[i]->type == "Anti-Aicraft" || masterBoardPointer->minionRoster[i]->type == "Interceptor")
			{
				numberOfAA++;
			}

			if (masterBoardPointer->minionRoster[i]->type == "Gunboat" || masterBoardPointer->minionRoster[i]->type == "Cruiser" || masterBoardPointer->minionRoster[i]->type == "Battleship")
			{
				numberOfSeaCombatants++;
			}

			if (masterBoardPointer->minionRoster[i]->type == "APC" || masterBoardPointer->minionRoster[i]->type == "IFV" || masterBoardPointer->minionRoster[i]->type == "Transport_Copter")
			{
				numberOfInfantryTransports++;
			}

			if (masterBoardPointer->minionRoster[i]->type == "Lander")
			{
				numberOfLanders++;
			}

		}

		//Look at enemy units and count total air units
		//Must be able to see those units, otherwise compie is cheating
		//Must not be in transport, since that location is -1/-1
		if (masterBoardPointer->minionRoster[i] != NULL && masterBoardPointer->minionRoster[i]->team != masterBoardPointer->playerFlag
			&& masterBoardPointer->minionRoster[i]->locationX != -1
			&& masterBoardPointer->Board[masterBoardPointer->minionRoster[i]->locationX][masterBoardPointer->minionRoster[i]->locationY].withinVision[masterBoardPointer->playerFlag] == true)
		{
			if (masterBoardPointer->minionRoster[i]->type == "Interceptor" ||
				masterBoardPointer->minionRoster[i]->type == "Attack_Copter" ||
				masterBoardPointer->minionRoster[i]->type == "Bomber" ||
				masterBoardPointer->minionRoster[i]->type == "Transport_Copter")
			{
				numberOfEnemyAirUnits++;
			}
		}
	}

	//Go through each factory and purchase a unit based on unit totals and available treasury.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			//First determine "surplus" treasury, which accounts for at least purchasing an infantry at every possible factory.
			int availableTreasury = masterBoardPointer->playerRoster[masterBoardPointer->playerFlag].treasury + 1000 - totalFactoriesLeft * 1000;

			bool minionPurchased = false;

			//Comparison function to save space
			auto potentiallyBuyLandMinion = [&](std::string inputMinion, char purchaseSite, int& runningTotalOfMinions, int& runningTotalOfFactories)
			{
				if (masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite) <= availableTreasury
					&& masterBoardPointer->playerRoster[masterBoardPointer->playerFlag].treasury >= masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite)
					&& std::find(masterBoardPointer->banList.begin(), masterBoardPointer->banList.end(), inputMinion) == masterBoardPointer->banList.end()
					&& minionPurchased == false)
				{
					//Must be able to actually afford the unit.				
					int success = masterBoardPointer->attemptPurchaseMinion(inputMinion, x, y, masterBoardPointer->playerFlag);
					if (success == 0)
					{
						runningTotalOfMinions++;
						runningTotalOfFactories--;
						minionPurchased = true;
					}
				}
			};

			//If the current tile is our factory and there is no one already on top
			if (masterBoardPointer->Board[x][y].symbol == 'h' && masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag && masterBoardPointer->Board[x][y].hasMinionOnTop == false)
			{
				//MODIFIED	
				//If enemy has at least 1 air unit, and we either have no AA, or they twice or more than our AA
				//And we can afford, and is not banned
				if (numberOfEnemyAirUnits > 0 && (numberOfAA == 0 || numberOfEnemyAirUnits / numberOfAA >= 2))
				{
					potentiallyBuyLandMinion("Anti-Aircraft", 'h', numberOfAA, totalFactoriesLeft);
				}

				//If we have a proper proportion of tanks, buy cavalry.
				//Must not be banned.
				if (int(numberOfTanks / 3) > numberOfCavalry)
				{
					potentiallyBuyLandMinion("Recon", 'h', numberOfCavalry, totalFactoriesLeft);
				}

				//If we have enough cav, buy rocket if not banned
				if (int(numberOfTanks / 2) > numberOfRanged)
				{
					potentiallyBuyLandMinion("Rocket_Artillery", 'h', numberOfRanged, totalFactoriesLeft);
				}

				//If we can't afford rocket, maybe we can afford arty (If not banned)
				if (int(numberOfTanks / 2) > numberOfRanged)
				{
					potentiallyBuyLandMinion("Artillery", 'h', numberOfRanged, totalFactoriesLeft);
				}

				//Attempt to buy APC for infantry support
				if (int(numberOfInfantry / 4) > numberOfInfantryTransports)
				{
					potentiallyBuyLandMinion("APC", 'h', numberOfInfantryTransports, totalFactoriesLeft);
				}

				//Attempt to buy IFV for infantry support
				if (int(numberOfInfantry / 4) > numberOfInfantryTransports)
				{
					potentiallyBuyLandMinion("IFV", 'h', numberOfInfantryTransports, totalFactoriesLeft);
				}

				//Infantry and specialists have a similar proportion.
				//Special check to prevent from buying too many infantry.
				if (int(numberOfInfantry / 3) > numberOfSpecialists
					&& (compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].onlyInfantryArmySizeHere <
						compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].infantryMaxHere))
				{
					potentiallyBuyLandMinion("Specialist", 'h', numberOfSpecialists, totalFactoriesLeft);
				}

				//Otherwise just buy other frontline units				
				potentiallyBuyLandMinion("Heavy_Armor", 'h', numberOfTanks, totalFactoriesLeft);

				potentiallyBuyLandMinion("Modern_Armor", 'h', numberOfTanks, totalFactoriesLeft);

				potentiallyBuyLandMinion("Armor", 'h', numberOfTanks, totalFactoriesLeft);

				if (compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].onlyInfantryArmySizeHere <
					compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].infantryMaxHere)
				{
					potentiallyBuyLandMinion("Infantry", 'h', numberOfInfantry, totalFactoriesLeft);
				}

			}

			//Airbase production
			//If the current tile is our factory and there is no one already on top
			if (masterBoardPointer->Board[x][y].symbol == 'A' &&
				masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag &&
				masterBoardPointer->Board[x][y].hasMinionOnTop == false)
			{

				//Slightly different lambda function for air minions, since they don't care about total number of available factories
				auto potentiallyBuyAirMinion = [&](std::string inputMinion, char purchaseSite, int& runningTotalOfMinions)
				{
					if (masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite) <= availableTreasury
						&& masterBoardPointer->playerRoster[masterBoardPointer->playerFlag].treasury >= masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite)
						&& std::find(masterBoardPointer->banList.begin(), masterBoardPointer->banList.end(), inputMinion) == masterBoardPointer->banList.end()
						&& minionPurchased == false)
					{
						//Must be able to actually afford the unit.				
						int success = masterBoardPointer->attemptPurchaseMinion(inputMinion, x, y, masterBoardPointer->playerFlag);
						if (success == 0)
						{
							runningTotalOfMinions++;
							minionPurchased = true;
						}
					}
				};

				if (int(numberOfTanks / 3) > numberOfBombers)
				{
					potentiallyBuyAirMinion("Bomber", 'A', numberOfBombers);
				}

				if (int(numberOfTanks / 3) > numberOfBombers)
				{
					potentiallyBuyAirMinion("Attack_Copter", 'A', numberOfBombers);
				}

				if (int(numberOfInfantry / 4) > numberOfInfantryTransports)
				{
					potentiallyBuyAirMinion("Transport_Copter", 'A', numberOfInfantryTransports);
				}

			}

			//Port production
			//If the current tile is our port and there is no one already on top
			if (masterBoardPointer->Board[x][y].symbol == 'P' &&
				masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag &&
				masterBoardPointer->Board[x][y].hasMinionOnTop == false)
			{

				//Slightly different lambda function for air minions, since they don't care about total number of available factories
				auto potentiallyBuySeaMinion = [&](std::string inputMinion, char purchaseSite, int& runningTotalOfMinions)
				{
					if (masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite) <= availableTreasury
						&& masterBoardPointer->playerRoster[masterBoardPointer->playerFlag].treasury >= masterBoardPointer->consultMinionCostChart(inputMinion, purchaseSite)
						&& std::find(masterBoardPointer->banList.begin(), masterBoardPointer->banList.end(), inputMinion) == masterBoardPointer->banList.end()
						&& minionPurchased == false)
					{
						//Must be able to actually afford the unit.				
						int success = masterBoardPointer->attemptPurchaseMinion(inputMinion, x, y, masterBoardPointer->playerFlag);
						if (success == 0)
						{
							runningTotalOfMinions++;
							minionPurchased = true;
						}
					}
				};

				//If we already have a navy to protect b-ship
				if (numberOfPorts < numberOfSeaCombatants - 2 && numberOfSeaCombatants > 0)
				{
					potentiallyBuySeaMinion("Battleship", 'P', numberOfSeaCombatants);
				}

				//Haven't yet reached large enough navy
				if (numberOfPorts > numberOfSeaCombatants - 2)
				{
					potentiallyBuySeaMinion("Cruiser", 'P', numberOfSeaCombatants);
				}

				//If we have less naval units than ports
				if (numberOfPorts > numberOfSeaCombatants)
				{
					potentiallyBuySeaMinion("Gunboat", 'P', numberOfSeaCombatants);
				}

				//If we have less naval units than ports
				if (numberOfPorts > numberOfLanders)
				{
					potentiallyBuySeaMinion("Lander", 'P', numberOfSeaCombatants);
				}
			}


		}

	}
	return 0;
}

int compie::newDetermineProduction()
{
	//The order of magnitude is 100 for most values since combat values are based on gold cost, which is in thousands
	double navalPortValueFactor = 100;
	double navalHQValueFactor = 200;
	double transportFactor = 50;

	double landPropertyFactor = 100;
	double landHQValueFactor = 200;
	double terrainFactor = 0.1;
	double nearbyMinionNegativeMultiplier = 200;

	//Range should be range 0-1, decimal.
	double efficiencyFactor = 0.1;	//1 indicates subtract full cost of a minion

	//Note, higher number makes infantry more preferred.
	double nonInfantryPenalty = 2;

	buildCompieProductionPropertyRoster();

	//Start with ships
	for (int n = 0; n < compiePropertyRoster.size(); n++ )
	{
		//Must be port and NOT have any minion on it
		if (compiePropertyRoster.at(n).symbol == 'P'  && compiePropertyRoster.at(n).recordedTile->hasMinionOnTop == false)
		{
			//Create custom path map for this port
			std::vector<std::vector<pathSquare>> portTerrainOnlyPathMap(masterBoardPointer->BOARD_WIDTH, std::vector<pathSquare>(masterBoardPointer->BOARD_HEIGHT));

			masterBoardPointer->buildTerrainOnlyPathMap(true, compiePropertyRoster.at(n).recordedTile->locationX, compiePropertyRoster.at(n).recordedTile->locationY, "Gunboat", portTerrainOnlyPathMap);
			//Requires buildTerrainOnlyPathMap to now accept minion typeand reference to pathmap instead of minion.

			//Now go through map and for each tile, add score to this port's options depending on what we see
			for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
			{
				for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
				{
					//Must be accessible to a ship
					if (portTerrainOnlyPathMap[x][y].distanceFromMinion != -1)
					{
						double inverseDistance = 1 / double (1 + portTerrainOnlyPathMap[x][y].distanceFromMinion);
						if (masterBoardPointer->Board[x][y].hasMinionOnTop)
						{
							Minion * minionOnTop = masterBoardPointer->Board[x][y].minionOnTop;

							//Enemy sea unit, must be visible
							if (minionOnTop->team != compiePlayerFlag && minionOnTop->domain == sea 
								&& masterBoardPointer->Board[x][y].withinVision[compiePlayerFlag] == true)
							{
								for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
								{
									//First see what the combat results are
									double bestAttackScore = 0;
									double throwawayPriScore = 0;
									double throwawaySecScore = 0;
									masterBoardPointer->consultAttackValuesChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, minionOnTop->type, throwawayPriScore, throwawaySecScore, bestAttackScore);

									//Then multiply by the cost of the enemy minion and subtract by our minion's cost and efficiencyFactor
									double combatValue = bestAttackScore * masterBoardPointer->consultMinionCostChart(minionOnTop->type, '~');
									double combatCost = masterBoardPointer->consultMinionCostChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, '~') * efficiencyFactor;

									compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * ( combatValue - combatCost);
								}
							}								
						}
							
						//Enemy port or other property (Accessible to ship?!?!?)
						if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != compiePlayerFlag)
						{
							for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
							{
								compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * navalPortValueFactor;
							}
						}

						//Shoals are good for landers/gunboats
						if (masterBoardPointer->Board[x][y].symbol == '*')
						{
						//This is not great but allows landers and gunboats to be rep'd for transporting to shoals and such
						compiePropertyRoster.at(n).purchasePreferenceList.at(0).preferenceScore += inverseDistance * transportFactor;
						compiePropertyRoster.at(n).purchasePreferenceList.at(2).preferenceScore += inverseDistance * transportFactor * 2;
						}
					}

					//For enemy HQ, we ignore if it's accessible and just compute the straight line distance.
					if (masterBoardPointer->Board[x][y].symbol == 'Q' && masterBoardPointer->Board[x][y].controller != compiePlayerFlag)
					{
						double inverseDistance = 1 / double (1 + masterBoardPointer->computeDistance(x, compiePropertyRoster.at(n).recordedTile->locationX, y, compiePropertyRoster.at(n).recordedTile->locationY));
						for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
						{
							compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * navalHQValueFactor;
						}
					}
				}
			}
		}
	
		//Now factor in the amount of money we have - the more out of budget a minion costs, the more it decreases its preference.
		for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
		{
			//Multiply the preference score by how far from purchasing we are (Assuming we don't have enough treasury currently)
			int minionCost = masterBoardPointer->consultMinionCostChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, '~');
			double overCostFactor = masterBoardPointer->playerRoster[compiePlayerFlag].treasury / double (minionCost);

			if (overCostFactor > 1)
				overCostFactor = 1;

			compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore *= overCostFactor;
		}
	}
	
	//Next land minions
	for (int n = 0; n < compiePropertyRoster.size(); n++)
	{
		//Must be factory and NOT have any minion on it
		if (compiePropertyRoster.at(n).symbol == 'h' && compiePropertyRoster.at(n).recordedTile->hasMinionOnTop == false)
		{
			//Create one path map for vehicles, later we will try to differentiate for infantry too
			std::vector<std::vector<pathSquare>> factoryVehicleTerrainOnlyPathMap(masterBoardPointer->BOARD_WIDTH, std::vector<pathSquare>(masterBoardPointer->BOARD_HEIGHT));

			masterBoardPointer->buildTerrainOnlyPathMap(true, compiePropertyRoster.at(n).recordedTile->locationX, compiePropertyRoster.at(n).recordedTile->locationY, "Armor", factoryVehicleTerrainOnlyPathMap);

			//Now go through map and for each tile, add score to this factory's options depending on what we see
			for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
			{
				for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
				{
					//Must be accessible to an vehicle
					if (factoryVehicleTerrainOnlyPathMap[x][y].distanceFromMinion != -1)
					{
						double inverseDistance = 1 / double(1 + factoryVehicleTerrainOnlyPathMap[x][y].distanceFromMinion);
						if (masterBoardPointer->Board[x][y].hasMinionOnTop)
						{
							Minion* minionOnTop = masterBoardPointer->Board[x][y].minionOnTop;

							//Enemy minion, must be visible
							if (minionOnTop->team != compiePlayerFlag && masterBoardPointer->Board[x][y].withinVision[compiePlayerFlag] == true)
							{
								for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
								{
									//Add score from a potential combat
									double bestAttackScore = 0;
									double throwawayPriScore = 0;
									double throwawaySecScore = 0;
									masterBoardPointer->consultAttackValuesChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, minionOnTop->type, throwawayPriScore, throwawaySecScore, bestAttackScore);
									
									//Then multiply by the cost of the enemy minion
									double combatValue = bestAttackScore * masterBoardPointer->consultMinionCostChart(minionOnTop->type, '~');
									double combatCost = masterBoardPointer->consultMinionCostChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, '~') * efficiencyFactor;

									compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * (combatValue - combatCost);
								}
							}

							//If friendly minion, subtract a small amount based on distance
							if(minionOnTop->team == compiePlayerFlag)
								for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
								{
									if(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type == minionOnTop->type)
										compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore -= inverseDistance * nearbyMinionNegativeMultiplier;
								}

						}

						//Enemy property
						if (masterBoardPointer->Board[x][y].checkForProperty() == true && masterBoardPointer->Board[x][y].controller != compiePlayerFlag)
						{
							for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
							{
								//Adjust depending on if infantry or not
								double adjustedPropertyFactor = 0;
								if (compiePropertyRoster.at(n).purchasePreferenceList.at(j).specialtyGroup == infantry)
									adjustedPropertyFactor = landPropertyFactor;
								else
									adjustedPropertyFactor =  landPropertyFactor / nonInfantryPenalty;

								compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * adjustedPropertyFactor;
							}
						}

					}

					//For enemy HQ, we ignore if it's accessible and just compute the straight line distance.
					if (masterBoardPointer->Board[x][y].symbol == 'Q' && masterBoardPointer->Board[x][y].controller != compiePlayerFlag)
					{
						double inverseDistance = 1 / double(1 + masterBoardPointer->computeDistance(x, compiePropertyRoster.at(n).recordedTile->locationX, y, compiePropertyRoster.at(n).recordedTile->locationY));
						for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
						{
							compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore += inverseDistance * landHQValueFactor;
						}
					}

				}
			}

			//Now factor in the amount of money we have - the more out of budget a minion costs, the more it decreases its preference.
			for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
			{
				//Multiply the preference score by how far from purchasing we are (Assuming we don't have enough treasury currently)
				int minionCost = masterBoardPointer->consultMinionCostChart(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, '~');
				double overCostFactor = masterBoardPointer->playerRoster[compiePlayerFlag].treasury / double(minionCost);

				if (overCostFactor > 1)
					overCostFactor = 1;

				compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore *= overCostFactor;
			}
		}
	}

	//Now actually try to build minions

	//Variables to determine best property to build at
	int bestBase = -1;
	int bestMinion = -1;
	double bestPreferenceScore = 0;
	//Keep attempting to buy minions until we decide otherwise
	bool stillBuyingMinions = true;
	while (stillBuyingMinions == true)
	{

		//Go through property roster looking for best base to build at, and best minion to buy there.
		for (int n = 0; n < compiePropertyRoster.size(); n++)
		{
			//Must be free to build a minion
			if (compiePropertyRoster.at(n).recordedTile->hasMinionOnTop == false)
			{
				for (int j = 0; j < compiePropertyRoster.at(n).purchasePreferenceList.size(); j++)
				{
					if (compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore > bestPreferenceScore)
					{
						//We must be allowed to build this minion AND it's in our faction
						if (masterBoardPointer->checkFactionAvailability(compiePropertyRoster.at(n).purchasePreferenceList.at(j).type, compiePlayerFlag) == true &&
							std::find(masterBoardPointer->banList.begin(), masterBoardPointer->banList.end(), compiePropertyRoster.at(n).purchasePreferenceList.at(j).type) == masterBoardPointer->banList.end())
						{
							bestPreferenceScore = compiePropertyRoster.at(n).purchasePreferenceList.at(j).preferenceScore;
							bestBase = n;
							bestMinion = j;
						}
					}
				}
			}
		}

		if (bestBase != -1 && bestMinion != -1)
		{
			std::cout << "Would like to buy: " << compiePropertyRoster.at(bestBase).purchasePreferenceList.at(bestMinion).type << std::endl;
			if (masterBoardPointer->consultMinionCostChart(compiePropertyRoster.at(bestBase).purchasePreferenceList.at(bestMinion).type, '~') <= masterBoardPointer->playerRoster[compiePlayerFlag].treasury)
			{
				//Must be able to actually afford the unit.				
				int success = masterBoardPointer->attemptPurchaseMinion(compiePropertyRoster.at(bestBase).purchasePreferenceList.at(bestMinion).type,
					compiePropertyRoster.at(bestBase).recordedTile->locationX, compiePropertyRoster.at(bestBase).recordedTile->locationY, masterBoardPointer->playerFlag);
			
				//If purchased our preferred minion successfully, reset counters and continue trying to purchase minions.
				if (success == 0)
				{
					bestBase = -1;
					bestMinion = -1;
					bestPreferenceScore = 0;
				}
				else
				{
					stillBuyingMinions = false;
				}
					

			}
			else 
			{	//If we didn't have enough money to buy preferred minion, stop buying minions for this turn.
				//(Might be saving up for a larger minion, for example)
				stillBuyingMinions = false;
			}
		}
		else 
		{
			//If we couldn't find a valid property, we're done building minions for this turn.
			stillBuyingMinions = false;
		}

	}

	return 0;

}

int compie::buildCompieMinionRoster()
{
	int numberOfCompieMinions = 0;

	//A new compieMinionRoster every turn, instead of trying to track minion deaths.

	if (InputLayer->MainMenu->gameTurn > 30)
		std::cout << "Late" << std::endl;

	//Go through minion Roster.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		compieMinionRoster[i] = NULL;
		//Must be a valid minion, and compie's minion. 
		if (masterBoardPointer->minionRoster[i] != NULL && masterBoardPointer->minionRoster[i]->team == masterBoardPointer->playerFlag)
		{
			numberOfCompieMinions++;
			compieMinionRoster[i] = new compieMinionRecord(masterBoardPointer->minionRoster[i]);

			int gridX = 0;
			int gridY = 0;

			//Also count number of infantry/total units in this land mass. Must not be naval minion.
			//If minion is being transported, check the transporter's location.
			if (masterBoardPointer->minionRoster[i]->transporter == NULL)
			{
				gridX = masterBoardPointer->minionRoster[i]->locationX;
				gridY = masterBoardPointer->minionRoster[i]->locationY;
			}
			else
			{
				gridX = masterBoardPointer->minionRoster[i]->transporter->locationX;
				gridY = masterBoardPointer->minionRoster[i]->transporter->locationY;
			}

			if (compieLandMassMap.grid[gridX][gridY].landMassNumber > 0)
			{
				compieLandMassMap.roster[compieLandMassMap.grid[gridX][gridY].landMassNumber].armySizeHere++;
				if (masterBoardPointer->minionRoster[i]->specialtyGroup == infantry)
				{
					compieLandMassMap.roster[compieLandMassMap.grid[gridX][gridY].landMassNumber].onlyInfantryArmySizeHere++;
				}
			}
		}
	}

	return 0;
}

int compie::buildCompieProductionPropertyRoster()
{
	compiePropertyRoster.clear();

	//Go through map and find all of our factories, ports, etc.
	for (int x = 0; x < masterBoardPointer->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < masterBoardPointer->BOARD_HEIGHT; y++)
		{
			//If the current tile is friendly production-property
			if (masterBoardPointer->Board[x][y].controller == masterBoardPointer->playerFlag)
				if (masterBoardPointer->Board[x][y].symbol == 'P' || masterBoardPointer->Board[x][y].symbol == 'h' || masterBoardPointer->Board[x][y].symbol == 'A')
				{
					compiePropertyRoster.emplace_back(&masterBoardPointer->Board[x][y]);
				}

		}
	}

	return 0;
}