#ifndef TILE_H__
#define TILE_H__

#include "Minion.hpp"
#include <string>

class tile 
{public:
	char symbol;
	bool hasMinionOnTop;
	Minion* minionOnTop;
	bool withinRange;
	std::string description;
};

#endif /* TILE_H__ */

