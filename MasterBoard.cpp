//Copyright 2022, Supercontinent Software Ltd.
//
//	masterboard.cpp
//
/*
The MasterBoard class is responsible for all "mechanical" gameplay actions. It contains the actual board with all tiles and minions,
and calculates movements, attacks, supply, production, etc. Masterboard and inputLayer interact as a team, with cursor, minion, and tile
classes as subordinate supporting classes.
*/

#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include <string>
#include <iostream>
#include "inputLayer.hpp"
#include "compie.hpp"
#include <cmath>
#include "mainmenu.h"
#include <algorithm>

//Faction 0 is neutral faction, which gets no special units.
bool minionsAllowedForFaction[5][32] =
//                                          Inf     Insrg   Oper    Spec    recon   tech    APC     IFV     Arty    Armr    Asgun   Upgarm  antia   rockt   hvyarm  mdnarm  tcop    acop    int     advf    multi   bomb,   gboat   cruis   land    sub     ship    cvn     vctlau  shvy    cavlry  
/*Neutral*/								  { true,   false,  false,  true,   true,   false,  true,   false,  true,   true,   false,  false,  true,   true,   true,   false,  true,   true,   true,   false,  false,  true,   true,   true,   true,   true,   true,   true,   false,  false,  false, true,
/*North Redonia*/                           true,   false,  false,  true,   true,   false,  true,   false,  false,  true,   true,   true,   true,   true,   true,   false,  true,   true,   true,   false,  true,   true,   true,   true,   true,   true,   true,   true,   false,  false,  false, true,
/*South Redonia*/                           true,   false,  false,  true,   true,   false,  false,   true,  true,   true,   false,  false,  true,   true,   false,  true,   true,   true,   true,   true,   false,  true,   true,   true,   true,   true,   true,   true,   false,  false,  false, true,
/*Ormosa*/                                  true,   true,   true,   true,   false,   true,  true,   false,  true,   true,   false,  false,  true,   true,   true,   false,  true,   true,   true,   false,  false,  true,   true,   true,   true,   true,   true,   true,   false,  false,  false, true,
/*Torran*/                                  true,   false,  false,  true,   true,   false,  true,   false,  true,   true,   false,  false,  true,   true,   true,   false,  true,   true,   true,   false,  false,  true,   true,   true,   true,   true,   true,   true,   true,   true,   true , true  };


//If the two input coordinates are next to each other, return true. Otherwise, return false.
//This can be either horizontal or vertical adjacency.
bool isAdjacent(int inputX1, int inputX2, int inputY1, int inputY2)
{
	if ((abs(inputX1 - inputX2) <= 1) && (abs(inputY1 - inputY2) == 0)
		|| (abs(inputX1 - inputX2) == 0) && (abs(inputY1 - inputY2) <= 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}


int MasterBoard::computeDistance(int inputX1, int inputX2, int inputY1, int inputY2)
{
	return abs(inputX1 - inputX2) + abs(inputY1 - inputY2);

}

//Returns if the observer has any minions adjacent to the indicated XY combo.
bool MasterBoard::hasAdjacentMinion(int inputX, int inputY, int observerNumber)
{
	bool adjacentMinionExists = false;

	if (inputX < BOARD_WIDTH - 1 && Board[inputX + 1][inputY].hasMinionOnTop == true
		&& Board[inputX + 1][inputY].minionOnTop->team == observerNumber)
	{
		adjacentMinionExists = true;
	}
	else
		if (inputY < BOARD_HEIGHT - 1 && Board[inputX][inputY + 1].hasMinionOnTop == true
			&& Board[inputX][inputY + 1].minionOnTop->team == observerNumber)
		{
			adjacentMinionExists = true;
		}
		else
			if (inputY > 0 && Board[inputX][inputY - 1].hasMinionOnTop == true
				&& Board[inputX][inputY - 1].minionOnTop->team == observerNumber)
			{
				adjacentMinionExists = true;
			}
			else
				if (inputX > 0 && Board[inputX - 1][inputY].hasMinionOnTop == true
					&& Board[inputX - 1][inputY].minionOnTop->team == observerNumber)
				{
					adjacentMinionExists = true;
				}

	return adjacentMinionExists;
}

//Check all adjacent tiles if an enemy landmine is there, and if so, that landmine attacks.
//This function is called after movement.
//Multiple landmines may be activated after a move, even if the minion is destroyed by the first attack.
int MasterBoard::landmineCheck(int inputX, int inputY, inputLayer* InputLayer, int observerNumber)
{
	if (inputX < BOARD_WIDTH - 1 && Board[inputX + 1][inputY].hasMinionOnTop == true
		&& Board[inputX + 1][inputY].minionOnTop->team != playerFlag
		&& Board[inputX + 1][inputY].minionOnTop->type == "Landmine")
	{
 		landmineAttack( inputX + 1, inputY, InputLayer, observerNumber);
	}

	if (inputY < BOARD_HEIGHT - 1 && Board[inputX][inputY + 1].hasMinionOnTop == true
		&& Board[inputX][inputY + 1].minionOnTop->team != playerFlag
		&& Board[inputX ][inputY + 1].minionOnTop->type == "Landmine")
	{
		landmineAttack( inputX , inputY + 1, InputLayer, observerNumber);
	}

	if (inputY > 0 && Board[inputX][inputY - 1].hasMinionOnTop == true
		&& Board[inputX][inputY - 1].minionOnTop->team != playerFlag
		&& Board[inputX ][inputY - 1].minionOnTop->type == "Landmine")
	{
		landmineAttack( inputX, inputY - 1, InputLayer, observerNumber);
	}
	
	if (inputX > 0 && Board[inputX - 1][inputY].hasMinionOnTop == true
		&& Board[inputX - 1][inputY].minionOnTop->team != playerFlag
		&& Board[inputX - 1][inputY].minionOnTop->type == "Landmine")
	{
		landmineAttack( inputX - 1, inputY, InputLayer, observerNumber);
	}

	return 0;

}

void MasterBoard::singleSquareResupply(Minion* SupplyUnit, bool isItDuringUpkeep, inputLayer* InputLayer, int observerNumber, int xVal, int yVal)
{
	if (Board[xVal][yVal].hasMinionOnTop == true && Board[xVal][yVal].minionOnTop->team == playerFlag)
	{
		if ((Board[xVal][yVal].minionOnTop->domain != air && SupplyUnit->type != "APC")
		|| (Board[xVal][yVal].minionOnTop->domain == air && SupplyUnit->type != "Aircraft_Carrier"))
		{
			bool supplied = false;
			if (Board[xVal][yVal].minionOnTop->currentFuel != Board[xVal][yVal].minionOnTop->maxFuel)
			{
				Board[xVal][yVal].minionOnTop->currentFuel = Board[xVal][yVal].minionOnTop->maxFuel;
				supplied = true;
			}
			if (Board[xVal][yVal].minionOnTop->currentPriAmmo != Board[xVal][yVal].minionOnTop->maxPriAmmo)
			{
				Board[xVal][yVal].minionOnTop->currentPriAmmo = Board[xVal][yVal].minionOnTop->maxPriAmmo;
				supplied = true;
			}
			if (Board[xVal][yVal].minionOnTop->currentSecAmmo != Board[xVal][yVal].minionOnTop->maxSecAmmo)			
			{
				Board[xVal][yVal].minionOnTop->currentSecAmmo = Board[xVal][yVal].minionOnTop->maxSecAmmo;
				supplied = true;
			}
			if (supplied == true)
			{
				InputLayer->supplyGraphics(this, observerNumber, Board[xVal][yVal].minionOnTop, xVal, yVal);
			}
		}
	}
	return;
}


//Are the currentfuel vs max fuel checks needed ?
int MasterBoard::individualResupply(Minion* SupplyUnit, bool isItDuringUpkeep, inputLayer* InputLayer, int observerNumber)
{

	int x = SupplyUnit->locationX;
	int y = SupplyUnit->locationY;

	//Check each surrounding tile for a ground or sea unit and resupply them if this is APC
	//Or if this is ac carrier and they're air.
	if (x < BOARD_WIDTH - 1)
	{
		singleSquareResupply( SupplyUnit, isItDuringUpkeep, InputLayer, observerNumber, x + 1, y);
	}

	if (y < BOARD_HEIGHT - 1)
	{
		singleSquareResupply( SupplyUnit, isItDuringUpkeep, InputLayer, observerNumber, x , y + 1);
	}

	if (x > 0)
	{
		singleSquareResupply( SupplyUnit, isItDuringUpkeep, InputLayer, observerNumber, x - 1, y );
	}

	if (y > 0)
	{
		singleSquareResupply( SupplyUnit, isItDuringUpkeep, InputLayer, observerNumber, x , y - 1);
	}

	//Carrier gets to resupply its carried minion
	if (SupplyUnit->type != "Aircraft_Carrier" && SupplyUnit->firstMinionBeingTransported != NULL)
	{
		if (SupplyUnit->firstMinionBeingTransported->currentFuel != SupplyUnit->firstMinionBeingTransported->maxFuel ||
			SupplyUnit->firstMinionBeingTransported->currentPriAmmo != SupplyUnit->firstMinionBeingTransported->maxPriAmmo ||
			SupplyUnit->firstMinionBeingTransported->currentSecAmmo != SupplyUnit->firstMinionBeingTransported->maxSecAmmo)
		{
			SupplyUnit->firstMinionBeingTransported->currentFuel = SupplyUnit->firstMinionBeingTransported->maxFuel;
			SupplyUnit->firstMinionBeingTransported->currentPriAmmo = SupplyUnit->firstMinionBeingTransported->maxPriAmmo;
			SupplyUnit->firstMinionBeingTransported->currentSecAmmo = SupplyUnit->firstMinionBeingTransported->maxSecAmmo;
			InputLayer->supplyGraphics(this, observerNumber, SupplyUnit, SupplyUnit->locationX, SupplyUnit->locationY);
		}
	}

	//Unless during upkeep then this counts as a move.
	if (isItDuringUpkeep == false)
	{
		SupplyUnit->status = hasfired;
		deselectMinion();
	}

	return 0;
}

//Attacker vs defender matrix. Attacker determines row, while defender determines column.
//In order they are Infantry, Specialist, Armor, Artillery, Cavalry, Rocket, Heavy Armor, and Anti-Air. AGN == Assault Gun. AFI == Advanced Fighter. MDN == Modern Armor. MUL == Multirole.
//When updating ATTACK_VALUES_MATRIX, also update consultAttackValuesChart, consultMinionCostChart, movement cost, and Minion(). (Tile, masteboard, minion.)
//														i     s     a     r     c     R     T     A     v	  h		P  	  f		b	  L		B	  C	    G	  U		V		K/S    MDN/SHV	AFI
const double ATTACK_VALUES_MATRIX[27][22] = {	/*i*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,     	0,		0,
												/*s*/	0.0,  0.00, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,	  0,	0,	  0,    0,	  0,	0,		0.2,	0.15,	0,
												/*a*/	0.0,  0.00, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,	  0.25,	0,	  0,    0.25, 0,	0,		0.2,	0.15,	0,
												/*r*/	0.90, 0.85, 0.70, 0.75, 0.75, 0.75, 0.40, 0.75, 0,	  0,	0.70, 0,	0,	  0.30,	0.30, 0.35, 0.45, 0,	0.30,	0.4,	0.3,	0,
												/*c*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,    0,	  0,	0,		0,		0,		0,
												/*R*/	0.95, 0.90, 0.80, 0.85, 0.85, 0.85, 0.50, 0.85, 0,	  0,	0.80, 0,	0,	  0.60, 0.40, 0.50, 0.70, 0,	0.40,	0.5,	0.45,	0,
												/*T*/	0.0,  0.00, 0.75, 0.80, 0.80, 0.85, 0.55, 0.80, 0.00, 0.00, 0.90, 0,	0,	  0.15, 0,	  0,	0.15, 0,	0,		0.55,	0.4,	0,
												/*A*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.95, 1.00, 0, 	  0.60, 0.70, 0,	0,	  0,	0,	  0,	0,		0,		0,		0.5,
												/*v*/	0.00, 0.00, 0.50, 0.50, 0.60, 0.70, 0.35, 0.50, 0.00, 0.00, 0.60, 0,	0,	  0.35,	0.25, 0.30, 0.35, 0,	0.25,	0.35,	0.3,	0,
												/*h*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*P*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*f*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.80, 1.0,	0,	  0.60,	0.70, 0,	0,	  0,	0,	  0,	0,		0,		0,		0.45,
												/*b*/	0.95, 0.90, 0.80, 0.85, 0.85, 0.85, 0.50, 0.85, 0,	  0,	0.80, 0,	0,	  0.80,	0.50, 0.60, 0.85, 0,	0.50,	0.5,	0.45,	0,
												/*L*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*B*/	1.0,  0.95, 0.85, 0.90, 0.90, 0.90, 0.60, 0.90, 0,	  0,	0.90, 0,	0,	  0.70,	0.50, 0.60, 0.75, 0,	0.50, 	0.6,	0.5,	0,
												/*C*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.95, 1.00, 0, 	  0.60, 0.70, 0,	0,	  0,	0,	  0,	0,		0,		0,		0.45,
												/*G*/	0,    0,    0,    0,    0,    0,    0,	  0,	0,	  0,	0, 	  0,	0,	  0.60, 0.40, 0.50, 0.60, 0,	0.40,	0,		0,		0,
												/*U*/	0,    0,    0,    0,    0,    0,    0,	  0,	0,	  0,	0, 	  0,	0,	  0.75,	0.40, 0.70, 0.95, 0.50,	0.40,	0,		0,		0,
												/*V*/	0,    0,    0,    0,    0,    0,    0,	  0,	0,	  0,	0, 	  0,	0,	  0,	0,	  0,    0,	  0,	0, 		0,		0,		0,
												/*K*/	0.95, 0.90, 0.80, 0.85, 0.85, 0.85, 0.50, 0.85, 0,	  0,	0.80, 0,	0,	  0.60, 0.40, 0.50, 0.70, 0,	0.40,	0.5,	0.4,	0,
												/*S*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.95, 1.00, 0, 	  0.60, 0.70, 0,	0,	  0,	0,	  0,	0,		0 ,		0,		0.45,
												/*AGN*/ 0.70, 0.65, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,	  0,	0,	  0,    0,	  0,	0,		0.2,	0.15,	0,
												/*MDN*/	0.0,  0.00, 0.75, 0.80, 0.80, 0.85, 0.55, 0.80, 0.00, 0.00, 0.90, 0,	0,	  0.15, 0,	  0,	0.15, 0,	0,		0.55, 	0.55 ,	0,
												/*MUL*/	0,	  0,  	0.50, 0.60, 0.65, 0.70, 0.20, 0.50, 0.50, 0.70, 0.65, 0.35,	0.50, 0.25,	0.20, 0.15, 0.25, 0,	0.2,	0.2,	0.15 ,	0.25,
												/*AFI*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.80, 1.0,	0,	  0.60,	0.70, 0,	0,	  0,	0,	  0,	0,		0,		0,		0.6,
												/*IFV*/	1.0,  0.95, 0.20, 0.30, 0.40, 0.45, 0.05, 0.20, 0.15, 0.25, 0.40, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.05,	0.02,	0 ,
                                                /*VIC*/	1.0,  0.95, 0.70, 0.75, 0.85, 0.85, 0.4,  0.75, 0,	  0,	0.70, 0,	0,	  0.45, 0.30, 0.35, 0.55, 0,	0.30,	0.5,	0.3,	0};



const double SECONDARY_ATTACK_VALUES_MATRIX[27][22] = {	
												//		i     s     a     r     c     R     T     A     v     h     P	  f		b	  L		B	  C	    G	  U		V		K/S     MDN/SHV AFI
												/*i*/	0.55, 0.50, 0.05, 0.10, 0.15, 0.25, 0.01, 0.05, 0.05, 0.10, 0.10, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.01,	0.01,	0,
												/*s*/	0.60, 0.55, 0.08, 0.12, 0.18, 0.30, 0.02, 0.08, 0.08, 0.12, 0.12, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.02,	0.02,	0,
												/*a*/	0.70, 0.65, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.10, 0.15, 0.35, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.05,	0.02,	0,
												/*r*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*c*/	0.75, 0.70, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.10, 0.15, 0.35, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.05,	0.02,	0,
												/*R*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0, 		0,
												/*T*/	0.85, 0.80, 0.15, 0.25, 0.40, 0.50, 0.10, 0.20, 0.15, 0.25, 0.40, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.1,	0.05,	0,
												/*A*/	1.0,  0.95, 0.20, 0.30, 0.40, 0.45, 0.05, 0.20, 0.95, 1.00, 0.40, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.05,	0.02,	0,
												/*v*/	0.65, 0.65, 0.10, 0.20, 0.35, 0.45, 0.05, 0.10, 0.55, 0.75, 0.35, 0,	0,	  0.05,	0.05, 0.05,	0.05, 0,	0.05,	0.05,	0.05,	0,
												/*h*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*P*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*f*/	0,    0,    0,    0,    0,    0,    0,	  0,	0.40, 0.5,	0,	  0.30,	0.35, 0,	0,	  0,	0,	  0,	0,		0,		0,		0.25,
												/*b*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0, 	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*L*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0  ,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*B*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*C*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0.60, 0.40, 0.50, 0.60, 0.50,	0.40,	0,		0,		0,
												/*G*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*U*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*V*/	0,    0,    0,    0,    0,    0,    0,	  0,	0,	  0,	0, 	  0,	0,	  0,	0,	  0,    0,	  0,	0 ,		0,		0,		0,
												/*K*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0,
												/*S*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0 ,		0,		0,
												/*AGN*/ 0.70, 0.65, 0.55, 0.70, 0.70, 0.80, 0.20, 0.65, 0.00, 0.00, 0.70, 0,	0,	  0,	0,	  0,    0,	  0,	0,		0.2 ,	0.15,	0,
												/*MDN*/	1.0,  0.95, 0.20, 0.30, 0.40, 0.50, 0.15, 0.20, 0.20, 0.35, 0.50, 0,	0,	  0.05,	0,	  0,	0.05, 0,	0,		0.15,   0.10,	0,
												/*MUL*/	0.65, 0.60, 0.10, 0.15, 0.30, 0.40, 0.05, 0.10, 0.20, 0.25, 0.30, 0.25,	0.35, 0.05,	0.05, 0.05,	0.05, 0,	0.05,	0.05,	0.05,	0.20,
												/*AFI*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0	,
												/*IFV*/	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0,		0, 
                                                /*VIC*/	0,    0,    0,    0,    0,    0,    0,	  0,    0,    0,	0,	  0,	0,	  0,	0,	  0,	0,	  0,	0,		0,		0, 		0 };

//Assign numeric values for different units to access attack values matrix easier.
//Assumes attacking minion is selected. Otherwise withinRange won't work.
//Needs defaults to catch error!!!!

double MasterBoard::consultAttackValuesChart(std::string attackingType, std::string defendingType,  double & primaryAttackValue, double & secondaryAttackValue , double & bestAttackValue) 
{
	int x = -1;
	int y = -1;


	if (defendingType == "Infantry" || defendingType == "Operative" || defendingType == "Cavalry" || defendingType == "Landmine")
		x = 0;
	else
	if (defendingType == "Specialist" || defendingType == "Insurgent" || defendingType == "Engineer")
		x = 1;
	else
	if (defendingType == "Armor" || defendingType == "Upgunned_Armor" )
		x = 2;
	else
	if (defendingType == "Artillery" || defendingType == "Assault_Gun" )
		x = 3;
	else
	if (defendingType == "Recon" || defendingType == "Technical")
		x = 4;
	else
	if (defendingType == "Rocket_Artillery" || defendingType == "Victory_Launcher")
		x = 5;
	else
	if (defendingType == "Heavy_Armor")
		x = 6;
	else
	if (defendingType == "Anti-Aircraft" || defendingType == "IFV")
		x = 7;
	else
	if (defendingType == "Attack_Copter")
		x = 8;
	else
	if (defendingType == "Transport_Copter")
		x = 9;
	else
	if (defendingType == "APC")
		x = 10;
	else
	if (defendingType == "Interceptor")
		x = 11;
	else
	if (defendingType == "Bomber" || defendingType == "Multirole")
		x = 12;
	else
	if (defendingType == "Lander")
		x = 13;
	else
	if (defendingType == "Battleship")
		x = 14;
	else
	if (defendingType == "Cruiser")
		x = 15;
	else
	if (defendingType == "Gunboat")
		x = 16;
	else
	if (defendingType == "Submarine")
		x = 17;
	else
	if (defendingType == "Aircraft_Carrier")
		x = 18;
	else
	if (defendingType == "Artillery_Emplacement" || defendingType == "SAM_Site")
		x = 19;
	else
	if (defendingType == "Modern_Armor" || defendingType == "Super_Heavy_Armor")
		x = 20;
	else
	if (defendingType == "Advanced_Fighter")
		x = 21;


	if (attackingType == "Infantry" || attackingType == "Operative" || attackingType == "Cavalry" || attackingType == "Engineer")
		y = 0;
	else
	if (attackingType == "Specialist" || attackingType == "Insurgent")
		y = 1;
	else
	if (attackingType == "Armor")
		y = 2;
	else
	if (attackingType == "Artillery" || attackingType == "Landmine")
		y = 3;
	else
	if (attackingType == "Recon" || attackingType == "Technical")
		y = 4;
	else
	if (attackingType == "Rocket_Artillery")
		y = 5;
	else
	if (attackingType == "Heavy_Armor" || attackingType == "Upgunned_Armor")
		y = 6;
	else
	if (attackingType == "Anti-Aircraft")
		y = 7;
	else
	if (attackingType == "Attack_Copter")
		y = 8;
	else
	if (attackingType == "Transport_Copter")
		y = 9;
	else
	if (attackingType == "APC")
		y = 10;
	else
	if (attackingType == "Interceptor")
		y = 11;
	else
	if (attackingType == "Bomber")
		y = 12;
	else
	if (attackingType == "Lander")
		y = 13;
	else
	if (attackingType == "Battleship")
		y = 14;
	else
	if (attackingType == "Cruiser")
		y = 15;
	else
	if (attackingType == "Gunboat")
		y = 16;
	else
	if (attackingType == "Submarine")
		y = 17;
	else
	if (attackingType == "Aircraft_Carrier")
		y = 18;
	else
	if (attackingType == "Artillery_Emplacement")
		y = 19;
	else
	if (attackingType == "SAM_Site")
		y = 20;
	else
	if (attackingType == "Assault_Gun")
		y = 21;
	else
	if (attackingType == "Modern_Armor" || attackingType == "Super_Heavy_Armor")
		y = 22;
	else
	if (attackingType == "Multirole")
		y = 23;
	else
	if (attackingType == "Advanced_Fighter")
		y = 24;
	else
	if (attackingType == "IFV")
		y = 25;
    else
	if (attackingType == "Victory_Launcher")
		y = 26;


	if (x == -1 || y == -1)
	{
		std::cout << "ERROR Does not exist within attack values matrix" << std::endl;
		return -1;
	}
	else 
	{
		primaryAttackValue = ATTACK_VALUES_MATRIX[y][x];
		secondaryAttackValue = SECONDARY_ATTACK_VALUES_MATRIX[y][x];
		
		//Determine the best of the two values for easy access
		if (primaryAttackValue > secondaryAttackValue)
			bestAttackValue = primaryAttackValue;
		else bestAttackValue = secondaryAttackValue;

		return 0;
	}


}

double MasterBoard::consultAttackValuesChart(Minion& attackingMinion, Minion& defendingMinion, bool& isAmmoUsed, int& weaponUsed, bool ignoreLimitations)
{
	//Assume ammo is not used until told otherwise.
	isAmmoUsed = false;

	//Weapon used starts at 0, ie nothing used
	weaponUsed = 0;

	double primaryAttackScore = 0;
	double secondaryAttackScore = 0;

	//Throwaway since we're gonna check for ammo and other things
	double bestAttackValue = 0;

	double finalAttackScore = 0;

	consultAttackValuesChart(attackingMinion.type, defendingMinion.type, primaryAttackScore, secondaryAttackScore , bestAttackValue);

	//If ignoreLimitations is on, calculate by ignoring status, ammo, and distance.
	if (ignoreLimitations == true)
	{
		if (primaryAttackScore > secondaryAttackScore)
		{
			finalAttackScore = primaryAttackScore;
			weaponUsed = 1;
		}
		else if (primaryAttackScore <= secondaryAttackScore)
		{
			finalAttackScore = secondaryAttackScore;
			weaponUsed = 2;
		}
		if (weaponUsed == 1 && attackingMinion.maxPriAmmo > 0)
		{
			isAmmoUsed = true;
		}
		if (weaponUsed == 2 && attackingMinion.maxSecAmmo > 0)
		{
			isAmmoUsed = true;
		}

	}
	else
	{
		if (attackingMinion.rangeType == hybridRange)
		{
			if (attackingMinion.status == gaveupmovehasntfired || attackingMinion.status == hasntmovedorfired)
			{	//If hybrid and didn't move, can still do direct attack with secondary weapon.
				//Had to change to allow AA to counterattack with secondary weapon.
				if (isAdjacent(attackingMinion.locationX, defendingMinion.locationX, attackingMinion.locationY, defendingMinion.locationY))
				{
					if (attackingMinion.currentSecAmmo > 0)
					{
						isAmmoUsed = true;
						finalAttackScore = secondaryAttackScore;
						weaponUsed = 2;
					}
					else if (attackingMinion.maxSecAmmo == 0)
					{
						isAmmoUsed = false;
						finalAttackScore = secondaryAttackScore;
						weaponUsed = 2;
					}
				}
				else
				{
					//If hybrid and didn't move, and not within adjacent, can attack with ranged attack. AKA primary.
					int distance = computeDistance(attackingMinion.locationX, defendingMinion.locationX, attackingMinion.locationY, defendingMinion.locationY);
					if (distance > attackingMinion.minAttackRange && distance <= attackingMinion.attackRange)
					{
						if (attackingMinion.currentPriAmmo > 0)
						{
							isAmmoUsed = true;
							finalAttackScore = primaryAttackScore;
							weaponUsed = 1;
						}
						else if (attackingMinion.maxPriAmmo == 0)
						{
							isAmmoUsed = false;
							finalAttackScore = primaryAttackScore;
							weaponUsed = 1;
						}
					}
				}
			}
			//If hybrid and moved, can only do direct attack with secondary weapon.
			if (attackingMinion.status == hasmovedhasntfired)
			{
				if (isAdjacent(attackingMinion.locationX, defendingMinion.locationX, attackingMinion.locationY, defendingMinion.locationY))
				{
					if (attackingMinion.currentSecAmmo > 0)
					{
						isAmmoUsed = true;
						finalAttackScore = secondaryAttackScore;
						weaponUsed = 2;
					}
					else if (attackingMinion.maxSecAmmo == 0)
					{
						isAmmoUsed = false;
						finalAttackScore = secondaryAttackScore;
						weaponUsed = 2;
					}
				}
			}


		}
		if (attackingMinion.rangeType == rangedFire)
		{
			if (attackingMinion.status == gaveupmovehasntfired)
			{
				//If rangedFire and didn't move, and not within adjacent, can attack with ranged attack. AKA primary.
				int distance = computeDistance(attackingMinion.locationX, defendingMinion.locationX, attackingMinion.locationY, defendingMinion.locationY);
				if (distance > attackingMinion.minAttackRange && distance <= attackingMinion.attackRange)
				{
					if (attackingMinion.currentPriAmmo > 0)
					{
						isAmmoUsed = true;
						finalAttackScore = primaryAttackScore;
						weaponUsed = 1;
					}
					else if (attackingMinion.maxPriAmmo == 0)
					{
						isAmmoUsed = false;
						finalAttackScore = primaryAttackScore;
						weaponUsed = 1;
					}
				}
			}

		}

		if (attackingMinion.rangeType == directFire)
		{
			if (attackingMinion.status != hasfired)
			{

				if (isAdjacent(attackingMinion.locationX, defendingMinion.locationX, attackingMinion.locationY, defendingMinion.locationY))
				{
					if (primaryAttackScore > secondaryAttackScore
						&& (attackingMinion.currentPriAmmo > 0 || attackingMinion.maxPriAmmo == 0))
					{
						finalAttackScore = primaryAttackScore;
						weaponUsed = 1;
					}
					else if (primaryAttackScore <= secondaryAttackScore
						&& (attackingMinion.currentSecAmmo > 0 || attackingMinion.maxSecAmmo == 0))
					{
						finalAttackScore = secondaryAttackScore;
						weaponUsed = 2;
					}
				}
				if (weaponUsed == 1 && attackingMinion.maxPriAmmo > 0)
				{
					isAmmoUsed = true;
				}
				if (weaponUsed == 2 && attackingMinion.maxSecAmmo > 0)
				{
					isAmmoUsed = true;
				}
			}

		}
	}
	return finalAttackScore;

}


//Return of -1 indicates the minion requested does not exist, or it can't be bought there.
//New functionality: Input of '~' for propertyType indicates informational-only use of this function,
//For instance, to determine if a unit even exists, or what its price is.
int MasterBoard::consultMinionCostChart(std::string minionType, char propertyType)
{
	//Can't purchase defenses or landmines
	if (minionType == "Artillery_Emplacement" || minionType == "SAM_Site" || minionType == "Landmine")
	{
		return  100000;
	}

	bool canItBeBoughtHere = false;

	//Allows non-base inputs for information only.
	if (propertyType == '~')
		canItBeBoughtHere = true;

	//These represent what that property repairs, not necessarily if you can buy units there. Only h can buy ground troops.
	if (propertyType == 'A' && (minionType == "Attack_Copter" || minionType == "Transport_Copter" || minionType == "Interceptor"
		|| minionType == "Bomber" || minionType == "Multirole" || minionType == "Advanced_Fighter"))
	{
		canItBeBoughtHere = true;
	}
	if ((propertyType == 'n' || propertyType == 'H' || propertyType == 'Q' || propertyType == 'h') &&
		(minionType == "Infantry" || minionType == "Engineer" || minionType == "Specialist" || minionType == "Armor" || minionType == "Artillery" || minionType == "Rocket_Artillery"
			|| minionType == "Heavy_Armor" || minionType == "Anti-Aircraft" || minionType == "Recon" || minionType == "APC" || minionType == "Upgunned_Armor"
			|| minionType == "Assault_Gun" || minionType == "Insurgent" || minionType == "Operative" || minionType == "Modern_Armor" || minionType == "IFV" 
            || minionType == "Technical" || minionType == "Cavalry" || minionType == "Super_Heavy_Armor" || minionType == "Victory_Launcher"))
	{
		canItBeBoughtHere = true;
	}
	if (propertyType == 'P' && (minionType == "Battleship" || minionType == "Gunboat" || minionType == "Lander"
		|| minionType == "Submarine" || minionType == "Cruiser" || minionType == "Aircraft_Carrier"))
	{
		canItBeBoughtHere = true;
	}

	if (canItBeBoughtHere == false)
	{
		return -1;
	}

	int price = -1;

	if ( minionType == "Infantry")
		price = 1000;
		else
    if ( minionType == "Insurgent")
        price = 1500;
        else
    if ( minionType == "Cavalry")
		price = 3000;
		else
	if( minionType == "Operative")
		price = 5000;
		else
	if (minionType == "Engineer")
		price = 4000;
		else
	if ( minionType == "Specialist")
		price = 3000;
		else
	if ( minionType == "Armor")
		price = 7000;
		else
    if ( minionType == "Upgunned_Armor")
        price = 10000;
        else
	if ( minionType == "Artillery")
		price = 6000;
		else
	if ( minionType == "Assault_Gun")
		price = 6000;
		else
	if ( minionType == "Recon")
		price = 4000;
		else
	if (minionType == "Technical")
		price = 3000;
		else
	if ( minionType == "Rocket_Artillery")
		price = 15000;
		else
    if ( minionType == "Victory_Launcher")
		price = 18000;
		else
	if ( minionType == "Heavy_Armor")
		price = 16000;
		else
	if ( minionType == "Super_Heavy_Armor")
		price = 18000;
		else
    if ( minionType == "Modern_Armor")
		price = 20000;
		else
	if ( minionType == "Anti-Aircraft")
		price = 8000;
		else
	if ( minionType == "Attack_Copter")
		price = 9000;
		else
	if ( minionType == "Transport_Copter")
		price = 5000;
		else
	if ( minionType == "APC")
		price = 5000;
		else
	if (minionType == "IFV")
		price = 7000;
		else
	if ( minionType == "Interceptor")
		price = 15000;
		else
	if ( minionType == "Multirole")
		price = 11000;
		else
	if ( minionType == "Bomber")
		price = 18000;
		else
	if( minionType == "Advanced_Fighter")
		price = 20000;
		else
	if ( minionType == "Aircraft_Carrier")
		price = 25000;
		else
	if ( minionType == "Battleship")
		price = 25000;
		else
	if ( minionType == "Gunboat")
		price = 6000;
		else
	if ( minionType == "Cruiser")
		price = 14000;
		else
	if ( minionType == "Submarine")
		price = 18000;
		else
	if ( minionType == "Lander")
		price = 12000;



	return price;
}

//Currently this is not doing what it should be doing- only partial initialization.
MasterBoard::MasterBoard(sf::Texture* inputTexture , sf::Texture* inputSecondTexture)
{
	playerFlag = 0;
	myTexture = inputTexture;
	mySecondTexture = inputSecondTexture;

	//Not sure why we have to do an extra row, whatever! (Tried with one less and it caused out of bounds..... i am insane?!?!)
	//First resize board to meet savegame specifications.
	//Also resize mypathMap.
	Board.resize(BOARD_WIDTH + 1);
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		Board[i].resize(BOARD_HEIGHT);
	}

	//Initialize cursor.
	cursor.initialize(inputTexture, this);

	//Re-initialize windows
	WINDOW_HEIGHT = 10;
	WINDOW_WIDTH = 12;
	totalNumberOfMinions = 0;

	//Initialize MinionRoster to NULL.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		minionRoster[i] = NULL;
	}

	playerRoster.resize(NUMBEROFPLAYERS + 1);
	for (int i = 0; i < NUMBEROFPLAYERS + 1; i++)
	{
		playerRoster[i].treasury = 0;
		playerRoster[i].numberOfMinions = 0;
		playerRoster[i].playerFlag = i;
		playerRoster[i].name = "UNASSIGNED";
		playerRoster[i].stillAlive = true;
	}


	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].locationX = x;
			Board[x][y].locationY = y;
			Board[x][y].withinVision.resize(NUMBEROFPLAYERS + 1);
			for (int n = 0; n < NUMBEROFPLAYERS + 1; n++)
				Board[x][y].withinVision[n] = false;
		}
	}


	return;
}

//Called by masterboard for normal operations
//This service builds another parallel set of distances that ignores all units.
int MasterBoard::buildTerrainOnlyPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion)
{

	return buildTerrainOnlyPathMap(isItInitialCall, x, y, inputMinion->type, inputMinion->terrainOnlyPathMap);

}

//Called by compie to build its custom path maps
int MasterBoard::buildTerrainOnlyPathMap(bool isItInitialCall, int x, int y, std::string inputType, std::vector<std::vector<pathSquare>> &inputPathMap)	
{

	//If this is first call, reset myPathMap
	if (isItInitialCall == true)
	{
		//Clear myPathMap
		for (int i = 0; i < BOARD_WIDTH; i++)
		{
			for (int j = 0; j < BOARD_HEIGHT; j++)
			{
				inputPathMap[i][j].distanceFromMinion = -1;
				inputPathMap[i][j].wasVisited = false;
			}
		}
	}

	if (Board[x][y].consultMovementChart(inputType) == 99)
		return 0;


	//Check each neighbor to find lowest path FROM minion to this square, from among them.
	int lowestNeighboringPath = 99999;
	if (x - 1 >= 0 && inputPathMap[x - 1][y].wasVisited == true)
	{
		if (inputPathMap[x - 1][y].distanceFromMinion != -1 && inputPathMap[x - 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputPathMap[x - 1][y].distanceFromMinion;

		}
	}
	if (y - 1 >= 0 && inputPathMap[x][y - 1].wasVisited == true)
	{
		if (inputPathMap[x][y - 1].distanceFromMinion != -1 && inputPathMap[x][y - 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputPathMap[x][y - 1].distanceFromMinion;

		}
	}
	if (x + 1 < BOARD_WIDTH && inputPathMap[x + 1][y].wasVisited == true)
	{
		if (inputPathMap[x + 1][y].distanceFromMinion != -1 && inputPathMap[x + 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputPathMap[x + 1][y].distanceFromMinion;
		}
	}
	if (y + 1 < BOARD_HEIGHT && inputPathMap[x][y + 1].wasVisited == true)
	{
		if (inputPathMap[x][y + 1].distanceFromMinion != -1 && inputPathMap[x][y + 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputPathMap[x][y + 1].distanceFromMinion;

		}
	}

	//We now have lowest path to objective from visited neighbors.
	//If no visited neighbors, lowestNeighboringPath should be 99999. This means we can't get here from minion.
	if (isItInitialCall == true)
	{
		//Initialize start point
		//Cost to get to here from here 0.
		inputPathMap[x][y].distanceFromMinion = 0;
	}
	else if (lowestNeighboringPath == 99999)
	{   //It's physically impossible to get here from minion
		inputPathMap[x][y].distanceFromMinion = -1;
	}
	else
	{
		inputPathMap[x][y].distanceFromMinion = lowestNeighboringPath + Board[x][y].consultMovementChart(inputType);
	}

	inputPathMap[x][y].wasVisited = true;


	//Now call the function for eaching neighbor that is passable (not 99 move cost or off board), and hasn't been visited, or
	//is passable,  (And has been visited) and has a higher (path score - that place's cost):
	//This is to either do a first look or to update a square that has a higher current score than ours
	if (x - 1 >= 0)
	{
		//Terrain only path will ignore all minions.
		if ((Board[x - 1][y].consultMovementChart(inputType) != 99 && inputPathMap[x - 1][y].wasVisited != true) ||
			(Board[x - 1][y].consultMovementChart(inputType) != 99 &&
			(inputPathMap[x - 1][y].distanceFromMinion - Board[x - 1][y].consultMovementChart(inputType)) > inputPathMap[x][y].distanceFromMinion))
		{
			buildTerrainOnlyPathMap(false, x - 1, y, inputType, inputPathMap);			
		}
	}

	if (y - 1 >= 0)
	{
		//Terrain only path will ignore all minions.
		if ((Board[x][y - 1].consultMovementChart(inputType) != 99 && inputPathMap[x][y - 1].wasVisited != true) ||
			(Board[x][y - 1].consultMovementChart(inputType) != 99 &&
			(inputPathMap[x][y - 1].distanceFromMinion - Board[x][y - 1].consultMovementChart(inputType)) > inputPathMap[x][y].distanceFromMinion))
		{
			buildTerrainOnlyPathMap(false, x, y - 1, inputType, inputPathMap);
		}
	}

	if (x + 1 < BOARD_WIDTH)
	{
		//Terrain only path will ignore all minions.
		if ((Board[x + 1][y].consultMovementChart(inputType) != 99
			&& inputPathMap[x + 1][y].wasVisited != true) || (Board[x + 1][y].consultMovementChart(inputType) != 99
				&& (inputPathMap[x + 1][y].distanceFromMinion - Board[x + 1][y].consultMovementChart(inputType)) > inputPathMap[x][y].distanceFromMinion))
		{
			buildTerrainOnlyPathMap(false, x + 1, y, inputType, inputPathMap);
		}
	}

	if (y + 1 < BOARD_HEIGHT)
	{
		//Terrain only path will ignore all minions.
		if ((Board[x][y + 1].consultMovementChart(inputType) != 99 && inputPathMap[x][y + 1].wasVisited != true) || (Board[x][y + 1].consultMovementChart(inputType) != 99 &&
			(inputPathMap[x][y + 1].distanceFromMinion - Board[x][y + 1].consultMovementChart(inputType)) > inputPathMap[x][y].distanceFromMinion))
		{
			buildTerrainOnlyPathMap(false, x, y + 1, inputType, inputPathMap);
		}
	}


	return 1;
}

//This service builds a parallel set of distances that ignores enemy units that you cannot see.
//Used for doing "traps".
int MasterBoard::buildApparentPathMap(bool isItInitialCall, int x, int y, Minion* inputMinion)
{

	//If this is first call, reset myPathMap
	if (isItInitialCall == true)
	{
		//Clear myPathMap
		for (int i = 0; i < BOARD_WIDTH; i++)
		{
			for (int j = 0; j < BOARD_HEIGHT; j++)
			{
				inputMinion->apparentPathMap[i][j].distanceFromMinion = -1;
				inputMinion->apparentPathMap[i][j].wasVisited = false;
			}
		}
	}

	if (Board[x][y].consultMovementChart(inputMinion->type) == 99)
		return 0;


	//Check each neighbor to find lowest path FROM minion to this square, from among them.
	int lowestNeighboringPath = 99999;
	if (x - 1 >= 0 && inputMinion->apparentPathMap[x - 1][y].wasVisited == true)
	{
		if (inputMinion->apparentPathMap[x - 1][y].distanceFromMinion != -1 && inputMinion->apparentPathMap[x - 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputMinion->apparentPathMap[x - 1][y].distanceFromMinion;

		}
	}
	if (y - 1 >= 0 && inputMinion->apparentPathMap[x][y - 1].wasVisited == true)
	{
		if (inputMinion->apparentPathMap[x][y - 1].distanceFromMinion != -1 && inputMinion->apparentPathMap[x][y - 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputMinion->apparentPathMap[x][y - 1].distanceFromMinion;

		}
	}
	if (x + 1 < BOARD_WIDTH && inputMinion->apparentPathMap[x + 1][y].wasVisited == true)
	{
		if (inputMinion->apparentPathMap[x + 1][y].distanceFromMinion != -1 && inputMinion->apparentPathMap[x + 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputMinion->apparentPathMap[x + 1][y].distanceFromMinion;
		}
	}
	if (y + 1 < BOARD_HEIGHT && inputMinion->apparentPathMap[x][y + 1].wasVisited == true)
	{
		if (inputMinion->apparentPathMap[x][y + 1].distanceFromMinion != -1 && inputMinion->apparentPathMap[x][y + 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = inputMinion->apparentPathMap[x][y + 1].distanceFromMinion;

		}
	}

	//We now have lowest path to objective from visited neighbors.
	//If no visited neighbors, lowestNeighboringPath should be 99999. This means we can't get here from minion.
	if (isItInitialCall == true)
	{
		//Initialize start point
		//Cost to get to here from here 0.
		inputMinion->apparentPathMap[x][y].distanceFromMinion = 0;
	}
	else if (lowestNeighboringPath == 99999)
	{   //It's physically impossible to get here from minion
		inputMinion->apparentPathMap[x][y].distanceFromMinion = -1;
	}
	else
	{
		inputMinion->apparentPathMap[x][y].distanceFromMinion = lowestNeighboringPath + Board[x][y].consultMovementChart(inputMinion->type);
	}

	inputMinion->apparentPathMap[x][y].wasVisited = true;




	//Now call the function for eaching neighbor that is passable (not 99 move cost or off board), and hasn't been visited, or
	//is passable,  (And has been visited) and has a higher (path score - that place's cost):
	//This is to either do a first look or to update a square that has a higher current score than ours
	//Also no enemies can be here
	//This does not include air and non-air. They can pass through each other.
	if (x - 1 >= 0)
	{
		//Also need to find if any adjacent friendly minions to this square
		bool friendlyAdjacentMinion = hasAdjacentMinion(x - 1, y, playerFlag);

		//See if enemy stealth minion here.
		bool unseenStealthEnemyHere = false;
		if (Board[x - 1][y].hasMinionOnTop == true && Board[x - 1][y].minionOnTop->team != playerFlag && Board[x - 1][y].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
		{
			unseenStealthEnemyHere = true;
		}

		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x - 1][y].consultMovementChart(inputMinion->type) != 99 && inputMinion->apparentPathMap[x - 1][y].wasVisited != true) ||
			(Board[x - 1][y].consultMovementChart(inputMinion->type) != 99 &&
			(inputMinion->apparentPathMap[x - 1][y].distanceFromMinion - Board[x - 1][y].consultMovementChart(inputMinion->type)) > inputMinion->apparentPathMap[x][y].distanceFromMinion))
			if (Board[x - 1][y].withinVision[playerFlag] == false || Board[x - 1][y].hasMinionOnTop != true || Board[x - 1][y].minionOnTop->team == cursor.selectMinionPointer->team || unseenStealthEnemyHere == true
				|| (cursor.selectMinionPointer->domain != air && Board[x - 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x - 1][y].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x - 1, y, inputMinion);
			}
	}

	if (y - 1 >= 0)
	{
		//Also need to find if any adjacent friendly minions to this square
		bool friendlyAdjacentMinion = hasAdjacentMinion(x, y - 1, playerFlag);

		//See if enemy stealth minion here.
		bool unseenStealthEnemyHere = false;
		if (Board[x][y - 1].hasMinionOnTop == true && Board[x][y - 1].minionOnTop->team != playerFlag && Board[x][y - 1].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
		{
			unseenStealthEnemyHere = true;
		}

		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x][y - 1].consultMovementChart(inputMinion->type) != 99 && inputMinion->apparentPathMap[x][y - 1].wasVisited != true) ||
			(Board[x][y - 1].consultMovementChart(inputMinion->type) != 99 &&
			(inputMinion->apparentPathMap[x][y - 1].distanceFromMinion - Board[x][y - 1].consultMovementChart(inputMinion->type)) > inputMinion->apparentPathMap[x][y].distanceFromMinion))
			if (Board[x][y - 1].withinVision[playerFlag] == false || Board[x][y - 1].hasMinionOnTop != true || Board[x][y - 1].minionOnTop->team == cursor.selectMinionPointer->team || unseenStealthEnemyHere == true
				|| (cursor.selectMinionPointer->domain != air && Board[x][y - 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y - 1].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x, y - 1, inputMinion);
			}
	}

	if (x + 1 < BOARD_WIDTH)
	{
		//Also need to find if any adjacent friendly minions to this square
		bool friendlyAdjacentMinion = hasAdjacentMinion(x + 1, y, playerFlag);

		//See if enemy stealth minion here.
		bool unseenStealthEnemyHere = false;
		if (Board[x + 1][y].hasMinionOnTop == true && Board[x + 1][y].minionOnTop->team != playerFlag && Board[x + 1][y].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
		{
			unseenStealthEnemyHere = true;
		}

		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x + 1][y].consultMovementChart(inputMinion->type) != 99
			&& inputMinion->apparentPathMap[x + 1][y].wasVisited != true) || (Board[x + 1][y].consultMovementChart(inputMinion->type) != 99
				&& (inputMinion->apparentPathMap[x + 1][y].distanceFromMinion - Board[x + 1][y].consultMovementChart(inputMinion->type)) > inputMinion->apparentPathMap[x][y].distanceFromMinion))
			if (Board[x + 1][y].withinVision[playerFlag] == false || Board[x + 1][y].hasMinionOnTop != true || Board[x + 1][y].minionOnTop->team == cursor.selectMinionPointer->team || unseenStealthEnemyHere == true
				|| (cursor.selectMinionPointer->domain != air && Board[x + 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x + 1][y].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x + 1, y, inputMinion);
			}
	}

	if (y + 1 < BOARD_HEIGHT)
	{
		//Also need to find if any adjacent friendly minions to this square
		bool friendlyAdjacentMinion = hasAdjacentMinion(x, y + 1, playerFlag);

		//See if enemy stealth minion here.
		bool unseenStealthEnemyHere = false;
		if (Board[x][y + 1].hasMinionOnTop == true && Board[x][y + 1].minionOnTop->team != playerFlag && Board[x][y + 1].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
		{
			unseenStealthEnemyHere = true;
		}

		//Apparent path will assume a non-visible tile has no minion in it. Thus how it "appears" to the player.
		if ((Board[x][y + 1].consultMovementChart(inputMinion->type) != 99 && inputMinion->apparentPathMap[x][y + 1].wasVisited != true) || (Board[x][y + 1].consultMovementChart(inputMinion->type) != 99 &&
			(inputMinion->apparentPathMap[x][y + 1].distanceFromMinion - Board[x][y + 1].consultMovementChart(inputMinion->type)) > inputMinion->apparentPathMap[x][y].distanceFromMinion))
			if (Board[x][y + 1].withinVision[playerFlag] == false || Board[x][y + 1].hasMinionOnTop != true || Board[x][y + 1].minionOnTop->team == cursor.selectMinionPointer->team || unseenStealthEnemyHere == true ||
				(cursor.selectMinionPointer->domain != air && Board[x][y + 1].minionOnTop->domain == air)
				|| (cursor.selectMinionPointer->domain == air && Board[x][y + 1].minionOnTop->domain != air))
			{
				buildApparentPathMap(false, x, y + 1, inputMinion);
			}
	}


	return 1;
}


int MasterBoard::buildPath(bool isItInitialCall, int x, int y, std::string minionType, std::vector<std::vector<pathSquare>>& pathMapPointer)
{

	//If this is first call, reset myPathMap
	if (isItInitialCall == true)
	{
		//Clear myPathMap
		for (int i = 0; i < BOARD_WIDTH; i++)
		{
			for (int j = 0; j < BOARD_HEIGHT; j++)
			{
				pathMapPointer[i][j].distanceFromMinion = -1;
				pathMapPointer[i][j].wasVisited = false;
			}
		}
	}

	if (Board[x][y].consultMovementChart(minionType) == 99)
		return 0;



	//Check each neighbor to find lowest path FROM minion to this square, from among them.
	int lowestNeighboringPath = 99999;
	if (x - 1 >= 0 && pathMapPointer[x - 1][y].wasVisited == true)
	{
		if (pathMapPointer[x - 1][y].distanceFromMinion != -1 && pathMapPointer[x - 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = pathMapPointer[x - 1][y].distanceFromMinion;

		}
	}
	if (y - 1 >= 0 && pathMapPointer[x][y - 1].wasVisited == true)
	{
		if (pathMapPointer[x][y - 1].distanceFromMinion != -1 && pathMapPointer[x][y - 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = pathMapPointer[x][y - 1].distanceFromMinion;

		}
	}
	if (x + 1 < BOARD_WIDTH && pathMapPointer[x + 1][y].wasVisited == true)
	{
		if (pathMapPointer[x + 1][y].distanceFromMinion != -1 && pathMapPointer[x + 1][y].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = pathMapPointer[x + 1][y].distanceFromMinion;
		}
	}
	if (y + 1 < BOARD_HEIGHT && pathMapPointer[x][y + 1].wasVisited == true)
	{
		if (pathMapPointer[x][y + 1].distanceFromMinion != -1 && pathMapPointer[x][y + 1].distanceFromMinion < lowestNeighboringPath)
		{
			lowestNeighboringPath = pathMapPointer[x][y + 1].distanceFromMinion;

		}
	}

	//We now have lowest path to objective from visited neighbors.
	//If no visited neighbors, lowestNeighboringPath should be 99999. This means we can't get here from minion.
	if (isItInitialCall == true)
	{
		//Initialize start point
		//Cost to get to here from here 0.
		pathMapPointer[x][y].distanceFromMinion = 0;
	}
	else if (lowestNeighboringPath == 99999)
	{   //It's physically impossible to get here from minion
		pathMapPointer[x][y].distanceFromMinion = -1;
	}
	else
	{
		pathMapPointer[x][y].distanceFromMinion = lowestNeighboringPath + Board[x][y].consultMovementChart(minionType);
	}

	pathMapPointer[x][y].wasVisited = true;


	//Now call the function for eaching neighbor that is passable (not 99 move cost or off board), and hasn't been visited, or
	//is passable,  (And has been visited) and has a higher (path score - that place's cost):
	//This is to either do a first look or to update a square that has a higher current score than ours
	//Also no enemies can be here
	//This does not include air and non-air. They can pass through each other.
	if (x - 1 >= 0)
	{
		if ((Board[x - 1][y].consultMovementChart(minionType) != 99 && pathMapPointer[x - 1][y].wasVisited != true) ||
			(Board[x - 1][y].consultMovementChart(minionType) != 99 && (pathMapPointer[x - 1][y].distanceFromMinion - Board[x - 1][y].consultMovementChart(minionType)) > pathMapPointer[x][y].distanceFromMinion))
			if (Board[x - 1][y].hasMinionOnTop != true || Board[x - 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x - 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x - 1][y].minionOnTop->domain != air))
			{
				buildPath(false, x - 1, y, minionType, pathMapPointer);
			}
	}

	if (y - 1 >= 0)
	{
		if ((Board[x][y - 1].consultMovementChart(minionType) != 99 && pathMapPointer[x][y - 1].wasVisited != true) || (Board[x][y - 1].consultMovementChart(minionType) != 99 && (pathMapPointer[x][y - 1].distanceFromMinion - Board[x][y - 1].consultMovementChart(minionType)) > pathMapPointer[x][y].distanceFromMinion))
			if (Board[x][y - 1].hasMinionOnTop != true || Board[x][y - 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y - 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y - 1].minionOnTop->domain != air))
			{
				buildPath(false, x, y - 1, minionType, pathMapPointer);
			}
	}

	if (x + 1 < BOARD_WIDTH)
	{
		if ((Board[x + 1][y].consultMovementChart(minionType) != 99 && pathMapPointer[x + 1][y].wasVisited != true) || (Board[x + 1][y].consultMovementChart(minionType) != 99 && (pathMapPointer[x + 1][y].distanceFromMinion - Board[x + 1][y].consultMovementChart(minionType)) > pathMapPointer[x][y].distanceFromMinion))
			if (Board[x + 1][y].hasMinionOnTop != true || Board[x + 1][y].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x + 1][y].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x + 1][y].minionOnTop->domain != air))
			{
				buildPath(false, x + 1, y, minionType, pathMapPointer);
			}
	}

	if (y + 1 < BOARD_HEIGHT)
	{
		if ((Board[x][y + 1].consultMovementChart(minionType) != 99 && pathMapPointer[x][y + 1].wasVisited != true) || (Board[x][y + 1].consultMovementChart(minionType) != 99 && (pathMapPointer[x][y + 1].distanceFromMinion - Board[x][y + 1].consultMovementChart(minionType)) > pathMapPointer[x][y].distanceFromMinion))
			if (Board[x][y + 1].hasMinionOnTop != true || Board[x][y + 1].minionOnTop->team == cursor.selectMinionPointer->team || (cursor.selectMinionPointer->domain != air && Board[x][y + 1].minionOnTop->domain == air) || (cursor.selectMinionPointer->domain == air && Board[x][y + 1].minionOnTop->domain != air))
			{
				buildPath(false, x, y + 1, minionType, pathMapPointer);
			}
	}


	return 1;
}

//Tells us how an attacker would perform against an enemy. Useful for actual attack or calculation - doesn't actually deal damage.
//Returns a number between 0 - 100 - actual health, not percent.
//Also sets wouldAmmoBeUsed so caller can use for actual damage calcs and then subtract ammo if needed
double MasterBoard::calculateDamageDealt(Minion* attackingMinion, Minion* defendingMinion, bool& wouldAmmoBeUsed, int& weaponUsed, bool ignoreLimitations)
{
	if (attackingMinion == NULL)
	{
		std::cout << "Could not calculate damage dealt for " << playerRoster[defendingMinion->team].name << "'s " << defendingMinion->type << std::endl;
		return -1;
	}
	if (defendingMinion == NULL)
	{
		std::cout << "Could not calculate damage dealt for " << playerRoster[attackingMinion->team].name << "'s " << attackingMinion->type << std::endl;
		return -1;
	}

	//Assume ammo is not used until told otherwise.
	wouldAmmoBeUsed = false;

	//Weapon used starts at 0, ie nothing used
	weaponUsed = 0;

	//First find attacking fire power. 
	double attackerFirePower = consultAttackValuesChart(*attackingMinion, *defendingMinion, wouldAmmoBeUsed, weaponUsed, ignoreLimitations);
	attackerFirePower = attackerFirePower * attackingMinion->calculateVetBonus();

	//Calculate defense factor. If air unit it remains 1.
	double defenseFactor = 1;
	if (defendingMinion->domain != air)
	{
		defenseFactor = Board[defendingMinion->locationX][defendingMinion->locationY].defenseFactor;
	}
	defenseFactor = defenseFactor * defendingMinion->calculateVetBonus();

	return attackerFirePower * attackingMinion->health / defenseFactor;
}

int MasterBoard::clearBoard(inputLayer* InputLayer)
{
	//Need a way to clear board before loading. Using constructor doesn't do the trick.

	windowLocationX = 0;
	windowLocationY = 0;

	cursor.XCoord = 1;
	cursor.YCoord = 1;

	//Wipe out ban list
	banList.clear();

	//Initialize MinionRoster to NULL AND kill all minions.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL)
		{
			delete minionRoster[i];	//Avoid unecessary complication from destroyMinion()
			minionRoster[i] = NULL;
		}
	}

	//Clear map
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].clearTile();
		}
	}

	return 0;
}

//Ensures cursor stays within the window.
int MasterBoard::checkWindow()
{

	//Incremental moves
	if (cursor.getX() == (windowLocationX) && cursor.getX() != 0)						//If the cursor moves to the left edge of the window AND it's not at the edge of the board
		windowLocationX--;																			//Shift the window left

	if (cursor.getX() == (windowLocationX + WINDOW_WIDTH - 1) && (windowLocationX + WINDOW_WIDTH != BOARD_WIDTH))		//If the cursor moves to the right edge of the window AND it's not at the edge of the board
		windowLocationX++;																																//Shift the window to the right

	if (cursor.getY() == windowLocationY && cursor.getY() != 0)						//If the cursor moves to the top edge of the window AND it's not at the edge of the board
		windowLocationY--;																//Shift the window up.

	if (cursor.getY() == (windowLocationY + WINDOW_HEIGHT - 1) && (windowLocationY + WINDOW_HEIGHT != BOARD_HEIGHT))				//If the cursor moves to the bottom of the window AND it's not at the bottom of the board
		windowLocationY++;																														//Shift the window down once.



		//Non incremental moves. If cursor somehow got to the right of the window																																	//Attempt to center cursor on window if it got moved non-incrementally:
	if (cursor.getX() >= windowLocationX + WINDOW_WIDTH - 1 && (windowLocationX + WINDOW_WIDTH <= BOARD_WIDTH))
	{
		if (cursor.getX() >= BOARD_WIDTH - WINDOW_WIDTH - 1)
		{
			windowLocationX = BOARD_WIDTH - WINDOW_WIDTH;
		}
		else
		{
			windowLocationX = cursor.getX();
		}

	}
	//Non incremental moves. If cursor somehow got below our window
	if (cursor.getY() >= windowLocationY + WINDOW_HEIGHT - 1 && (windowLocationY + WINDOW_HEIGHT <= BOARD_HEIGHT))
	{
		if (cursor.getY() >= BOARD_HEIGHT - WINDOW_HEIGHT - 1)
		{
			windowLocationY = BOARD_HEIGHT - WINDOW_HEIGHT;
		}
		else
		{
			windowLocationY = cursor.getY();
		}

	}

	//Non incremental moves. If cursor somehow got to the left of our window.
	if (cursor.getX() < windowLocationX)
	{
		windowLocationX = cursor.getX();
	}

	//Non incremental moves. If cursor somehow got above our window.
	if (cursor.getY() < windowLocationY)
	{
		windowLocationY = cursor.getY();
	}

	return 0;
}

//This is called when selectMinion is called. It sets the visible squares the minion can move through.
int MasterBoard::setRangeField(int inputX, int inputY)
{
	Minion* myMinion = cursor.selectMinionPointer;

	//For debug purposes
	buildTerrainOnlyPathMap(true, inputX, inputY, cursor.selectMinionPointer);

	//Build a path map for this particular minion.
	buildPath(true, inputX, inputY, cursor.selectMinionPointer->type, myMinion->truePathMap);

	//Set the minion's tile to within Range, always.
	Board[inputX][inputY].withinRange = true;

	//Go thru entire board and set withinRange or not
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//May also be different domain
			//Also must be within range
			//Also must have enough fuel
			if (myMinion->truePathMap[x][y].distanceFromMinion != -1 && myMinion->truePathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->movementRange
				&& myMinion->truePathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->currentFuel)
				if (Board[x][y].hasMinionOnTop != true || Board[x][y].minionOnTop->team == playerFlag
					|| (Board[x][y].minionOnTop->domain == air && cursor.selectMinionPointer->domain != air)
					|| (Board[x][y].minionOnTop->domain != air && cursor.selectMinionPointer->domain == air))
				{
					Board[x][y].withinRange = true;
				}
		}
	}

	//Then re-build the path map for apparent path.
	buildApparentPathMap(true, inputX, inputY, cursor.selectMinionPointer);

	//Set the minion's tile to within Range, always.
	Board[inputX][inputY].withinApparentRange = true;

	//Go thru entire board and set withinRange or not
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//Also need to find if any adjacent friendly minions to this square
			bool friendlyAdjacentMinion = hasAdjacentMinion(x, y, playerFlag);
			//See if enemy stealth minion here.
			bool unseenStealthEnemyHere = false;
			if (Board[x][y].hasMinionOnTop == true && Board[x][y].minionOnTop->team != playerFlag && Board[x][y].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
			{
				unseenStealthEnemyHere = true;
			}

			//If the current tile DOES not have minion on top (IE we will be able to move there)
			//Also must be within range
			//Also must have enough fuel
			//Again, here if the tile is not visible we are pretending to allow movement.
			if (myMinion->apparentPathMap[x][y].distanceFromMinion != -1 && myMinion->apparentPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->movementRange
				&& myMinion->apparentPathMap[x][y].distanceFromMinion <= cursor.selectMinionPointer->currentFuel)
				if (Board[x][y].withinVision[playerFlag] == false || Board[x][y].hasMinionOnTop != true || Board[x][y].minionOnTop->team == playerFlag
					|| unseenStealthEnemyHere == true)
				{
					Board[x][y].withinApparentRange = true;

				}
		}
	}



	return 0;


}

//First call will be cursor location
int MasterBoard::setCursorPath(bool firstTime, int inputX, int inputY)
{
	if (cursor.selectMinionFlag == false || cursor.selectMinionPointer->status != hasntmovedorfired)
		return 1;

	Minion* myMinion = cursor.selectMinionPointer;

	if (firstTime == true)
	{
		//First clear board of paths
		//Go thru entire board and set withinCursorPath to false
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				Board[x][y].withinCursorPath = false;
			}
		}
	}

	int lowestCostToSelectedMinion = myMinion->apparentPathMap[inputX][inputY].distanceFromMinion;
	int choice = 0;

	if (Board[inputX][inputY].withinApparentRange == true || Board[inputX][inputY].withinRange == true)
		Board[inputX][inputY].withinCursorPath = true;

	if (Board[inputX][inputY].withinApparentRange == false && firstTime == true)
	{
		return 1;
	}

	if (inputX > 0 && (myMinion->apparentPathMap[inputX - 1][inputY].distanceFromMinion < lowestCostToSelectedMinion &&
		myMinion->apparentPathMap[inputX - 1][inputY].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myMinion->apparentPathMap[inputX - 1][inputY].distanceFromMinion;
		choice = 1;
	}
	if (inputY > 0 && (myMinion->apparentPathMap[inputX][inputY - 1].distanceFromMinion < lowestCostToSelectedMinion &&
		myMinion->apparentPathMap[inputX][inputY - 1].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myMinion->apparentPathMap[inputX][inputY - 1].distanceFromMinion;
		choice = 2;
	}
	if (inputY < BOARD_HEIGHT - 1 && (myMinion->apparentPathMap[inputX][inputY + 1].distanceFromMinion < lowestCostToSelectedMinion &&
		myMinion->apparentPathMap[inputX][inputY + 1].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myMinion->apparentPathMap[inputX][inputY + 1].distanceFromMinion;
		choice = 3;
	}
	if (inputX < BOARD_WIDTH - 1 && (myMinion->apparentPathMap[inputX + 1][inputY].distanceFromMinion < lowestCostToSelectedMinion &&
		myMinion->apparentPathMap[inputX + 1][inputY].distanceFromMinion != -1))
	{
		lowestCostToSelectedMinion = myMinion->apparentPathMap[inputX + 1][inputY].distanceFromMinion;
		choice = 4;
	}

	switch (choice)
	{
	case(1):
		setCursorPath(false, inputX - 1, inputY);
		break;
	case(2):
		setCursorPath(false, inputX, inputY - 1);
		break;
	case(3):
		setCursorPath(false, inputX, inputY + 1);
		break;
	case(4):
		setCursorPath(false, inputX + 1, inputY);
		break;
	case(0):
		return 1;
	}

	return 0;
}

int MasterBoard::setIndividualVisionField(int inputX, int inputY, int visionLeft, int minionX, int minionY, int playerToSee)
{
	//If this is the minion starting point, we can see.
	if (inputX == minionX && inputY == minionY)
	{
		Board[inputX][inputY].withinVision[playerToSee] = true;


	}

	//As long as this tile is not a non-adjacent forest, we can see.
	if (Board[inputX][inputY].symbol != '+' || isAdjacent(inputX, minionX, inputY, minionY))
	{
		Board[inputX][inputY].withinVision[playerToSee] = true;
	}

	if (Board[inputX][inputY].controller == playerFlag)
	{
		Board[inputX][inputY].withinVision[playerToSee] = true;
	}

	//If this was our last tile, we had the chance to "see it", but we won't go further.
	if (visionLeft <= 0)
	{
		return 1;
	}

	int nextX = inputX;
	int nextY = inputY;

	//Each adjacent square will be called if it hasn't already been found withinVision == true. 
	if (inputX != 0)
	{
		nextX = inputX - 1;

		if (visionLeft >= 1)
		{
			setIndividualVisionField(nextX, inputY, visionLeft - 1, minionX, minionY, playerToSee);
		}
	}

	if (inputX < (BOARD_WIDTH - 1))
	{
		nextX = inputX + 1;
		if (visionLeft >= 1)
		{
			setIndividualVisionField(nextX, inputY, visionLeft - 1, minionX, minionY, playerToSee);
		}
	}

	if (inputY < (BOARD_HEIGHT - 1))
	{

		nextY = inputY + 1;

		if (visionLeft >= 1)
		{
			setIndividualVisionField(inputX, nextY, visionLeft - 1, minionX, minionY, playerToSee);
		}

	}

	if (inputY != 0)
	{

		nextY = inputY - 1;

		if (visionLeft >= 1)
		{
			setIndividualVisionField(inputX, nextY, visionLeft - 1, minionX, minionY, playerToSee);
		}

	}
	return 0;

}

//Determine vision field for all minions for a player.
//This should be called every time the turn changes, and every time  minion moves or attacks. Also any time a minion is created. 
//Update uses observerNumber to determine who gets vision update. If another function moves a minion, it should call setVisionField
//For that team.
int MasterBoard::setVisionField(int observerNumber)
{
	//If observer is "neutral", they can see the whole map.
	if (observerNumber == 0 || fogOfWar == false)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				Board[x][y].withinVision[observerNumber] = true;

			}
		}

		return 0;
	}

	//First set the board to no vision initially.
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//If you do control a property, it is actually visible.
			if (Board[x][y].controller == observerNumber)
				Board[x][y].withinVision[observerNumber] = true;
			else
				Board[x][y].withinVision[observerNumber] = false;
		}
	}

	//Go through the roster of minions and determine if it's player controlled.
	//Then set vision for everything that minion can see.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL && minionRoster[i]->transporter == NULL)
		{
			if (minionRoster[i]->team == observerNumber)
			{
				Minion* myMinion = minionRoster[i];
				//If minion is on mountain, add 2 to vision.
				if (Board[myMinion->locationX][myMinion->locationY].symbol == 'M')
					setIndividualVisionField(myMinion->locationX, myMinion->locationY, myMinion->visionRange + 2, myMinion->locationX, myMinion->locationY, observerNumber);
				else setIndividualVisionField(myMinion->locationX, myMinion->locationY, myMinion->visionRange, myMinion->locationX, myMinion->locationY, observerNumber);
			}
		}
	}

	return 0;
}

//Determine attack range field.
int MasterBoard::setAttackField(int inputX, int inputY, int inputRange)		//Primary difference between move and attack is attack range goes over all units, ignoring them.
{

	int distanceX = 0;
	int distanceY = 0;

	//If out of ammo for both weapons, and neither weapon is infinite (Maxammo = 0), return.
	if (cursor.selectMinionPointer->currentPriAmmo <= 0 && cursor.selectMinionPointer->maxPriAmmo != 0
			&& cursor.selectMinionPointer->currentSecAmmo <= 0 && cursor.selectMinionPointer->maxSecAmmo != 0)
	{
		return 1;
	}

	//These are all cases where direct fire can occur:
	if ((cursor.selectMinionPointer->rangeType == directFire) &&
		(cursor.selectMinionPointer->status == hasmovedhasntfired || cursor.selectMinionPointer->status == gaveupmovehasntfired))
	{
		if (inputX < BOARD_WIDTH - 1)
			Board[inputX + 1][inputY].withinRange = true;

		if (inputX > 0)
			Board[inputX - 1][inputY].withinRange = true;

		if (inputY < BOARD_HEIGHT - 1)
			Board[inputX][inputY + 1].withinRange = true;

		if (inputY > 0)
			Board[inputX][inputY - 1].withinRange = true;

	}
	else	//If not a direct fire scenario, it's a ranged scenario:
		if ((cursor.selectMinionPointer->rangeType == rangedFire) &&
			(cursor.selectMinionPointer->status == gaveupmovehasntfired))
		{

			for (int x = 0; x < BOARD_WIDTH; x++)
			{
				for (int y = 0; y < BOARD_HEIGHT; y++)
				{

					distanceX = abs(inputX - x);
					distanceY = abs(inputY - y);

					//Must be within range and also outside minimum range 
					if ((distanceX + distanceY) <= Board[inputX][inputY].minionOnTop->attackRange &&
						(distanceX + distanceY) > cursor.selectMinionPointer->minAttackRange)
					{
						Board[x][y].withinRange = true;
					}
					else
					{
						Board[x][y].withinRange = false;
					}
				}
			}
		}
		else	//Or it's a hybrid fire scenario
			if (cursor.selectMinionPointer->rangeType == hybridRange
				&& cursor.selectMinionPointer->status == gaveupmovehasntfired)
			{
				for (int x = 0; x < BOARD_WIDTH; x++)
				{
					for (int y = 0; y < BOARD_HEIGHT; y++)
					{

						distanceX = abs(inputX - x);
						distanceY = abs(inputY - y);

						//Must be within range. Ignore minimum range.
						if ((distanceX + distanceY) <= Board[inputX][inputY].minionOnTop->attackRange)
						{
							Board[x][y].withinRange = true;
						}
						else
						{
							Board[x][y].withinRange = false;
						}
					}
				}
			}
			else
				if (cursor.selectMinionPointer->status == hasmovedhasntfired
					&& cursor.selectMinionPointer->rangeType == hybridRange)
				{
					if (inputX < BOARD_WIDTH - 1)
						Board[inputX + 1][inputY].withinRange = true;

					if (inputX > 0)
						Board[inputX - 1][inputY].withinRange = true;

					if (inputY < BOARD_HEIGHT - 1)
						Board[inputX][inputY + 1].withinRange = true;

					if (inputY > 0)
						Board[inputX][inputY - 1].withinRange = true;

				}

	//Minion's tile should not have red indicator
	Board[inputX][inputY].withinRange = false;
	return 0;
}

int MasterBoard::setDropField(int inputX, int inputY)
{

	//If not transport its drop field is zero.
	if (cursor.selectMinionPointer->specialtyGroup != smallTransport && cursor.selectMinionPointer->specialtyGroup != largeTransport
		&& cursor.selectMinionPointer->specialtyGroup != aircraftCarrier)
	{
		return 1;
	}


	//Must have a dude onboard
	if (cursor.selectMinionPointer->firstMinionBeingTransported == NULL)
	{
		return 1;
	}


	//Check each adjacent tile, if it is not impassible or off map, put in range for transport.
	if (inputX < BOARD_WIDTH - 1 && Board[inputX + 1][inputY].consultMovementChart(cursor.selectMinionPointer->firstMinionBeingTransported->type) != 99)
		Board[inputX + 1][inputY].withinRange = true;

	if (inputX > 0 && Board[inputX - 1][inputY].consultMovementChart(cursor.selectMinionPointer->firstMinionBeingTransported->type) != 99)
		Board[inputX - 1][inputY].withinRange = true;

	if (inputY < BOARD_HEIGHT - 1 && Board[inputX][inputY + 1].consultMovementChart(cursor.selectMinionPointer->firstMinionBeingTransported->type) != 99)
		Board[inputX][inputY + 1].withinRange = true;

	if (inputY > 0 && Board[inputX][inputY - 1].consultMovementChart(cursor.selectMinionPointer->firstMinionBeingTransported->type) != 99)
		Board[inputX][inputY - 1].withinRange = true;

	return 0;
}

bool MasterBoard::checkFactionAvailability(std::string minionType, int inputPlayer)
{
    int playerFaction = playerRoster[inputPlayer].playerFaction;
    int minionNumber = -1;

    //Find ID number for minion type
    if ( minionType == "Infantry")
		minionNumber = 0;
		else
    if ( minionType == "Insurgent")
        minionNumber = 1;
        else
	if( minionType == "Operative")
		minionNumber = 2;
		else
	if ( minionType == "Specialist")
		minionNumber = 3;
		else
	if ( minionType == "Recon")
		minionNumber = 4;
		else
	if (minionType == "Technical")
		minionNumber = 5;
		else
    if ( minionType == "APC")
		minionNumber = 6;
		else
	if (minionType == "IFV")
	    minionNumber = 7;
		else
    if ( minionType == "Artillery")
	    minionNumber = 8;
		else
    if ( minionType == "Armor")
	    minionNumber = 9;
		else
	if ( minionType == "Assault_Gun")
		minionNumber = 10;
		else
    if ( minionType == "Upgunned_Armor")
   	    minionNumber = 11;
        else
   	if ( minionType == "Anti-Aircraft")
   	    minionNumber = 12;
        else
	if ( minionType == "Rocket_Artillery")
		minionNumber = 13;
        else
	if ( minionType == "Heavy_Armor")
		minionNumber = 14;
        else
	if ( minionType == "Modern_Armor")
		minionNumber = 15;
        else
	if ( minionType == "Transport_Copter")
		minionNumber = 16;
        else
	if ( minionType == "Attack_Copter")
		minionNumber = 17;
        else
	if ( minionType == "Interceptor")
		minionNumber = 18;
		else
	if( minionType == "Advanced_Fighter")
		minionNumber = 19;
		else
	if ( minionType == "Multirole")
		minionNumber = 20;
		else
	if ( minionType == "Bomber")
		minionNumber = 21;
		else
	if ( minionType == "Gunboat")
		minionNumber = 22;
		else
	if ( minionType == "Cruiser")
		minionNumber = 23;
		else
	if ( minionType == "Lander")
		minionNumber = 24;
		else
	if ( minionType == "Submarine")
		minionNumber = 25;
		else
	if ( minionType == "Battleship")
		minionNumber = 26;
		else
	if ( minionType == "Aircraft_Carrier")
		minionNumber = 27;
        else
	if ( minionType == "Victory_Launcher")
		minionNumber = 28;
        else
    if ( minionType == "Super_Heavy_Armor")
		minionNumber = 29;
        else
	if ( minionType == "Cavalry")
		minionNumber = 30;
		else
	if (minionType == "Engineer")
		minionNumber = 31;



    if(minionNumber == -1)
        return false;

    bool allowed = minionsAllowedForFaction[playerFaction][minionNumber];
    return allowed;

}


//attemptResult should return 1 if unsuccessful
//Or 0 if successful
int MasterBoard::attemptPurchaseMinion(std::string inputType, int inputX, int inputY, int inputTeam)
{
	int attemptResult = 1;

	//First verify that this unit is not on banList. If it is, return 1.
	if (std::find(banList.begin(), banList.end(), inputType) != banList.end())
		return attemptResult;

    //Then check to see if it's within the faction options
	if (checkFactionAvailability(inputType, inputTeam) == false)
		return attemptResult;

	if (playerRoster[playerFlag].treasury >= consultMinionCostChart(inputType, Board[inputX][inputY].symbol) && Board[inputX][inputY].hasMinionOnTop == false)
	{
		attemptResult = createMinion(inputType, inputX, inputY, playerFlag, 100, hasfired, 0, 0, -1, -1, -1);
		if (attemptResult == 0)
			playerRoster[playerFlag].treasury -= consultMinionCostChart(inputType, Board[inputX][inputY].symbol);

	}

	return attemptResult;


}

int MasterBoard::buildImprovement(inputLayer* InputLayer, int inputX, int inputY)
{
	
	//First check for terrain constraints:
	tile* myTile = &Board[inputX][inputY];

	//Make sure engineer is below and can only build on forest/plains/const zone
	if (myTile->hasMinionOnTop == false || myTile->minionOnTop->type != "Engineer" || (myTile->symbol != '.' && myTile->symbol != '+'))
	{
		return 1;
	}


	//Determine build points based on engineer's health
	int pointsToBuild = int(std::round(myTile->minionOnTop->health / 10));
	int buildCost = 0;

	//Late will want to change cost for building a mine, airbase, etc.
	if (myTile->symbol == '.' || myTile->symbol == '+')
	{
		buildCost = pointsToBuild * 100;
	}

	//If player cannot afford cost, return failure. Otherwise subtract and build.
	if (playerRoster[playerFlag].treasury < buildCost)
	{
		return 1;
	}
	else
	{
		playerRoster[playerFlag].treasury -= buildCost;
		myTile->capturePoints -= pointsToBuild;
		myTile->minionOnTop->isCapturing = true;

		if (myTile->capturePoints <= 0)
		{
			//Change the underlying tile, and reset cap points.
			myTile->symbol = 'f';      //fort
			myTile->capturePoints = 20;
			myTile->minionOnTop->isCapturing = false;
		}

		myTile->setCharacterstics(InputLayer->inputLayerTexture, this);

		InputLayer->status = gameBoard;
		InputLayer->repairGraphics(this, playerFlag, myTile->minionOnTop, inputX, inputY);

		//If we made it this far we did a successful deploy, so deselect
		cursor.selectMinionPointer->status = hasfired;
		deselectMinion();

		return 0;
	}

}

int MasterBoard::deployLandmine(inputLayer* InputLayer, int inputX, int inputY)
{
	//First check for terrain constraints:
	tile* myTile = &Board[inputX][inputY];

	//Landmine must not be banned
	if (std::find(banList.begin(), banList.end(), "LandMine") != banList.end())
		return 1;

	//Prevent landmine insertion on top of another and prevent landmine deploy within sea/river
	if (myTile->hasMinionOnTop == true || myTile->consultMovementChart("Landmine") == 99)
	{
		return 1;
	}

	//If player cannot afford cost, return failure. Otherwise subtract 2k and build.
	if (playerRoster[playerFlag].treasury < 2000)
	{
		return 1;
	}
	else
	{
		playerRoster[playerFlag].treasury -= 2000;
		createMinion("Landmine", inputX, inputY, playerFlag, 100, 0, 0, 0, -1, -1, -1);
		InputLayer->status = gameBoard;
		InputLayer->repairGraphics(this, playerFlag, myTile->minionOnTop, inputX, inputY);

	}

	//If we made it this far we did a successful deploy, so deselect
	cursor.selectMinionPointer->status = hasfired;
	deselectMinion();

	return 0;
}


int MasterBoard::createMinion(std::string inputType, int inputX, int inputY, int inputTeam, int inputHealth, int status, int veterancy, int beingTransported, int inputFuel, int inputPriAmmo, int inputSecAmmo)
{
	//Cannot create minion illegally (On top of another, or in bad location)
	if ( (Board[inputX][inputY].hasMinionOnTop == true && beingTransported == 0 ) || Board[inputX][inputY].consultMovementChart(inputType) == 99)
		return 1;

	//Loop through and find the next NULL pointer indicating a non-allocated part of the array.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] == NULL)
		{
			//Successful creation of new minion.
			minionRoster[i] = new Minion(i, inputX, inputY, inputType, inputTeam, this, inputHealth, veterancy, beingTransported, inputFuel, inputPriAmmo, inputSecAmmo, myTexture, mySecondTexture);
			if (minionRoster[i] != NULL)
			{
				minionRoster[i]->status = (minionStatus)status;
				totalNumberOfMinions++;
				if (beingTransported == 0) //Only access the board if minion has legit location
				{
					Board[inputX][inputY].minionOnTop = minionRoster[i];
				}
				//If it's being transported, load it into the transport. This assumes it has valid transport at its location.
				else if(Board[inputX][inputY].minionOnTop->specialtyGroup == smallTransport || Board[inputX][inputY].minionOnTop->specialtyGroup == largeTransport || Board[inputX][inputY].minionOnTop->specialtyGroup == aircraftCarrier)
				{
					if(Board[inputX][inputY].minionOnTop->firstMinionBeingTransported == NULL)
						Board[inputX][inputY].minionOnTop->firstMinionBeingTransported = minionRoster[i];
					else
						Board[inputX][inputY].minionOnTop->secondMinionBeingTransported = minionRoster[i];

					minionRoster[i]->transporter = Board[inputX][inputY].minionOnTop;

					minionRoster[i]->locationX = -1;
					minionRoster[i]->locationY = -1;
				}
				setVisionField(inputTeam);

				//Increment that player's number of Minions
				playerRoster[inputTeam].numberOfMinions++;
				return 0;
			}
		}
	}

	//No room found, print error message and return 0.
	std::cout << "ERROR, no more supply cap for new minion!" << std::endl;
	return 1;

}

//First check ensures if minion has moved and shot, you can't select. Also if it's ranged and moved, you can't.
int MasterBoard::selectMinion(int inputX, int inputY)
{
	if (inputX == -1 || inputY == -1)
	{
		std::cout << "Invalid selectMinion coordinates!" << std::endl;
		return -1;
	}

	//Return if already selected
	if (cursor.selectMinionFlag == true)
	{
		std::cout << "Minion already selected." << std::endl;
		return 1;
	}

	//Friendly minion
	//Has either not moved at all, or moved but not fired and isn't ranged
	//Or gave up move, hasn't fired
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team == playerFlag
		&& (Board[inputX][inputY].minionOnTop->status == hasntmovedorfired ||
		(Board[inputX][inputY].minionOnTop->status == hasmovedhasntfired && Board[inputX][inputY].minionOnTop->rangeType != rangedFire) ||
			Board[inputX][inputY].minionOnTop->status == gaveupmovehasntfired))
	{
		//Cannot select minion if landmine type.
		if (Board[inputX][inputY].minionOnTop->type != "Landmine")
		{
			cursor.selectMinionPointer = Board[inputX][inputY].minionOnTop;
			cursor.selectMinionFlag = true;

			//If minion hasn't moved or fired yet, display its movement range.
			if (cursor.selectMinionPointer->status == hasntmovedorfired)
			{
				setRangeField(inputX, inputY);
				return 0;
			}
			else //If minion has moved but hasn't fired, and is direct combat, display attack range.
				//And is not carrying a guy
				if (cursor.selectMinionPointer->status == hasmovedhasntfired && (cursor.selectMinionPointer->rangeType == directFire
					|| cursor.selectMinionPointer->rangeType == hybridRange) && cursor.selectMinionPointer->firstMinionBeingTransported == NULL)
				{
					setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
					return 0;
				}

				else //If minion stood in place and hasn't fired, display attack range. And is not carrying a guy.
					if (cursor.selectMinionPointer->status == gaveupmovehasntfired && cursor.selectMinionPointer->firstMinionBeingTransported == NULL)
					{
						setAttackField(inputX, inputY, cursor.selectMinionPointer->attackRange);
						return 0;
					}		//If transport that either moved or stood in place, we still select. But may not display drop field.
					else if ((cursor.selectMinionPointer->status == hasmovedhasntfired || cursor.selectMinionPointer->status == gaveupmovehasntfired)
						&& (cursor.selectMinionPointer->specialtyGroup == largeTransport || cursor.selectMinionPointer->specialtyGroup == smallTransport || cursor.selectMinionPointer->specialtyGroup == aircraftCarrier))
					{
						//If minion is transport and has a guy embarked, show drop field.
						//The new assumption here is that a transport MUST have a one range direct attack weapon, because if it is carrying a minion, its "withinRange" is being set 
						//based on the drop range, which is always one.
						if (cursor.selectMinionPointer->firstMinionBeingTransported != NULL)
							setDropField(inputX, inputY);
						return 0;
					}
		}
	}
	return 1;
}

//Must find the closest valid tile where we can move, starting with inputX/inputY.
//Return 0 means successfully made it to location
//Return 1 means we dropped prematurely at this input location (Could be where we started).
//ValidatePath also prints its own graphics, indicating the path of movement.
int MasterBoard::validatePath(int& inputX, int& inputY, inputLayer* graphicsLayer, int whoIsWatching, bool transportMove)
{
	bool madeItToTheEnd = false;
	bool hitEnemy = false;
	Minion* selectedMinion = cursor.selectMinionPointer;

	int currentX = inputX;
	int currentY = inputY;

	int potentialDropX = inputX;
	int potentialDropY = inputY;

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].hasBeenValidated = false;
		}
	}


	Board[currentX][currentY].hasBeenValidated = true;

	//Check each adjacent space. If it is within the cursor path and doesn't have enemy minion, turn cursorPath here off, and return its validatePath value.
	//If a drop happens, return 1.

	while (madeItToTheEnd == false && hitEnemy == false)
	{
		//Graphics - show move
		graphicsLayer->movementGraphics(this, whoIsWatching, selectedMinion, currentX, currentY);

		//If we have reached the cursor, do final evaluation.
		if (cursor.getX() == currentX && cursor.getY() == currentY)
		{
			//If there is a minion here, even though we reached end, we are "trapped", and we need to go back to potential drop.
			//Must also NOT be a transport move - i.e. a normal move. Otherwise no trap occurred.
			if (Board[currentX][currentY].hasMinionOnTop == true && transportMove == false)
				hitEnemy = true;
			else
				madeItToTheEnd = true;
		}
		else

			//Keep evaluating spots along the path.
			//Only one spot should have "withinCursorPath && notValidated" so we use ifelse to go to each neighboring square
			//If a spot is on path AND has no minion. Potential drop, and now move evaluation there.
			if (currentX > 0 && Board[currentX - 1][currentY].withinCursorPath == true && Board[currentX - 1][currentY].hasBeenValidated == false)
			{
				if (Board[currentX - 1][currentY].hasMinionOnTop == false)
				{
					potentialDropX = currentX - 1;
					potentialDropY = currentY;
					Board[currentX - 1][currentY].hasBeenValidated = true;	//Prevent from coming back here

					//Now move currentX/Y to evaluate next box
					currentX--;

				}
				else //If there is a minion here, but it's 1. not enemy or 2. not our domain, we can pass THROUGH here.
					if ((Board[currentX - 1][currentY].minionOnTop->domain != air && selectedMinion->domain == air) || (Board[currentX - 1][currentY].minionOnTop->domain == air && selectedMinion->domain != air)
						|| Board[currentX - 1][currentY].minionOnTop->team == playerFlag)
					{
						Board[currentX - 1][currentY].hasBeenValidated = true;	//Prevent from coming back here
						//We can still proceed, but this is not a new potential drop point.
						currentX--;


					}
					else
					{
						hitEnemy = true;
					}
			}
			else if (currentX < BOARD_WIDTH - 1 && Board[currentX + 1][currentY].withinCursorPath == true && Board[currentX + 1][currentY].hasBeenValidated == false)
			{
				if (Board[currentX + 1][currentY].hasMinionOnTop == false)
				{
					potentialDropX = currentX + 1;
					potentialDropY = currentY;
					Board[currentX + 1][currentY].hasBeenValidated = true;	//Prevent from coming back here

					//Now move currentX/Y to evaluate next box
					currentX++;

				}
				else //If there is a minion here, but it's 1. not enemy or 2. not our domain, we can pass THROUGH here.
					if ((Board[currentX + 1][currentY].minionOnTop->domain != air && selectedMinion->domain == air) || (Board[currentX + 1][currentY].minionOnTop->domain == air && selectedMinion->domain != air)
						|| Board[currentX + 1][currentY].minionOnTop->team == playerFlag)
					{
						Board[currentX + 1][currentY].hasBeenValidated = true;	//Prevent from coming back here
						//We can still proceed, but this is not a new potential drop point.
						currentX++;
					}
					else
					{
						hitEnemy = true;
					}
			}
			else if (currentY < BOARD_HEIGHT - 1 && Board[currentX][currentY + 1].withinCursorPath == true && Board[currentX][currentY + 1].hasBeenValidated == false)
			{
				if (Board[currentX][currentY + 1].hasMinionOnTop == false)
				{
					potentialDropX = currentX;
					potentialDropY = currentY + 1;
					Board[currentX][currentY + 1].hasBeenValidated = true;	//Prevent from coming back here

					//Now move currentX/Y to evaluate next box
					currentY++;

				}
				else //If there is a minion here, but it's 1. not enemy or 2. not our domain, we can pass THROUGH here.
					if ((Board[currentX][currentY + 1].minionOnTop->domain != air && selectedMinion->domain == air) || (Board[currentX][currentY + 1].minionOnTop->domain == air && selectedMinion->domain != air)
						|| Board[currentX][currentY + 1].minionOnTop->team == playerFlag)
					{
						Board[currentX][currentY + 1].hasBeenValidated = true;	//Prevent from coming back here
						//We can still proceed, but this is not a new potential drop point.
						currentY++;
					}
					else
					{
						hitEnemy = true;
					}
			}
			else if (currentY > 0 && Board[currentX][currentY - 1].withinCursorPath == true && Board[currentX][currentY - 1].hasBeenValidated == false)
			{
				if (Board[currentX][currentY - 1].hasMinionOnTop == false)
				{
					potentialDropX = currentX;
					potentialDropY = currentY - 1;
					Board[currentX][currentY - 1].hasBeenValidated = true;	//Prevent from coming back here

					//Now move currentX/Y to evaluate next box
					currentY--;

				}
				else //If there is a minion here, but it's 1. not enemy or 2. not our domain, we can pass THROUGH here.
					if ((Board[currentX][currentY - 1].minionOnTop->domain != air && selectedMinion->domain == air) || (Board[currentX][currentY - 1].minionOnTop->domain == air && selectedMinion->domain != air)
						|| Board[currentX][currentY - 1].minionOnTop->team == playerFlag)
					{
						Board[currentX][currentY - 1].hasBeenValidated = true;	//Prevent from coming back here
						//We can still proceed, but this is not a new potential drop point.
						currentY--;
					}
					else
					{
						hitEnemy = true;
					}
			}
	}

	if (madeItToTheEnd == true)
	{
		inputX = cursor.getX();
		inputY = cursor.getY();
		return 0;       //No trap, made it to objective tile.
	}
	else    //If a trap occurred, set outbound input values to the last good drop point.
	{
		inputX = potentialDropX;
		inputY = potentialDropY;
		return 1;       //Trapped. May have moved some, may not have moved at all.
	}



}


int MasterBoard::moveMinion(int inputX, int inputY, inputLayer* InputLayer, int  observerNumber)
{
	Minion* selectedMinion = cursor.selectMinionPointer;

	//First make sure the move is legal. Must be within range, must not have moved.
	//If in "apparent range" that means there's an enemy hiding there, we proceed until validatePath.
	if (Board[inputX][inputY].withinRange == false && Board[inputX][inputY].withinApparentRange == false)
	{
		return 1;
	}

	//Make sure minion is able to move.
	if (cursor.selectMinionPointer->status == hasfired ||
		cursor.selectMinionPointer->status == hasmovedhasntfired ||
		cursor.selectMinionPointer->status == gaveupmovehasntfired)
	{
		return 1;
	}

	//Also need to find if any adjacent friendly minions to this square
	bool friendlyAdjacentMinion = hasAdjacentMinion(inputX, inputY, playerFlag);
	//See if enemy stealth minion here.
	bool unseenStealthEnemyHere = false;
	if (Board[inputX][inputY].hasMinionOnTop == true && Board[inputX][inputY].minionOnTop->team != playerFlag && Board[inputX][inputY].minionOnTop->stealthMode == true && friendlyAdjacentMinion == false)
	{
		unseenStealthEnemyHere = true;
	}

	//If there is a minion below cursor AND it's not this exact minion, movement failure
	//Again, this is for visible tiles.
	if (Board[inputX][inputY].hasMinionOnTop == true
		&& Board[inputX][inputY].withinVision[playerFlag] == true
		&& unseenStealthEnemyHere == false
		&& !(cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		return 1;
	}

	//If there is a minion below cursor AND it IS this exact minion, "fake" move.
	if (Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true
		&& (cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		cursor.selectMinionPointer->status = gaveupmovehasntfired;
		deselectMinion();
		return 0;
	}

	//Track minion location. As we validate path, these new values may change.
	int newX = selectedMinion->locationX;
	int newY = selectedMinion->locationY;

	//This is the "trap" check. If it appears movable, but has a minion, you get trapped.
	//validatePath will potentially change inputX and inputY.
	bool loadMove = false;
	int didTrapHappen = validatePath(newX, newY, InputLayer, observerNumber, loadMove);

	//If the inputX/inputY has not changed, that means we got trapped and couldn't move at all.
	//Still counts as moving.
	if (newX == selectedMinion->locationX && newY == selectedMinion->locationY)
	{
		cursor.selectMinionPointer->status = hasmovedhasntfired;
	}
	else
	{
		//Otherwise move.

		//Find old address of the minion
		int oldX = selectedMinion->locationX;
		int oldY = selectedMinion->locationY;

		//Clear the old tile, set the new tile.
		Board[oldX][oldY].hasMinionOnTop = false;
		Board[newX][newY].hasMinionOnTop = true;

		Board[newX][newY].minionOnTop = Board[oldX][oldY].minionOnTop;
		Board[oldX][oldY].minionOnTop = NULL;

		//Reset capture points for tile.
		//Reset minion's cap status
		Board[oldX][oldY].capturePoints = 20;
		selectedMinion->isCapturing = false;

		//"Move" the minion to the new location.
		selectedMinion->locationX = newX;
		selectedMinion->locationY = newY;

		cursor.selectMinionPointer->currentFuel -= cursor.selectMinionPointer->truePathMap[newX][newY].distanceFromMinion;

		cursor.selectMinionPointer->status = hasmovedhasntfired;
	}

	//A trap may have happened, even if we moved. 
	if (didTrapHappen == 1)
	{
		InputLayer->trapGraphics(this, observerNumber, selectedMinion, inputX, inputY);
	}

	//Set stealth mode for an insurgent type that moved into a forest, building, or mountain.
	//If that minion is in a non-stealth terrain, set to false.
	if (selectedMinion->type == "Insurgent")
		if (Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'h'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'H'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'Q'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'P'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'A'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'n'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == '+'
			|| Board[selectedMinion->locationX][selectedMinion->locationY].symbol == 'M')
		{
			selectedMinion->stealthMode = true;
		}
		else selectedMinion->stealthMode = false;

	//Deselect minion after moving
	deselectMinion();

	//If moved next to a landmine, it now attacks.
	landmineCheck(newX, newY, InputLayer, observerNumber);

	setVisionField(playerFlag);

	return 0;
}

//Depending on sub's status, either submerges or surfaces, and changes the sprite to reflect that as well.
int MasterBoard::diveOrSurfaceSub()
{
	if (cursor.selectMinionPointer == NULL || cursor.selectMinionPointer->type != "Submarine")
		return 1;

	if (cursor.selectMinionPointer->stealthMode == true)
	{
		cursor.selectMinionPointer->stealthMode = false;
		cursor.selectMinionPointer->mySprite.setTextureRect(rectArray[18][playerFlag + 4]);
	}
	else
	{
		cursor.selectMinionPointer->stealthMode = true;
		cursor.selectMinionPointer->mySprite.setTextureRect(rectArray[19][playerFlag + 4]);
	}
	
	//Dive/surface takes a fire move.
	cursor.selectMinionPointer->status = hasfired;
	deselectMinion();

	return 0;
}



int MasterBoard::pickUpMinion(int inputX, int inputY, inputLayer* InputLayer, int  observerNumber)
{
	Minion* selectedMinion = cursor.selectMinionPointer;

	//First make sure the move is legal. Must be within range, must not have moved.
	if (Board[inputX][inputY].withinRange == false)
	{
		return 1;
	}

	//Make sure minion is able to move.
	if (cursor.selectMinionPointer->status == hasfired ||
		cursor.selectMinionPointer->status == hasmovedhasntfired ||
		cursor.selectMinionPointer->status == gaveupmovehasntfired)
	{
		return 1;
	}

	//If there is a minion below cursor and it is not the selected minion:
	//Also, must be same team
	if (Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true
		&& Board[cursor.getX()][cursor.getY()].minionOnTop->team == playerFlag
		&& !(cursor.selectMinionPointer->locationX == cursor.getX()
			&& cursor.selectMinionPointer->locationY == cursor.getY()))
	{
		//Potential ways this can work:
		//Infantry with small transport, with first slot empty.
		//Any land unit with large transport, with first or second slot empty
		if ((selectedMinion->specialtyGroup == infantry && Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == smallTransport && Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported == NULL)
			|| (selectedMinion->domain == land && Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == largeTransport && (Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported == NULL || Board[cursor.getX()][cursor.getY()].minionOnTop->secondMinionBeingTransported == NULL))
			|| (selectedMinion->domain == air && Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == aircraftCarrier && Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported == NULL))
		{
			inputX = selectedMinion->locationX;
			inputY = selectedMinion->locationY;

			//Now validate path and then either successfully load, or do trap movement
			//If it appears movable, but has a minion, you get trapped.
			//validatePath will potentially change inputX and inputY.
			bool loadMove = true;
			int didTrapHappen = validatePath(inputX, inputY, InputLayer, observerNumber, loadMove);

			//If the inputX has not changed, that means we got trapped and have to stand in place. 
			//Still counts as moving.
			if (inputX == selectedMinion->locationX && inputY == selectedMinion->locationY && didTrapHappen == 1)
			{
				cursor.selectMinionPointer->status = hasmovedhasntfired;
				deselectMinion();
				InputLayer->trapGraphics(this, observerNumber, selectedMinion, inputX, inputY);

			}
			else if (didTrapHappen == 1)
			{    //We moved but got trapped before able to load. Execute move.

				//Find old address of the minion
				int oldX = selectedMinion->locationX;
				int oldY = selectedMinion->locationY;

				//Clear the old tile, set the new tile.
				Board[oldX][oldY].hasMinionOnTop = false;
				Board[inputX][inputY].hasMinionOnTop = true;

				Board[inputX][inputY].minionOnTop = Board[oldX][oldY].minionOnTop;
				Board[oldX][oldY].minionOnTop = NULL;

				//Reset capture points for tile.
				//Reset minion's cap status
				Board[oldX][oldY].capturePoints = 20;
				selectedMinion->isCapturing = false;

				//"Move" the minion to the new location.
				selectedMinion->locationX = inputX;
				selectedMinion->locationY = inputY;

				cursor.selectMinionPointer->currentFuel -= cursor.selectMinionPointer->truePathMap[inputX][inputY].distanceFromMinion;

				cursor.selectMinionPointer->status = hasmovedhasntfired;
				deselectMinion();
				InputLayer->trapGraphics(this, observerNumber, selectedMinion, inputX, inputY);
			}
			else if (didTrapHappen == 0)
			{   //Successful load occurred.

				//Put the minion in the transport.
				//If small transport or aircraft carrier, can only do first slot.
				//Otherwise, choose whichever slot is empty.
				if (Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == smallTransport || Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == aircraftCarrier)
				{
					Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported = selectedMinion;
				}
				else if (Board[cursor.getX()][cursor.getY()].minionOnTop->specialtyGroup == largeTransport)
				{
					if (Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported == NULL)
						Board[cursor.getX()][cursor.getY()].minionOnTop->firstMinionBeingTransported = selectedMinion;
					else
						Board[cursor.getX()][cursor.getY()].minionOnTop->secondMinionBeingTransported = selectedMinion;
				}


				selectedMinion->transporter = Board[cursor.getX()][cursor.getY()].minionOnTop;

				//Find old address of the minion
				int oldX = selectedMinion->locationX;
				int oldY = selectedMinion->locationY;

				//Clear the old tile, set the new tile.
				Board[oldX][oldY].hasMinionOnTop = false;
				Board[oldX][oldY].minionOnTop = NULL;

				//Reset capture points for tile.
				Board[oldX][oldY].capturePoints = 20;

				//"Move" the minion to the new location.
				//This is for "debugging", pretty bad but it needs to be invalid.
				selectedMinion->locationX = -1;
				selectedMinion->locationY = -1;

				cursor.selectMinionPointer->status = hasmovedhasntfired;
				deselectMinion();


			}

			setVisionField(playerFlag);

			return 0;
		}


	}


	return 1;
}

int MasterBoard::dropOffMinion()
{
	//Cannot drop off if recently deployed.
	if (cursor.selectMinionPointer->status == hasfired)
	{
		return 1;
	}

	//If not in range cannot drop off
	//Also, if any minion on this tile, even if in range, can't drop off
	if (Board[cursor.getX()][cursor.getY()].withinRange == false || Board[cursor.getX()][cursor.getY()].hasMinionOnTop == true)
	{
		return 1;
	}

	//Otherwise drop off minion.
	Board[cursor.getX()][cursor.getY()].hasMinionOnTop = true;

	//Small transport drops off its first guy and stops.
	if (cursor.selectMinionPointer->specialtyGroup == smallTransport || cursor.selectMinionPointer->specialtyGroup == aircraftCarrier)
	{
		Board[cursor.getX()][cursor.getY()].minionOnTop = cursor.selectMinionPointer->firstMinionBeingTransported;
		cursor.selectMinionPointer->firstMinionBeingTransported = NULL;
	}
	//Large trans. drops off first guy and the moves second to first.
	else if (cursor.selectMinionPointer->specialtyGroup == largeTransport)
	{
		Board[cursor.getX()][cursor.getY()].minionOnTop = cursor.selectMinionPointer->firstMinionBeingTransported;
		cursor.selectMinionPointer->firstMinionBeingTransported = cursor.selectMinionPointer->secondMinionBeingTransported;
		cursor.selectMinionPointer->secondMinionBeingTransported = NULL;

	}

	Board[cursor.getX()][cursor.getY()].minionOnTop->transporter = NULL;
	Board[cursor.getX()][cursor.getY()].minionOnTop->status = hasfired;

	//"Move" the minion to the new location.
	Board[cursor.getX()][cursor.getY()].minionOnTop->locationX = cursor.getX();
	Board[cursor.getX()][cursor.getY()].minionOnTop->locationY = cursor.getY();


	//If this was the last minion transported, this minion is done moving.
	if (cursor.selectMinionPointer->firstMinionBeingTransported == NULL && cursor.selectMinionPointer->secondMinionBeingTransported == NULL)
	{
		cursor.selectMinionPointer->status = hasfired;
	}

	//If we made it this far we did a successful drop, so deselect transport
	deselectMinion();

	setVisionField(playerFlag);
	return 0;
}

//Capture property - any minion with captureCapable can capture property. Takes 20 cap points to capture, and does 1 per health per turn.
//Nominally should take two turns for a minion to cap a property.
std::string MasterBoard::captureProperty(tile* inputTile, Minion* inputMinion, inputLayer* InputLayer, int  observerNumber)
{
	int previousOwner = inputTile->controller;

	//Check for actual property before we capture
	if (inputTile->checkForProperty() != true)
	{
		return "Not a property";
	}

	//Check for ability to capture this  tile
	if(inputMinion->captureCapable == false)
	{
		return "Not capture capable";
	}

	std::string textToReturn = "";
	//Subtract capturing minion's health
	int pointsToTake = int(std::round(inputMinion->health / 10));
	inputTile->capturePoints -= pointsToTake;
	inputMinion->isCapturing = true;

	InputLayer->captureGraphics(this, observerNumber, inputMinion, inputMinion->locationX, inputMinion->locationY);


	if (inputTile->capturePoints <= 0)
	{
		inputTile->controller = inputMinion->team;
		inputTile->capturePoints = 20;

		//If this is that player's headquarters, they are no longer alive - have lost.
		if (inputTile->symbol == 'Q')
		{
			playerDefeat(previousOwner, playerFlag, InputLayer);
		}

		inputMinion->isCapturing = false;

		//Create event text telling player property was captured.
		textToReturn += "PLAYER ";
		textToReturn += char(previousOwner);							//MUST FIX IMPLEMENTATION of Char-32 nonsense.
		textToReturn += "'s ";
		textToReturn += inputTile->description;
		textToReturn += " CAPTURED!";

		inputTile->mySprite.setTextureRect(rectArray[inputTile->textureRectAnchorX + inputTile->controller][inputTile->textureRectAnchorY]);
		inputTile->myFogSprite.setTextureRect(rectArray[inputTile->textureRectAnchorX + inputTile->controller][inputTile->textureRectAnchorY + 1]);

	}


	inputMinion->status = hasfired;


	return textToReturn;
}


//Does player victory process. Can be called after playerDefeat, or directly if a victory condition was met.
//Only print victory if human player present.
int MasterBoard::playerVictory(int winningPlayer, inputLayer* InputLayer)
{
	std::cout << "Player " << winningPlayer << " wins" << std::endl;

	bool humanPlayerPresent = false;
	for (int i = 1; i < playerRoster.size(); i++)
	{
		if (playerRoster[i].playerType != computerPlayer)
			humanPlayerPresent = true;
	}

	if (humanPlayerPresent == true)
		InputLayer->printPlayerVictory(winningPlayer, this);

	if (missionFlag == false || playerRoster[winningPlayer].playerType == computerPlayer)
	{
		InputLayer->exitToMainMenu(this);
		
		//Track number of wins
		if (InputLayer->MainMenu->battleLabOn == true)
		{
			InputLayer->MainMenu->battleLabWinningPlayer = winningPlayer ;
			InputLayer->MainMenu->battleLabReset = true;
		}
	}
	else
	{
		InputLayer->NextMission(this);
	}

	return 0;

}


//Destroys all minions owned by loser.
//Transfers properties to either the HQ capper, or neutral.
//Does NOT discriminate - you must set winningPlayer = 0 if all minions were destroyed.
//Doesn't print defeat if both computer players are playing
int MasterBoard::playerDefeat(int losingPlayer, int winningPlayer, inputLayer* InputLayer)
{
	std::cout << "Player defeated" << std::endl;
	playerRoster[losingPlayer].stillAlive = false;

	bool throwAway = false;
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL && minionRoster[i]->team == losingPlayer)
		{
			destroyMinion(minionRoster[i], false, InputLayer, true);
		}
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (Board[x][y].controller == losingPlayer)
			{
				Board[x][y].controller = winningPlayer;

				//If it was being capped by winning player, reset cap points. Otherwise, they remain.
				if (winningPlayer != 0 && Board[x][y].hasMinionOnTop == true && Board[x][y].minionOnTop->team == winningPlayer)
				{
					Board[x][y].capturePoints = 20;
				}
			}
		}
	}

	bool humanPlayerPresent = false;
	int playersLeft = 0;
	//Determine if this was the last player alive
	for (int i = 1; i < playerRoster.size(); i++)
	{
		if (playerRoster[i].stillAlive == true)
		{
			playersLeft++;
		}

		if (playerRoster[i].playerType != computerPlayer)
			humanPlayerPresent = true;
	}

	//As long as at least one player is a human player, print the defeat screen. Otherwise carry on.
	if (humanPlayerPresent == true)
		InputLayer->printPlayerDefeat(losingPlayer, this);


	bool gameOver = false;
	if (playersLeft <= 1)
	{
		int winningPlayer = 0;
		for (int i = 1; i < playerRoster.size(); i++)
		{
			if (playerRoster[i].stillAlive == true)
			{
				winningPlayer = i;
			}
		}

		playerVictory(winningPlayer, InputLayer);
		gameOver = true;
	}

	//Then increment turn if appropriate (Died during player turn, game is not over yet.)
	if (losingPlayer == playerFlag && gameOver == false)
	{
		//Observer will be next player
		endTurn(InputLayer);
	}

	//Return game over code if needed
	if (gameOver == true)
		return 999;
	else 	
		return 0;
}

int MasterBoard::deselectMinion()
{

	if (cursor.selectMinionFlag == false)
	{
		std::cout << "Minion already de-selected." << std::endl;
		return 1;
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			Board[x][y].withinRange = false;
			Board[x][y].withinCursorPath = false;
			Board[x][y].withinApparentRange = false;
		}
	}

	cursor.selectMinionFlag = false;
	cursor.selectMinionPointer = NULL;


	return 0;
}

//Landmine attack is the only way a landmine does damage. It does not counterfire and it doesn't use normal attackMinion.
int MasterBoard::landmineAttack(int attackingX, int attackingY, inputLayer* InputLayer, int observerNumber)
{

	
	//Simplify by finding shorthand values first.
	Minion* attackingMinion = Board[attackingX][attackingY].minionOnTop;

	//For each adjacent square, check if it has a minion on it, and deal damage to that minion, regardless of team.
	//Landmines don't receive splash damage from other landmines.
	if(attackingX > 0 && Board[attackingX-1][attackingY].hasMinionOnTop == true  && Board[attackingX-1][attackingY].minionOnTop->type != "Landmine"  )
		splashAttackMinion(attackingMinion, attackingX-1, attackingY, InputLayer, observerNumber , 2);
	
	if(attackingY > 0 && Board[attackingX][attackingY-1].hasMinionOnTop == true && Board[attackingX][attackingY-1].minionOnTop->type != "Landmine")
		splashAttackMinion(attackingMinion, attackingX, attackingY-1, InputLayer, observerNumber, 2);
	
	if(attackingX < BOARD_WIDTH - 1 && Board[attackingX+1][attackingY].hasMinionOnTop == true && Board[attackingX+1][attackingY].minionOnTop->type != "Landmine")
		splashAttackMinion(attackingMinion  , attackingX + 1, attackingY, InputLayer, observerNumber , 2);

	if(attackingY < BOARD_HEIGHT - 1 && Board[attackingX ][attackingY + 1].hasMinionOnTop == true && Board[attackingX][attackingY + 1].minionOnTop->type != "Landmine")
		splashAttackMinion(attackingMinion, attackingX , attackingY + 1, InputLayer, observerNumber , 2);

	//Set graphics for splash damage sprites
	bool splashGraphicsOn = true;			

	//Combat graphics for landmine, with both tiles equivalent, to cause graphics to be landmine-style.
	if(InputLayer->inputLayerTexture != NULL)
		InputLayer->combatGraphics(this, observerNumber, &Board[attackingX][attackingY], &Board[attackingX][attackingY] , splashGraphicsOn);

	//Landmine is autodestroyed.
	destroyMinion(attackingMinion, false, InputLayer, false);
	setVisionField(playerFlag);


	return 0;
}


//Additionally, can only attack once. If artillery, cannot have moved or be adjacent.
//If hybrid, different attack based on if you have moved or not.
int MasterBoard::attackMinion(int inputX, int inputY, inputLayer* InputLayer, int observerNumber)
{

	//Simplify by finding shorthand values first.
	Minion* attackingMinion = cursor.selectMinionPointer;
	Minion* defendingMinion = Board[inputX][inputY].minionOnTop;

	//If a minion dies and causes the game to end, need to track that.
	int gameOverCode = 0;

	if (attackingMinion == NULL || defendingMinion == NULL)
	{
		std::cout << "Couldn't attack, one of the minions doesn't exist!" << std::endl;
		return -1;
	}


	bool isAmmoUsed = false;
	int weaponUsed = 0;
	bool ignoreRealMapLimitations = false;


	//Need to ensure we can't attack if we can't do any damage.
	if (calculateDamageDealt(attackingMinion, defendingMinion, isAmmoUsed, weaponUsed, ignoreRealMapLimitations) <= 0)
		return 1;

	//Can't attack if you can't see.
	if (Board[inputX][inputY].withinVision[playerFlag] == false)
		return 1;

	bool defenderAlive = true;

	//Cannot attack twice!
	if (attackingMinion->status == hasfired)
	{
		return 1;
	}

	//If artillery type, cannot attack adjacent (Minimum range)
	if ((attackingMinion->rangeType == rangedFire)
		&& (isAdjacent(defendingMinion->locationX, attackingMinion->locationX, defendingMinion->locationY, attackingMinion->locationY)))
	{
		return 1;
	}

    //Splash attack occurs before main attack. 
	bool splashGraphicsOn = false;
    if(attackingMinion->specialtyGroup == splashAttack)
    {
        //For each adjacent square, check if it has a minion on it, and deal damage to that minion, regardless of team.
        if(inputX > 0 && Board[inputX-1][inputY].hasMinionOnTop == true)
            splashAttackMinion(attackingMinion, inputX-1, inputY, InputLayer, observerNumber , 4);
            
        if(inputY > 0 && Board[inputX][inputY-1].hasMinionOnTop == true )
            splashAttackMinion(attackingMinion, inputX, inputY-1, InputLayer, observerNumber , 4);
            
        if(inputX < BOARD_WIDTH - 1 && Board[inputX+1][inputY].hasMinionOnTop == true )
            splashAttackMinion(attackingMinion , inputX + 1, inputY, InputLayer, observerNumber , 4);
        
        if(inputY < BOARD_HEIGHT - 1 && Board[inputX ][inputY + 1].hasMinionOnTop == true )
            splashAttackMinion(attackingMinion, inputX , inputY + 1, InputLayer, observerNumber , 4);
		
		//Set graphics for splash damage sprites
		splashGraphicsOn = true;			
    }
	
	//Combat graphics for attacker
	InputLayer->combatGraphics(this, observerNumber, &Board[attackingMinion->locationX][attackingMinion->locationY], &Board[inputX][inputY] , splashGraphicsOn);


	isAmmoUsed = false;
	//Decrease defender's health by attack value. Decrease ammo as needed.
	int randomFactor = (rand() % 10);       //Rand now adds between 0-9.
	int damageDealt = randomFactor + calculateDamageDealt(attackingMinion, defendingMinion, isAmmoUsed, weaponUsed, ignoreRealMapLimitations);
	if (damageDealt < 0)
		damageDealt = 0;
	defendingMinion->health -= damageDealt;

	if (isAmmoUsed == true)
	{
		if (weaponUsed == 1)
			attackingMinion->currentPriAmmo--;
		if (weaponUsed == 2)
			attackingMinion->currentSecAmmo--;
	}


	if (defendingMinion->health < 5)
	{
		//If defender falls below 4, it dies.
		bool printMessage = true;
		int defendingPlayer = defendingMinion->team;
		gameOverCode = destroyMinion(defendingMinion, printMessage, InputLayer, false);
		defenderAlive = false;
		if (attackingMinion->veterancy < 3 )
		{
			attackingMinion->veterancy++;
		}
		setVisionField(defendingPlayer);	//Change vision field to account for dead minion.
	}
	else	//Cannot be artillery type. Cannot be non-Artillery if artillery was attacking.
		if ((attackingMinion->rangeType == directFire || attackingMinion->rangeType == hybridRange)
			&& (defendingMinion->rangeType == directFire || defendingMinion->rangeType == hybridRange)
			&& (isAdjacent(cursor.getX(), attackingMinion->locationX, cursor.getY(), attackingMinion->locationY)))
		{
			//Defending landmine does not perform counterfire.
			if(defendingMinion->type != "Landmine")	
			{
				//If defender still alive, then perform defensive counterfire.
				//Same calculations as above - includes veterancy
				isAmmoUsed = false;

				//Determine base damage to see if unit actually does ANY damage.
				int randomFactor = (rand() % 10);       //Rand now adds between 0-9.
				int baseDamage = calculateDamageDealt(defendingMinion, attackingMinion, isAmmoUsed, weaponUsed, ignoreRealMapLimitations);
				int damageDealt = 0;

				//If base damage is above 0, add random factor and move on.
				if (baseDamage > 0)
					damageDealt = randomFactor + baseDamage;
				else damageDealt = 0;       //If not above 0, then damagedealt is 0.

				attackingMinion->health -= damageDealt;

				if (isAmmoUsed == true)
				{
					if (weaponUsed == 1)
						attackingMinion->currentPriAmmo--;
					if (weaponUsed == 2)
						attackingMinion->currentSecAmmo--;
				}

				//Combat graphics for defender
				//Defender never does splash damage.
				if (damageDealt > 0)
					InputLayer->combatGraphics(this, observerNumber, &Board[inputX][inputY], &Board[attackingMinion->locationX][attackingMinion->locationY], false);

			}
		}

	//Is game over? (999)
	if (gameOverCode != 999)
	{
		if (attackingMinion->health < 5)			//The attacker can be destroyed too!
		{
			bool printMessage = true;
			if (defenderAlive == true && defendingMinion->veterancy <= 3)
			{
				defendingMinion->veterancy++;
			}
			gameOverCode = destroyMinion(attackingMinion, printMessage, InputLayer, false);
			setVisionField(playerFlag);
		}
		else
		{
			//If the attacker survived, deselect.
			attackingMinion->status = hasfired;
			deselectMinion();
		}

	}

	//If game ended from a miniond dying, return game end code
	if (gameOverCode == 999)
		return 999;
	else
		return 0;

}

//This function executes a splash damage attack, which deals damage to all adjacent minions of X and Y.
//This occurs before main attack since the attacking minion may be destroyed in that attack.
int MasterBoard::splashAttackMinion( Minion * splashAttackingMinion , int inputX, int inputY, inputLayer* InputLayer, int observerNumber, int splashFactorInverse)
{
    Minion* splashDefendingMinion = Board[inputX][inputY].minionOnTop;

    if(Board[inputX][inputY].hasMinionOnTop == false)
        return -1;

    //Return values will be ignored since this is splash, no ammo used, etc.
	bool isAmmoUsed = false;
	int weaponUsed = 0;
	bool ignoreRealMapLimitations = false;
	
    //Decrease defender's health by attack value. No ammo used in splash attack.
	int randomFactor = (rand() % 10);       //Rand now adds between 0-9.
	int damageDealt = calculateDamageDealt(splashAttackingMinion, splashDefendingMinion, isAmmoUsed, weaponUsed, ignoreRealMapLimitations);
	
	//Must be greater than 0 to get rando, otherwise it was either -1 or 0 (Illegal attack)
	if (damageDealt > 0)
		damageDealt += randomFactor;

    //Splash modifier: Usually 25% of normal attack, rounding down. For landmine it's 50%.
    damageDealt = damageDealt / splashFactorInverse;
    
    if (damageDealt < 0)
		damageDealt = 0;
	splashDefendingMinion->health -= damageDealt;
	

	// below this line still needs work:
	//Need to compare against attackMinion to ensure everything is in parallel
	if (splashDefendingMinion->health < 5)
	{
		//If defender falls below 4, it dies.
		bool printMessage = true;
		int defendingPlayer = splashDefendingMinion->team;
		destroyMinion(splashDefendingMinion, printMessage, InputLayer, false);
		setVisionField(defendingPlayer);	//Change vision field to account for dead minion.
	}

}

int MasterBoard::destroyMinion(Minion* inputMinion, bool printMessage, inputLayer* InputLayer, bool AdminKill)
{
	if (inputMinion == NULL)
	{
		std::cout << "Could not destroy mininion, it doesn't exist." << std::endl;
		return 1;
	}

	int minionController = inputMinion->team;

	//If carrying a guy, kill that guy first
	if (inputMinion->secondMinionBeingTransported != NULL)
	{
		destroyMinion(inputMinion->secondMinionBeingTransported, printMessage, InputLayer, AdminKill);
	}

	if (inputMinion->firstMinionBeingTransported != NULL)
	{
		destroyMinion(inputMinion->firstMinionBeingTransported, printMessage, InputLayer, AdminKill);
	}

	//If minion is being transported, then do not attmept to deselect or talk to its location.
	//Otherwise, deselct it and reset it.
	if (inputMinion->transporter == NULL)
	{
		deselectMinion();
		//Tell the board it has no minions associated in the location where the Minion was alive.
		//Reset capture points for the property.
		Board[inputMinion->locationX][inputMinion->locationY].hasMinionOnTop = false;
		Board[inputMinion->locationX][inputMinion->locationY].capturePoints = 20;
	}

	if (printMessage == true)
	{
		//Create event text telling player it was destroyed.
		InputLayer->eventText += "PLAYER ";
		InputLayer->eventText += char(playerFlag + '0');
		InputLayer->eventText += "'s ";
		InputLayer->eventText += inputMinion->description;
		InputLayer->eventText += " DESTROYED!";
	}

	//Clean up. Currently since we're not cleaning up the minion roster after a death, there is a hole that may prevent saving properly.
	totalNumberOfMinions--;
	minionRoster[inputMinion->seniority] = NULL;
	delete inputMinion;

	int gameEndCode = 0;	//999 indicates game over

	//Decrease player minion count.
	playerRoster[minionController].numberOfMinions--;

	if (playerRoster[minionController].numberOfMinions <= 0 && AdminKill == false)
	{
		//"Neutral" player will be the "winner" of a defeat via destruction of all minions.
		if (playerRoster[minionController].stillAlive == true)
		{
			gameEndCode = playerDefeat(minionController, 0, InputLayer);
		}

	}

	return gameEndCode;
}

int MasterBoard::endTurn(inputLayer* InputLayer) {

	int gameTurnIncrement = 0;

	do
	{
		//Either increment playerFlag or set it to zero, thus cycling through the players.
		if (playerFlag < NUMBEROFPLAYERS)
		{
			playerFlag++;

		}
		else
			if (playerFlag >= NUMBEROFPLAYERS)
			{
				//We need to tell inputLayer to tell mainMenu to increment gameTurn.
				gameTurnIncrement = 1;
				playerFlag = 1;
			}
	} while (playerRoster[playerFlag].stillAlive == false);



	//Reset every minion's status.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		if (minionRoster[i] != NULL)
		{
			minionRoster[i]->status = hasntmovedorfired;
		}
	}

	//If single player, remain in gameBoard mode. If not, change mode.
	if (isItSinglePlayerGame == true)
	{
		InputLayer->status = gameBoard;
	}
	else	//Depends on remote and if hosting or client
		if (InputLayer->MainMenu->gameType == localSkirmish)
		{
			InputLayer->status = waitingForNextLocalPlayer;
		}
		else if (InputLayer->MainMenu->gameType == remoteHost)
			{
				InputLayer->status = waitingForClient;
			}
			else if (InputLayer->MainMenu->gameType == remoteClient)
				{
					InputLayer->status = waitingForHost;
				}


	return gameTurnIncrement;

}

//Upkeep always collects income. It is only called on the first turn of a new game, or at the end of a turn.
int MasterBoard::upkeep(inputLayer* InputLayer, int observerNumber)
{

	//If we have reached the end of the mission based on turn length, then end the mission with a winner.
	if (missionTurnLength != 0 && InputLayer->MainMenu->gameTurn >= missionTurnLength)
	{
		playerVictory(whoHoldsOut, InputLayer);
	}

	//Set vision field for current player
	setVisionField(playerFlag);

	//Provide income for the current player based on properties he controls.
	//Move cursor to HQ if it exists
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (Board[x][y].controller == playerFlag)
			{
				//Provide income
				playerRoster[playerFlag].treasury += Board[x][y].production;

				if (Board[x][y].symbol == 'Q')
				{
					cursor.relocate(x, y);
				}
			}

		}
	}

	repairMinions(InputLayer, observerNumber);

	resupplyMinions(InputLayer, observerNumber);


	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If it's a minion you own and it's not being transported
		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag && minionRoster[i]->transporter == NULL)
		{
			if (minionRoster[i]->domain == air && Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol != 'A')
			{
				minionRoster[i]->currentFuel -= 5;
				if (minionRoster[i]->currentFuel <= 0)
				{
					destroyMinion(minionRoster[i], "No fuel", InputLayer, false);
				}
			}
			else
				if (minionRoster[i]->domain == sea && Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol != 'P')
				{
					minionRoster[i]->currentFuel -= 2;

					//Submarine costs more fuel to stay submerged.
					if (minionRoster[i]->type == "Submarine" && minionRoster[i]->stealthMode == true)
						minionRoster[i]->currentFuel -= 3;

					if (minionRoster[i]->currentFuel <= 0)
					{
						destroyMinion(minionRoster[i], "No fuel", InputLayer, false);
					}
				}
		}
	}

	//Move cursor to HQ
	return 0;
}

int MasterBoard::individualRepair(Minion* MinionToRepair,  inputLayer* InputLayer, int observerNumber)
{
	int success = 0;		

	//Can only repair friendly non-infantry minion
	if (MinionToRepair != NULL && MinionToRepair->team == playerFlag && MinionToRepair->specialtyGroup != infantry)
	{
		//Must have enough money

		int minionCost = consultMinionCostChart(MinionToRepair->type, '~');
		//silent repair
		if (MinionToRepair->health > 94)
		{
			MinionToRepair->health = 100;
		}
		//"1 health" repair
		//Need enough money to repair
		else if (MinionToRepair->health <= 94 && MinionToRepair->health > 84 && int(minionCost / 10) <= playerRoster[playerFlag].treasury)
		{
			MinionToRepair->health = 100;
			playerRoster[playerFlag].treasury -= int(minionCost / 10);
		}
		else if (int(minionCost / 10) <= playerRoster[playerFlag].treasury)
		{
			MinionToRepair->health += 10;
			playerRoster[playerFlag].treasury -= int(minionCost / 10);
		}
		else 
		{
			success = 1;
		}
	}
	else success = 1;


	if (success == 0)
	{
		InputLayer->repairGraphics(this, observerNumber, MinionToRepair, MinionToRepair->locationX, MinionToRepair->locationY);
		
		//If we made it this far we did a successful repair, so deselect
		cursor.selectMinionPointer->status = hasfired;
		deselectMinion();

	}

	return success;
}

int MasterBoard::repairMinions(inputLayer* InputLayer, int observerNumber)
{
	std::cout << "Repair ALL MINIONS" << std::endl;

	//This was terminating "early" since it would hit a NULL spot so minions wouldn't get seen to be healed.
	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		bool repairThisMinion = false;


		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag)
		{
			//If it is air minion being carried by carrier, that carrier can repair.
			if (minionRoster[i]->transporter != NULL && minionRoster[i]->transporter->type == "Aircraft_Carrier")
			{
				if (minionRoster[i]->health < 100)
				{
					repairThisMinion = true;
				}
			}
			else 
			if(minionRoster[i]->transporter == NULL)	//If it's a minion you own and it's not being transported
			{
				tile* tileToExamine = &Board[minionRoster[i]->locationX][minionRoster[i]->locationY];
				//If it is on a player controlled tile, and that tile is a "repairing" tile for the given unit.
				if (tileToExamine->controller == playerFlag && consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) != -1)
				{
					if (minionRoster[i]->health < 100)
					{
						repairThisMinion = true;
					}
				}
			}
		}

		if (repairThisMinion == true)
		{
			bool showRepair = true;
			
			int minionCost = consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol);

			//silent repair
			if (minionRoster[i]->health > 94)
			{
				minionRoster[i]->health = 100;
			}
			//"1 health" repair
			//Need enough money to repair
			else if (minionRoster[i]->health <= 94 && minionRoster[i]->health > 84 && int(minionCost/10) <= playerRoster[playerFlag].treasury)
			{
				minionRoster[i]->health = 100;
				playerRoster[playerFlag].treasury -= int(minionCost / 10);
			}
			//"2" repair, but close enough to not just add
			else if (minionRoster[i]->health <= 84 && minionRoster[i]->health > 80 && int(minionCost / 5) <= playerRoster[playerFlag].treasury)
			{
				minionRoster[i]->health = 100;
				playerRoster[playerFlag].treasury -= int(minionCost/ 5);
			}
			//Standard "2" repair
			else if( int(minionCost / 5) <= playerRoster[playerFlag].treasury)
			{
				minionRoster[i]->health += 20;
				playerRoster[playerFlag].treasury -= int(minionCost / 5);
			}
			else 
			{
				showRepair = false;
			}

			if(showRepair == true)
				InputLayer->repairGraphics(this, observerNumber, minionRoster[i], minionRoster[i]->locationX, minionRoster[i]->locationY);
		}

	}

	return 0;
}

int MasterBoard::resupplyMinions(inputLayer* InputLayer, int observerNumber)
{

	for (int i = 0; i < GLOBALSUPPLYCAP; i++)
	{
		//If it's a minion you own and it's not being transported
		if (minionRoster[i] != NULL && minionRoster[i]->team == playerFlag && minionRoster[i]->transporter == NULL)
		{
			tile* tileToExamine = &Board[minionRoster[i]->locationX][minionRoster[i]->locationY];
			//If it is on a player controlled tile, and that tile is a "repairing/resupplying" tile for the given unit.
			if (tileToExamine->controller == playerFlag && consultMinionCostChart(minionRoster[i]->type, Board[minionRoster[i]->locationX][minionRoster[i]->locationY].symbol) != -1)
			{
				//Check to see if resupply is even necessary
				if (minionRoster[i]->currentFuel != minionRoster[i]->maxFuel
					|| minionRoster[i]->currentPriAmmo != minionRoster[i]->maxPriAmmo
					|| minionRoster[i]->currentSecAmmo != minionRoster[i]->maxSecAmmo)
				{
					minionRoster[i]->currentFuel = minionRoster[i]->maxFuel;
					minionRoster[i]->currentPriAmmo = minionRoster[i]->maxPriAmmo;
					minionRoster[i]->currentSecAmmo = minionRoster[i]->maxSecAmmo;
					cursor.relocate(minionRoster[i]->locationX, minionRoster[i]->locationY);
					InputLayer->supplyGraphics(this, observerNumber, minionRoster[i], minionRoster[i]->locationX, minionRoster[i]->locationY);
				}
			}

			if (minionRoster[i]->type == "APC")
			{
				individualResupply(minionRoster[i], true, InputLayer, observerNumber);
			}

		}
	}

	//Need to add APC in too

	return 0;
}