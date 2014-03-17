// ================================================================================================
// Filename: "ServerStart.cpp"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// This file is where server execution will start. It will create an instance of the ServerEngine
// and start it.
// ================================================================================================

#include <iostream>
#include <Windows.h>
#include "SNetworkController.h"
#include "ServerEngine.h"

int main(int argc, char* argv[])
{
	bool usingArtificalLatency = true;
	bool waitingForPlayers = true;
	int numPlayers = 0;
	sf::Clock gameClock;

	// Introduction Output
	std::cout << "Pong Server\nClass: ICS 167\n\n";

	// Get y/n for artificial latency
	std::cout << "Using artificial latency? (Y/N) : Default = on\n";

	char latencyOption[256];
	fgets(latencyOption, 256, stdin);	// Get the next characters off input stream

	if((latencyOption[0] == 'n') || (latencyOption[0] == 'N'))	// If the first character on the input isn't 'n', latency is defaulted to on
	{
		usingArtificalLatency = false;
		std::cout << "Artificial latency is off.\n";
	}
	else
	{
		std::cout << "Artificial latency is on.\n";
	}

	SNetworkController networkControl(&gameClock, usingArtificalLatency);
	ServerEngine engine(&networkControl);

	while(!networkControl.IsReady())
	{
		Sleep(2500);
		std::wcout << "NWC is not Ready...\n";
	}

	std::cout << "Server Engine Starting...\n";
	engine.Run();
}