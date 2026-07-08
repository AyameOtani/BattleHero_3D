#include "Score3D.h"
#include "Master.h"
#include <fstream> // ファイルの入出力ができる
#include <algorithm> // そーと用
#include "GameManager.h"


Score3D::Score3D()
{

}

Score3D::~Score3D()
{
	
}

void Score3D::ScoreInitialize()
{
	mnFontSmall = CreateFontToHandle(
		NULL,
		45,   // 小さめのフォントサイズ
		3
	);
}


void Score3D::ScoreFinalize()
{
	DeleteFontToHandle(mnFontSmall);
}



// 比較するためのやつ
// AのほうがBより大きかったらtrueを返す
bool Score3D::CompareRank(const RankData& scoreA, const RankData& scoreB)
{
	return scoreA.score > scoreB.score;
}


void Score3D::LoadRanking()
{
	mbError = false; // ロードするときにfalseにしていお

	// ifstream...入力スクリーム
	std::ifstream file(txtname); // ファイルを開く
	
	//  開けなかったら今のデータを消さずに終了
	if (!file.is_open())
	{
		mbError = true; // 開けなかったらtrue
		return;
	}

	mvRank.clear(); // 同じスコアがめちゃ増えたから入れた
	int score; // 一つずつ取り出したいから
	std::string name;

	// 名前　スコアで読広
	while (file >> name >> score) // ぜんぶ読み込みたいからWhileにした
	{
		mvRank.push_back({score, name});
	}
	// greater これで大きい順にしてとなるらしい
	// 念のため大きい順に並び替えておく
	// stable_sortは 同じ点なら先にあったほうが優先されるってなる
	std::stable_sort(mvRank.begin(), mvRank.end(), CompareRank);
}

// セーブやりい
void Score3D::SaveRanking()
{
	mnRankIn = -1;   // いったん圏外

	// 今回のスコアをリストに追加
	mvRank.push_back({ mnRankingScore, GameManager::Players.name });
	// 並び替え  初めから終わりまで
	std::stable_sort(mvRank.begin(), mvRank.end(), CompareRank);
	//std::reverse(mvRank.begin(), mvRank.end()); //ならべたものを逆にしてもよいっぽいなこれ

	// ランクイン判定 MaxSize以内に入ったか
	for (int i = 0; i < mvRank.size(); ++i)
	{
		if (mvRank[i].score == mnRankingScore && mvRank[i].name == GameManager::Players.name)
		{
			if (i < MaxSize)
			{
				mnRankIn = i; // 何位か保存
			}
			break; // 最初の一致だけ取る
		}
	}

	//  リストのサイズが５以上なら５にする
	if (mvRank.size() > MaxSize)
	{
		mvRank.resize(MaxSize);
	}

	// mvRankが空なら保存しないで全消えを防ぐ
	if (mvRank.empty())
	{
		return;
	}

	// ファイルに上書き保存
	// ofstream...出力ファイルストリームというらしい書き込みに使う
	std::ofstream fail(txtname);
	if (fail.is_open())
	{
		for (const auto& i : mvRank)
		{
			// スコアと名前をセットで書き出す 先に名前にした
			fail << i.name << " " << i.score << std::endl;
		}
		fail.close(); // 閉じる
	}
}

// 表示 これだと、同じ点数だったらぜんぶ色が変わる
void Score3D::DrawRanking(int x, int y)
{
	auto ranking = GetRanking(); // 取得
	int color = GetColor(255, 255, 255);
	int num = ((int)ranking.size() / 2); // 半分でずらすやつ

	unsigned int colors[] =
	{
		GetColor(255,215,0),   // 1位
		GetColor(192,192,192), // 2位
		GetColor(140,72,65)    // 3位
	};

	
	if (mbError)
	{
		DrawString(x -150, y, "スコアランキングが読み込まれませんでした!", GetColor(255, 255, 255));
	}
	else
	{
		DrawString(x + 150, y, "スコアランキング", GetColor(255, 255, 255));
		DrawString(x - 260, y + 45 , "-----------------------------------------------", GetColor(255, 255, 255));

		for (int i = 0; i < ranking.size(); ++i)
		{
			// YとXを変えてる
			int DrawY = (i < (num)) ? (y + 120 +(i * 65)) : (y + 120 + ((i - num) * 65));
			int DrawX = (i < (num)) ? 200 : 1100; // 5以上はXを変える


			DrawFormatStringToHandle(
				DrawX + 50,
				DrawY,
				color,
				mnFontSmall,
				"%d位",
				i + 1	// 1位からやりたい
			);

			DrawFormatStringToHandle(
				DrawX + 200,
				DrawY,
				color,
				mnFontSmall,
				"%s",
				ranking[i].name.c_str()
			);

			DrawFormatStringToHandle(
				DrawX + 500,
				DrawY,
				color,
				mnFontSmall,
				"%d点",
				ranking[i].score
			);
		}
	}

}

void Score3D::DrawMemo()
{
	auto ranking = GetRanking(); // 取得
	int color = GetColor(255, 255, 255);
	for (int i = 0; i < ranking.size(); ++i)
	{
		// ランクに入ったら出す
		if (i == mnRankIn)
		{
			DrawFormatString(
				1500,
				450,
				color,
				"ランクイン!"
			);
		}
	}

	// 勝ったら出す
	if (mbWin)
	{
		DrawFormatString(
			1500,
			520,
			color,
			"勝利ボーナス\n+4000"
		);
	}
}


void Score3D::ScoreGet(int time, int playerHp, int kill)
{
	mbWin = false; // リセット
	// メンバに代入
	mnTime = time;
	mnPlayerHp = playerHp;
	mnKillCount = kill;
	//mnRankingScore = time + playerHp + kill;

	// 勝ったときに+4000
	if (Master::mpSceneManager->GetCurrentSceneType() == SceneManager::SCENE_TYPE::WIN_RESULT_3D)
	{
		mnRankingScore = time + playerHp + kill + 4000;
		mbWin = true;
	}
	else if(Master::mpSceneManager->GetCurrentSceneType() == SceneManager::SCENE_TYPE::LOSE_RESULT_3D)
	{
		mnRankingScore = time + playerHp + kill;
	}
}


std::string Score3D::TotalGameScore()
{
	// ステータスが存在していたらintに最大HPを入れる
	int MaxHp;
	if (Master::mpNextStatus)
	{
		MaxHp = (int)Master::mpNextStatus->Get_Base_MaxHp();
	}
	else
	{
		MaxHp = 150; // ないならこてい150
	}

	int total = 0; // メンバ
	// 時間
	if ( mnTime >= 120) { total += 50;}
	else if(mnTime >= 60) { total += 30;}
	else{ total += 10;}

	// HP
	if (mnPlayerHp >= ((MaxHp / 3) * 2)) { total += 50;}
	else if (mnPlayerHp >= MaxHp / 2){ total += 30;}
	else{ total += 10;}

	// キル数
	if (mnKillCount >= 4) { total += 50;}
	else if (mnKillCount >= 2) { total += 30;}
	else{ total += 10;}


	if (total == 150) // 満点
	{
		return "SSS";
	}
	if (total == 130) // 50 50 30
	{
		return "SS";
	}
	if (total == 110) // 50 30 30
	{
		return "S";
	}
	if (total == 90) // 30 30 30
	{
		return "A";
	}
	if (total == 70) // 30 30 10
	{
		return "B";
	}
	if (total == 50) // 30 10 10
	{
		return "C";
	}
	if (total == 30) // 10 10 10
	{
		return "D";
	}

	// 例外はBかね
	return "なし";
}