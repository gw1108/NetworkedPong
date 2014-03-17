// ================================================================================================
// Filename: "ClientHandler.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 25, 2014
// ================================================================================================
// Class Description:
// 
// The ClientHandler class will be used to interact with a specific client. It will be responsible 
// for receiving the Client's paddle data and sending the Server's game state to the client. Upon
// creation, the ClientHandler will establish a clock sync with its' client as well as calculate
// the latency. The ClientHandler will maintain the most recent position and direction of its'
// assigned clients paddle along with the timestamp of when it was received.
//
// The ClientHandler class makes all of its' members and methods private but allows the 
// NetworkController class access by declaring it a friend. This is so that no other client code
// directly utilizes the ClientHandler class. All usage of this class should be done in an instance
// of the NetworkController class.
// ================================================================================================

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <Thread>
#include <string>
#include <mutex>
#include <list>
#include <SFML\Network\TcpSocket.hpp>
#include <SFML\System\Clock.hpp>
#include "GameData.h"
#include "TimeSync.h"
#include "PaddleUpdate.h"

class ClientHandler
{

public:

	// The NetworkController class is the only class that should be able to
	// interact with a ClientHandler
	friend class SNetworkController;

private:

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	ClientHandler(int clientNumber, sf::TcpSocket *connectedSocket, const sf::Clock *gameClock, bool usingArtLatency);
	~ClientHandler(void);

	// Method Prototypes
	void SendGameData(GameData currentState);
	void SendInitialize(void);
	void SocketListening(void);
	void SendPacket(sf::Packet packet);
	void StopListeningThread(void);
	void StopOutgoingThread(void);
	void SendTimeSyncRequest(void);
	void TimeSyncReceived(TimeSync timeSync);
	PaddleUpdate GetPaddleData(void);
	void Disconnect(void);
	void SendOutThread(void);

	// Inlined Methods
	bool IsConnected(void){ return m_connected; }
	bool IsReady(void){ return m_ready; }
	int GetClockDifference(void){ return m_clientClockDifference; }

	// ============================================================================================
	// Class Data Members
	// ============================================================================================

	// Game Data 
	const sf::Clock *m_gameClock;

	// Client Data
	int m_clientNumber;
	sf::TcpSocket *m_clientSocket;
	int m_clientClockDifference;
	int m_clientLatency;
	int m_syncState;
	int m_artificialLatency;
	bool m_ready;

	// Paddle Data
	std::mutex m_dataLock;
	PaddleUpdate m_lastUpdate;

	// Thread Data
	std::thread *m_clientListeningThread;
	bool m_connected;

	// Outgoing Data
	std::thread *m_outThread;
	std::mutex m_outLock;
	std::list<sf::Packet> m_outPackets;
};

#endif