// ================================================================================================
// Filename: "Paddle.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// This is the class implementation file for the Paddle class. For a class description see the 
// header file "Paddle.h"
// ================================================================================================

#include "Paddle.h"
#include "GameData.h"

// Initialize Static Constants
const float Paddle::WIDTH = 16;
const float Paddle::HEIGHT = 128;
const float Paddle::BASE_MOVE_RATE = 5;
const sf::Color Paddle::PLAYER1_COLOR = sf::Color(0, 255, 0);
const sf::Color Paddle::PLAYER2_COLOR = sf::Color(0, 0, 255);

// ===== Constructor ==============================================================================
// The constructor will use class constants to define the size of the sf::RectangleShape parent.
// It will abstract away most of the set-up required for an sf::RectangleShape. The color and 
// starting position of the paddle will be dictated by which player is passed to the constructor.
// 
// Input:
//	[IN] int player	-	which player this paddle belongs to, 1 or 2
//
// Output: none
//  ===============================================================================================
Paddle::Paddle(int player)
{
	setSize(sf::Vector2f(WIDTH, HEIGHT));
	setOrigin(sf::Vector2f((WIDTH / 2), (HEIGHT / 2)));
	setFillColor(sf::Color(0, 0, 0));

	if(player == 1)
	{
		setOutlineColor(PLAYER1_COLOR);
		setPosition((float)(GameData::LEFT_GOAL_LINE + getOrigin().x), (float)(GameData::BOARD_HEIGHT / 2));
	}
	else if(player == 2)
	{
		setOutlineColor(PLAYER2_COLOR);
		setPosition((float)(GameData::RIGHT_GOAL_LINE - getOrigin().x), (float)(GameData::BOARD_HEIGHT / 2));
	}
	else
	{
		// Error Assigning Player
	}
}

// ===== MoveUp ===================================================================================
// The MoveUp method will adjust the position of the paddle using the parent classes methods.
// The method will return false if the move is not allowed.
//
// Input:
//	[IN]	int speedMod	-	the mod is used to speed up or slow down the opponents paddle. 
//								This is used by the clients to "catch up" to the server.
//
// Output:
//	[OUT]	boolean moveAllowed	- true if move is possible and position adjusted; false otherwise
// ================================================================================================
bool Paddle::MoveUp(float speedMod)
{
	if((getPosition().y - (Paddle::HEIGHT / 2)) > (speedMod * BASE_MOVE_RATE))
	{
		move(0, ((-1) * (speedMod * BASE_MOVE_RATE)));
		return true;
	}
	else
	{
		return false;
	}
}

// ===== MoveUp ===================================================================================
// The MoveUp method will adjust the position of the paddle using the parent classes methods.
// The method will return false if the move is not allowed.
//
// Input:
//	[IN]	int speedMod	-	the mod is used to speed up or slow down the opponents paddle. 
//								This is used by the clients to "catch up" to the server.
//
// Output:
//	[OUT]	boolean moveAllowed	- true if move is possible and position adjusted; false otherwise
// ================================================================================================
bool Paddle::MoveDown(float speedMod)
{
	if((getPosition().y + (Paddle::HEIGHT / 2)) < (GameData::BOARD_HEIGHT - (speedMod * BASE_MOVE_RATE)))
	{
		move(0, (speedMod * BASE_MOVE_RATE));
		return true;
	}
	else
	{
		return false;
	}
}

// ===== SetHealth ================================================================================
// The method will set the m_health member with the passed value. It will also modify the appearance
// of the paddle based on the amount of health remaining.
//
// Input:
//	[IN]	int health	-	the "health" of the paddle
//
// Output: none
// ================================================================================================
void Paddle::SetHealth(int health)
{
	m_health = health;

	if(m_health > 100)
	{
		m_health = 100;
	}

	if(m_health <= 0)
	{
		setOutlineThickness(0);
	}
	else if(m_health <= 25)
	{
		setOutlineThickness((-1) * (Paddle::WIDTH / 16));
	}
	else if(m_health <= 50)
	{
		setOutlineThickness((-1) * (Paddle::WIDTH / 6));
	}
	else if(m_health <= 75)
	{
		setOutlineThickness((-1) * (Paddle::WIDTH / 3));
	}
	else if(m_health >= 100)
	{
		setOutlineThickness((-1) * (Paddle::WIDTH / 2));
	}
}