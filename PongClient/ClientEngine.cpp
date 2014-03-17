// ================================================================================================
// Filename: "ClientEngine.cpp"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// This is the implementation file for the ClientEngine class. For class description see the header
// file "ClientEngine.h"
// ================================================================================================

#define _USE_MATH_DEFINES
#include <sstream>
#include <iostream>
#include <math.h>
#include "ClientEngine.h"
#include "Calculator.h"

// Initialize Constants
const int ClientEngine::SHORT_THRESHOLD = 5;
const int ClientEngine::MEDIUM_THRESHOLD = 10;
const int ClientEngine::FAST_THRESHOLD = 20;
const float ClientEngine::MEDIUM_SPEED_MOD = 1.5;
const float ClientEngine::FAST_SPEED_MOD = 2;

// ===== Constructor ==============================================================================
// The constructor will set-up the GUI and initialize all game values for the client. It will start
// the engine and begin rendering and checking for data from server.
//
// Input: none
// Output: none
//  ===============================================================================================
ClientEngine::ClientEngine(CNetworkController *networkControl)
{
	// Initialize Engine Members
	m_networkControl = networkControl;
	m_engineClock.restart();
	m_running = false;

	// Initialize Game Pieces
	m_mainWindow = new sf::RenderWindow(sf::VideoMode(GameData::BOARD_WIDTH, GameData::BOARD_HEIGHT), "Pong Client", sf::Style::Close | sf::Style::Titlebar);

	m_leftPaddle = new Paddle(1);
	m_rightPaddle = new Paddle(2);

	if(m_networkControl->GetPlayerNumber() == 1)
	{
		m_clientPaddle = m_leftPaddle;
		m_opponentPaddle = m_rightPaddle;
	}
	else
	{
		m_clientPaddle = m_rightPaddle;
		m_opponentPaddle = m_leftPaddle;
	}

	m_oppMoveDirection = Paddle::STILL;
	m_oppSpeedMod = 1;

	// Initialize Window Members
	if(!m_gameFont.loadFromFile("arial.ttf"))
	{
		std::cout << "Font Error\n";
		m_mainWindow->close();
	}

	m_player1ScoreText.setFont(m_gameFont);
	m_player1ScoreText.setCharacterSize(48);
	m_player2ScoreText.setFont(m_gameFont);
	m_player2ScoreText.setCharacterSize(48);

	m_player1Score = 0;
	m_player2Score = 0;

	m_player1ScoreText.setColor(sf::Color(255, 255, 255));
	m_player2ScoreText.setColor(sf::Color(255, 255, 255));

	m_player1ScoreText.setOrigin((m_player1ScoreText.getLocalBounds().width / 2), (m_player1ScoreText.getLocalBounds().height / 2));
	m_player2ScoreText.setOrigin((m_player2ScoreText.getLocalBounds().width / 2), (m_player2ScoreText.getLocalBounds().height / 2));

	m_player1ScoreText.setPosition(((float)(GameData::BOARD_WIDTH / 2) - 150), 25);
	m_player2ScoreText.setPosition(((float)(GameData::BOARD_WIDTH / 2) + 150), 25);

	UpdateScoreText();
}

// ===== Destructor ===============================================================================
// The destructor will ensure all dynamically allocated memory is released.
// ================================================================================================
ClientEngine::~ClientEngine(void)
{
	delete m_leftPaddle;
	delete m_rightPaddle;
	delete m_mainWindow;
}

// ===== Run ======================================================================================
// This method will be the "infinite" loop that starts when the engine starts. Every iteration it
// will check for data from server, update the game state based on server data, respond to user
// input, render the main window, and send a paddle update to the server.
//
// Input: none
// Output: none
// ================================================================================================
void ClientEngine::Run(void)
{
	bool wPressed = false;
	bool sPressed = false;
	int paddleDirection;

	m_running = true;

	GameData startData = m_networkControl->GetNextData();
	m_gameBall.setPosition(startData.ballX, startData.ballY);
	m_gameBall.SetMoveAngle(startData.ballAngle);

	while(m_running)
	{
		// Limit Engine to ~60 Cycles a Second
		if(m_engineClock.getElapsedTime().asMilliseconds() <= GameData::ENGINE_SPEED)
		{
			Render();
			continue;
		}
		else
		{
			m_engineClock.restart();

			// Check DataQueue
			if(!m_networkControl->DataQueueEmpty())
			{
				m_gameState = m_networkControl->GetNextData();
				UpdateGameData();
			}
			else
			{
				m_gameBall.Move(1);

				// Interpolate The Opponents Paddle
				if(m_networkControl->GetPlayerNumber() == 1)
				{
					InterpolateOpponent(m_gameState.playerTwoPaddlePosition, m_gameState.playerTwoDirection);
				}
				else
				{
					InterpolateOpponent(m_gameState.playerOnePaddlePosition, m_gameState.playerOneDirection);
				}
			}

			// Get Latest User Input
			sf::Event event;
			while(m_mainWindow->pollEvent(event))
			{
				if(event.type == sf::Event::Closed)
				{
					m_mainWindow->close();
				}
				else if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::W))
				{
					wPressed = true;
				}
				else if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S))
				{
					sPressed = true;
				}
				else if((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::W))
				{
					wPressed = false;
				}
				else if((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::S))
				{
					sPressed = false;
				}
			} 

			// Adjust Client Paddle
			if((wPressed) && (!sPressed))
			{
				m_clientPaddle->MoveUp(1);
				paddleDirection = Paddle::UP;
			}
			else if((sPressed) && (!wPressed))
			{
				m_clientPaddle->MoveDown(1);
				paddleDirection = Paddle::DOWN;
			}
			else
			{
				paddleDirection = Paddle::STILL;
			}

			CheckPaddleCollisions();

			// Render The Game Screen
			Render();

			// Send Paddle Update
			m_networkControl->SendPaddleUpdate(m_clientPaddle->getPosition().y, paddleDirection);

		}
	}
}

// ===== UpdateGameData ===========================================================================
// This method take a GameData structure that was received by the server and use it to update all
// gameState variables on the client. It will call the InterpolateXXXXX() methods for the opponent 
// paddle and ball positions. It will also check for scores, and winning conditions.
//
// Input: none
// Output: none
// ================================================================================================
void ClientEngine::UpdateGameData()
{
	// Update Paddle Health
	if(m_leftPaddle->GetHealth() != m_gameState.playerOneHealth)
	{
		std::cout << "Player 1 Health change, new health = " << m_gameState.playerOneHealth << std::endl;
		m_leftPaddle->SetHealth(m_gameState.playerOneHealth);
	}

	if(m_rightPaddle->GetHealth() != m_gameState.playerTwoHealth)
	{
		std::cout << "Player 2 Health change, new health = " << m_gameState.playerTwoHealth << std::endl;
		m_rightPaddle->SetHealth(m_gameState.playerTwoHealth);
	}

	// Interpolate Ball Position
	InterpolateBall(m_gameState.ballX, m_gameState.ballY, m_gameState.ballAngle);

	// Interpolate The Opponents Paddle
	if(m_networkControl->GetPlayerNumber() == 1)
	{
		InterpolateOpponent(m_gameState.playerTwoPaddlePosition, m_gameState.playerTwoDirection);
	}
	else
	{
		InterpolateOpponent(m_gameState.playerOnePaddlePosition, m_gameState.playerOneDirection);
	}

	// Check for Score
	if(m_player1Score != m_gameState.playerOneScore)
	{
		m_player1Score = m_gameState.playerOneScore;
		UpdateScoreText();

		// Ball needs to be reset, not interpolated after a score.
		m_gameBall.setPosition(m_gameState.ballX, m_gameState.ballY);
		m_gameBall.SetMoveAngle(m_gameState.ballAngle);
	}
	if(m_player2Score != m_gameState.playerTwoScore)
	{
		m_player2Score = m_gameState.playerTwoScore;
		UpdateScoreText();

		// Ball needs to be reset, not interpolated after a score.
		m_gameBall.setPosition(m_gameState.ballX, m_gameState.ballY);
		m_gameBall.SetMoveAngle(m_gameState.ballAngle);
	}

	// Check for Win
	if(m_gameState.gameWon)
	{
		if(m_gameState.winningPlayer == 1)
		{
			std::cout << "Player 1 Won the Game!\n";
			m_running = false;
		}
		else
		{
			std::cout << "Player 2 Won the Game!\n";
			m_running = false;
		}
	}
}

// ===== UpdateScoreText ==========================================================================
// Method will update the sf::Text members with the latest scores.
//
// Input: none
// Output: none
// ================================================================================================
void ClientEngine::UpdateScoreText(void)
{
	std::stringstream p1String;
	std::stringstream p2String;

	p1String << m_player1Score;
	p2String << m_player2Score;

	m_player1ScoreText.setString(p1String.str());
	m_player2ScoreText.setString(p2String.str());
}

// ===== InterpolateBall ==========================================================================
// Method will adjust ball position from the current clients view to the latest server view. This
// method is resonsible for making sure that the ball moves smoothly instead of "teleporting" around.
// The method will be responsible for adjusting the angle of the balls travel, its' position, and 
// setting the speed mod if needed.
//
// Input:
//	[IN] float ballX		-	the server's ball's x-coordinate
//	[IN] float ballY		-	the server's ball's y-coordinate
//	[IN] double ballAngle	-	the server's ball's angle of travel
//
// Output: none
// ================================================================================================
void ClientEngine::InterpolateBall(float ballX, float ballY, double ballAngle)
{
	if(m_gameBall.GetMoveAngle() == ballAngle)
	{
		double distance = Calculator::GetDistanceTo(m_gameBall.getPosition().x, m_gameBall.getPosition().y, ballX, ballY);

		if(distance < MEDIUM_THRESHOLD)
		{
			m_gameBall.Move(1);
		}
		else if(distance < FAST_THRESHOLD)
		{
			m_gameBall.Move(MEDIUM_SPEED_MOD);
		}
		else
		{
			m_gameBall.Move(FAST_SPEED_MOD);
		}
	}
	else
	{
		m_gameBall.setPosition(ballX, ballY);
		m_gameBall.SetMoveAngle(ballAngle);
	}
}

// ===== CheckPaddleCollisions ====================================================================
// Method will check if they ball has impacted a paddle and adjust its' angle accordingly
//
// Input: none
// Output: none
// ================================================================================================
void ClientEngine::CheckPaddleCollisions(void)
{
		if(m_gameBall.getPosition().x < (GameData::BOARD_WIDTH / 2))		// Check Left Side
	{
		if((Calculator::CheckCollision(m_gameBall, *m_leftPaddle)) && (m_gameState.playerOneHealth > 0))
		{
			std::cout << "Left Paddle Hit\n";

			double newAngle = Calculator::GetAngleTo(m_leftPaddle->getPosition().x, 
													 m_leftPaddle->getPosition().y,
													 m_gameBall.getPosition().x,
													 m_gameBall.getPosition().y);

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

			m_gameBall.SetMoveAngle(newAngle);
			m_gameBall.setPosition((m_leftPaddle->getPosition().x + (Paddle::WIDTH / 2) + (Ball::RADIUS / 2)), m_gameBall.getPosition().y);
		}
	}
	else if(m_gameBall.getPosition().x > (GameData::BOARD_WIDTH / 2))	// Check Right Side	
	{
		if((Calculator::CheckCollision(m_gameBall, *m_rightPaddle)) && (m_gameState.playerTwoHealth > 0))
		{
			std::cout << "Right Paddle Hit\n";

			double newAngle = Calculator::GetAngleTo(m_rightPaddle->getPosition().x, 
													 m_rightPaddle->getPosition().y,
													 m_gameBall.getPosition().x,
													 m_gameBall.getPosition().y);

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

			m_gameBall.SetMoveAngle(newAngle);
			m_gameBall.setPosition((m_rightPaddle->getPosition().x - (Paddle::WIDTH / 2) - (Ball::RADIUS / 2) - 16), m_gameBall.getPosition().y);
		}
	}

}

// ===== InterpolateOpponent ======================================================================
// Method will adjust the opponents paddle position. This method is resonsible for making sure that 
// the paddle moves smoothly instead of "teleporting" around. The method will be responsible for 
// adjusting the paddle's position, and setting the speed mod if needed.
//
// Input:
//	[IN] float paddleLoc	-	the opponent paddle's y-coordinate
//	[IN] int paddleDir		-	the opponent paddle's direction of movement
//
// Output: none
// ================================================================================================
void ClientEngine::InterpolateOpponent(float paddleLoc, int paddleDir)
{
	if(paddleLoc > m_opponentPaddle->getPosition().y)	// Paddle is too high
	{
		double distance = paddleLoc - m_opponentPaddle->getPosition().y;

		if(distance < SHORT_THRESHOLD)
		{
			m_opponentPaddle->setPosition(m_opponentPaddle->getPosition().x, paddleLoc);
		}
		else if(distance < MEDIUM_THRESHOLD)
		{
			m_opponentPaddle->MoveDown(1);
		}
		else if(distance < FAST_THRESHOLD)
		{
			m_opponentPaddle->MoveDown(MEDIUM_SPEED_MOD);
		}
		else
		{
			m_opponentPaddle->MoveDown(FAST_SPEED_MOD);
		}
	}
	else if(paddleLoc < m_opponentPaddle->getPosition().y)	// Paddle is too low
	{
		double distance = m_opponentPaddle->getPosition().y - paddleLoc;

		if(distance < SHORT_THRESHOLD)
		{
			m_opponentPaddle->setPosition(m_opponentPaddle->getPosition().x, paddleLoc);
		}
		else if(distance < MEDIUM_THRESHOLD)
		{
			m_opponentPaddle->MoveUp(1);
		}
		else if(distance < FAST_THRESHOLD)
		{
			m_opponentPaddle->MoveUp(MEDIUM_SPEED_MOD);
		}
		else
		{
			m_opponentPaddle->MoveUp(FAST_SPEED_MOD);
		}
	}


	m_oppMoveDirection = paddleDir;
}

// ===== Render ===================================================================================
// Method will render the main window, the ball, scores, and paddles
//
// Input: none
// Output: none
// ================================================================================================
void ClientEngine::Render(void)
{
	// Clear Screen
	m_mainWindow->clear(sf::Color(0, 0, 0));

	// Draw Pieces
	m_mainWindow->draw(m_player1ScoreText);
	m_mainWindow->draw(m_player2ScoreText);
	m_mainWindow->draw(*m_leftPaddle);
	m_mainWindow->draw(*m_rightPaddle);

	if(m_gameState.healthPackSpawned)
	{
		m_mainWindow->draw(m_healthPack);
	}

	m_mainWindow->draw(m_gameBall);

	// Display Screen
	m_mainWindow->display();
}