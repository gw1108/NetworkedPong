// ================================================================================================
// Filename: "SNetworkController.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 25, 2014
// ================================================================================================
// Class Description:
// 
// The SNetworkController will be responsible for setting up the initial listener, accepting client
// connection requests, receive client data, and transmitting game state data to the clients.
//
// The SNetworkController will abstract the individual client connection away from the game engine.
// all requests to send/receive data should be sent through the SNetworkController.
// ================================================================================================

#ifndef SNETWORKCONTROLLER_H
#define SNETWORKCONTROLLER_H

#include <Thread>
#include <string>
#include <SFML\Network\TcpListener.hpp>
#include "ClientHandler.h"
#include "GameData.h"
#include "PaddleUpdate.h"

class SNetworkController
{

public:

	// ============================================================================================
	// Constant Class Members
	// ============================================================================================
	static const int LISTENING_PORT;
	static const int MAX_PLAYERS;

	// ============================================================================================
	// Methods
	// ============================================================================================

	// Constructor/Destructor Prototypes
	SNetworkController(const sf::Clock *gameClock, bool usingArtLatency);
	~SNetworkController(void);

	// Method Prototypes
	void ListenForClients(void);
	void SendGameData(GameData currentState);
	void Disconnect(void);
	void StopListeningThread(void);
	bool GetPlayerPaddleData(int player, PaddleUpdate &engineUpdate);
	bool IsReady();
	bool ClientsConnected(void);

private:

	// ============================================================================================
	// Methods
	// ============================================================================================

	void SocketListening(void);

	// ============================================================================================
	// Members
	// ============================================================================================

	// Game Data
	const sf::Clock *m_gameClock;

	// Connection Request Listening Data Members
	std::thread *m_listeningThread;
	sf::TcpListener m_listener;
	int m_numPlayers;
	bool m_ready;
	bool m_usingArtLatency;

	// Client Handlers
	ClientHandler *m_player1;
	ClientHandler *m_player2;
};

#endif