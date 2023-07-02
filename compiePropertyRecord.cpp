//Copyright 2022, Supercontinent Software Ltd.
//
//	compiePropertyRecord.cpp
//

#include "compiePropertyRecord.hpp"
#include <thread>
#include <iostream>


compiePropertyRecord::compiePropertyRecord(tile* inputTile)
{

	recordedTile = inputTile;
	symbol = inputTile->symbol;

	if (symbol == 'P')
	{
		purchasePreferenceList.emplace_back("Gunboat", 0);
		purchasePreferenceList.emplace_back("Cruiser", 0);
		purchasePreferenceList.emplace_back("Lander", 0);
		purchasePreferenceList.emplace_back("Submarine", 0);
		purchasePreferenceList.emplace_back("Battleship", 0);
		purchasePreferenceList.emplace_back("Carrier", 0);
	}
	else
		if (symbol == 'h')
		{
			purchasePreferenceList.emplace_back("Infantry", 0);
			//More needed
		}


}


