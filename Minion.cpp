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
//When updating Minion(), also update ATTACK_VALUES_MATRIX, consultAttackValuesChart, movement costs and consultMinionCostChart.
//This assumes that the transport already exists if the minion is being carried. Can cause NULL dereference!
Minion::Minion(int inputSeniority, int inputX, int inputY, char inputType, int inputTeam, MasterBoard* Environment, int inputHealth, int inputVeterancy, int beingTransported)
{
	veterancy = inputVeterancy;
	seniority = inputSeniority;
	minionEnvironment = Environment;
	type = inputType;
	health = inputHealth;

	//If being transported, this minion doesn't take up any space.
	//Also, it will be transported by whomever we indicate with InputX and Y
	if (beingTransported == 1) 
	{
		if (Environment->Board[inputX][inputY].hasMinionOnTop != NULL)
		{
			transporter = Environment->Board[inputX][inputY].minionOnTop;
			Environment->Board[inputX][inputY].minionOnTop->minionBeingTransported = this;
		}
		else std::cout << "Bad transport, minion doesn't exist!!" << std::endl;

		locationX = -1;
		locationY = -1;
	}
	else	//If not transported, proceed as normal.
	{
		Environment->Board[inputX][inputY].hasMinionOnTop = true;
		locationX = inputX;
		locationY = inputY;
	}

	switch (inputType)
	{
	case('i'):
	{		
		description = "Infantry";
		movementRange = 3;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		Image =		  { 'i',' ','i',
						' ','i',' ',
						' ',' ',' ' };
		break;
	}
	case('s'):
	{
		description = "Specialist";
		movementRange = 2;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = infantry;
		Image = {				's',' ','s',
								' ','s',' ',
								' ',' ',' ' };

		break;
	}
	case('a'):
	{
		description = "Armor";
		movementRange = 6;
		attackRange = 1; 
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		Image =		{	'-','n',' ',
						'=','=','=',
						' ',' ',' ' };
		break;
	}
	case('A'):
	{
		description = "Anti-Air";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		Image = {		'\\','\\','A',
						'=','=','=',
						' ',' ',' ' };
		break;
	}
	case('T'):
	{
		description = "Heavy Armor";
		movementRange = 5;
		attackRange = 1;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = normal;
		Image = {		'-','H',' ',
						'=','=','=',
						' ',' ',' ' };
		break;
	}
	case('r'):
	{
		description = "Artillery";
		movementRange = 5;
		attackRange = 3;
		visionRange = 2;
		rangeType = rangedFire;
		specialtyGroup = normal;
		Image = {		' ','\\','A',
						'=','=','=',
						' ',' ',' ' };
		break;
	}
	case('c'):
	{
		description = "Cavalry";
		movementRange = 8;
		attackRange = 1;
		visionRange = 5;
		rangeType = directFire;
		specialtyGroup = normal;
		Image = {		'-','n',' ',
						'o','=','o',
						' ',' ',' ' };
		break; 
	}
	case('R'): 
	{ 
		description = "Rocket";
		movementRange = 5;
		attackRange = 5;
		visionRange = 2;
		rangeType = rangedFire;
		specialtyGroup = normal;
		Image = {			'n','\\','\\',
							'o','=','o',
							' ',' ',' ' };
		break; 
	}
	case('v'):
	{
		description = "Viper";
		movementRange = 6;
		attackRange = 1;
		visionRange = 3;
		rangeType = directFire;
		specialtyGroup = normal;
		Image = { '-','+','-',
				'<','=','*',
				' ',' ',' ' };
		break;
	}
	case('h'):
	{
		description = "Huey";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = transport;
		Image = { '+',' ','+',
				'<','=','=',
				' ',' ',' ' };
		break;
	}
	case('P'):
	{
		description = "APC";
		movementRange = 6;
		attackRange = 0;
		visionRange = 2;
		rangeType = directFire;
		specialtyGroup = transport;
		Image = { '/','=','=',
				'=','=','=',
				' ',' ',' ' };
		break;
	}
	}

	team = inputTeam;


}

double Minion::calculateVetBonus() 
{
	double combatBonus = 1.0;

		if (veterancy == 1) 
		{
			combatBonus = 1.05;
		} 
		else if (veterancy == 2) 
		{
			combatBonus = 1.1;
		}
		else if (veterancy == 3) 
		{
			combatBonus = 1.2;
		}

		return combatBonus;

}

//
//Destructor. Minion has no dynamic variables to clean up so it is empty.
//
Minion::~Minion() {}