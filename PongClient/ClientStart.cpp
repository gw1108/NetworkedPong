// ================================================================================================
// Filename: "ClientStart.cpp"
// ================================================================================================
// Primary Author: Travis Smith
// Last Modified: Mar 04, 2014
// ================================================================================================
// This file is where client execution will start. It will get the server IP from the user, attempt
// a connection and if successful, will prepare the client for play.
// ================================================================================================

#include <Windows.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Paddle.h"
#include "CNetworkController.h"
#include "ClientEngine.h"

// Game Constants
const int PONG_SERVER_PORT = 8585;
const std::string PONG_SERVER_LOCAL_HOST = "127.0.0.1"; 

// Function Prototypes
bool CheckIP(std::string &ipString);
unsigned GetNumberOfDigits (unsigned i);

int main(int argc, char* argv[])
{
	// Introduction Output
	std::cout << "Pong Client\nClass: ICS 167\n";

	// State Booleans
	bool ipValid = false;
	bool initialized = false;

	// Connection Data
	std::string ipString;
	sf::IpAddress serverIP;
	sf::Clock gameClock;

	while (!ipValid)
	{
		std::cout << "\nPlease enter the IP address of the PongServer (xxx.xxx.xxx.xxx)\nIncluding zeros; xxx.67.xxx.xxx should be written as xxx.067.xxx.xxx\nEnter 'LH' for local host\n\nIP: ";

		if(CheckIP(ipString))
		{
			ipValid = true;
			serverIP = ipString;
		}
	}

	// Attempt Connection to Server
	CNetworkController serverConnection(serverIP, PONG_SERVER_PORT, &gameClock);

	if(serverConnection.IsConnected())
	{
		std::cout << "Connection Established with Server.\nWaiting for initialization data from server...\n";

		while(!serverConnection.ShouldStartGame())
		{
			Sleep(1000);
		}

		std::cout << "Client Engine Starting...\n";
		ClientEngine engine(&serverConnection);
		engine.Run();
	}
	else
	{
		std::cout << "Error attempting Server Connection.\n";
		getchar();
		return 1;
	}

	getchar();
}

// ===== CheckIP ==================================================================================
// This method checks that the entered IP is within range and has the proper IP format. If the 
// entered address is not within range or formatted correctly, it will return false. It is important
// to realize that the method attempts to grab the first 256 characters on the input stream, but only
// checks the first 15. This is to "flush" the input buffer, in case the user enters too many 
// character which will cause the check to be called again immediately. If the entered IP address is
// valid, or the user enteres "LH", the method will assign the ip as a string the to passed string
// paramter.
//
// Input: 
//	[IN/OUT] std::string &ipString	-	a reference to a string that will be used to hold the ipAddress
//
// Output: 
//	[OUT] bool						-	true if IP is valid, false otherwise
// ================================================================================================
bool CheckIP(std::string &ipString)
{
	char tempIP[256];
	std::string delimiter = ".";

	fgets(tempIP, 256, stdin);	// Get the next characters off input stream

	std::string command = std::string(tempIP);
	//remove \n
	command.pop_back();
	//check if it's now empty
	if(command.empty())
	{
		std::cout << "Error Parsing : empty string detected";
		return false;
	}
	
	if(_stricmp(command.c_str(), "LH") == 0)
	{
		std::cout << "Local Host Specified\n";
		ipString = PONG_SERVER_LOCAL_HOST;
		return true;
	}

	//check if everything is a number or the delimeter
	if(command.find_first_not_of("0123456789"+delimiter) != std::string::npos)
	{
		std::cout << "Error Parsing : invalid character " + command.find_first_not_of("0123456789"+delimiter);
		return false;
	}

	size_t pos;
	int value;
	int numberOfDelimeters = 0;
	while(command.length() != 0 && numberOfDelimeters < 4){
		if((pos = command.find(delimiter)) == std::string::npos)
		{
			pos = command.length();
		}
		//check if s.substr(0,pos); is a int
		value = atoi(command.substr(0,pos).c_str());
		if(GetNumberOfDigits(value) > 3)
		{
			std::cout << "Error Parsing : 4 or more numbers detected between '.'";
			return false;
		}
		ipString.append(std::to_string(value));
		if(pos != command.length())
		{
			ipString.append(".");
		}
		command.erase(0, pos + delimiter.length());
		numberOfDelimeters++;
	}
	if(numberOfDelimeters > 3)
	{
		std::cout << "Error Parsing : 4 or more '.' detected";
		return false;
	}

	std::cout << "IP is Valid : " << ipString << std::endl;
	return true;
}

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}