// ================================================================================================
// Filename: "ServerEngine.h"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// Class Description:
// 
// The ServerEngine class will control all game logic and updates to the game data. It will contain
// the SNetworkController.
// ================================================================================================ 

#ifndef SERVERENGINE_H
#define SERVERENGINE_H

#include <SFML\System\Clock.hpp>
#include "GameData.h"
#include "Paddle.h"
#include "Ball.h"
#include "SNetworkController.h"
#include "HealthPack.h"

class ServerEngine
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	ServerEngine(SNetworkController *networkControl);
	~ServerEngine(void){}

	// Prototypes
	void Run(void);

private:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Prototypes
	void GetClientPaddleData(void); 
	void UpdatePositions(void);
	void UpdateState(void);


	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Network Controller
	SNetworkController *m_networkControl;

	// Engine Members
	bool m_running;
	sf::Clock m_engineClock;

	// Game Pieces
	Paddle *m_leftPaddle;
	Paddle *m_rightPaddle;
	Ball m_ball;
	HealthPack m_healthPack;

	// Game State
	GameData m_currentState;
	int m_player1LastUpdate;
	int m_player2LastUpdate;
	bool m_player1Scoring;
	bool m_player2Scoring;

};

#endif