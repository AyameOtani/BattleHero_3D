#include "BoostBar.h"
#include "Unit.h"
#include "Utility.h"
#include "Player3D.h"
#include "Master.h"

BoostBar::BoostBar(Unit* unit)
{
	mpUnit = unit;

	mnHandel_Tex = LoadGraph("Resource/2d/HP3.png");
	if (mnHandel_Tex == -1)
	{
		printfDx("光の画像がない");
	}
}

BoostBar::~BoostBar()
{
	DeleteGraph(mnHandel_Tex);
}

void BoostBar::Update()
{
	// ジャンプ中じゃなかったら回復
	if (mpUnit->IsOverHeat()) // OH中だったら回復
	{
		mpUnit->AddBoost(3.5f); // 回復処理で、回復の関数をよぶ
	}
	else if (!mpUnit->IsJumping() && !mpUnit->IsDashing() && !mpUnit->IsFlying() && !mpUnit->IsDushFlying())
	{
		mpUnit->AddBoost(2.5f); // 回復処理で、回復の関数をよぶ
	}

	// 点滅のタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 50.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}
}

void BoostBar::PlayerBoostBar()
{
	// 位置とサイズ
	const int x = Utility::SCREEN_WIDTH / 2 - 270; // 画面の半分の位置
	const int y = Utility::SCREEN_HEIGHT / 2 + 435; // 下の方
	const int width = 550; // 横幅
	const int height = 33; // 高さ

	// ゲージの割合
	float rate = mpUnit->GetBoost() / mpUnit->GetMaxBoost(); // 0から1の割合にしている
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}
	if (rate > 1.0f)
	{
		rate = 1.0f;
	}


	// 背景 黒色
	DrawBox(x, y, x + width, y + height, GetColor(80, 80, 80), TRUE);

	// オーバーヒートしていないなら
	if (!mpUnit->IsOverHeat())
	{
		// 今のゲージの色設定
		int color = 0;
		if (rate < 0.3f) // rateが0.3未満になったら
		{
			// 今の rate が 0.3 の中でどのくらい残っているか
			float t = rate / 0.3f; // 0.0から0.3を　0.0から1.0に変換する
			int r = (int)(255 * (1.0f - t * 0.3f)); // 赤の強さ
			int g = (int)(80 * t + 20); //緑の強さ
			color = GetColor(r, g, 50); // 青は50で固定
		}
		else
		{
			color = GetColor(91, 221, 109);// 通常は緑
		}

		// PlayerでoverrideしたUnitのゲッターを使っている
		if (rate < 1.0f && (!mpUnit->IsJumping() || !mpUnit->IsDashing())) // Maxじゃないかつ回復しているなら（ジャンプや走りちゃうなら）
		{
			if (mFlushTimer) // 点滅タイマーが有効だったら
			{
				// 点滅ON
				DrawBox(x, y, x + (int)(width * rate), y + height, color, TRUE);
			}
			else
			{
				// 点滅OFF
				// もとの色を少し暗くする
				int r, g, b;
				GetColor2(color, &r, &g, &b);
				r = (int)(r * 0.8f);
				g = (int)(g * 0.8f);
				b = (int)(b * 0.8f);

				int darkColor = GetColor(r, g, b);
				DrawBox(x, y, x + (int)(width * rate), y + height, darkColor, TRUE);
			}
		}
		else
		{
			DrawBox(x, y, x + (int)(width * rate), y + height, color, TRUE);
		}

		// 数値 文字
		DrawFormatString(x + 8, y + 8, GetColor(255, 255, 255), "BOOST", mpUnit->GetBoost(), mpUnit->GetMaxBoost());
	}
	else // おーばーひーと中だったら
	{
		int OverHeat = GetColor(231, 77, 77);// オーバーヒート
		DrawBox(x, y, x + (int)(width * rate), y + height, OverHeat, TRUE);

		// 数値 オーバーヒート
		DrawFormatString(x + 220, y + 8, GetColor(255, 255, 255), "OVER HEAT", mpUnit->GetBoost(), mpUnit->GetMaxBoost());
	}
	// 枠線 白
	DrawBox(x, y, x + width, y + height, GetColor(255, 255, 255), FALSE);




	// 二重テクスチャ　スクロールしている
	// 今は画像の幅をwidthに合わせてる
	{
		// バーの位置とサイズ
		int barX1 = x;
		int barY1 = y;
		int barX2 = x + (int)(width * rate);
		int barY2 = y + height;
		// スクロール画像のサイズ取得
		int imgW, imgH;
		GetGraphSize(mnHandel_Tex, &imgW, &imgH);
		// 明るく重ねる
		SetDrawBlendMode(DX_BLENDMODE_ADD, 60);

		// バーサイズ
		int barWidth = barX2 - barX1;
		int barHeight = barY2 - barY1;

		// 横スクロール：画像の一部を切り出して拡大描画
		// 左スクロールに変更：scrollX を減少させる
		scrollX -= 8;

		// 負の値に対応した正しい切り出し開始位置
		int srcX = (scrollX % imgW + imgW) % imgW;

		if (srcX + barWidth <= imgW)
		{
			// 1回の描画でOK（切り出し範囲が画像内に収まる）
			DrawRectExtendGraph(barX1, barY1, barX2, barY2,
				srcX, 0, barWidth, barHeight,
				mnHandel_Tex, TRUE);
		}
		else
		{
			// 2回に分けて描画してループ表現
			int firstW = imgW - srcX;
			int secondW = barWidth - firstW;

			// 右端から描画
			DrawRectExtendGraph(barX1, barY1, barX1 + firstW, barY2,
				srcX, 0, firstW, barHeight,
				mnHandel_Tex, TRUE);

			// 左端から描画
			DrawRectExtendGraph(barX1 + firstW, barY1, barX2, barY2,
				0, 0, secondW, barHeight,
				mnHandel_Tex, TRUE);
		}

		// 合成モード解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	

}

void BoostBar::EnemyBoostBar()
{
	// 敵の頭上に出すようなベクター
	// HpBarはthisで描画しているから、タグとかいらんくても描画できるhzu　1231
	VECTOR enemyPos = mpUnit->GetPosition();


	// 今のシーンがチュートリアルじゃないなら処理をする
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		enemyPos.y += 215.0f;
	}
	else
	{
		enemyPos.y += 245.0f;
	}

	float width = 150.0f;  // HPバー横幅
	float height = 10.0f;   // HPバー縦幅

	// カメラの視線方向を取得
	VECTOR camDir = VGet(0.0f, 0.0f, 0.0f);
	// カメラの注視点から位置をひいて、見ている方向を出しチエル
	camDir = VSub(Master::mpCamera->GetLookAtPosition(), Master::mpCamera->GetPosition());
	camDir = VNorm(camDir); // 正規化

	// 背面チェック
	VECTOR EnemyHpBar = VSub(enemyPos, Master::mpCamera->GetPosition());
	EnemyHpBar = VNorm(EnemyHpBar);
	float dot = VDot(EnemyHpBar, camDir);

	if (dot < 0.0f)
	{
		return; // カメラの死角にいるバーは映らないようにしている
	}

	// 値が０から１を超えたり下回ったりしないように
	float Enemyrate = mpUnit->GetBoost() / mpUnit->GetMaxBoost();
	if (Enemyrate < 0.0f)
	{
		Enemyrate = 0.0f;
	}
	if (Enemyrate > 1.0f)
	{
		Enemyrate = 1.0f;
	}

	//  描画前にライト・環境光をリセット
	SetUseLighting(false);                    // ライト無効
	SetUseSpecular(false);                    // 鏡面反射無効

	// ビルボード用ベクトル
	VECTOR camRight = VNorm(VCross(VGet(0, 1, 0), camDir)); // カメラのっ右方向のベクトル
	VECTOR camUp = VNorm(VCross(camDir, camRight)); // カメラの上方向のベクトル

	VECTOR halfRight = VScale(camRight, width / 2.0f); // 横方向の半分の長さ
	VECTOR halfUp = VScale(camUp, height / 2.0f);      // 縦方向の半分の長さ


	// 白い枠線（　しかくでかいてる）
	{
		float BigFrame = 0.5f; // 枠の太さ
		// 背景より少し大きくする
		VECTOR frameRight = VScale(camRight, (width / 2.0f) + BigFrame);
		VECTOR frameUp = VScale(camUp, (height / 2.0f) + BigFrame);

		VECTOR centerPos1 = VSub(enemyPos, frameRight);
		VECTOR centerPos2 = VAdd(enemyPos, frameRight);

		VERTEX3D frameVertex[4];
		frameVertex[0].pos = VAdd(centerPos1, frameUp); // 左上
		frameVertex[1].pos = VAdd(centerPos2, frameUp); // 右上
		frameVertex[2].pos = VSub(centerPos1, frameUp); // 左下
		frameVertex[3].pos = VSub(centerPos2, frameUp); // 右下

		for (int i = 0; i < 4; i++)
		{
			frameVertex[i].dif = GetColorU8(255, 255, 255, 255); // 白
		}

		VERTEX3D framePolygon[6];
		framePolygon[0] = frameVertex[2]; // 左下
		framePolygon[1] = frameVertex[0]; // 左上
		framePolygon[2] = frameVertex[1]; // 右上
		framePolygon[3] = frameVertex[2]; // 左下
		framePolygon[4] = frameVertex[1]; // 右上
		framePolygon[5] = frameVertex[3]; // 右下
		DrawPolygon3D(framePolygon, 2, DX_NONE_GRAPH, TRUE); // 白枠の描画
	}


	// 拝啓　黒用
	{
		VECTOR centerPos1 = VSub(enemyPos, halfRight); // 中心 1 まとめたVSub
		VECTOR certerPos2 = VAdd(enemyPos, halfRight); // 中心 2まろめたVAdd

		// 背景の4頂点
		VERTEX3D bgVertex[4];
		bgVertex[0].pos = VAdd(centerPos1, halfUp); // 左上  // 中心座標と左上になる座標を足す
		bgVertex[1].pos = VAdd(certerPos2, halfUp); // 右上
		bgVertex[2].pos = VSub(centerPos1, halfUp); // 左下
		bgVertex[3].pos = VSub(certerPos2, halfUp); // 右下

		for (int i = 0; i < 4; i++)// ディフューズカラー
		{
			bgVertex[i].dif = GetColorU8(80, 80, 80, 255); // 黒 いっちゃん右はAlpha
		}
		// 背景ポリゴン
		VERTEX3D bgPolygon[6];
		bgPolygon[0] = bgVertex[2]; // 左下
		bgPolygon[1] = bgVertex[0]; // 左上
		bgPolygon[2] = bgVertex[1]; // 右上
		bgPolygon[3] = bgVertex[2]; // 左下
		bgPolygon[4] = bgVertex[1]; // 右上
		bgPolygon[5] = bgVertex[3]; // 右下

		DrawPolygon3D(bgPolygon, 2, DX_NONE_GRAPH, TRUE);// 背景　黒の描画
	}


	// HP赤用 
	{
		// HPバーの左端を基準にする
		VECTOR leftEnd = VSub(enemyPos, halfRight); // 左端の位置（固定）
		// HP割合に応じた「右端」の位置を計算
		VECTOR fgRight = VScale(camRight, width * Enemyrate); // HP割合分の横幅
		VECTOR rightEnd = VAdd(leftEnd, fgRight); // 左端から右方向にHP分伸ばす
		// 頂点設定
		VERTEX3D fgVertex[4];
		fgVertex[0].pos = VAdd(leftEnd, halfUp);   // 左上（固定）
		fgVertex[1].pos = VAdd(rightEnd, halfUp);  // 右上（可変）
		fgVertex[2].pos = VSub(leftEnd, halfUp);   // 左下（固定）
		fgVertex[3].pos = VSub(rightEnd, halfUp);  // 右下（可変）

		for (int i = 0; i < 4; i++)
		{
			fgVertex[i].dif = GetColorU8(44, 200, 220, 255);
		}
		// 前景ポリゴン（HP残量）
		VERTEX3D fgPolygon[6];
		fgPolygon[0] = fgVertex[2]; // 左下
		fgPolygon[1] = fgVertex[0]; // 左上
		fgPolygon[2] = fgVertex[1]; // 右上
		fgPolygon[3] = fgVertex[2]; // 左下
		fgPolygon[4] = fgVertex[1]; // 右上
		fgPolygon[5] = fgVertex[3]; // 右下
		DrawPolygon3D(fgPolygon, 2, DX_NONE_GRAPH, TRUE); // Hpバー　赤の描画
	}

	//  他の3D描画のために戻す
	SetUseLighting(true);
	SetUseSpecular(true);
}