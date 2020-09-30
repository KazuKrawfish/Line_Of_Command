#ifndef MINION_HPP__
#define MINION_HPP__

#include <string>

class Minion
{	public:
	int addMinion(int inputSeniority, int inputLocation, char inputType, int inputTeam);
	Minion();
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
};

#endif /* MINION_HPP__ */

