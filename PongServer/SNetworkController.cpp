// ================================================================================================
// Filename: "SNetworkController.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 25, 2014
// ================================================================================================
// This is the class implementation file for the SNetworkController class. For a class description
// see the header file "SNetworkController.h"
// ================================================================================================

#include <iostream>
#include "SNetworkController.h"

// Initialize Static Constants
const int SNetworkController::LISTENING_PORT = 8585;
const int SNetworkController::MAX_PLAYERS = 2;

// ===== Constructor ==============================================================================
// The constructor set up the listener that waits for clients to connect to the server.
//
// The Constructor will receive a a const pointer to the engines game clock. This clock will be used
// to syncronize the server with the client.
// 
// Input:
//	[IN] sf::Clock *gameClock			- the game engines clock
//
// Output: none
//
//  ===============================================================================================
SNetworkController::SNetworkController(const sf::Clock *gameClock, bool usingArtLatency) : m_gameClock(gameClock)
{
	// Initialize Data Members
	m_ready = false;
	m_numPlayers = 0;
	m_usingArtLatency = usingArtLatency;

	m_player1 = NULL;
	m_player2 = NULL;

	// Start Listening Thread
	m_listeningThread = new std::thread(&SNetworkController::ListenForClients, this);
}

// ===== Destructor ===============================================================================
// The destructor will ensure all dynamically allocated memory is released.
// ================================================================================================
SNetworkController::~SNetworkController(void)
{
	StopListeningThread();
	delete m_listeningThread;
	delete m_player1;
	delete m_player2;
}

// ===== ListenForClients =========================================================================
// This is the method that the listening thread will continously loop through until two clients 
// have connected to the server. This method and the thread running it will assign ClientHandlers
// to any clients requesting a connection. Once two players have connected, the method will set 
// the m_ready member to true and the thread will be allowed to terminate.
// 
// Input: none
// Output: none
//  ===============================================================================================
void SNetworkController::ListenForClients(void)
{
	sf::TcpSocket *tempClient = new sf::TcpSocket;


	// Set-Up the port listener
	if(m_listener.listen(LISTENING_PORT) != sf::Socket::Done)
	{
		std::cout << "Error establishing listener.\n";
	}
	else
	{
		std::cout << "Listener established on port.\n";
	}

	// Loop until MAX_PLAYERS reached
	while(!m_ready)
	{
		std::cout << "Waiting For Clients...\n";

		if(m_listener.accept(*tempClient) == sf::Socket::Done)	// BLOCKING
		{
			// Client Successfully Connected
			if(m_numPlayers == 0)
			{
				std::cout << "Player one has joined the game.\n";

				// Create Client Handler for Player 1
				m_player1 = new ClientHandler(1, tempClient, m_gameClock, m_usingArtLatency);

				// Create a new TcpSocket for the next player; Client Handler will release the memory
				tempClient = new sf::TcpSocket;
			}
			else if(m_numPlayers == 1)
			{
				std::cout << "Player Two has joined the game. Starting Game.\n";

				// Create Client Handler for Player 2
				m_player2 = new ClientHandler(2, tempClient, m_gameClock, m_usingArtLatency);

				tempClient = NULL;

				m_ready = true;
			}

			m_numPlayers++;
		}
	}

}

// ===== SendGameState ============================================================================
// Method will pass the current state to both client handlers so they can send it to their respective
// clients.
//
// Input:
//	[IN] GameState currentState	- the current game state
//
// Output: none
// ================================================================================================
void SNetworkController::SendGameData(GameData currentState)
{
	m_player1->SendGameData(currentState);
	m_player2->SendGameData(currentState);
}

// ===== Disconnect ===============================================================================
// Method will tell each clientHandler to disconnect from their clients.
//
// Input: none
// Output: none
// ================================================================================================
void SNetworkController::Disconnect(void)
{
	m_player1->Disconnect();
	m_player2->Disconnect();
}

// ===== StopThread ===============================================================================
// Method will terminate the listening thread.
//
// Input: none
// Output: none
// ================================================================================================
void SNetworkController::StopListeningThread()
{
	if (m_listeningThread != NULL)
	{
		m_ready = true;				// Set Thread-Loop conditional to false
		m_listeningThread->join();	// Wait for thread to end
	}
}

// ===== GetPlayerPaddleData ======================================================================
// The method will call the appropriate client handlers GetPaddleData method and place the data
// into the reference. If the client handlers are NULL then the method will return false.
//
// Input:
//	[IN] int player						- the player to whom the data belongs
//	[IN/OUT] PaddleUpdate &engineUpdate	- the reference to store the data in
//
// Output
//	[OUT] bool							- true if new data was stored, false otherwise
// ================================================================================================
bool SNetworkController::GetPlayerPaddleData(int player, PaddleUpdate &engineUpdate)
{
	PaddleUpdate temp;

	if(player == 1)
	{
		if(m_player1 != NULL)
		{
			temp = m_player1->GetPaddleData();
		}
		else
		{
			std::cout << "Returning False.\n";
			return false;
		}
	}
	else if(player == 2)
	{
		if(m_player2 != NULL)
		{
			temp = m_player2->GetPaddleData();
		}
		else
		{
			std::cout << "Returning False.\n";
			return false;
		}
	}
	else
	{
		std::cout << "Returning False.\n";
		return false;
	}

	engineUpdate.m_paddleLoc = temp.m_paddleLoc;
	engineUpdate.m_paddleDir = temp.m_paddleDir;
	engineUpdate.m_timestamp = temp.m_timestamp;

	return true;
}

// ===== IsReady ==================================================================================
// Will check if both clientHandlers are ready.
//
// Input:none
//
// Output:
//	[OUT] bool		- true if both handlers are ready, false otherwise
//
// ================================================================================================
bool SNetworkController::IsReady(void)
{
	if((m_player1 == NULL) || (m_player2 == NULL))
	{
		return false;
	}
	else
	{
		if(m_player1->IsReady() && m_player2->IsReady())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

// ===== ClientsConnected ==================================================================================
// Will check if both clients are still connected to the server.
//
// Input:none
//
// Output:
//	[OUT] bool		- true if both clients are connected, false otherwise
//
// ================================================================================================
bool SNetworkController::ClientsConnected(void)
{
	if(m_player1->IsConnected() && m_player2->IsConnected())
	{
		return true;
	}
	else
	{
		return false;
	}
}