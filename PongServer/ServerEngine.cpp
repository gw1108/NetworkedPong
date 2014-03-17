// ================================================================================================
// Filename: "ServerEngine.cpp"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// This is the implementation file for the ServerEngine class. For class description see the header
// file "ServerEngine.h"
// ================================================================================================

#define _USE_MATH_DEFINES
#include <iostream>
#include "ServerEngine.h"
#include "Calculator.h"

// ===== Constructor ==============================================================================
// The constructor will initialize all game values.
//
// Input: none
// Output: none
//  ===============================================================================================
ServerEngine::ServerEngine(SNetworkController *networkControl) : m_networkControl(networkControl)
{
	// Initialize Engine Members
	m_running = false;
	m_engineClock.restart();

	m_leftPaddle = new Paddle(1);
	m_rightPaddle = new Paddle(2);

	m_player1Scoring = false;
	m_player2Scoring = false;
}

// ===== Run ======================================================================================
// This method will be the "infinite" loop that starts when the engine starts. Every iteration it
// will check for data from the clients, update the game state based on client data, and send the
// current GameData to the clients.
//
// Input: none
// Output: none
// ================================================================================================
void ServerEngine::Run(void)
{
	m_currentState.startGame = true;
	m_ball.Serve(0);
	m_currentState.ballAngle = m_ball.GetMoveAngle();
	m_networkControl->SendGameData(m_currentState);
	m_running = true;

	while(m_running)
	{
		// Limit Engine to ~60 Cycles a Second
		if(m_engineClock.getElapsedTime().asMilliseconds() < GameData::ENGINE_SPEED)
		{
			continue;
		}
		else
		{
			m_engineClock.restart();
		}

		if(!m_networkControl->ClientsConnected())
		{
			std::cout << "A client has disconnected, stopping engine.\n";
			m_running = false;
		}

		// Update HealthPack
		m_healthPack.Update();
		m_currentState.healthPackSpawned = m_healthPack.Spawned();

		// Check For Client Data
		GetClientPaddleData();
		UpdatePositions();

		if(m_player1Scoring)
		{
			m_currentState.playerOneScore += 1;

			if(m_currentState.playerOneScore >= GameData::MAX_POINTS)
			{
				m_currentState.gameWon = true;
				m_currentState.winningPlayer = 1;
				m_running = false;
			}

			std::cout << "Player 1 Scored.\n";
			m_currentState.playerOneHealth = 100;
			m_currentState.playerTwoHealth = 100;
			m_player1Scoring = false;
			m_ball.Serve(1);
		}
		else if(m_player2Scoring)
		{
			m_currentState.playerTwoScore += 1;

			if(m_currentState.playerTwoScore >= GameData::MAX_POINTS)
			{
				m_currentState.gameWon = true;
				m_currentState.winningPlayer = 2;
				m_running = false;
			}

			std::cout << "Player 2 Scored.\n";
			m_currentState.playerOneHealth = 100;
			m_currentState.playerTwoHealth = 100;
			m_player2Scoring = false;
			m_ball.Serve(2);
		}

		// Send GameState Update
		UpdateState();
		m_networkControl->SendGameData(m_currentState);
	}
}

// ===== GetClientPaddleData ======================================================================
// The method will retreive the latest paddle information from the client handlers. It will then
// use this information to update the current game state. The method must check if the client
// handler returned NULL, which means there is no new paddle data.
//
// Input: none
// Output: none
// ================================================================================================
void ServerEngine::GetClientPaddleData(void)
{ 
	PaddleUpdate temp;

	// Get Player 1's Data
	if(m_networkControl->GetPlayerPaddleData(1, temp))
	{
		m_leftPaddle->setPosition(m_leftPaddle->getPosition().x, temp.m_paddleLoc);
		m_currentState.playerOnePaddlePosition = temp.m_paddleLoc;
		m_currentState.playerOneDirection = temp.m_paddleDir;
		m_player1LastUpdate = temp.m_timestamp;
	}

	// Get Player 2's Data
	if(m_networkControl->GetPlayerPaddleData(2, temp))
	{
		m_rightPaddle->setPosition(m_rightPaddle->getPosition().x, temp.m_paddleLoc);
		m_currentState.playerTwoPaddlePosition = temp.m_paddleLoc;;
		m_currentState.playerTwoDirection = temp.m_paddleDir;
		m_player2LastUpdate = temp.m_timestamp;
	}
}

// ===== UpdatePositions ==========================================================================
// This method will update the ball and paddle positions. It will also check for paddle collisions
// and scores.
//
// Input: none
// Output: none
// ================================================================================================
void ServerEngine::UpdatePositions(void)
{
	// Update Ball
	m_ball.Move(1);

	// Check for Paddle Collisions
	if(m_ball.getPosition().x < (GameData::BOARD_WIDTH / 2))		// Check Left Side
	{
		if((Calculator::CheckCollision(m_ball, *m_leftPaddle)) && (m_currentState.playerOneHealth > 0))
		{
			std::cout << "Left Paddle Hit\n";

			m_currentState.playerOneHealth -= 10;

			double newAngle = Calculator::GetAngleTo(m_leftPaddle->getPosition().x, 
													 m_leftPaddle->getPosition().y,
													 m_ball.getPosition().x,
													 m_ball.getPosition().y);

			std::cout << "Angle = " << (newAngle * (180 / M_PI)) << " degress.\n";

			// Ensure angle is not too steep; 
			// Too steep if: (60 < angle < 180) or (180 < angle < 300)
			if((newAngle > (M_PI / 3)) && (newAngle < M_PI))
			{
				newAngle = (M_PI / 3);
			}
			else if((newAngle > M_PI) && (newAngle < ((5 * M_PI) / 3)))
			{
				newAngle = ((5 * M_PI) / 3);
			}

			m_ball.SetMoveAngle(newAngle);
			m_ball.setPosition((m_leftPaddle->getPosition().x + (Paddle::WIDTH / 2) + (Ball::RADIUS / 2)), m_ball.getPosition().y);
		}
	}
	else if(m_ball.getPosition().x > (GameData::BOARD_WIDTH / 2))	// Check Right Side	
	{
		if((Calculator::CheckCollision(m_ball, *m_rightPaddle)) && (m_currentState.playerTwoHealth > 0))
		{
			std::cout << "Right Paddle Hit\n";

			m_currentState.playerTwoHealth -= 10;

			double newAngle = Calculator::GetAngleTo(m_rightPaddle->getPosition().x, 
													 m_rightPaddle->getPosition().y,
													 m_ball.getPosition().x,
													 m_ball.getPosition().y);

			std::cout << "Angle = " << (newAngle * (180 / M_PI)) << " degress.\n";

			// Ensure angle is not too steep; 
			// Too steep if: (angle < 120 ) or (angle > 240)
			if(newAngle < ((4 * M_PI) / 6))
			{
				newAngle = ((4 * M_PI) / 6);
			}
			else if(newAngle > ((4 * M_PI) / 3))
			{
				newAngle = ((4 * M_PI) / 3);
			}

			m_ball.SetMoveAngle(newAngle);
			m_ball.setPosition((m_rightPaddle->getPosition().x - (Paddle::WIDTH / 2) - (Ball::RADIUS / 2) - 16), m_ball.getPosition().y);
		}
	}

	// Check for Scores
	if(m_ball.getPosition().x <= GameData::LEFT_GOAL_LINE)
	{
		m_player2Scoring = true;
	}
	else if(m_ball.getPosition().x >= GameData::RIGHT_GOAL_LINE)
	{
		m_player1Scoring = true;
	}

	// Check for healthPack collision
	if(m_currentState.healthPackSpawned)
	{
		if(Calculator::CheckCollision(m_ball, m_healthPack))
		{
			// Check which paddle hit ball last
			if((m_ball.GetMoveAngle() >= ((3 * M_PI) / 2)) || (m_ball.GetMoveAngle() <= ((M_PI) / 2))) // Ball Moving to the Right
			{
				// Left Paddle hit last
				int healAmount = m_healthPack.GetHealAmount();
				m_currentState.playerOneHealth += healAmount;

				std::cout << "Player One healed for " << healAmount << std::endl;

				if(m_currentState.playerOneHealth > 100)
				{
					m_currentState.playerOneHealth = 100;
				}
			}
			else
			{
				// Right Paddle hit last
				int healAmount = m_healthPack.GetHealAmount();
				m_currentState.playerTwoHealth += healAmount;

				std::cout << "Player Two healed for " << healAmount << std::endl;

				if(m_currentState.playerTwoHealth > 100)
				{
					m_currentState.playerTwoHealth = 100;
				}
			}
		}
	}
}

// ===== UpdateState ==============================================================================
// Method will update the m_currentState member with the newest data.
//
// Input: none
// Output: none
// ================================================================================================
void ServerEngine::UpdateState(void)
{
	m_currentState.ballAngle = m_ball.GetMoveAngle();
	m_currentState.ballX = m_ball.getPosition().x;
	m_currentState.ballY = m_ball.getPosition().y;
}