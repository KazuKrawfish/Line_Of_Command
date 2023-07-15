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
		purchasePreferenceList.emplace_back("Gunboat", 0, normal);
		purchasePreferenceList.emplace_back("Cruiser", 0 , normal);
		purchasePreferenceList.emplace_back("Lander", 0 , largeTransport);
		purchasePreferenceList.emplace_back("Submarine", 0 , normal);
		purchasePreferenceList.emplace_back("Battleship", 0, normal);
		purchasePreferenceList.emplace_back("Carrier", 0, aircraftCarrier);
	}
	else
		if (symbol == 'h')
		{
			purchasePreferenceList.emplace_back("Infantry", 0 , infantry);
			purchasePreferenceList.emplace_back("Engineer", 0, infantry);
			purchasePreferenceList.emplace_back("Insurgent", 0, infantry);
			purchasePreferenceList.emplace_back("Operative", 0, infantry);
			purchasePreferenceList.emplace_back("Specialist", 0, infantry);
			purchasePreferenceList.emplace_back("Cavalry", 0, infantry);
			purchasePreferenceList.emplace_back("Armor", 0, normal);
			purchasePreferenceList.emplace_back("Upgunned_Armor", 0, normal);
			purchasePreferenceList.emplace_back("Assault_Gun", 0, normal);
			purchasePreferenceList.emplace_back("Anti-Aircraft", 0, normal);
			purchasePreferenceList.emplace_back("Heavy_Armor", 0, normal);
			purchasePreferenceList.emplace_back("Modern_Armor", 0, normal);
			purchasePreferenceList.emplace_back("Super_Heavy_Armor", 0, normal);
			purchasePreferenceList.emplace_back("Artillery", 0, normal);
			purchasePreferenceList.emplace_back("Recon", 0, normal);
			purchasePreferenceList.emplace_back("Technical", 0, normal);
			purchasePreferenceList.emplace_back("Rocket_Artillery", 0, normal);
			purchasePreferenceList.emplace_back("Victory_Launcher", 0, splashAttack);
			purchasePreferenceList.emplace_back("APC", 0, smallTransport);
			purchasePreferenceList.emplace_back("IFV", 0, smallTransport);
		}


}


