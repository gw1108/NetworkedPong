// ================================================================================================
// Filename: "Paddle.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// Class Description:
// 
// The Paddle class will extend an sf::RectangleShape and provide the functionality needed for a 
// rectangle to operate as a paddle within the context of the game.
// 
// The class will maintain its' own constants that dictate size and behavior.
// ================================================================================================

#ifndef PADDLE_H
#define PADDLE_H

#include <SFML\Graphics\RectangleShape.hpp>

class Paddle: public sf::RectangleShape
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor Prototype
	Paddle(int player);

	// Method Prototypes
	bool MoveUp(float speedMod);
	bool MoveDown(float speedMod);
	void SetHealth(int health);
	
	// Inlined Methods
	int GetHealth(void){ return m_health; }

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	enum Direction {UP, DOWN, STILL};

	// Constants
	static const float WIDTH;
	static const float HEIGHT;
	static const float BASE_MOVE_RATE;
	static const sf::Color Paddle::PLAYER1_COLOR;
	static const sf::Color Paddle::PLAYER2_COLOR;

private:

	// Health
	int m_health;

};

#endif