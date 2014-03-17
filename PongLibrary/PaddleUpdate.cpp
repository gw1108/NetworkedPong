// ================================================================================================
// Filename: "PaddleUpdate.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Mar 06, 2014
// ================================================================================================
// This is the class implementation file for the PaddleUpdate class. For a class description see the 
// header file "PaddleUpdate.h"
// ================================================================================================

#include "PaddleUpdate.h"
#include "Paddle.h"

// ===== Default Constructor ======================================================================
// The constructor will use zero all data members.
// 
// Input: none
// Output: none
//  ===============================================================================================
PaddleUpdate::PaddleUpdate(void)
{
	m_paddleLoc = 0;
	m_paddleDir = Paddle::STILL;
	m_timestamp = 0;
}

// ===== Copy Constructor =========================================================================
// The constructor will copy all members directly.
// 
// Input: none
// Output: none
//  ===============================================================================================
PaddleUpdate::PaddleUpdate(const PaddleUpdate &original)
{
	m_paddleLoc = original.m_paddleLoc;
	m_paddleDir = original.m_paddleDir;
	m_timestamp = original.m_timestamp;
}

// ===== Packet Input Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the PaddleUpdate 
// class can be input into a packet for delivery. 
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet to receive the update
//	[IN]	PaddleUpdate& update	- a reference to the update
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator<<(sf::Packet& packet, PaddleUpdate& update)
{
	packet	<< update.m_paddleLoc << update.m_paddleDir << update.m_timestamp;

	return packet;
}

// ===== Packet Output Overload ====================================================================
// This method overloads the stream operator used with the sf::Packet class so that the PaddleUpdate 
// class can be extracted from a packet. 
//
// Input:
//	[IN/OUT] sf::Packet& packet		- the packet that contains the update
//	[IN]	PaddleUpdate& update	- a reference to the update that will receive the data
//
// Output:
//	[OUT] sf::Packet& packet		- will return the passed packet reference
// ================================================================================================
sf::Packet& operator>>(sf::Packet& packet, PaddleUpdate& update)
{
	packet	>> update.m_paddleLoc >> update.m_paddleDir >> update.m_timestamp;

	return packet;
}