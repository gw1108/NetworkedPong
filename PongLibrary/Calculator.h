// ================================================================================================
// Filename: "Calculator.h"
// ================================================================================================
// Author: Travis Smith
// Last Modified: Feb 27, 2014
// ================================================================================================
// Class Description:
// 
// The Calculator class contains some static methods that can be used to calculate some values
// needed by the engine. Its' constructor will be private so that an instance of Calculator can 
// not be created.
// ================================================================================================

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "Ball.h"
#include "Paddle.h"
#include "HealthPack.h"

class Calculator
{
public:

	// ============================================================================================
	// Methods
	// ============================================================================================

	static double GetAngleTo(double originX, double originY, double destX, double destY);
	static double GetDistanceTo(double firstX, double firstY, double secondX, double secondY);
	static bool Calculator::CheckCollision(Ball ball, Paddle paddle);
	static bool Calculator::CheckCollision(Ball ball, HealthPack hPack);
	
private:

	// Constructor
	Calculator(void);

};

#endif