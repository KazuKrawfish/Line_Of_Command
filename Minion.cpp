#include "Minion.hpp"
#include "MasterBoard.hpp"

//
//Default Constructor. Should not be used.
//
Minion::Minion()
{
	locationX = -1;
	locationY = -1;
	type = ' ';
	team = 0;						//Team 0 is the neutral team.
	status = hasntmovedorfired;
	health = 100;
	
}

//
//Always use this constructor, it gives the new Minion all required properties.
//
Minion::Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam, MasterBoard* Environment)
{
	seniority = inputSeniority;
	minionEnvironment = Environment;
	locationX = inputX;
	locationY = inputY;
	type = inputType;
	switch (inputType)
	{
	case('i'):
	{		
		description = "Infantry";
		movementRange = 3;
		attackRange = 1;
		break;
	}
	case('t'):
	{
		description = "Tank";
		movementRange = 6;
		attackRange = 1; 
		break;
	}
	case('A'):
	{
		description = "Artillery";
		movementRange = 5;
		attackRange = 3;
		break;
	}
	case('C'):
	{
		description = "Cavalry";
		movementRange = 8;
		attackRange = 1;
		break; 
	}
	case('R'): 
	{ 
		description = "Rocket";
		movementRange = 4;
		attackRange = 5;
		break; 
	}
	}

	team = inputTeam;

	Environment->Board[inputX][inputY].hasMinionOnTop = true;
	health = 100;

}

//
//Destructor. Minion has no dynamic variables to clean up so it is empty.
//
Minion::~Minion() {}