#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>
#include "MasterBoard.hpp"

class Minion
{	
	public:
	
	//Constructors
	Minion(int inputSeniority, int inputLocation, char inputType, int inputTeam, MasterBoard* Environment);
	Minion();
	~Minion();
	
	
	//Member variables. Should be protected with functions dealing with them!
	bool isAlive;
	int Location;
	char type;
	bool isMinionSelected;
	int movementRange;
	int attackRange;
	std::string description;
	int team;
	bool hasMoved;
	bool hasAttacked;
	double health;
	bool artilleryCanAttack;
	MasterBoard* minionEnvironment;
};

#endif /* MINION_HPP__ */

