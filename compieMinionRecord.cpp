//Copyright 2022, Supercontinent Software Ltd.
//
//	compieMinionRecord.cpp
//

#include "compieMinionRecord.hpp"
#include <thread>
#include <iostream>

compieMinionRecord::compieMinionRecord(Minion* inputMinion)
{
	recordedMinion = inputMinion;
	return ;
}

int compieMinionRecord::initializeRecord(Minion* inputMinion)
{
	recordedMinion = inputMinion;
	return 0;
}