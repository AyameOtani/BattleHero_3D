#pragma once
#include <DxLib.h>
#include <vector>
#include <string>

//選択シーンになったら下になんかエフェクトを出したい
// ずっと回転＋キャラ変えてもリセットされない感じの

class SelectEffect
{
public:
	// CPPに定義ないからdefaultにしないと未解決のシンボルエラーでる
	SelectEffect() = default;
	~SelectEffect() = default;

	// 初期データ　てくすちゃ読み込み
	bool Load(std::string filename);
	// 押した瞬間にエフェクトを出す
	void StartHold(const VECTOR& playerPos);

	//位置の変更とか回転とか
	void Update(const VECTOR& playerPos);

	// 描画
	void Draw() const;
	void DrawHoldingBoard() const; // 描画を設定

private:
	VECTOR mvCenter = VGet(0.0f, 0.0f, 0.0f); // 中心位置
	float mfAngle = 0.0f; //回転
	float mfSpeed = 0.02f; // 回転速度

	float mfSize = 200.0f; // 大きさ   魔法は１４０
	float mfYOffset = 7.0f; // Y

	int mnTex = -1; // texture

	// 描画の輝度で使う
	int mnBright = 200;


	std::string msFilename; // ファイル保存
};