#ifndef TILE_H__
#define TILE_H__

#include "Minion.hpp"
#include <string>

class tile 
{
public:
	tile() {
		hasMinionOnTop = false;
		withinRange = false;
		minionOnTop = NULL;
	}
	char symbol;
	bool hasMinionOnTop;
	Minion* minionOnTop;
	bool withinRange;
	std::string description;
	double defenseFactor;		//Higher is better (ie mountains are 1.4)
};

#endif /* TILE_H__ */

