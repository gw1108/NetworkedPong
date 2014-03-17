// ================================================================================================
// Filename: "Ball.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// This is the class implementation file for the Ball class. For a class description see the 
// header file "Ball.h"
// ================================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Ball.h"
#include "GameData.h"

// Initialize Static Constants
const float Ball::RADIUS = 16;
const float Ball::BASE_MOVE_RATE = 3;
const sf::Color Ball::BALL_COLOR = sf::Color(255, 255, 255);

// ===== Constructor ==============================================================================
// The constructor will use class constants to define the radius and color of the sf::CircleShape 
// parent. It will abstract away most of the set-up required for an sf::CircleShape.
// 
// Input: none
// Output: none
//  ===============================================================================================
Ball::Ball(void)
{
	setRadius(RADIUS);
	setOrigin(sf::Vector2f((RADIUS / 2), (RADIUS / 2)));
	setFillColor(BALL_COLOR);
	setPosition((float)(GameData::BOARD_WIDTH / 2), (float)(GameData::BOARD_HEIGHT / 2));
	m_moveAngle = (0);
}

// ===== Move =====================================================================================
// The Move method will adjust the balls position based on its' MOVE_RATE and m_moveAngle. The Ball
// will handle collisions with the walls of the game board, but will not handle paddle collisions 
// or scores. After the ball moves, the engine will need to manually check if the ball has collided
// with a paddle and adjust it accordingly. If the ball reaches the goal and a score takes place,
// the ball will need to be manually reset. The ball will continue to bounce around inside the
// game board until the engine changes, or resets, its angle and position.
// 
// Input: 
//	[IN] int speedMode	-	the mod is used to speedUp or slowDown the ball, this is used by clients
//							to "catch up" to the server if they are too far behind.
// Output: none
// ================================================================================================
void Ball::Move(float speedMod)
{
	// Get X and Y Moves
	float xMove = (float)((speedMod * BASE_MOVE_RATE) * cos(m_moveAngle));
	float yMove = ((-1) * (float)((speedMod * BASE_MOVE_RATE) * sin(m_moveAngle)));	// Reverse yMove because y-plane grows downward

	// Calculate the new position
	float newX = (getPosition().x + xMove);	// Center of Ball
	float newY = (getPosition().y + yMove);	// Center of Ball

	// Check for board collisions
	if((newY - RADIUS) <= 0)	// Top Collision
	{
		m_rebounded = true;

		if(m_moveAngle < (M_PI / 2))	// Angle in quadrant 1
		{
			m_moveAngle = (m_moveAngle - (2 * m_moveAngle) + (2 * M_PI));
			newY = RADIUS;
		}
		else	// Angle in quadrant 2
		{
			// Find Reference Angle
			double refAngle = (M_PI - m_moveAngle);
			m_moveAngle = (m_moveAngle + (2 * refAngle));
			newY = RADIUS;
		}
	}
	else if((newY + RADIUS) >= GameData::BOARD_HEIGHT)	// Bottom Collision
	{
		m_rebounded = true;

		if(m_moveAngle < ((3 * M_PI) / 2))	// Angle in quadrant 3
		{
			// Find Reference Angle
			double refAngle = (m_moveAngle - M_PI);
			m_moveAngle = (m_moveAngle - (2 * refAngle));
			newY = (GameData::BOARD_HEIGHT - RADIUS);
		}
		else	// Angle in quadrant 4
		{
			// Find Reference Angle
			double refAngle = ((2* M_PI) - m_moveAngle);
			m_moveAngle = (m_moveAngle + (2 * refAngle) - (2 * M_PI));
			newY = (GameData::BOARD_HEIGHT - RADIUS);
		}
	}
	else if((newX - RADIUS) <= 0)	// Left Collision
	{
		m_rebounded = true;

		if(m_moveAngle < M_PI)	// Angle in quadrant 2
		{
			// Find Reference Angle
			double refAngle = (M_PI - m_moveAngle);
			m_moveAngle = refAngle;
			newX = RADIUS;
		}
		else	// Angle in quardrant 3
		{
			// Find Reference Angle
			double refAngle = (m_moveAngle - M_PI);
			m_moveAngle = ((2 * M_PI) - refAngle);
			newX = RADIUS;
		}
	}
	else if((newX + RADIUS) >= GameData::BOARD_WIDTH)	// Right Collision
	{
		m_rebounded = true;

		if(m_moveAngle < (M_PI / 2))	// Angle in quadrant 1
		{
			m_moveAngle = (M_PI - m_moveAngle);
			newX = (GameData::BOARD_WIDTH - RADIUS);
		}
		else	// Angle in quardrant 4
		{
			// Find Reference Angle
			double refAngle = ((2 * M_PI) - m_moveAngle);
			m_moveAngle = (M_PI + refAngle);
			newX = (GameData::BOARD_WIDTH - RADIUS);
		}
	}

	setPosition(newX, newY);
}

// ===== Serve ====================================================================================
// Serve will place the ball in the center of the game board and set its' m_moveAngle to the 
// non-scoring player's side or randomly chosen if starting a new game.
//
// Input: 
//		[IN] int scoringPlayer - the player who just scored, or 0 if new game
//
// Output: none
// ================================================================================================
void Ball::Serve(int scoringPlayer)
{
	setPosition((float)(GameData::BOARD_WIDTH / 2), (float)(GameData::BOARD_HEIGHT / 2));

	if(scoringPlayer == 1)
	{
		m_moveAngle = 0;
	}
	else if(scoringPlayer == 2)
	{
		m_moveAngle = M_PI;
	}
	else
	{
		// Seed RNG and get a random number
		srand((unsigned int)(time(0)));
		int angleInDegrees = rand();

		// Set random number between 0 and 359
		angleInDegrees = (angleInDegrees % 360);

		// Ensure angle is not too steep; 
		// Too steep if: (60 < angle < 120) or (240 < angle < 300)

		if((angleInDegrees > 60) && (angleInDegrees < 120))
		{
			angleInDegrees = 60;
		}
		else if((angleInDegrees > 240) && (angleInDegrees < 300))
		{
			angleInDegrees = 240;
		}

		// Convert to Radians
		m_moveAngle = ((double)angleInDegrees * (M_PI / 180));
	}
}
