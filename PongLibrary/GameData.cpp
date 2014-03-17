// ================================================================================================
// Filename: "GameData.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// This is the class implementation file for the GameData class. For a class description see the 
// header file "GameData.h"
// ================================================================================================

#include "GameData.h"
#include "Paddle.h"

// Set Constants
const int GameData::ENGINE_SPEED = 17;
const int GameData::BOARD_WIDTH = 800;
const int GameData::BOARD_HEIGHT = 600;
const int GameData::MAX_POINTS = 3;
const int GameData::LEFT_GOAL_LINE = 20;
const int GameData::RIGHT_GOAL_LINE = (BOARD_WIDTH - 20);

// ===== Default Constructor ======================================================================
// The default constructor will be used to zero all values prior to the GameState being used by 
// the engine.
//
// Input: none
// Output: none
// ================================================================================================
GameData::GameData(void)
{
	// Zero Player One Data
	playerOneScore = 0;
	playerOnePaddlePosition = (BOARD_HEIGHT / 2);
	playerOneDirection = Paddle::STILL;
	playerOneHealth = 100;

	// Zero Player Two Data
	playerTwoScore = 0;
	playerTwoPaddlePosition = (BOARD_HEIGHT / 2);
	playerTwoDirection = Paddle::STILL;
	playerTwoHealth = 100;

	// Zero Ball Data
	ballX = (BOARD_WIDTH / 2);			
	ballY = (BOARD_HEIGHT / 2);		
	ballAngle = 0;	

	// Zero Game Data
	playerScored = false;
	scoringPlayer = 0;
	gameWon = false;			
	winningPlayer = 0;		
	startGame = false;			
	healthPackSpawned = false;
}

// ===== Packet Input Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the GameData 
// class can be input into a packet for delivery to a client. 
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet to receive the GameData
//	[IN]	GameData& gameData		- a reference to the GameData
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator<<(sf::Packet& packet, GameData& gameData)
{
	packet	<< gameData.playerOneScore << gameData.playerOnePaddlePosition << gameData.playerOneDirection << gameData.playerOneHealth
			<< gameData.playerTwoScore << gameData.playerTwoPaddlePosition << gameData.playerTwoDirection << gameData.playerTwoHealth
			<< gameData.ballX << gameData.ballY << gameData.ballAngle << gameData.playerScored
			<< gameData.scoringPlayer << gameData.gameWon << gameData.winningPlayer << gameData.startGame
			<< gameData.healthPackSpawned;

	return packet;
}

// ===== Packet Output Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the GameData 
// class can be extracted from a packet by a client 
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet that contains the GameData
//	[IN]	GameData& gameData		- a reference to the GameData that will receive the data
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator>>(sf::Packet& packet, GameData& gameData)
{
	packet	>> gameData.playerOneScore >> gameData.playerOnePaddlePosition >> gameData.playerOneDirection >> gameData.playerOneHealth
			>> gameData.playerTwoScore >> gameData.playerTwoPaddlePosition >> gameData.playerTwoDirection >> gameData.playerTwoHealth
			>> gameData.ballX >> gameData.ballY >> gameData.ballAngle >> gameData.playerScored 
			>> gameData.scoringPlayer >> gameData.gameWon >> gameData.winningPlayer >> gameData.startGame
			>> gameData.healthPackSpawned;


	return packet;
}