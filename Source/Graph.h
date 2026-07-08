#pragma once
#include "Object2D.h"

class Graph : public Object2D
{
public:
	Graph(std::string filename, VECTOR initPos, float Rate);
	virtual ~Graph();

	void Update() override;
	void Draw() override;

	// プレイヤーの顔の証明写真
	static int PlayerPhoto(std::string filename);

	// VICTORY画面で出すやつ
	static int VictoryPhoto(std::string filename);

	// DEFEAT画面で出す画像
	static int DefeatPhoto(std::string filename);

};
