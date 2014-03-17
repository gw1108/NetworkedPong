// ================================================================================================
// Filename: "HealthPack.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Mar 11, 2014
// ================================================================================================
// Class Description:
// 
// This class is used to heal player's paddles during play. The HealthPack will spawn periodically
// in the middle of the game board and when the ball collides with it, the last player ot hit the 
// ball will receive a heal. 
//
// The HealthPack class will extend an sf::RectangleShape. The HealthPack will maintain a counter
// so that it knows when to spawn.
// 
// The class will maintain its' own constants that dictate heal amount and behavior.
// ================================================================================================

#ifndef HEALTHPACK_H
#define HEALTHPACK_H

#include <SFML\Graphics\RectangleShape.hpp>

class HealthPack: public sf::RectangleShape
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor Prototype
	HealthPack(void);

	// Method Prototypes
	void Update(void);
	int GetHealAmount(void);

	// Inlined Methods
	bool Spawned(void){ return m_isSpawned; }

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Constants
	static const int SPAWN_RATE;
	static const float SIDE_DIMENSION;
	static const sf::Color KIT_COLOR;

private:

	int m_loopsSinceLastHeal;
	bool m_isSpawned;

};

#endif