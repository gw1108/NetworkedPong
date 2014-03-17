// ================================================================================================
// Filename: "CNetworkController.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 19, 2014
// ================================================================================================
// This is the class implementation file for the CNetworkController class. For a class description
// see the header file "CNetworkController.h"
// ================================================================================================

#include <iostream>
#include <SFML\Network\Packet.hpp>
#include <Windows.h>
#include "TimeSync.h"
#include "CNetworkController.h"
#include "PaddleUpdate.h"

const int CNetworkController::MAX_QUEUE = 1;

// ===== Constructor ==============================================================================
// The constructor will attempt to establish a connection to the passed IP and port number. If
// successful, it will start the listening thread and set m_connected to true. If the connection
// fails, it will not start the thread and will set m_connected to false.
// 
// Input:
//	[IN] sf::ipAddress ipAddress	-	the IP address of the server
//	[IN] int portNumber				-	the port number that the server is listening to
//
// Output: none
//
//  ===============================================================================================
CNetworkController::CNetworkController(sf::IpAddress ipAddress, int portNumber, const sf::Clock *gameClock) : m_gameClock(gameClock)
{
	// Attempt to establish connection
	if (m_serverConnection.connect(ipAddress, portNumber) != sf::TcpSocket::Done)
	{
		// Error Starting Connection
		m_connected = false;
		m_startGame = false;
		m_listeningThread = NULL;
		m_outThread = NULL;
	}
	else
	{
		// Connection established; Start thread
		m_connected = true;
		m_startGame = false;
		m_listeningThread = new std::thread(&CNetworkController::SocketListening, this);
		m_outThread = new std::thread(&CNetworkController::SendOutThread, this);
		m_artificialLatency = 0;
	}
}

// ===== Destructor ===============================================================================
// The destructor will ensure all dynamically allocated memory is released.
// ================================================================================================
CNetworkController::~CNetworkController()
{
	StopThread();
	StopOutgoingThread();
	delete m_listeningThread;
	delete m_outThread;
}

// ===== StopThread ===============================================================================
// Method will terminate the socket listening thread.
//
// Input: none
// Output: none
// ================================================================================================
void CNetworkController::StopThread()
{
	if (m_listeningThread != NULL)
	{
		m_connected = false;		// Set Thread-Loop conditional to false
		m_listeningThread->join();	// Wait for thread to end
	}
}

// ===== StopOutgoingThread =======================================================================
// Method will terminate the outgoing thread.
//
// Input: none
// Output: none
// ================================================================================================
void CNetworkController::StopOutgoingThread()
{
	if (m_outThread != NULL)
	{
		m_connected = false;	// Set Thread-Loop conditional to false
		m_outThread->join();	// Wait for thread to end
	}
}

// ===== SendPaddleUpdate =========================================================================
// This method will take the clients paddle information and package it along with a timestamp. Once
// packaged, the method will send the packet through the m_serverConnection.
//
// Input: 
//	[IN] double yLocation	- the current Y-Coordinate of the top of the paddle
//	[IN] int direction		- an int representing the direction the paddle is currently moving
//
// Output: none
// ================================================================================================
void CNetworkController::SendPaddleUpdate(float yLocation, int direction)
{
	sf::Uint8 commandCode = GameData::PADDLE_UPDATE;

	PaddleUpdate update;
	update.m_paddleLoc = yLocation;
	update.m_paddleDir = direction;
	update.m_timestamp = m_gameClock->getElapsedTime().asMilliseconds();

	sf::Packet dataPacket;

	dataPacket << commandCode << update;

	SendPacket(dataPacket);
}

// ===== SocketListening ==========================================================================
// This method will loop while connected to the server. When a packet is received, the method
// will check the command code and handle the data accordingly. The only two command codes that
// should be received by a client, are GAME_UPDATE and TIME_SYNC.
//
// The SocketListening thread will wait m_artificialLatency when it receives a packet.
//
// Input: none
// Output: none
// ================================================================================================
void CNetworkController::SocketListening(void)
{
	sf::TcpSocket::Status receiveStatus;
	sf::Packet receivedPacket;
	sf::Uint8 cmdCode;

	while (m_connected)
	{
		receiveStatus = m_serverConnection.receive(receivedPacket);	// Blocking

		if(receiveStatus == sf::TcpSocket::Done)
		{
			receivedPacket >> cmdCode;

			if(cmdCode == GameData::INITIALIZE)
			{
				std::cout << "Received Init Packet\n";
				sf::Uint8 player;
				sf::Uint32 latency;

				receivedPacket >> player >> latency;

				m_playerNum = player;
				m_artificialLatency = latency;

				std::cout << "Assigned as Player: " << m_playerNum << " Latency set at: " << m_artificialLatency << "ms\n";
			}
			else if(cmdCode == GameData::GAME_UPDATE)
			{
				GameData newData;
				receivedPacket >> newData;

				if(!m_startGame)
				{
					m_startGame = newData.startGame;	// Check for GameStart Command
				}

				m_dataLock.lock();		// Lock Data

				if(m_dataQueue.size() > MAX_QUEUE)
				{
					m_dataQueue.pop_front();
				}

				m_dataQueue.push_back(newData);

				m_dataLock.unlock();	// Unlock Data
			}
			else if(cmdCode == GameData::TIME_SYNC)
			{
				std::cout << "Time Sync Request Received.\n";
				TimeSync syncRequest;

				receivedPacket >> syncRequest;

				syncRequest.clientRecieveTime = m_gameClock->getElapsedTime().asMilliseconds();
				SendTimeSync(syncRequest);
			}
			else
			{
				std::cout << "Error with Command Code...\n";
			}
		}
		else if(receiveStatus == sf::TcpSocket::Disconnected)
		{
			std::cout <<"Disconnected from Server.\n";
			m_connected = false;
		}
		else
		{
			std::cout <<"Error Receiving Last Packet.\n";
		}
	}
}

// ===== GetNextData ==============================================================================
// The method will return the most recent GameData update received by the server. It will be 
// controlled by a mutex lock to ensure is doesn't grab data while the listening thread is writing
// new data.
// 
// Input: none
//
// Output:
//	[OUT] GameData m_newData	-	the latest GameData update from the server
// ================================================================================================
GameData CNetworkController::GetNextData(void)
{
	m_dataLock.lock();		// Lock Data

	GameData data = m_dataQueue.front();
	m_dataQueue.pop_front();

	m_dataLock.unlock();	// Unlock Data

	return data;
}

// ===== SendTimeSync =============================================================================
// This method will be called when the server sends a clock sync command. The method will create
// a packet containing time stamps that will allow the server to adjust for latency.
//
// Input:none
// Output: none
// ================================================================================================
void CNetworkController::SendTimeSync(TimeSync serverRequest)
{
	sf::Packet syncPacket;
	sf::Uint8 cmdCode = GameData::TIME_SYNC;

	serverRequest.clientSendTime = m_gameClock->getElapsedTime().asMilliseconds();

	syncPacket << cmdCode << serverRequest;

	std::cout << "Sending Time Sync\n";
	SendPacket(syncPacket);
}

// ===== SendPacket ===============================================================================
// This method will place the packet into the outPacket list so that it can be sent to the server
// by the outThread.
//
// Input: 
//	[IN] sf::Packet packet - the packet to be sent
//
// Output: none
// ================================================================================================
void CNetworkController::SendPacket(sf::Packet packet)
{
	m_outLock.lock();

	m_outPackets.push_back(packet);

	m_outLock.unlock();
}

// ===== SendOutThread ============================================================================
// This method is used to simulate artificial latency between the client and server. It will wait
// m_artificialLatency time, before sending out all packets in the outPackets list. This method will
// be the starting place for the m_outThread member and should only ever be called by that thread.
//
// Input: none
// Output: none
// ================================================================================================
void CNetworkController::SendOutThread(void)
{
	sf::Packet tempPacket;
	sf::Clock lagClock;

	while(m_connected)
	{
		if(lagClock.getElapsedTime().asMilliseconds() < m_artificialLatency)
		{
			continue;
		}
		else
		{
			lagClock.restart();

			m_outLock.lock();

			while(!m_outPackets.empty())
			{
				tempPacket = m_outPackets.front();
				m_outPackets.pop_front();
				m_serverConnection.send(tempPacket);
			}

			m_outLock.unlock();
		}
	}
}



