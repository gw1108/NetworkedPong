// ================================================================================================
// File Name: "Calculator.cpp"
// Author: Smith, Travis
// Last Modified: Feb 27, 2014
// ================================================================================================
// The is the implementation file for the Calculator class. For class details see the header file
// "Calculator.h"
// ================================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <SFML/Graphics/Rect.hpp>
#include "Calculator.h"

// ==== Calculator::GetAngelTo ====================================================================
// This method is used to find the angle from the origin to the destination points passed to
// the method. The vertical math may appear in reverse because the GUI y-values increase as 
// you move "down" on the screen. The angle returned will be from 0 to 2PI. When calculating the new
// angle a ball should be moving, the paddle should be considered the origin and the ball the 
// destination.
//
//	Input:
//		[IN] double originX	- the starting point's X coordinate
//		[IN] double originY	- the starting point's Y coordinate
//		[IN] double destX	- the ending point's X coordinate
//		[IN] double destY	- the ending point's Y coordinate
//
//	Output:
//		[OUT] double angle	- the angle between the two points, in radians
//
// ================================================================================================
double Calculator::GetAngleTo(double originX, double originY, double destX, double destY)
{
	double deltaY;
	double deltaX;
		
	double angle;
		
	// Check for 90 and 270
	if(originX == destX)
	{
		deltaY = destY - originY;
			
		if(deltaY < 0)
		{
			return (M_PI / 2);
		}
		else
		{
			return ((3 * M_PI) / 2);
		}
	}
	
	// Check for 0 and 180
	if(originY == destY)
	{
		deltaX = destX - originX;
			
		if(deltaX >= 0)
		{
			return 0;
		}
		else
		{
			return M_PI;
		}
	}
		
	deltaY = -(destY - originY);
	deltaX = destX - originX;
		
	angle = (atan2(deltaY, deltaX));		
		
	if(angle < 0)
	{
		angle += (2 * (M_PI));
	}
		
	return angle;
}
	
// ==== Calculator::GetDistanceTo =================================================================
// This method is used to find the distance between two points
//
//	Input:
//		[IN] double firstX	- the first point's X coordinate
//		[IN] double firstY	- the first point's Y coordinate
//		[IN] double secondX	- the second point's X coordinate
//		[IN] double secondY	- the second point's Y coordinate
//
//	Output:
//		[OUT] double totalDist	- the distance between the points
//
// ================================================================================================
double Calculator::GetDistanceTo(double firstX, double firstY, double secondX, double secondY)
{
	double xDist = (secondX - firstX);
	xDist *= xDist;
		
	double yDist = (secondY - firstY);
	yDist *= yDist;
		
	double totalDist = sqrt(xDist + yDist);
		
	return totalDist;
}

// ==== Calculator::CheckCollision ================================================================
// Method should be called to check if the game ball and a paddle have collided or not. The method
// will use the bounding rectangles of both objects and determine if they intersect one another.
// If the two objects intersect, the method will return true.
//
//	Input:
//		[IN] Ball ball		- the game ball
//		[IN] Paddle paddle	- the paddle to be checked
//
//	Output:
//		[OUT] bool result	- true if the objects have collided, false otherwise
//
// ================================================================================================
bool Calculator::CheckCollision(Ball ball, Paddle paddle)
{
	// Get Bounding Rectangles
	sf::Rect<float> ballBounds = ball.getGlobalBounds();
	sf::Rect<float> paddleBounds = paddle.getGlobalBounds();

	// Check for Intersection
	bool result = ballBounds.intersects(paddleBounds);

	return result;
}

// ==== Calculator::CheckCollision ================================================================
// Method should be called to check if the game ball and a healthPack have collided or not. The 
// method will use the bounding rectangles of both objects and determine if they intersect one another.
// If the two objects intersect, the method will return true.
//
//	Input:
//		[IN] Ball ball		- the game ball
//		[IN] HealthPack		- the health pack
//
//	Output:
//		[OUT] bool result	- true if the objects have collided, false otherwise
//
// ================================================================================================
bool Calculator::CheckCollision(Ball ball, HealthPack hPack)
{
	// Get Bounding Rectangles
	sf::Rect<float> ballBounds = ball.getGlobalBounds();
	sf::Rect<float> packBounds = hPack.getGlobalBounds();

	// Check for Intersection
	bool result = ballBounds.intersects(packBounds);

	return result;
}