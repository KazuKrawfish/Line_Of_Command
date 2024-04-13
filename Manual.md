# Line of Command Manual

#
## Getting Started

There are three options upon game start:
    
    -New Game: Can choose a new campaign or skirmish (Using combination of computer players and hot-seat players)
    -Load Game: Find a saved game
    -Editor Mode: Toggle this to allow editing units and terrain while playing a map

### New Game

This selection offers either campaign or skirmish. There is currently only one campaign although the mechanism exists to easily import other campaigns, but this was disabled for release. The campaign follows the Redonian Army as it brings "freedom" to its neighbors through warfare.

The skirmish mode allows for a single game. Based on the scenario you load, it can have between 1-4 players, and any number of properties and units. To choose a new map, you must enter the EXACT name of the map file. Any error will prompt for another name. Thus, to load scenario "midway.txt", enter "midway".

You cannot currently back out of new-game, you must either select a map and then exit to main menu, or restart the game.

### Load Game

Load game by selecting the map using the arrow buttons on the side. To load the auto-save file, which is saved after every turn, select "auto".

#
## GamePlay

### Controls

Control is based around the cursor, a black box with spaces between the corners. This can be used with either the mouse or keyboard, and all gameplay inputs are based on the cursor's location.

    -Left click once, outside the cursor - Moves the cursor to that location
    -Left click once, inside the cursor - Attempts to select the minion below the cursor, or the property below if there is no minion there.
    -Left click while minion/property is selected - Deselect.
    -Right click while minion is selected - Attempt to either move, attack, or capture. This is based on the minion's current state, and where the click occurs.
    -WASD - Move cursor one square in a given direction.
    -T - Attempt select below cursor.
    -M - If minion  selected, move below the cursor.
    -R - If minion selected, attack below the cursor.
    -#0 - Toggle hide/show minions.
    -#9 - Show range of minion beneath cursor.
  
### Editor Mode

Editor mode has several features that are accessed via the below keyboard inputs:

    -X - Insert minion at cursor location. Follow this with the minion type's first three letters. Infantry is "inf", Armor is "arm", etc. Must be legal placement, so no tanks in the water!
    -Q - Change the tile at cursor location. Follow this with the tile symbol letter. Plains is ".", forest is "+", etc. Must also be legal placement, so cannot place a land tile below a battleship, for instance.
    -Z - Delete minion at cursor location.
    -V - Immediately win the current game. If in campaign, will advance to the next mission.

### Minion State

Minions have four states - Ready to move, moved, hold, and finished.

    -Ready to move - The minion's movement range is shown by blue squares. It must complete this phase before capturing or attacking.
    -Moved - After moving, the minion can now attack and other special actions like capture. Attack range is shown by red squares. Ranged minions like artillery cannot attack after moving.
    -Hold - A minion that is ready to move can hold by "moving" on its own position. Any minion can attack after holding.
    -Finished - The minion has performed a move/hold AND an attack/special action, and can perform no more actions.

Special actions

    -Infantry minions can capture after moving/holding, by right clicking on their own position. They must be above an enemy property.
    -Transports can unload after moving/holding. Their drop range is shown in orange, and they drop the unit wherever is clicked.
    -Supply units (APC) can resupply all adjacent minions after moving/holding. Do so by right clicking on their own position.
    -Operative can deploy landmine by clicking on an adjacent tile after moving. This costs gold.
    -Engineer can build fortifications after moving, by clicking on their own position. This costs gold and can only be clear/forest tiles.

### Production

Factories, airbases, and ports all can produce minions. These minions are in the finished state upon production, so they have to wait a turn to act. To do so:

    -Select the property with two left clicks.
    -A graphic list of units will appear. Left-click the desired minion.
    -If there are sufficient funds in the treasury, that amount is deducted and the minion is built.

#
## Game Turn Elements

There are several elements of a turn: Upkeep and player actions, which includes minion actions, production, and victory condition check.

### Upkeep

Every turn begins with upkeep:
    
    -Every property adds 1000 gold to the player's treasury.
    -Air and sea minions pay a fuel cost for upkeep. If they run out of fuel they are destroyed.
    -Minions on a friendly property are resupplied (Free) and repaired if funds permit (20 HP repaired, with cost proportional to the minion's cost)

### Player Actions

    -The player can move all minions as discussed previously.
    -The player can produce units at friendly factories, airbases, and ports.
    -The player may defeat a player if he captures that player's headquarters or destroys all that player's minions. If all enemy players are defeated he wins.
    -The player may lose if he loses all his minions on his own turn (A difficult feat indeed!)


