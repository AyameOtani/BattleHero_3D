#include "Slide3D.h"


void Slide3D::Update()
{
	mnSlideTimer += 2;

	if (mnSlideTimer >= mnLastSlide)
	{
		mbSlideEnd = true;
	}
}

void Slide3D::Reset()
{
	mnSlideTimer = 0;
	mbSlideEnd = false;
}

// スライドさせるやる
float Slide3D::SlideDirection(int Start, int End, float startX, float endX) const
{
	// タイマーがスタートより小さかったらX
	if (mnSlideTimer <= Start) return startX;
	// タイマーがスタートより大きかったららX
	if (mnSlideTimer >= End)   return endX;

	// 割合 0.0から1.0
	float rate = (float)(mnSlideTimer - Start) / (float)(End - Start);
	// 減速
	float slow = 1.0f - (1.0f - rate) * (1.0f - rate);

	return startX + (endX - startX) * slow;
}
