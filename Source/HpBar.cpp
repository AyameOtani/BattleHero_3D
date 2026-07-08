#include "HpBar.h"
#include "Unit.h"
#include "Utility.h"

#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"

HpBar::HpBar(Unit* unit)
{
	mpUnit = unit;
	mfSlowHp = mpUnit->GetMaxHp();

	mnHandel_Tex = LoadGraph("Resource/2d/HP3.png");
	if (mnHandel_Tex == -1)
	{
		printfDx("光の画像がない");
	}
}

HpBar::~HpBar()
{
	DeleteGraph(mnHandel_Tex);
}

void HpBar::Update()
{
	// 遅れて表示用のHP
	float hp = mpUnit->GetHp();
	if (mfSlowHp > hp)
	{
		mfSlowHp -= mfSlowSpeed; // 毎フレーム減らす
		if (mfSlowHp <= hp)
		{
			mfSlowHp = hp;
		}
	}
	else if (mfSlowHp < hp)
	{
		mfSlowHp = hp;
		if (mfSlowHp >= hp) // 上限をチェックしないと、回復時に突破しちゃう　11/10
		{
			mfSlowHp = hp;
		}
	}

	// 点滅のタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 50.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
		mCurrentPhoto = 0;
	}


}

void HpBar::PlayerHpBar()
{
	// 位置とサイズ
	const int x = Utility::SCREEN_WIDTH / 2 - 270; // 画面の半分の位置
	const int y = Utility::SCREEN_HEIGHT / 2 + 390; // 下の方
	const int width = 550; // 横幅
	const int height = 33; // 高さ

	// ゲージの割合
	float rate = mpUnit->GetHp() / mpUnit->GetMaxHp(); // 0から1の割合にしている
	float slowRate = mfSlowHp / mpUnit->GetMaxHp(); // 0から1の割合にしている


	// 値が０から１を超えたり下回ったりしないように
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}
	if (rate > 1.0f)
	{
		rate = 1.0f;
	}
	if (slowRate < 0.0f)
	{
		slowRate = 0.0f;
	}
	if (slowRate > 1.0f)
	{
		slowRate = 1.0f;
	}

	DrawBox(x, y, x + width, y + height, GetColor(80, 80, 80), TRUE);// 背景 黒色

	int color = 0;
	if (rate < 0.3) // 0.3以下になったら赤くする処理
	{
		float t = rate / 0.3f; // ここで0.0から0.3を、0.0から1.0に変換する
		int r = (int)(255.0f * (1.0f - t * 0.3f)); // 赤色の強さ
		int g = (int)(80.0f * t + 20.0f); // 緑色の強さ
		color = GetColor(r, g, 50); // 青は固定で50

		if (mFlushTimer)
		{
			// 点滅（明かる方）
			DrawBox(x, y, x + (int)(width * rate), y + height, color, TRUE);
		}
		else
		{
			// 点滅（暗いほう）
			int r, g, b;
			GetColor2(color, &r, &g, &b); // GetColor2で取得する

			// 0.7ずつかけて少し暗くする
			r = (int)(r * 0.8f);
			g = (int)(g * 0.8f);
			b = (int)(b * 0.8f);

			int DarkColor = GetColor(r, g, b);
			DrawBox(x, y, x + (int)(width * rate), y + height, DarkColor, TRUE);
		}
	}
	else
	{
		// こっちはグラデーションありのやつ
		int GradWidth = (int)(width * rate);
		for (int i = 0; i < GradWidth; i++)
		{
			float t = (float)i / GradWidth;

			// 左は暗め　右hあ明るめにしてう
			int r = (int)(60 + 80 * t); // R
			int g = (int)(130 + 90 * t); // G
			int b = (int)(200 + 55 * t); // B

			int GradColor = GetColor(r, g, b);

			// HPの端から右端のとこまで塗りたいから
			// 1pxずつ描画
			DrawBox(
				x + i, // 今描いている縦線の左
				y,
				x + i + 1, // 1px右
				y + height,
				GradColor,
				TRUE
			);
		}
	}

	// 減少中のHPの描画
	if (slowRate > rate)
	{
		int slowColor = GetColor(243, 169, 41); // 赤色	

		// HPの端から右端のとこまで塗りたいから
		DrawBox(x + (int)(width * rate), y, x + (int)(width * slowRate), y + height, slowColor, TRUE);
	}
	// 枠線 白
	DrawBox(x, y, x + width, y + height, GetColor(255, 255, 255), FALSE);
	// 数値 文字
	DrawFormatString(x + 8, y + 8, GetColor(255, 255, 255), "HP", mpUnit->GetHp(), mpUnit->GetMaxHp());



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



void HpBar::EnemyHpBar()
{
	// 敵の頭上に出すようなベクター
	// HpBarはthisで描画しているから、タグとかいらんくても描画できるhzu　1231
	VECTOR enemyPos = mpUnit->GetPosition();

	// 今のシーンがチュートリアルじゃないなら処理をする
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		enemyPos.y += 200.0f;
	}
	else
	{
		enemyPos.y += 230.0f;
	}
	

	float width = 150.0f;  // HPバー横幅
	float height = 10.0f;   // HPバー縦幅

	// カメラの視線方向を取得
	VECTOR camDir = VGet(0.0f, 0.0f, 0.0f);
	// カメラの注視点から位置をひいて、見ている方向を出してる
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
	float Enemyrate = mpUnit->GetHp() / mpUnit->GetMaxHp();
	float slowERate = mfSlowHp / mpUnit->GetMaxHp();
	if (Enemyrate < 0.0f)
	{
		Enemyrate = 0.0f;
	}
	if (Enemyrate > 1.0f)
	{
		Enemyrate = 1.0f;
	}
	if (slowERate < 0.0f)
	{
		slowERate = 0.0f;
	}
	if (slowERate > 1.0f)
	{
		slowERate = 1.0f;
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
			fgVertex[i].dif = GetColorU8(213, 44, 44, 255);
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



		// ゆっくりHPバーの描画 
		// HPバーの減った部分を基準にする
		VECTOR slowLeftEnd = rightEnd;
		// 遅延バーの右端
		VECTOR slowRightEnd = VAdd(leftEnd, VScale(camRight, width * slowERate));

		// 頂点設定
		VERTEX3D SlowVertex[4];
		SlowVertex[0].pos = VAdd(slowLeftEnd, halfUp);   // 左上（固定）
		SlowVertex[1].pos = VAdd(slowRightEnd, halfUp);  // 右上（可変）
		SlowVertex[2].pos = VSub(slowLeftEnd, halfUp);   // 左下（固定）
		SlowVertex[3].pos = VSub(slowRightEnd, halfUp);  // 右下（可変）


		for (int i = 0; i < 4; i++)
		{
			SlowVertex[i].dif = GetColorU8(243, 169, 41, 255);
		}

		// 前景ポリゴン（HP残量）
		VERTEX3D SlowPolygon[6];
		SlowPolygon[0] = SlowVertex[2]; // 左下
		SlowPolygon[1] = SlowVertex[0]; // 左上
		SlowPolygon[2] = SlowVertex[1]; // 右上
		SlowPolygon[3] = SlowVertex[2]; // 左下
		SlowPolygon[4] = SlowVertex[1]; // 右上
		SlowPolygon[5] = SlowVertex[3]; // 右下


		if (slowERate > Enemyrate)
		{
			DrawPolygon3D(SlowPolygon, 2, DX_NONE_GRAPH, TRUE); // Hpバー　赤の描画
		}
	}


	//  他の3D描画のために戻す
	SetUseLighting(true);
	SetUseSpecular(true);
}

