// ================================================================================================
// Filename: "PaddleUpdate.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Mar 06, 2014
// ================================================================================================
// Class Description:
// 
// This class is used to package the data needed for paddle updates that are sent to the server.
// ================================================================================================

#ifndef PADDLEUPDATE_H
#define PADDLEUPDATE_H

#include <SFML\Network\Packet.hpp>

class PaddleUpdate
{

public:

	// Constructors
	PaddleUpdate(void);
	PaddleUpdate(const PaddleUpdate &original);

	// Data Members
	float m_paddleLoc;
	int m_paddleDir;
	int m_timestamp;
};

// Packet Overloads for GameData
sf::Packet& operator<<(sf::Packet& packet, PaddleUpdate& update);
sf::Packet& operator>>(sf::Packet& packet, PaddleUpdate& update);


#endif