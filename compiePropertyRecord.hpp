//Copyright 2022, Supercontinent Software Ltd.
//
//	compiePropertyRecord.hpp
//

#ifndef COMPIE_PROPERTY_RECORD_H__
#define COMPIE_PROPERTY_RECORD_H__

#include "Minion.hpp"
#include <string>
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"

class inputLayer;
class Cursor;
class mainMenu;

struct preferredPurchase
{
	std::string type = "";
	double preferenceScore = 0;

	preferredPurchase(std::string inputType, double inputPreference) 
	{
		type = inputType;
		preferenceScore = inputPreference;
	};
};

class compiePropertyRecord
{

public:
	compiePropertyRecord(tile* inputTile);

	//Tile this record points to
	tile* recordedTile = NULL;

	//List of potential minions this property would like to purchase
	std::vector <preferredPurchase> purchasePreferenceList;

	int purchasePriority = 0;

	char symbol = '_';

};


#endif /* COMPIE_PROPERTY_RECORD__ */
