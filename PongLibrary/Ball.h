// ================================================================================================
// Filename: "Ball.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// Class Description:
// 
// The Ball class will extend an sf::CircleShape and provide the functionality needed for a circle
// to operate as a ball within the context of the game.
// 
// The class will maintain its' own constants that dictate size and behavior.
// ================================================================================================

#ifndef BALL_H
#define BALL_H

#include <SFML\Graphics\CircleShape.hpp>

class Ball: public sf::CircleShape
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor Prototype
	Ball(void);

	// Method Prototypes
	void Move(float speedMod);
	void Serve(int scoringPlayer);

	// Inlined Methods
	double GetMoveAngle(void){ return m_moveAngle; }
	void SetMoveAngle(double angle){ m_moveAngle = angle; }
	bool HasRebounded(void){ return m_rebounded; }
	void SetRebound(bool state) { m_rebounded = state; }

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Constants
	static const float RADIUS;
	static const float BASE_MOVE_RATE;
	static const sf::Color BALL_COLOR;

private:

	double m_moveAngle;
	bool m_rebounded;

};

#endif