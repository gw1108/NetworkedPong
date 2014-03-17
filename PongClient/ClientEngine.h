// ================================================================================================
// Filename: "ClientEngine.h"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// Class Description:
// 
// The ClientEngine class will control all game logic and updates to the game data.
// ================================================================================================ 

#ifndef CLIENTENGINE_H
#define CLIENTENGINE_H

#include <list>
#include <mutex>
#include <thread>
#include <SFML\System\Clock.hpp>
#include <SFML\Graphics.hpp>
#include "GameData.h"
#include "CNetworkController.h"
#include "Paddle.h"
#include "Ball.h"
#include "HealthPack.h"

class ClientEngine
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	ClientEngine(CNetworkController *networkControl);
	~ClientEngine(void);

	// Prototypes
	void Render(void);

	// Inlined Methods
	void Run(void);
	void Stop(void){ m_running = false; }

private:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Prototypes
	void UpdateGameData();
	void CheckPaddleCollisions(void);
	void UpdateScoreText(void);
	void InterpolateBall(float ballX, float ballY, double ballAngle);
	void InterpolateOpponent(float paddleLoc, int paddleDir);

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Speed Mod Constants
	static const int SHORT_THRESHOLD;
	static const int MEDIUM_THRESHOLD;
	static const int FAST_THRESHOLD;
	static const float MEDIUM_SPEED_MOD;
	static const float FAST_SPEED_MOD;

	// Engine Related Members
	CNetworkController *m_networkControl;
	sf::Clock m_engineClock;
	bool m_running;

	// Game Pieces
	GameData m_gameState;
	Paddle *m_rightPaddle;
	Paddle *m_leftPaddle;
	Paddle *m_clientPaddle;
	Paddle *m_opponentPaddle;
	int m_oppMoveDirection;
	int m_oppSpeedMod;
	Ball m_gameBall;

	// Window Members
	sf::RenderWindow *m_mainWindow;
	sf::Font m_gameFont;
	int m_player1Score;
	sf::Text m_player1ScoreText;
	int m_player2Score;
	sf::Text m_player2ScoreText;
	HealthPack m_healthPack;

};

#endif