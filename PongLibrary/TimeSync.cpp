// ================================================================================================
// Filename: "TimeSync.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// This is the class implementation file for the TimeSync class. For a class description see the 
// header file "TimeSync.h"
// ================================================================================================

#include "TimeSync.h"

// ===== Default Constructor ======================================================================
// The default constructor will be used to zero all values prior to the TimeSync being used.
//
// Input: none
// Output: none
// ================================================================================================
TimeSync::TimeSync(void)
{
	serverSendTime = 0;
	clientRecieveTime = 0;
	clientSendTime = 0;	
	serverReceivedTime = 0;
}

// ===== Packet Input Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the TimeSync 
// class can be input into a packet for delivery to a client or the server. 
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet to receive the TimeSync
//	[IN]	TimeSync& timeSync		- a reference to the TimeSync
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator<<(sf::Packet& packet, TimeSync& timeSync)
{
	packet	<< timeSync.serverSendTime << timeSync.clientRecieveTime << timeSync.clientSendTime
			<< timeSync.serverReceivedTime;

	return packet;
}

// ===== Packet Output Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the TimeSync 
// class can be extracted from a packet by a client or the server.
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet that contains the TimeSync
//	[IN]	TimeSync& timeSync		- a reference to the TimeSync that will receive the data
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator>>(sf::Packet& packet, TimeSync& timeSync)
{
	packet	>> timeSync.serverSendTime >> timeSync.clientRecieveTime >> timeSync.clientSendTime
			>> timeSync.serverReceivedTime;

	return packet;
}