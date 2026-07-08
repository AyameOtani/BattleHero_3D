#pragma once
#include <string>

class Slide3D
{

public:
	struct SlideData
	{
		int SlideStart; // 動き出し時間
		int SlideEnd;   // 停止時間
		int SlideY;	   // 表示Y
		int SlideFont;  //　ふぉんと
		std::string moji; // もし
	};

	// スライド演出に使う
	float SlideDirection(int Start, int End, float startX, float endX) const;

	// ゲッター関係
	int GetTimer() const { return mnSlideTimer; }
	bool GetSlideEnd() const { return mbSlideEnd; }

	void Update();
	void Reset();

private:
	int mnSlideTimer = 0; // スライど時間
	const int mnLastSlide  = 160;	// 最後のスライdo

	bool mbSlideEnd = false;
};