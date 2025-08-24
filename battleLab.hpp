//Copyright 2025, Supercontinent Software Ltd.
//
//	battleLab.hpp
//

#ifndef BATTLELAB_H__
#define BATTLELAB_H__

struct returnData
{
	//Need to track these numbers for success metrics
	int repairThreshold = 40;					//10-90 represents when a minion will attempt to repair, on a 100 hp scale. 40 gives best behavior so far.
	int compieMinionAggressionBonus = 0;		//Intended range is -20 - 20 for reasonable behavior. Allows compie minions to attack even when the odds are against them.
	int compieInfantryAttackBonus = 2;			//Intended range is 1-3 for reasonable behavior. Makes attacking infantry more attractive - would be difficult to attack them with expensive minions otherwise
	int compieInfantryBaseline = 5;				//No idea what a reasonable range is, probably between 0 - 10. Describes min. number of infantry required in army.
	int compieArmySizeFactor = 4;				//Reasonable range is 1 - 8. 1 makes larger armies, 8 smaller.
	
	int playerOneWins = 0;
	int playerTwoWins = 0;
	int draws = 0;

};


#endif

