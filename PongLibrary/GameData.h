// ================================================================================================
// Filename: "GameData.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// Class Description:
// 
// The GameData class is a data structure that will maintain all values needed by the game to 
// work properly, it will contain ball and paddle positions, ball angle, scores, etc. A copy of
// the GameData is what will be send to the clients and it will also contain values that tell the 
// clients to display certain messages such as when a player scores. 
//
// The class will also maintain constants that are used to define certain game rules, bounds, or
// keywords used throughout the game.
//
// To increase simplicity, the GameData class will make all members public.
// ================================================================================================

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <SFML\Network\Packet.hpp>

class GameData
{

public:

	// Command Code Enum
	enum CommandCode {INITIALIZE, TIME_SYNC, GAME_UPDATE, PADDLE_UPDATE};

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	GameData(void);
	~GameData(void){}

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Static Constants
	static const int ENGINE_SPEED;		// The Loop Conditional to control the engine cycle rate of clients and server
	static const int BOARD_HEIGHT;		// Height of the playable area
	static const int BOARD_WIDTH;		// Width of the playable area
	static const int MAX_POINTS;		// Amount of points needed to win
	static const int LEFT_GOAL_LINE;	// X-Value that represents the left side goal
	static const int RIGHT_GOAL_LINE;	// X-Value that represents the right side goal

	// Player One Data
	int playerOneScore;
	float playerOnePaddlePosition;		// This is the y-coordinate of the center of the paddle
	int playerOneDirection;		
	int playerOneHealth;

	// Player Two Data
	int playerTwoScore;
	float playerTwoPaddlePosition;		// This is the y-coordinate of the center of the paddle
	int playerTwoDirection;		
	int playerTwoHealth;

	// Ball Data
	float ballX;						// Current x-coordinate of the ball
	float ballY;						// Current y-coordinate of the ball
	double ballAngle;					// Direction, in radians, the ball is moving

	// Gameplay Data
	bool playerScored;					// True if a player has scored since the last GameState was sent
	int scoringPlayer;					// Who the scoring player was
	bool gameWon;						// True if the game has been won
	int winningPlayer;					// The winning player
	bool startGame;						// Tells the client to start their game
	bool healthPackSpawned;				// If the healthpack is active

private:

};

// Packet Overloads for GameData
sf::Packet& operator<<(sf::Packet& packet, GameData& gameData);
sf::Packet& operator>>(sf::Packet& packet, GameData& gameData);

#endif