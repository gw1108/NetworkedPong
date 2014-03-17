// ================================================================================================
// Filename: "HealthPack.cpp"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Mar 11, 2014
// ================================================================================================
// This is the class implementation file for the HealthPack class. For a class description see the 
// header file "HealthPack.h"
// ================================================================================================

#include <random>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include "HealthPack.h"
#include "GameData.h"

// Initialize Static Constants
const int HealthPack::SPAWN_RATE = (7500 / GameData::ENGINE_SPEED);	// First Number is the number of ms to wait until spawn
const float HealthPack::SIDE_DIMENSION = 20;
const sf::Color HealthPack::KIT_COLOR = sf::Color(255, 0, 0);

// ===== Constructor ==============================================================================
// The constructor will use class constants to define the dimensions and color of the rectangle 
// parent. It will abstract away most of the set-up required for an sf::RectangleShape. It will also
// zero out its' class members.
// 
// Input: none
// Output: none
//  ===============================================================================================
HealthPack::HealthPack(void)
{
	setSize(sf::Vector2f(SIDE_DIMENSION, SIDE_DIMENSION));
	setOrigin(sf::Vector2f((SIDE_DIMENSION / 2), (SIDE_DIMENSION / 2)));
	setFillColor(KIT_COLOR);

	setPosition((float)(GameData::BOARD_WIDTH / 2), (float)(GameData::BOARD_HEIGHT / 2));
	
	m_isSpawned = false;
	m_loopsSinceLastHeal = 0;
}

// ===== Update ===================================================================================
// Update will increment the loop counter and set the spawn boolean to true if enough time has 
// passed.
// 
// Input: none
// Output: none
// ================================================================================================
void HealthPack::Update(void)
{
	if(!m_isSpawned)
	{
		m_loopsSinceLastHeal++;

		if(m_loopsSinceLastHeal >= SPAWN_RATE)
		{
			m_isSpawned = true;
		}
	}
}

// ===== GetHealAmount ============================================================================
// This method will randomly generate an integer that will represent the amount of health restored
// when this kit is used. This method will also set m_isSpawned to false, and reset the counter.
//
// Input: none
//
// Output:
//	[OUT] int healAmount	- the amount of health to restore
// ================================================================================================
int HealthPack::GetHealAmount(void)
{
	int healAmount;

	srand((unsigned int)(time(0)));

	healAmount = ((rand() % 16) + 10);	// Generate Latency Between 10 and 25
		
	m_isSpawned = false;
	m_loopsSinceLastHeal = 0;

	return healAmount;
}
