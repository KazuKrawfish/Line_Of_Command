#ifndef INPUT_HPP__
#define INPUT_HPP__

#include "compie.hpp"

enum gameInputLayer { gameBoard, menu, minionAction, propertyAction };
enum specialKey {showMinions, hideMinions};
class compie;
class mainMenu;


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
	int printSingleTile(int inputX, int inputY, std::string inputString, int teamNumber);
	int exitToMainMenu();
	
	std::string eventText = "";

	gameInputLayer status = gameBoard;
	specialKey minionVisibleStatus = showMinions;
	char requestedMinionToBuy = '\n';
	int unitPrice = -1;
	compie* computerPlayer;
	mainMenu* MainMenu;

private:

};


#endif /* INPUT_HPP__ */


