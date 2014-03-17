// ================================================================================================
// Filename: "TimeSync.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// Class Description:
// 
// The TimeSync instance is used when a client first connects to the server. The server will send
// a TimeSync request to the client, which will "stamp" the appropriate times in the class and then
// send it back. These times will allow the server to calculate the difference between the server
// and client clocks as well as estimate the latency.
//
// To increase simplicity, the TimeSync class will make all members public.
// ================================================================================================

#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <SFML\Network\Packet.hpp>

class TimeSync
{

public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	TimeSync(void);
	~TimeSync(void){}

	// ============================================================================================
	// Class Data Members
	// ============================================================================================
	
	int serverSendTime;
	int clientRecieveTime;
	int clientSendTime;
	int serverReceivedTime;

private:

};

// Packet Overloads for TimeSync
sf::Packet& operator<<(sf::Packet& packet, TimeSync& timeSync);
sf::Packet& operator>>(sf::Packet& packet, TimeSync& timeSync);

#endif