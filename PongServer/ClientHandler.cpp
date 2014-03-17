// ================================================================================================
// Filename: "ClientHandler.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 25, 2014
// ================================================================================================
// This is the class implementation file for the ClientHandler class. For a class description
// see the header file "ClientHandler.h"
// ================================================================================================

#include <SFML\Network\Packet.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <random>
#include <Windows.h>
#include "ClientHandler.h"

// ===== Constructor ==============================================================================
// The constructor will receive a pointer to a connected sf::TcpSocket and set up a thread to listen
// for data coming from the socket. Once the listening thread is established, the ClientHandler will
// send a request for a clock sync from the client. 
//
// The Constructor will receive a a const pointer to the engines game clock. This clock will be used
// to syncronize the server with the client.
// 
// Input:
//	[IN] int clientNumber				- which player this client represents
//	[IN] sf::TcpSocket connectedSocket	- the connect TcpClient
//	[IN] sf::Clock *gameClock			- the game engines clock
//
// Output: none
//
//  ===============================================================================================
ClientHandler::ClientHandler(int clientNumber, sf::TcpSocket *connectedSocket,
							 const sf::Clock *gameClock, bool usingArtLatency) : m_gameClock(gameClock)
{
	// Initialize Values
	m_clientNumber = clientNumber;
	m_clientSocket = connectedSocket;
	m_syncState = 0;
	m_ready = false;

	if(usingArtLatency)
	{
		srand((unsigned int)(time(0)));
		m_artificialLatency = ((rand() % 300) + 200);	// Generate Latency Between 200 and 499
														// This is the One-Way Latency
		
		std::cout << "Random Latency = " << m_artificialLatency << "ms\n";
	}
	else
	{
		m_artificialLatency = 0;

		std::cout << "Random Latency = " << m_artificialLatency << "ms\n";
	}

	// Start thread
	m_connected = true;
	m_clientListeningThread = new std::thread(&ClientHandler::SocketListening, this);
	m_outThread = new std::thread(&ClientHandler::SendOutThread, this);

	SendInitialize();
	SendTimeSyncRequest();
}

// ===== Destructor ===============================================================================
// The destructor will ensure all dynamically allocated memory is released.
// ================================================================================================
ClientHandler::~ClientHandler()
{
	StopListeningThread();
	StopOutgoingThread();
	delete m_clientListeningThread;
	delete m_outThread;
	delete m_clientSocket;
}

// ===== StopListeningThread ======================================================================
// Method will terminate the client listening thread.
//
// Input: none
// Output: none
// ================================================================================================
void ClientHandler::StopListeningThread()
{
	if (m_clientListeningThread != NULL)
	{
		m_connected = false;				// Set Thread-Loop conditional to false
		m_clientListeningThread->join();	// Wait for thread to end
	}
}

// ===== StopOutgoingThread =======================================================================
// Method will terminate the outgoing thread.
//
// Input: none
// Output: none
// ================================================================================================
void ClientHandler::StopOutgoingThread()
{
	if (m_outThread != NULL)
	{
		m_connected = false;	// Set Thread-Loop conditional to false
		m_outThread->join();	// Wait for thread to end
	}
}

// ===== SendGameState ============================================================================
// This method will send the current game state to its' connected client.
//
// Input: 
//	[IN] GameState currentState	- a data structure containing the current game state
//
// Output: none
// ================================================================================================
void ClientHandler::SendGameData(GameData currentState)
{
	sf::Packet dataPacket;
	sf::Uint8 cmdCode = GameData::GAME_UPDATE;

	dataPacket << cmdCode << currentState;

	SendPacket(dataPacket);
}

// ===== SendInitialize ===========================================================================
// Will send the player number to the client.
//
// Input: none
// Output: none
// ================================================================================================
void ClientHandler::SendInitialize(void)
{
	sf::Packet initPacket;
	sf::Uint8 cmdCode = GameData::INITIALIZE;
	sf::Uint8 playerNumber = m_clientNumber;
	sf::Uint32 artLatency = m_artificialLatency;

	std::cout<< "Sending Init Packet\n";

	initPacket << cmdCode << playerNumber << artLatency;

	SendPacket(initPacket);
}

// ===== SocketListening ==========================================================================
// This method will loop while waiting for data from the client. When the client sends its' updated
// paddle data, the method will extract the data from the packet, apply the appropriate masks to the
// 16 bit integer contained in the packet, and assign those to the paddleData array.
//
// The first 2-bits will be used for the direction the paddle is moving, and the last 14-bits will
// be used for the current position of the paddle.
//
// If artifical latency is turned on. The SocketListening thread will wait m_artificalLatency time
// after receiving a packet.
//
// Input: none
// Output: none
// ================================================================================================
void ClientHandler::SocketListening(void)
{
	sf::TcpSocket::Status receiveStatus;
	sf::Packet receivedPacket;
	sf::Uint8 commandCode;
	TimeSync timeSync;

	while (m_connected)
	{
		receiveStatus = m_clientSocket->receive(receivedPacket); // Blocking

		if(receiveStatus == sf::TcpSocket::Done)
		{
			// Extract Command Code
			receivedPacket >> commandCode;

			if(commandCode == GameData::TIME_SYNC)
			{
				receivedPacket >> timeSync;
				timeSync.serverReceivedTime = m_gameClock->getElapsedTime().asMilliseconds();
				TimeSyncReceived(timeSync);
			}
			else if(commandCode == GameData::PADDLE_UPDATE)
			{
				PaddleUpdate update;

				// Extract Paddle and Time Data
				receivedPacket >> update;

				// Assign Data to Class Members
				m_dataLock.lock();

				m_lastUpdate = update;

				m_dataLock.unlock();
			}
			else
			{
				std::cout << "Error with Command Code\n";
			}
		}
		else if(receiveStatus == sf::TcpSocket::Disconnected)
		{
			std::cout << "Client Disconnected.\n";
			m_connected = false;
		}
		else
		{
			std::cout << "Error Receiving Last Packet.\n";
		}
	}
}

// ===== SendTimeSyncRequest ======================================================================
// This method will be called when the server sends a clock sync command. The method will create
// a packet containing time stamps that will allow the server to adjust for latency.
//
// Input:none
// Output:none
// ================================================================================================
void ClientHandler::SendTimeSyncRequest(void)
{
	TimeSync syncRequest;
	sf::Packet syncPacket;
	sf::Uint8 cmdCode = GameData::TIME_SYNC;

	syncRequest.serverSendTime = m_gameClock->getElapsedTime().asMilliseconds();
	syncPacket << cmdCode << syncRequest;

	std::cout << "Sending Time Sync\n";
	SendPacket(syncPacket);
}

// ===== TimeSyncReceived =========================================================================
// This method will operate like a simple state machine and will change behavior based on how 
// many TimeSyncs the server has received. For each state, it will average the clock differences
// and latency estimations for the client to produce a more accurate measurement.
//
// Input:
//	[IN] TimeSync timeSync	- the received timeSync
//
// Output: none
// ================================================================================================
void ClientHandler::TimeSyncReceived(TimeSync timeSync)
{
	if(m_syncState == 0) // First Sync Returned
	{
		std::cout << "\nTime Sync " << (m_syncState + 1) << " Recieved from Client\n";

		int clientProcessTime = (timeSync.clientSendTime - timeSync.clientRecieveTime);
		std::cout << "Client Processing Time: " << clientProcessTime << "ms\n";
		int roundTripTime = (timeSync.serverReceivedTime - timeSync.serverSendTime - clientProcessTime);
		std::cout << "Round Trip Time: " << roundTripTime << "ms\n";
		int sendTimeDiff = (timeSync.serverSendTime + (roundTripTime / 2) - timeSync.clientRecieveTime);
		std::cout << "Send Time Diff: " << sendTimeDiff << "ms\n";
		int recTimeDiff = (timeSync.serverReceivedTime - (roundTripTime / 2) - timeSync.clientSendTime);
		std::cout << "Receive Time Diff: " << recTimeDiff << "ms\n";

		m_clientLatency = (roundTripTime / 2);
		std::cout << "Client Latency: " << m_clientLatency << "ms\n";
		m_clientClockDifference = ((sendTimeDiff + recTimeDiff) / 2);
		std::cout << "Client Clock Difference: " << m_clientClockDifference << "ms\n";

		m_syncState++;

		SendTimeSyncRequest();
	}
	else // Additional Syncs Returned
	{
		std::cout << "\nTime Sync " << (m_syncState + 1) << " Recieved from Client\n";

		int clientProcessTime = (timeSync.clientSendTime - timeSync.clientRecieveTime);
		std::cout << "Client Processing Time: " << clientProcessTime << "ms\n";
		int roundTripTime = (timeSync.serverReceivedTime - timeSync.serverSendTime - clientProcessTime);
		std::cout << "Round Trip Time: " << roundTripTime << "ms\n";
		int sendTimeDiff = (timeSync.serverSendTime + (roundTripTime / 2) - timeSync.clientRecieveTime);
		std::cout << "Send Time Diff: " << sendTimeDiff << "ms\n";
		int recTimeDiff = (timeSync.serverReceivedTime - (roundTripTime / 2) - timeSync.clientSendTime);
		std::cout << "Receive Time Diff: " << recTimeDiff << "ms\n";

		m_clientLatency = ((m_clientLatency + (roundTripTime / 2)) / 2);
		std::cout << "Client Latency: " << m_clientLatency << "ms\n";
		m_clientClockDifference = ((m_clientClockDifference + ((sendTimeDiff + recTimeDiff) / 2)) / 2);
		std::cout << "Client Clock Difference: " << m_clientClockDifference << "ms\n";

		m_syncState++;

		if(m_syncState < 3) // Send 3 Time Syncs
		{
			SendTimeSyncRequest();
		}
		else
		{
			std::cout << "Client Ready...\n";
			m_ready = true;
		}
	}
}

// ===== GetPaddleData ============================================================================
// Method will return the last paddle update received from the client.
//
// Input: none
//
// Output:
//	[OUT]	PaddleUpdate m_lastUpdate	- last update received from client
// ================================================================================================
PaddleUpdate ClientHandler::GetPaddleData(void)
{
	PaddleUpdate temp;

	m_dataLock.lock();

	temp = m_lastUpdate;

	m_dataLock.unlock();

	return temp;
}

// ===== Disconnect ===============================================================================
// This method will terminate the listening thread and disconnect the m_clientSocket.
//
// Input: none
// Output: none
// ================================================================================================
void ClientHandler::Disconnect(void)
{
	StopListeningThread();
	m_clientSocket->disconnect();
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
void ClientHandler::SendPacket(sf::Packet packet)
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
void ClientHandler::SendOutThread(void)
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
				m_clientSocket->send(tempPacket);
			}

			m_outLock.unlock();
		}
	}
}