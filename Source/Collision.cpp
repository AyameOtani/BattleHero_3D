#include "Collision.h"

bool Collision::CheckCircleToCircle(
	const VECTOR& centerPosA,
	const float& radiusA,
	const VECTOR& centerPosB,
	const float& radiusB)
{
	// “–‚½‚è”»’è‚ğ•`‚­

	// X²‚Ì‹——£i‰¡üj
	float distanceX = centerPosA.x - centerPosB.x;
	// Y²‚Ì‹——£icüj
	float distanceY = centerPosA.y - centerPosB.y;


	 // ”»’è
	float powX = distanceX * distanceX; // ‰¡ü‚Ì“ñæ
	float powY = distanceY * distanceY; // cü‚Ì“ñæ
	float powR = (radiusA + radiusB) * (radiusA + radiusB); // ‚»‚ê‚¼‚ê‚Ì”¼Œa‚ğ‘«‚µ‚½‚à‚Ì‚Ì“ñæ
	if ((powX + powY) <= powR)
	{
		return true; // ¬Œ÷
	}

	// ğŒ‚ğ–‚½‚³‚È‚¢‚Ì‚Å‚ ‚ê‚Î¸”s
	return false;
}



// “_‚Æ‰~‚Ì“–‚½‚è”»’è
bool Collision::CheckPointToCircle(
	const VECTOR& PointPos,
	const VECTOR& centerPos,   // A‚Ì‰~‚Ì’†SÀ•W
	const float& radius)        // A‚Ì‰~‚Ì”¼Œa
{
	// “–‚½‚è”»’è‚ğ•`‚­

	// X²‚Ì‹——£i‰¡üj
	float distanceX = PointPos.x - centerPos.x;
	// Y²‚Ì‹——£icüj
	float distanceY = PointPos.y - centerPos.y;


	// ”»’è
	float powX = distanceX * distanceX; // ‰¡ü‚Ì“ñæ
	float powY = distanceY * distanceY; // cü‚Ì“ñæ
	float powR = radius * radius; // ”¼Œa‚Ì“ñæ
	if ((powX + powY) <= powR)
	{
		return true; // ¬Œ÷
	}

	// ğŒ‚ğ–‚½‚³‚È‚¢‚Ì‚Å‚ ‚ê‚Î“–‚½‚Á‚Ä‚¢‚È‚¢
	return false;
}