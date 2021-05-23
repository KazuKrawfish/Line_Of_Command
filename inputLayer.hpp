#ifndef INPUT_HPP__
#define INPUT_HPP__

enum gameInputLayer { gameBoard, menu, minionAction, propertyAction };
enum specialKey {showMinions, hideMinions};

class inputLayer
{
public:

	int printStatus(MasterBoard* boardToPrint);
	int printMinionMenu(MasterBoard* boardToPrint);
	int printBoardMenu();
	int	printPropertyMenu();
	int printMenu();
	int printLowerScreen(MasterBoard* boardToPrint);
	int printUpperScreen(MasterBoard* boardToPrint);
	int printScreen(MasterBoard* boardToPrint);
	int gameBoardInput(char* Input, MasterBoard* boardToInput);
	int menuInput(char* Input, MasterBoard* boardToInput);
	int propertyMenuInput(char* Input, MasterBoard* boardToInput);
	int minionInput(char* Input, MasterBoard* boardToInput);
	//These will all be wrapped into the inputLayer class soon.
	std::string eventText = "";

	gameInputLayer status = gameBoard;
	specialKey minionVisibleStatus = showMinions;
	char requestedMinionToBuy = '\n';
	int unitPrice = -1;


private:

};


#endif /* INPUT_HPP__ */


