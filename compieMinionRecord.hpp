//Copyright 2022, Supercontinent Software Ltd.
//
//	compieMinionRecord.hpp
//

#ifndef COMPIE_MINION_RECORD_H__
#define COMPIE_MINION_RECORD_H__

#include "Minion.hpp"
#include <string>
#include "MasterBoard.hpp"
#include "Cursor.hpp"
#include "Tile.hpp"
#include "inputLayer.hpp"

class inputLayer;
class Cursor;
class mainMenu;

enum minionTaskingType { attackLocalMinion, captureLocalProperty, advance, holdPosition, withdraw, defendHQ, defendProp};
enum minionTaskingStatus { awaitingTasking, pendingConfirmation, taskingExecuted, immediateExecute };


class compieMinionRecord
{
	
public:
	compieMinionRecord(Minion *inputMinion);
	compieMinionRecord() { return; };


	int initializeRecord(Minion* inputMinion);
	
	minionTaskingType tasking = holdPosition;
	minionTaskingStatus taskingStatus = awaitingTasking;

	//Minion this record points to
	Minion* recordedMinion = NULL;

	//These describe tiles the minion may interact with this turn.
	tile* potentialMoveTile = NULL;
	tile* potentialAttackTile = NULL;
	tile* potentialGuardTile = NULL;
	
	//Points to tile the minion is going towards long-term, like for capping or attacking.
	tile* objectiveTile = NULL;
};

#endif /* COMPIE_MINION_RECORD__ */

