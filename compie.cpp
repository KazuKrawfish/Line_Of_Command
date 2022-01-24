#include "compie.hpp"
#include <thread>
#include <iostream>
#include "mainmenu.h"

//Need to have all "singlePlayer" status elements be based in masterboard, otherwise will be very confusing to view/update.

//Compie calculates its closest objective by using buildPath and finding potential objective that has the best distance.
//Then it builds a compie pathMap again on that objective
//SelectMinion now builds an apparentPathMap that overrides the initial one. Shouldn't matter for strategic considerations.

//Below functions are "utilities" that need to find a home.




int computeDistance(int inputX1, int inputX2, int inputY1, int inputY2)
{
	return abs(inputX1 - inputX2) + abs(inputY1 - inputY2);

}
//Above functions are "utilities" that need to find a home.
int compie::statusUpdate(MasterBoard* boardToUse) 
{
	double totalPropertiesCompieOwns = 0;
	double totalFactoriesCompieOwns = 0;

	//Clear all land masses of infantry/army size amounts
	for (int i = 1; i < compieLandMassMap.roster.size(); i++)
	{
		compieLandMassMap.roster[i].armySizeHere = 0;
		compieLandMassMap.roster[i].onlyInfantryArmySizeHere = 0;
	}

	buildCompieMinionRoster(boardToUse);

	//Now find factories and properties totals int total- we calculate the turn's armamentFactor as a global rate, not individual land masses.
	//Otherwise the compie would randomly skew certain areas to high-mech. and they'd never build units if it remained actually poor.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			if (boardToUse->Board[x][y].checkForProperty(boardToUse->Board[x][y].symbol) == true && boardToUse->Board[x][y].controller == boardToUse->playerFlag)
			{
				if (boardToUse->Board[x][y].symbol == 'h')
				{
					totalFactoriesCompieOwns++;
				}
				
				totalPropertiesCompieOwns++;

			}
		}
	}

	armamentFactor = totalFactoriesCompieOwns / totalPropertiesCompieOwns;
	for (int i = 1; i < compieLandMassMap.roster.size(); i++)
	{
		compieLandMassMap.roster[i].infantryMaxHere = armamentFactor * compieLandMassMap.roster[i].maxArmySize;
	}

	return 0;

}


//Builds land mass map for compie. Recursive since it has to build contiguous territories.
//Should return the number used. Which should only matter if this was the first one called.
int compie::buildLandMassMap(MasterBoard* boardToUse, int x, int y, int nextNumber)
{	
	int myNumber = compieLandMassMap.grid[x][y].landMassNumber;
	bool firstSquare = false;

	//If this territory is impassible, go no further.
	//Indicates this square is impassible and thus doesn't belong to any landMass.
	if (myNumber == -1)	//Previously assigned based on 99 == -1
		return -1;	

	//If not impassable
	if (myNumber >= 0 )
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
				buildLandMassMap(boardToUse, x- 1, y, nextNumber);
			}
		}
		if (y > 0)
		{
			if (compieLandMassMap.grid[x ][y -1].landMassNumber == 0)
			{
				compieLandMassMap.grid[x][y - 1].landMassNumber = myNumber;
				buildLandMassMap(boardToUse, x , y-1, nextNumber);
			}
		}
		if (y < boardToUse->BOARD_HEIGHT - 1)
		{
			if (compieLandMassMap.grid[x][y+ 1].landMassNumber == 0)
			{
				compieLandMassMap.grid[x][y + 1].landMassNumber = myNumber;
				buildLandMassMap(boardToUse, x, y + 1, nextNumber);
			}
		}
		if (x < boardToUse->BOARD_WIDTH- 1)
		{
			if (compieLandMassMap.grid[x + 1][y ].landMassNumber == 0)
			{
				compieLandMassMap.grid[x + 1][y].landMassNumber = myNumber;
				buildLandMassMap(boardToUse, x + 1, y , nextNumber);
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

int compie::analyzeSingleLandMass(MasterBoard* boardToUse, int landMassNumber) 
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
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
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
			if (x < boardToUse->BOARD_WIDTH - 1 && x < maxX)
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
			if (y < boardToUse->BOARD_HEIGHT - 1 && y < maxY)
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
	compieLandMassMap.roster[landMassNumber].totalArea = (maxX - minX + 1)  * (maxY - minY + 1 );

	//Compute landFactor and set for final product
	compieLandMassMap.roster[landMassNumber].landinessFactor = totalLandSquares / (compieLandMassMap.roster[landMassNumber].totalArea);

	compieLandMassMap.roster[landMassNumber].initialized = true;

	return 0;

}

//Helps compie determine semi-constant map elements.
int compie::analyzeMap(MasterBoard* boardToUse)
{
	//Go through each square on the board
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{

			//Find headquarters:
			if (boardToUse->Board[x][y].symbol == 'Q' && boardToUse->Board[x][y].controller == compiePlayerFlag)
			{
				headquartersX = x;
				headquartersY = y;
			}

			//Count number of properties on map
			if (boardToUse->Board[x][y].checkForProperty(boardToUse->Board[x][y].symbol) == true)
				totalPropertiesOnMap++;

			//Here we find which properties are impassible to an infantry, which helps determine the boundaries for compie's land mass map.
			int movementCostHere = boardToUse->Board[x][y].consultMovementChart('i', boardToUse->Board[x][y].symbol);
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
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			//If this landMassSquare is passable, call recursive builder.
			if(compieLandMassMap.grid[x][y].landMassNumber != -1)
				nextLandMassNumber = buildLandMassMap(boardToUse, x, y, nextLandMassNumber);
		}
	}

	//One for each land mass, one for 0 index, and one extra because it weirdly fails on me.
	compieLandMassMap.roster.resize(nextLandMassNumber);

	//Iterate through board and analyze any landmasses that have not been analyzed.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			
			if (compieLandMassMap.grid[x][y].landMassNumber != -1 ) 
			{
				if (compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].initialized == false)
				{
					analyzeSingleLandMass(boardToUse, compieLandMassMap.grid[x][y].landMassNumber);
				}
			}
			
		}
	}



	for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
		{
		std::cout << " " << compieLandMassMap.grid[x][y].landMassNumber;
		}
		std::cout << "\n";

	}

	for (int y = 1; y < compieLandMassMap.roster.size(); y++)
	{
		///(6 == 2*2) <-- 2: Half of landMass belongs to compie, 2: To provide room for maneuver and repair
		double normalizedArea = compieLandMassMap.roster[y].totalLandSquareCount * compieLandMassMap.roster[y].landinessFactor;
		compieLandMassMap.roster[y].maxArmySize = normalizedArea * compieLandMassMap.roster[y].edginessFactor / (4) + 2;
		
		if (compieLandMassMap.roster[y].maxArmySize < 4)
		{
			compieLandMassMap.roster[y].maxArmySize = 4;
		}
		
		std::cout << "Land Mass: " << y << std::endl;
		std::cout << "Total Area: " << compieLandMassMap.roster[y].totalArea << std::endl;
		std::cout << "Total Land Squares: " << compieLandMassMap.roster[y].totalLandSquareCount << std::endl;
		std::cout << "Edge Factor: " << compieLandMassMap.roster[y].edginessFactor << " (Land Edges: "<< compieLandMassMap.roster[y].landEdges<< ") (Impass. Edges: " << compieLandMassMap.roster[y].impassEdges << ")" << std::endl;
		std::cout << "Land Factor: " << compieLandMassMap.roster[y].landinessFactor << std::endl;
		std::cout << "Max Army Size here: " << compieLandMassMap.roster[y].maxArmySize << std::endl;
		std::cout << "----------" << std::endl;
	}

	return 0;
}

//Assumes minion is selected since we're going to check for withinRange.
int compie::defendHeadquarters(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord)
{

	//For now artillery doesn't do this
	if (selectedMinionRecord->recordedMinion->rangeType == rangedFire)
		return 0;

	Cursor* myCursor = &boardToUse->cursor;

	//If headquarters under attack by enemy
	if (boardToUse->Board[headquartersX][headquartersY].hasMinionOnTop && boardToUse->Board[headquartersX][headquartersY].minionOnTop->team != boardToUse->playerFlag)
	{
		//Check each adjacent tile to see if you can move there, and then set tasking to do so.
		if (headquartersX > 0 && boardToUse->Board[headquartersX - 1][headquartersY].withinRange == true &&
			(boardToUse->Board[headquartersX - 1][headquartersY].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY && myCursor->selectMinionPointer->locationX == headquartersX - 1))	)
		{
			selectedMinionRecord->potentialMoveTile =  & boardToUse->Board[headquartersX - 1][headquartersY];
			selectedMinionRecord->potentialAttackTile = &  boardToUse->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersX < boardToUse->BOARD_WIDTH - 1 && boardToUse->Board[headquartersX + 1][headquartersY].withinRange == true &&
			(boardToUse->Board[headquartersX + 1][headquartersY].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY && myCursor->selectMinionPointer->locationX == headquartersX + 1))	)
		{
			selectedMinionRecord->potentialMoveTile = &boardToUse->Board[headquartersX + 1][headquartersY];
			selectedMinionRecord->potentialAttackTile = &boardToUse->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersY > 0 && boardToUse->Board[headquartersX][headquartersY - 1].withinRange == true &&
			(boardToUse->Board[headquartersX][headquartersY - 1].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY - 1 && myCursor->selectMinionPointer->locationX == headquartersX))	)
		{
			selectedMinionRecord->potentialMoveTile = &boardToUse->Board[headquartersX ][headquartersY - 1];
			selectedMinionRecord->potentialAttackTile = &boardToUse->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else if (headquartersY < boardToUse->BOARD_HEIGHT - 1 && boardToUse->Board[headquartersX][headquartersY + 1].withinRange == true &&
			(boardToUse->Board[headquartersX][headquartersY + 1].hasMinionOnTop != true ||
			(myCursor->selectMinionPointer->locationY == headquartersY + 1 && myCursor->selectMinionPointer->locationX == headquartersX))	)
		{
			selectedMinionRecord->potentialMoveTile = &boardToUse->Board[headquartersX ][headquartersY +1 ];
			selectedMinionRecord->potentialAttackTile = &boardToUse->Board[headquartersX][headquartersY];
			selectedMinionRecord->tasking = defendHQ;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
	

	}
	
	
	return 0;
}


//Assumes minion is selected since we're going to check for withinRange.
int compie::defendProperty(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord)
{
	//For now artillery doesn't do this
	if (selectedMinionRecord->recordedMinion->rangeType == rangedFire)
		return 0;

	Cursor* myCursor = &boardToUse->cursor;

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//If property under attack by enemy
			if (boardToUse->Board[x][y].controller == boardToUse->playerFlag && boardToUse->Board[x][y].hasMinionOnTop && boardToUse->Board[x][y].minionOnTop->team != boardToUse->playerFlag)
			{
				//Check each adjacent tile to see if you can move there, and then set tasking to do so.
				if (x > 0 && boardToUse->Board[x - 1][y].withinRange == true &&
					(boardToUse->Board[x - 1][y].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x - 1)))
				{
					selectedMinionRecord->potentialMoveTile = &boardToUse->Board[x - 1][y];
					selectedMinionRecord->potentialAttackTile = &boardToUse->Board[x][y];
					return 1;
				}
				else if (x < boardToUse->BOARD_WIDTH - 1 && boardToUse->Board[x + 1][y].withinRange == true &&
					(boardToUse->Board[x + 1][y].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x + 1)))
				{
					selectedMinionRecord->potentialMoveTile = &boardToUse->Board[x + 1][y];
					selectedMinionRecord->potentialAttackTile = &boardToUse->Board[x][y];
					return 1;
				}
				else if (y > 0 && boardToUse->Board[x][y - 1].withinRange == true &&
					(boardToUse->Board[x][y - 1].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y - 1 && myCursor->selectMinionPointer->locationX == x)))
				{
					selectedMinionRecord->potentialMoveTile = &boardToUse->Board[x][y - 1];
					selectedMinionRecord->potentialAttackTile = &boardToUse->Board[x][y];
					return 1;
				}
				else if (y < boardToUse->BOARD_HEIGHT - 1 && boardToUse->Board[x][y + 1].withinRange == true &&
					(boardToUse->Board[x][y + 1].hasMinionOnTop != true ||
					(myCursor->selectMinionPointer->locationY == y + 1 && myCursor->selectMinionPointer->locationX == x)))
				{
					selectedMinionRecord->potentialMoveTile = &boardToUse->Board[x][y + 1];
					selectedMinionRecord->potentialAttackTile = &boardToUse->Board[x][y];
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

int compie::initalizeCompie(mainMenu* inputMenu, int inputPlayerFlag, inputLayer* providedInputLayer, MasterBoard* boardToUse) 
{
	InputLayer = providedInputLayer;
	menuPointer = inputMenu;
	compiePlayerFlag = inputPlayerFlag;
	compieMinionRoster.resize(GLOBALSUPPLYCAP);


	compieLandMassMap.grid.resize(boardToUse->BOARD_WIDTH + 1);

	for (int i = 0; i < boardToUse->BOARD_WIDTH; i++)
	{
		compieLandMassMap.grid[i].resize(boardToUse->BOARD_HEIGHT);

	}

	analyzeMap(boardToUse);

	return 0;
}

//Depends on minion health. If <5, looks for repair location.
//Otherwise, searches the map, first checking for an enemy minion to attack, and then checking for enemy property to capture, if infantry type.
//Now assumes a pathMap was built for this- ie minion was selected.
int compie::strategicAdvance(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord)
{
	int distanceFromMinionToObjective = 999;
	int bestDistanceTileToObjective = 999;

	if (boardToUse->cursor.selectMinionPointer == NULL)
		return -1;

	Minion* myMinion = boardToUse->cursor.selectMinionPointer;

	//Now go through the whole board.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
					
				bool throwaway = false;
				int throwAwayInt = 0;
				bool TestRun = true;
				//If the current tile contains a non-friendly minion this minion can attack them (damagedealt > 0)
				//This allows us to bypass air units, for instance.
				if (boardToUse->Board[x][y].hasMinionOnTop == true && boardToUse->Board[x][y].minionOnTop->team != boardToUse->playerFlag
					&& boardToUse->calculateDamageDealt(boardToUse->cursor.selectMinionPointer, boardToUse->Board[x][y].minionOnTop, throwaway, throwAwayInt, TestRun) > 0)
				{
					//If it is closer than what we previously had targeted, set it as the objective.
					//Must also be valid, ie. not -1. Can actually get there!
					int rangeToEnemy = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeToEnemy != -1 && rangeToEnemy < distanceFromMinionToObjective)
					{
						distanceFromMinionToObjective = rangeToEnemy;
						selectedMinionRecord->objectiveTile = &(boardToUse->Board[x][y]);
					}
				}
				else
					//Other option is it's a property tile, not ours, and not being capped by our units (No one friendly on top).
					//Also, we must be infantry for this option.
					if (boardToUse->cursor.selectMinionPointer->specialtyGroup == infantry && boardToUse->Board[x][y].checkForProperty(boardToUse->Board[x][y].symbol) == true && boardToUse->Board[x][y].controller != boardToUse->playerFlag
						&& (boardToUse->Board[x][y].hasMinionOnTop == false || boardToUse->Board[x][y].minionOnTop->team != boardToUse->playerFlag))
					{
						int rangeToProp = myMinion->terrainOnlyPathMap[x][y].distanceFromMinion;
						if (rangeToProp != -1 && rangeToProp < distanceFromMinionToObjective)
						{
							distanceFromMinionToObjective = rangeToProp;
							selectedMinionRecord->objectiveTile = &(boardToUse->Board[x][y]);
						}
					}
			
		}
	}
	
	//If we couldn't find an objective, return with -1.
	if (selectedMinionRecord->objectiveTile == NULL)
	{
		return -1;
	}

	//on that property, and search for the closest tile in our local area, that is within range.
	//Currently this means compie will not get "trapped". Because it will use true pathmap builder, buildPath.
	boardToUse->buildTerrainOnlyPathMap( true, selectedMinionRecord->objectiveTile->locationX, selectedMinionRecord->objectiveTile->locationY,  boardToUse->cursor.selectMinionPointer);

	//This offsets by adding the cost to move to the potentialObjectiveTile, and decreases by the amount to get to that interm. square.
	//Turning compiePathMatrix into cost from interm. to potentialObjectiveTile.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion += selectedMinionRecord->objectiveTile->consultMovementChart(boardToUse->cursor.selectMinionPointer->type, boardToUse->Board[selectedMinionRecord->objectiveTile->locationX][selectedMinionRecord->objectiveTile->locationY].symbol);
			boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion -= boardToUse->Board[x][y].consultMovementChart(boardToUse->cursor.selectMinionPointer->type, boardToUse->Board[x][y].symbol);
		}
	}


	//Now we set a new search box that is much smaller. 
	//This is for actual tiles we can move to, that are closest to the enemy tile we're targeting.

	Cursor* myCursor = &(boardToUse->cursor);
	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//Tile must be free of minions and within range
			//Currently prevents compie from getting trapped on strat. move.
			if (boardToUse->Board[x][y].hasMinionOnTop == false && boardToUse->Board[x][y].withinRange == true)
			{
					//If the minion is ranged, all adjacent tiles must be free from enemies.
					//Either we are on the board's edge, or the adjacent tile has a friendly minion, or no minion
				if (selectedMinionRecord->recordedMinion->rangeType != rangedFire || 
					(
					( x < 1 || (  boardToUse->Board[x-1][y].hasMinionOnTop == false || boardToUse->Board[x - 1][y].minionOnTop->team == compiePlayerFlag) )
					&& (y < 1 || (boardToUse->Board[x ] [y-1].hasMinionOnTop == false || boardToUse->Board[x][y-1].minionOnTop->team == compiePlayerFlag) )
					&& (x == boardToUse->BOARD_WIDTH - 1 || (boardToUse->Board[x + 1][y].hasMinionOnTop == false || boardToUse->Board[x+ 1][y].minionOnTop->team == compiePlayerFlag) )
					&& (y == boardToUse->BOARD_HEIGHT - 1 || (boardToUse->Board[x][y + 1].hasMinionOnTop == false || boardToUse->Board[x][y + 1].minionOnTop->team == compiePlayerFlag))
					) 
					)
				{
					//If the current tile is closest to the potentialObjectiveTile (NOT THE MINION!).
					int rangeBetweenTileAndObjective = boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion;
					if (rangeBetweenTileAndObjective > 0 && rangeBetweenTileAndObjective < bestDistanceTileToObjective)	//Might have been some jenkery with distances on pathMap.
					{
						selectedMinionRecord->potentialMoveTile = &(boardToUse->Board[x][y]);
						bestDistanceTileToObjective = rangeBetweenTileAndObjective;
					}
				}
			}
		}
	}

	//If the minion cannot move for some reason (Too many friendly units, etc) This should return 999
	return bestDistanceTileToObjective;

}

//If minion is damaged or air and needs fuel, looks for friendly property capable of supporting, and either moves onto it, or moving towards it.
int compie::strategicWithdraw(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord)
{
	int distancefromMinionToObjective = 999;
	int bestDistanceTileToObjective = 999;

	if (boardToUse->cursor.selectMinionPointer == NULL)
		return -1;

	Minion* myMinion = boardToUse->cursor.selectMinionPointer;

	//Now go through the whole board.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
				//If this is friendly controlled property capable of repairing//supplying our unit
				//We do care if occupied currently
				if (boardToUse->Board[x][y].controller == boardToUse->playerFlag 
					&& boardToUse->consultMinionCostChart(myMinion->type, boardToUse->Board[x][y].symbol)  != -1
					&& boardToUse->Board[x][y].hasMinionOnTop == false)
				{	//Need to make sure not factory = this is jamming up production
					if (boardToUse->Board[x][y].symbol != 'h') {
						//If this is the closest objective so far, set as objective.
						int rangeBetweenMinionAndTile = boardToUse->cursor.selectMinionPointer->apparentPathMap[x][y].distanceFromMinion;
						if (rangeBetweenMinionAndTile != -1 && distancefromMinionToObjective > rangeBetweenMinionAndTile)
						{
							selectedMinionRecord->objectiveTile = &boardToUse->Board[x][y];
							distancefromMinionToObjective = rangeBetweenMinionAndTile;
						}
					}
				}
			
		}
	}

	//If we couldn't find an objective, return with -1.
	if (selectedMinionRecord->objectiveTile == NULL)
	{
		return -1;
	}

	//on that property, and search for the closest tile in our local area, that is within range.
	//Currently this means compie will not get "trapped". Because it will use true pathmap builder, buildPath.
	boardToUse->buildTerrainOnlyPathMap(true, selectedMinionRecord->objectiveTile->locationX, selectedMinionRecord->objectiveTile->locationY, boardToUse->cursor.selectMinionPointer);

	//This offsets by adding the cost to move to the potentialObjectiveTile, and decreases by the amount to get to that interm. square.
	//Turning compiePathMatrix into cost from interm. to potentialObjectiveTile.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion += selectedMinionRecord->objectiveTile->consultMovementChart(boardToUse->cursor.selectMinionPointer->type,
				boardToUse->Board[selectedMinionRecord->objectiveTile->locationX][selectedMinionRecord->objectiveTile->locationY].symbol);
			boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion -= boardToUse->Board[x][y].consultMovementChart(boardToUse->cursor.selectMinionPointer->type, boardToUse->Board[x][y].symbol);
		}
	}


	//Now we set a new search box that is much smaller. 
	//This is for actual tiles we can move to, that are closest to the enemy tile we're targeting.

	Cursor* myCursor = &(boardToUse->cursor);
	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
	int maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;
	if (minX < 0)
	{
		minX = 0;
	}
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//Tile must be free of minions and within range
			//Currently prevents compie from getting trapped on strat. move.
			if (boardToUse->Board[x][y].hasMinionOnTop == false && boardToUse->Board[x][y].withinRange == true)
			{
				//If the current tile is closest to the potentialObjectiveTile (NOT THE MINION!).
				int rangeBetweenTileAndObjective = boardToUse->cursor.selectMinionPointer->terrainOnlyPathMap[x][y].distanceFromMinion;
				if (rangeBetweenTileAndObjective >= 0 && rangeBetweenTileAndObjective < bestDistanceTileToObjective)	//Might have been some jenkery with distances on pathMap.
				{
					selectedMinionRecord->potentialMoveTile = &(boardToUse->Board[x][y]);
					bestDistanceTileToObjective = rangeBetweenTileAndObjective;
				}
			}
		}
	}

	//If the minion cannot move for some reason (Too many friendly units, etc) This should return 999
	return bestDistanceTileToObjective;

}

//Checks surrounding tiles for best valued enemy.
//This function WILL immediately replace whatever the previous objective was as long as its suitability score is higher than what was passed in.
int compie::checkAdjacentTilesForBestValuedEnemy(int currentX, int currentY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord)
{
	double attackerDamageDealt = 0;
	double defenderCounterAttackDamageDealt = 0;

	//Within each of four adjacent tiles:
	//Calculate the damage our minion would do to them, as well as the counterattack damage:


	checkSingleTileForCombatValue(currentX, currentY, currentX - 1, currentY, myCursor, boardToUse, relativeSuitabilityScore, selectedMinionRecord);
	
	checkSingleTileForCombatValue(currentX, currentY, currentX + 1, currentY, myCursor, boardToUse, relativeSuitabilityScore, selectedMinionRecord);
	
	checkSingleTileForCombatValue(currentX, currentY, currentX, currentY - 1, myCursor, boardToUse, relativeSuitabilityScore, selectedMinionRecord);
	
	checkSingleTileForCombatValue(currentX, currentY, currentX, currentY + 1, myCursor, boardToUse, relativeSuitabilityScore, selectedMinionRecord);

	return 0;
}

int compie::checkSingleTileForCombatValue(int attackerX, int attackerY, int defenderX, int defenderY, Cursor* myCursor, MasterBoard* boardToUse, double* relativeSuitabilityScore, compieMinionRecord* selectedMinionRecord) 
{	

	//Any return of 1 will not have touched relativeSuitability or the compieRoster objective tiles so we're okay to just return.
	if (attackerX < 0 || defenderX < 0 || attackerY < 0 || defenderY < 0)
	{
		return 1;
	}
	
	if(attackerX >= boardToUse->BOARD_WIDTH || defenderX >= boardToUse->BOARD_WIDTH || attackerY >= boardToUse->BOARD_HEIGHT || defenderY >= boardToUse->BOARD_HEIGHT)
	{
		return 1;
	}

	if (boardToUse->Board[defenderX][defenderY].hasMinionOnTop == false || boardToUse->Board[defenderX][defenderY].withinVision[myCursor->selectMinionPointer->team] != true)
	{
		return 1;
	}

	if(	boardToUse->Board[defenderX][defenderY].minionOnTop->team == selectedMinionRecord->recordedMinion->team)
	{
		return 1;
	}
	


	int attackerCost = 0;
	int defenderCost = 0;

	//Determine minion costs based on types:
	//Uses ~ which gets cost regardless of base below us.
	attackerCost = boardToUse->consultMinionCostChart(myCursor->selectMinionPointer->type, '~');
	defenderCost = boardToUse->consultMinionCostChart(boardToUse->Board[defenderX][defenderY].minionOnTop->type, '~');

	bool willAmmoBeUsed = false;
	int weaponUsed = 0;
	bool ignoreLimitations = true;

	int attackerDamageDealt = boardToUse->calculateDamageDealt(myCursor->selectMinionPointer, boardToUse->Board[defenderX][defenderY].minionOnTop, willAmmoBeUsed, weaponUsed, ignoreLimitations);
	int defenderCounterAttackDamageDealt = boardToUse->calculateDamageDealt(boardToUse->Board[defenderX][defenderY].minionOnTop, myCursor->selectMinionPointer, willAmmoBeUsed, weaponUsed, ignoreLimitations) 
		* (boardToUse->Board[defenderX][defenderY].minionOnTop->health - attackerDamageDealt) / 100;
	
	if (defenderCounterAttackDamageDealt < 0)
		defenderCounterAttackDamageDealt = 0;

	//If ranged unit, defender  cannot counterattack.
	//If defender is ranged unit, also cannot counterattack
	if (myCursor->selectMinionPointer->rangeType == rangedFire || boardToUse->Board[defenderX][defenderY].minionOnTop->rangeType == rangedFire)
		defenderCounterAttackDamageDealt = 0;

	//This is the potential "value added" from combat, based on what we might lose vs gain.
	double newRelativeSuitabilityScore = ( (attackerDamageDealt * defenderCost )- (defenderCounterAttackDamageDealt * attackerCost) )  / 100;

	//If it's a better score, switch targets.
	if (newRelativeSuitabilityScore > * relativeSuitabilityScore)
	{
		//New criteria: defender's damage dealt must be less than 50
		if(defenderCounterAttackDamageDealt < 50 )
		{
		*relativeSuitabilityScore = newRelativeSuitabilityScore;
		selectedMinionRecord->potentialMoveTile = &(boardToUse->Board[attackerX][attackerY]);
		selectedMinionRecord->potentialAttackTile = &(boardToUse->Board[defenderX][defenderY]);
		//No return, we want to check all tiles
		}
	}
	 
	return 0;
	  



}

double compie::findBestValuedEnemyWithinLocalArea(MasterBoard* boardToUse, compieMinionRecord * selectedMinionRecord )
{
	Cursor* myCursor = &(boardToUse->cursor);

	//First set the starting point for our search (x).
	//Then set the upper bounds for our search (maxX).
	int minX = 0;
	int maxX = 0;
	int minY = 0;
	int maxY = 0;
	

	if (myCursor->selectMinionPointer->rangeType == directFire) 
	{
		minX = myCursor->selectMinionPointer->locationX - myCursor->selectMinionPointer->movementRange;
		maxX = myCursor->selectMinionPointer->locationX + myCursor->selectMinionPointer->movementRange;

		minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
		maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	
	}
	if (myCursor->selectMinionPointer->rangeType == rangedFire)
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
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
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
			if (selectedMinionRecord->recordedMinion->rangeType == directFire && 
				(boardToUse->Board[x][y].hasMinionOnTop == false || 
				(myCursor->selectMinionPointer->locationY == y && myCursor->selectMinionPointer->locationX == x)) 
				&& boardToUse->Board[x][y].withinRange == true)
			{
				checkAdjacentTilesForBestValuedEnemy(x, y, myCursor, boardToUse, &relativeSuitabilityScore, selectedMinionRecord);
				//Already checked for closeness in the function itself so no need to do anything else
			}
			//Check each tile if suitable for artillery fire
			//Must be visible
			if (selectedMinionRecord->recordedMinion->rangeType == rangedFire)	
				if ( boardToUse->Board[x][y].hasMinionOnTop == true 
				&& boardToUse->Board[x][y].withinVision[myCursor->selectMinionPointer->team] == true
				&& myCursor->selectMinionPointer->team != boardToUse->Board[x][y].minionOnTop->team)
				{
					//Must be within range but outside minimum range
					if( myCursor->selectMinionPointer->minAttackRange < computeDistance(myCursor->selectMinionPointer->locationX, x, myCursor->selectMinionPointer->locationY, y)  
						&& myCursor->selectMinionPointer->attackRange >= computeDistance(myCursor->selectMinionPointer->locationX, x, myCursor->selectMinionPointer->locationY, y))
 					checkSingleTileForCombatValue(myCursor->selectMinionPointer->locationX, myCursor->selectMinionPointer->locationY, x, y, myCursor, boardToUse, &relativeSuitabilityScore, selectedMinionRecord);
				}
		}
	}

	return relativeSuitabilityScore;
}

int compie::findPropertyWithinLocalArea(MasterBoard* boardToUse, int* returnX, int* returnY, compieMinionRecord* selectedMinionRecord)
{
	Cursor* myCursor = &(boardToUse->cursor);
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
	if (maxX > boardToUse->BOARD_WIDTH)
	{
		maxX = boardToUse->BOARD_WIDTH;
	}
	//Same with y and maxY.
	int minY = myCursor->selectMinionPointer->locationY - myCursor->selectMinionPointer->movementRange;
	int maxY = myCursor->selectMinionPointer->locationY + myCursor->selectMinionPointer->movementRange;
	if (minY < 0)
	{
		minY = 0;
	}
	if (maxY > boardToUse->BOARD_HEIGHT)
	{
		maxY = boardToUse->BOARD_HEIGHT;
	}

	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)

		{
			//std::cout << x << " " << y <<std::endl;
			//If the tile is property and is enemy controlled.
			if (boardToUse->Board[x][y].checkForProperty(boardToUse->Board[x][y].symbol) == true && boardToUse->Board[x][y].controller != boardToUse->playerFlag)
			{
				//If the current tile DOES not have minion on top (IE we will be able to move there)
				if (boardToUse->Board[x][y].hasMinionOnTop == false)
				{
					//If the current tile is within movement range AND is less distant than current closestEnemyProp.
					//This means the minion has to be "selected", to turn on withinRange flags.
					int rangeToProp = computeDistance(x, myCursor->selectMinionPointer->locationX, y, myCursor->selectMinionPointer->locationY);
					if (boardToUse->Board[x][y].withinRange == true && rangeToProp < distanceToClosestEnemyProperty)
					{
						distanceToClosestEnemyProperty = rangeToProp;
						selectedMinionRecord->potentialMoveTile = &(boardToUse->Board[x][y]);
						selectedMinionRecord->objectiveTile = &(boardToUse->Board[x][y]);
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
int compie::determinePotentialMinionTasking(MasterBoard* boardToUse, compieMinionRecord * selectedMinionRecord)
{
	std::cout << "Determining task for " << selectedMinionRecord->recordedMinion->description << " " << selectedMinionRecord->recordedMinion->seniority<< std::endl;

	int returnX = 0;
	int returnY = 0;


	if (boardToUse->cursor.selectMinionPointer == NULL)
	{
		return 1;
	}
	Minion* myMinion = boardToUse->cursor.selectMinionPointer;

	//If already repairing, keep repairing
	//As long as consultCost doesn't return -1, the minion CAN be repaired there
	//We may still attack guys next to us
	if (myMinion->health < 100 
		&& boardToUse->Board[myMinion->locationX][myMinion->locationY].controller == compiePlayerFlag
		&& boardToUse->consultMinionCostChart(myMinion->type, boardToUse->Board[myMinion->locationX][myMinion->locationY].symbol) != -1)
	{
		selectedMinionRecord->tasking = holdPosition;
		selectedMinionRecord->taskingStatus = immediateExecute;

	}
	else
	//If highly damaged or Air and low on fuel
	if (myMinion->health < 50 ||
		(myMinion->domain == air && (  myMinion->currentFuel < (myMinion->maxFuel / 3)  || myMinion->currentPriAmmo == 0 || myMinion->currentSecAmmo == 0)))   
	{
		int distance = strategicWithdraw(boardToUse, selectedMinionRecord);
		if (distance < 999 && distance >= 0)	//Error code is -1
		{
			selectedMinionRecord->tasking = withdraw;
			selectedMinionRecord->taskingStatus = immediateExecute;

			return 1;
		}
	}
	else
	//Otherwise
		//If already capturing continue that regardless.
		if (myMinion->isCapturing == true)
		{
			selectedMinionRecord->potentialMoveTile = &boardToUse->Board[myMinion->locationX][myMinion->locationY];
			selectedMinionRecord->objectiveTile = &boardToUse->Board[myMinion->locationX][myMinion->locationY];
			selectedMinionRecord->tasking = captureLocalProperty;
			selectedMinionRecord->taskingStatus = immediateExecute;
			return 1;
		}
		else
			if (defendHeadquarters(boardToUse, selectedMinionRecord) == 1) 
			{
				return 1;
			}
			else		//DefendProperty ensures that defending prop has higher priority than attacking a local minion regardless of cost
				if (defendProperty(boardToUse, selectedMinionRecord) == 1) 
				{
					selectedMinionRecord->tasking = defendProp;
					selectedMinionRecord->taskingStatus = immediateExecute;
					return 1;
				
				}
				else
				{
			
			//Otherwise see if there are enemies in local area within range and suitable to attack.
			double relativeSuitabilityScore = findBestValuedEnemyWithinLocalArea(boardToUse, selectedMinionRecord);
			if (relativeSuitabilityScore > 0 && selectedMinionRecord->potentialAttackTile != NULL)
			{
				selectedMinionRecord->tasking = attackLocalMinion;
				selectedMinionRecord->taskingStatus = immediateExecute;
				return 1;
			}

			//If infantry, attempt to capture local properties.
			if (boardToUse->cursor.selectMinionPointer->specialtyGroup == infantry)
			{
				int distance = findPropertyWithinLocalArea(boardToUse, &returnX, &returnY, selectedMinionRecord);
				if (distance < 999 && selectedMinionRecord->potentialMoveTile != NULL)
				{
					selectedMinionRecord->tasking = captureLocalProperty;
					selectedMinionRecord->taskingStatus = immediateExecute;
					return 1;
				}
			}
				
			
			int distance = strategicAdvance(boardToUse, selectedMinionRecord);
			if (distance < 999 && distance >= 0)	//Error code is -1
			{
				selectedMinionRecord->tasking = advance;
				selectedMinionRecord->taskingStatus = immediateExecute;
				return 1;
			}
			
		}



	
	
	//Check adjacent tiles if we want to attack them, even if we stood still.
	double suitabilityScore = 0;

	checkAdjacentTilesForBestValuedEnemy(selectedMinionRecord->recordedMinion->locationX, selectedMinionRecord->recordedMinion->locationY,
		&boardToUse->cursor, boardToUse, &suitabilityScore, selectedMinionRecord);
	
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
	
	return 0;
}

int compie::executeMinionTasks(MasterBoard* boardToUse, compieMinionRecord* selectedMinionRecord)
{
	std::cout << "Moving " << selectedMinionRecord->recordedMinion->description << " " << selectedMinionRecord->recordedMinion->seniority<< std::endl;

	int whoIsWatching = -1;		//-1 Is "default" meaning in a standard multiplayer game, nothing is printed during compie turn. It's hidden from view.
	
	//If we're in debug mode, give player vision over everything.
	if (menuPointer->debugMode == true)
	{
		whoIsWatching = 0;

	}
	else
		//If single player make sure player 1 can see, and print for them.
		if (boardToUse->isItSinglePlayerGame == true)
		{
			whoIsWatching = 1;

		}

	if (selectedMinionRecord->tasking == attackLocalMinion || selectedMinionRecord->tasking == defendHQ || selectedMinionRecord->tasking == defendProp)
	{
		//Move cursor
		if (boardToUse->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY) == 1)
		{
			std::cout << "fail move" << std::endl;
		}


		//moveMinion needs to contain all the status elements too.
		int moveResult = boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord, InputLayer, whoIsWatching);

		if (moveResult == 0)
		{
			//The move automatically deselects. Thus reselect.
			boardToUse->selectMinion(boardToUse->cursor.getX(), boardToUse->cursor.getY());
			boardToUse->attackMinion(selectedMinionRecord->potentialAttackTile->locationX, selectedMinionRecord->potentialAttackTile->locationY, InputLayer, whoIsWatching);
			
		}

	}

	if (selectedMinionRecord->tasking == captureLocalProperty)
	{
		//Move cursor
		if (boardToUse->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY) == 1)
		{
			std::cout << "fail move" << std::endl;
		}

		//moveMinion needs to contain all the status elements too.
		int moveResult = boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord, InputLayer, whoIsWatching);

		//Capture enemy property
		//The move automatically deselects. Thus reselect.
		if (moveResult == 0) 
		{
			boardToUse->selectMinion(boardToUse->cursor.getX(), boardToUse->cursor.getY());
			(void) boardToUse->captureProperty(selectedMinionRecord->objectiveTile, boardToUse->cursor.selectMinionPointer, InputLayer, whoIsWatching);

		}

	}

	if (selectedMinionRecord->tasking == advance)
	{
		//Move cursor
		boardToUse->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY);

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord, InputLayer, whoIsWatching);

	}

	if (selectedMinionRecord->tasking == withdraw)
	{
		//Move cursor
		boardToUse->cursor.relocate(selectedMinionRecord->potentialMoveTile->locationX, selectedMinionRecord->potentialMoveTile->locationY);

		//moveMinion needs to contain all the status elements too.
		boardToUse->moveMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord, InputLayer, whoIsWatching);

	}

	if (selectedMinionRecord->tasking == holdPosition)
	{
		//Do nothing
	}


	boardToUse->deselectMinion();

	boardToUse->setVisionField(whoIsWatching);
	boardToUse->checkWindow();
	InputLayer->printScreen(boardToUse, whoIsWatching, false);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	//Regardless of the tasking, it has now been executed.
	selectedMinionRecord->taskingStatus = taskingExecuted;
	


	return 1;
}

int compie::takeMyTurn(MasterBoard* boardToUse)
{
	int whoIsWatching = -1;		//-1 Is "default" meaning in a standard multiplayer game, nothing is printed during compie turn. It's hidden from view.

	//If we're in debug mode, give player vision over everything.
	if (menuPointer->debugMode == true)
	{
		whoIsWatching = 0;

	}
	else
		//If single player make sure player 1 can see, and print for them.
		if (boardToUse->isItSinglePlayerGame == true)
		{
			whoIsWatching = 1;

		}
	
	boardToUse->upkeep(InputLayer, whoIsWatching);

	statusUpdate(boardToUse);

	bool allMinionsHaveMoved = false;
	int waveIterator = 0;


	//Very first move: Artillery fire at targets of opportunity.
	for (int i = 0; i < compieMinionRoster.size(); i++)
	{
		if (compieMinionRoster[i] != NULL &&  compieMinionRoster[i]->recordedMinion->rangeType == rangedFire && compieMinionRoster[i]->taskingStatus == (awaitingTasking))
		{
			//Move cursor, then select minion, then determine tasks
			boardToUse->cursor.relocate(compieMinionRoster[i]->recordedMinion->locationX, compieMinionRoster[i]->recordedMinion->locationY);
			boardToUse->selectMinion(boardToUse->minionRoster[i]->locationX, boardToUse->minionRoster[i]->locationY);
			determinePotentialMinionTasking(boardToUse, compieMinionRoster[i]);

			if (compieMinionRoster[i]->tasking == attackLocalMinion)
			{
				executeMinionTasks(boardToUse, compieMinionRoster[i]);
			}
			else //If we couldn't find someone to shoot, reset to awaiting tasking, and move on.
			{
				compieMinionRoster[i]->taskingStatus = awaitingTasking;
				compieMinionRoster[i]->potentialMoveTile = NULL;
			}
			boardToUse->deselectMinion();
		}
	}

	//Go through minionRecord and determine tasking.
	while (allMinionsHaveMoved == false)
	{
		//For a single iterationa assume all minions moved until proven otherwise
		allMinionsHaveMoved = true;

		//Still need to defendHQ.

		for (int i = 0; i < compieMinionRoster.size(); i++)
		{
			if (compieMinionRoster[i] != NULL)

			{
				//Move cursor, then select minion
				boardToUse->cursor.relocate(compieMinionRoster[i]->recordedMinion->locationX, compieMinionRoster[i]->recordedMinion->locationY);

				boardToUse->selectMinion(boardToUse->cursor.XCoord, boardToUse->cursor.YCoord);


				//If awaitingTasking, determine tasking.
				if (compieMinionRoster[i]->taskingStatus == (awaitingTasking))
				{
					determinePotentialMinionTasking(boardToUse, compieMinionRoster[i]);
				}

				//If tasking is immediateExecute, do it now.
				//Should already be selected
				if (compieMinionRoster[i]->taskingStatus == (immediateExecute))
				{
					executeMinionTasks(boardToUse, compieMinionRoster[i]);
				}

				//If a single minion has not finished its tasking, set to false
				if (compieMinionRoster[i]->taskingStatus != taskingExecuted)
				{
					allMinionsHaveMoved = false;
				}

				//If we are on Wave 3, execute, regardless of priority.
				if (waveIterator == 3)
				{
					executeMinionTasks(boardToUse, compieMinionRoster[i]);
				}

				boardToUse->deselectMinion();

			}
		}


		waveIterator++;
	}
	

	//Still need to implement non-immediateExecute taskingStatus

	if ( boardToUse->playerRoster[compiePlayerFlag].stillAlive == true)
	{

		determineProduction(boardToUse);

		   
		int incrementGameTurn = boardToUse->endTurn(InputLayer);
		
		//This is convoluted but if only compies are playing for us to view, we want to skip past the "waiting for next player" screen
		//This is overriding the endTurn function.
		bool allComputerPlayers = true;
		for (int i = 1; i < boardToUse->NUMBEROFPLAYERS+1; i++)
		{
			if (boardToUse->playerRoster[i].playerType == humanPlayer)
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
		//Have to always keep an autosave!
		menuPointer->gameSave(".\\savegames\\Auto_save.txt", boardToUse);
	}

	return 1;
}

//Right now this is actually buying units too.
int compie::determineProduction(MasterBoard* boardToUse)
{

	int totalFactoriesLeft = 0;
	int numberOfTanks = 0;
	int numberOfInfantry = 0;
	int numberOfCavalry = 0;
	int numberOfSpecialists = 0;
	int numberOfRanged = 0;
	int numberOfBombers = 0;

	//Go through map and find all of our factories for totalFactories
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			//If the current tile is our factory
			if (boardToUse->Board[x][y].symbol == 'h' && boardToUse->Board[x][y].controller == boardToUse->playerFlag)
			{
				totalFactoriesLeft++;
			}
		}
	}

	//Go through minion Roster. Find various unit totals.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{

		if (boardToUse->minionRoster[i] != NULL && boardToUse->minionRoster[i]->team == boardToUse->playerFlag)
		{
			switch (boardToUse->minionRoster[i]->type)
			{
			case('i'):
			{
				numberOfInfantry++;
				break;
			}
			case('a'):
			case('T'):
			{
				numberOfTanks++;
				break;
			}
			case('c'):
			{
				numberOfCavalry++;

				break;
			}
			case('s'):
			{
				numberOfSpecialists++;
				break;
			}
			case('r'):
			case('R'):
			{
				numberOfRanged++;
				break;
			}
			case('v'):
			case('b'):
			{
				numberOfBombers++;
				break;
			}
			}
		}
	}

	//Go through each factory and purchase a unit based on unit totals and available treasury.
	for (int x = 0; x < boardToUse->BOARD_WIDTH; x++)
	{
		for (int y = 0; y < boardToUse->BOARD_HEIGHT; y++)
		{
			//First determine "surplus" treasury, which accounts for at least purchasing an infantry at every possible factory.
			int availableTreasury = boardToUse->playerRoster[boardToUse->playerFlag].treasury + 1000 - totalFactoriesLeft * 1000;

			//This can be tweaked. 
			//If game turn is after turn 4, we do not need as many infantry, so we we set availableTreasury to arbitrarily large
			//Every  factory amount will be "enough"
			if (menuPointer->gameTurn > 2)
				availableTreasury += 100000;

			//If the current tile is our factory and there is no one already on top
			if (boardToUse->Board[x][y].symbol == 'h' && boardToUse->Board[x][y].controller == boardToUse->playerFlag && boardToUse->Board[x][y].hasMinionOnTop == false)
			{


				//If we have a proper proportion of tanks, buy cavalry.
				if (int(numberOfTanks / 3) > numberOfCavalry 
					&& boardToUse->consultMinionCostChart('c', 'h') <= availableTreasury 
					&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('c', 'h'))
				{
					//Must be able to actually afford the unit.				
					boardToUse->attemptPurchaseMinion('c', x, y, boardToUse->playerFlag);
					numberOfCavalry++;
					totalFactoriesLeft--;

				}
				else	//If we have enough cav, buy rocket
					if (int(numberOfTanks / 2) > numberOfRanged
						&& boardToUse->consultMinionCostChart('R', 'h') <= availableTreasury
						&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('R', 'h'))
					{
						//Must be able to actually afford the unit.				
						boardToUse->attemptPurchaseMinion('R', x, y, boardToUse->playerFlag);
						numberOfCavalry++;
						totalFactoriesLeft--;

					}
					else	//If we can't afford rocket, maybe we can afford arty
					if (int(numberOfTanks / 2) > numberOfRanged
						&& boardToUse->consultMinionCostChart('r', 'h') <= availableTreasury
						&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('r', 'h'))
					{
						//Must be able to actually afford the unit.				
						boardToUse->attemptPurchaseMinion('r', x, y, boardToUse->playerFlag);
						numberOfCavalry++;
						totalFactoriesLeft--;

					}
						else		//Infantry and specialists have a similar proportion.
						if (int(numberOfInfantry / 3) > numberOfSpecialists 
							&& boardToUse->consultMinionCostChart('s', 'h') <= availableTreasury 
							&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('s', 'h')
							&& 
							(compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].onlyInfantryArmySizeHere  <
								compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].infantryMaxHere)
							)
						{
							//Must be able to actually afford the unit.

							boardToUse->attemptPurchaseMinion('s', x, y, boardToUse->playerFlag);
							numberOfSpecialists++;
							totalFactoriesLeft--;
						}
						else
							if (boardToUse->consultMinionCostChart('T', 'h') <= availableTreasury 
								&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('T', 'h'))
							{
								//Must be able to actually afford the unit.
								boardToUse->attemptPurchaseMinion('T', x, y, boardToUse->playerFlag);
								numberOfTanks++;
								totalFactoriesLeft--;
							}
							else if (boardToUse->consultMinionCostChart('a', 'h') <= availableTreasury 
								&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('a', 'h'))
							{
								//Must be able to actually afford the unit.
								boardToUse->attemptPurchaseMinion('a', x, y, boardToUse->playerFlag);
								numberOfTanks++;
								totalFactoriesLeft--;
							}
							else if (boardToUse->consultMinionCostChart('i', 'h') <= availableTreasury 
								&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('i', 'h')
								&& (compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].onlyInfantryArmySizeHere <
									compieLandMassMap.roster[compieLandMassMap.grid[x][y].landMassNumber].infantryMaxHere)
								)
							{

								//Must be able to actually afford the unit.
								boardToUse->attemptPurchaseMinion('i', x, y, boardToUse->playerFlag);
								numberOfInfantry++;
								totalFactoriesLeft--;
							}

			}

			//Airbase production
			//If the current tile is our factory and there is no one already on top
			if (boardToUse->Board[x][y].symbol == 'A' &&
				boardToUse->Board[x][y].controller == boardToUse->playerFlag &&
				boardToUse->Board[x][y].hasMinionOnTop == false)
			{
				if (int(numberOfTanks / 3) > numberOfBombers
					&& boardToUse->consultMinionCostChart('b', 'A') <= availableTreasury
					&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('b', 'A'))
				{
					//Must be able to actually afford the unit.
					boardToUse->attemptPurchaseMinion('b', x, y, boardToUse->playerFlag);
					numberOfBombers++;
				}
			}

			if (boardToUse->Board[x][y].symbol == 'A' &&
				boardToUse->Board[x][y].controller == boardToUse->playerFlag &&
				boardToUse->Board[x][y].hasMinionOnTop == false)
			{
				if (  int(numberOfTanks / 3) > numberOfBombers
					&& boardToUse->consultMinionCostChart('v', 'A') <= availableTreasury 
					&& boardToUse->playerRoster[boardToUse->playerFlag].treasury >= boardToUse->consultMinionCostChart('v', 'A'))
				{
					//Must be able to actually afford the unit.
					boardToUse->attemptPurchaseMinion('v', x, y, boardToUse->playerFlag);
					numberOfBombers++;
				}
			}


			


		}

	}
	return 0;
}

int compie::buildCompieMinionRoster(MasterBoard* boardToUse) 
{
	int numberOfCompieMinions = 0;

	//A new compieMinionRoster every turn, instead of trying to track minion deaths.

	//Go through minion Roster.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		compieMinionRoster[i] = NULL;
		//Must be a valid minion, and compie's minion. 
		if (boardToUse->minionRoster[i] != NULL && boardToUse->minionRoster[i]->team == boardToUse->playerFlag)
		{
			numberOfCompieMinions++;
			compieMinionRoster[i] = new compieMinionRecord(boardToUse->minionRoster[i]);

			//Also count number of infantry/total units in this land mass.
			compieLandMassMap.roster[compieLandMassMap.grid[boardToUse->minionRoster[i]->locationX][boardToUse->minionRoster[i]->locationY].landMassNumber].armySizeHere++;
			if (boardToUse->minionRoster[i]->specialtyGroup == infantry)
			{
				compieLandMassMap.roster[compieLandMassMap.grid[boardToUse->minionRoster[i]->locationX][boardToUse->minionRoster[i]->locationY].landMassNumber].onlyInfantryArmySizeHere++;
			}
		}
	}

	return 0;
}
