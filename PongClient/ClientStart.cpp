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
	char nextChar;
	int nextInt;

	int firstNum = 0;
	int secondNum = 0;
	int thirdNum = 0;
	int fourthNum = 0;

	fgets(tempIP, 256, stdin);	// Get the next characters off input stream

	for (int index = 0; index < 15; index++)
	{
		if ((index % 4) == 0) // First Number In IP Address Section
		{
			nextChar = tempIP[index];

			// Check for LH command
			if((nextChar == 'l') || (nextChar == 'L'))
			{
				nextChar = tempIP[index + 1];

				if((nextChar == 'h') || (nextChar == 'H'))
				{
					std::cout << "Local Host Specified\n"; 
					ipString = PONG_SERVER_LOCAL_HOST;
					return true;
				}
				else
				{
					// IP not Valid
					std::cout << "IP Format Error.\n"; 
					return false;
				}
			}



			nextInt = nextChar - '0';	// Get integer Value

			if ((nextInt >= 0) && (nextInt <= 2))	// Must Be between 0 and 2
			{
				if (index == 0)
				{
					firstNum += (100 * nextInt);
				}
				else if (index == 4)
				{
					secondNum += (100 * nextInt);
				}
				else if (index == 8)
				{
					thirdNum += (100 * nextInt);
				}
				else if (index == 12)
				{
					fourthNum += (100 * nextInt);
				}
			}
			else
			{
				std::cout << "IP-Address Format Error at index: " << index << " Value Entered: " << nextChar << std::endl;
				return false;
			}
		}
		else if ((index % 4) == 1)	// Second Number In IP Address Section
		{
			nextChar = tempIP[index];
			nextInt = nextChar - '0';	// Get integer Value
	
			if ((nextInt >= 0) && (nextInt <= 9))	// Must Be between 0 and 9
			{
				if (index == 1)
				{
					firstNum += (10 * nextInt);
				}
				else if (index == 5)
				{
					secondNum += (10 * nextInt);
				}
				else if (index == 9)
				{
					thirdNum += (10 * nextInt);
				}
				else if (index == 13)
				{
					fourthNum += (10 * nextInt);
				}
			}
			else
			{
				std::cout << "IP-Address Format Error at index: " << index << " Value Entered: " << nextChar << std::endl;
				return false;
			}
		}
		else if ((index % 4) == 2) // Third Number In IP Address Section
		{
			nextChar = tempIP[index];
			nextInt = nextChar - '0';	// Get integer Value

			if ((nextInt >= 0) && (nextInt <= 9))	// Must Be between 0 and 9
			{
				if (index == 2)
				{
					firstNum += (1 * nextInt);
				}
				else if (index == 6)
				{
					secondNum += (1 * nextInt);
				}
				else if (index == 10)
				{
					thirdNum += (1 * nextInt);
				}
				else if (index == 14)
				{
					fourthNum += (1 * nextInt);

					std::cout << "IP is Valid\nFirst: " << firstNum << " Second: " << secondNum << " Third: "
						<< thirdNum << " Fourth: " << fourthNum << std::endl;

					// Assign IP
					std::stringstream stream;
					stream << firstNum << '.' << secondNum << '.' << thirdNum << '.' << fourthNum;
					ipString = stream.str();
					std::cout << ipString << std::endl;
					return true;
				}
			}
			else
			{
				std::cout << "IP-Address Format Error at index: " << index << " Value Entered: " << nextChar << std::endl;
				return false;
			}
		}
		else if ((index == 3) || (index == 7) || (index == 11)) // Dot Seperator
		{
			nextChar = tempIP[index];

			if (nextChar != '.')
			{
				std::cout << "Error at index: " << index << " Expected: '.' Found: " << nextChar << std::endl;
				return false;
			}
		}
	}

	return false;
}
