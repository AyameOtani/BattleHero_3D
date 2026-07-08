#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

class Score3D
{
public:
	Score3D();
	~Score3D();
	void ScoreGet(int time, int playerHp, int kill);
	std::string TotalGameScore();


	// 名前とスコアをセットで
	struct RankData
	{
		int score;
		std::string name;
	};

	void SaveRanking();
	void LoadRanking();
	void DrawRanking(int x, int y);
	void DrawMemo(); // ランクインとかの表示

	// ランクこぴー
	std::vector<RankData> GetRanking() const { return mvRank; }

	// どっちが大きいかくらべる
    static bool CompareRank(const RankData& scoreA, const RankData& scoreB);

	// ランクにはいったかとか諸々
	bool IsRankIn() const { return mnRankIn != -1; }
	int GetRankIndex() const { return mnRankIn; }

	void ScoreInitialize(); // 初期化　文字大きさとか
	void ScoreFinalize(); // 終了スコア


private:
	// スコア保存するメンバ達
	int mnTime = 0;
	int mnPlayerHp = 0;
	int mnKillCount = 0;
	int mnRankingScore = 0;
	const int MaxSize = 10; // 最大で保存する数

	std::vector<RankData> mvRank; // ランクならべる

	int mnRankIn = -1; //  何位に入ったか
	bool mbWin = false; // 勝ったか
	bool mbError = false; // ファイルが開けなかった時の

	const std::string txtname = "Ranking.txt";

	int mnFontSmall = -1;

};