#include "Effect3.h"

Effect3::Effect3(VECTOR initPos, std::string filename)
	: Object3D(initPos)
{
	mpEffect = new EffectInfo();

	//mpEffect->color = GetColorU8(100, 18, 16, 255); // 今はでオレンジ色っぽい色filename, 

	for (int i = 0; i < PARTICLE_NUM; i++) // 回数を回してほしい
	{

		// 球面上の座標を求める
		//   ★DX_PI_F は 3.14 と同じ意味
		// 250.0f は注視点からどれだけ離れているかという意味
		// 距離は変えないで回転したい
		const float distance = 5.0f;
		mpEffect->particle[i].dir.x = distance * cosf(mfVerticalAngle / 180.0f * DX_PI_F) * sinf(mfHorizontalAngle / 180.0f * DX_PI_F);    // X座標
		//mpEffect->particle[i].dir.y+=5.0f;                                                 // Y座標
		mpEffect->particle[i].dir.z = -(distance * cosf(mfVerticalAngle / 180.0f * DX_PI_F) * cosf(mfHorizontalAngle / 180.0f * DX_PI_F)); // Z座標

		mfHorizontalAngle -= 5.0f; // 加算　水平方向アングル
		if (mfHorizontalAngle >= 180.0f) // もし180の角度になったら
		{
			mfHorizontalAngle -= 360.0f; // 調整をする
		}


		// 初期座標
		mpEffect->particle[i].pos = initPos;

		// 進行方向
		//mpEffect->particle[i].dir.x = ((float)GetRand(200) - 100.0f) / 70.0f;
		//mpEffect->particle[i].dir.y = ((float)GetRand(200) - 100.0f) / 50.0f;
		//mpEffect->particle[i].dir.z = ((float)GetRand(200) - 100.0f) / 50.0f;

		// 速度
		mpEffect->particle[i].speed = ((float)SPEED_RAND_MIN) / 100.0f;

		// 不透明度 
		mpEffect->particle[i].alpha = 1.0f;

		// 大きさ
		mpEffect->particle[i].size = ((float)SIZE_RAND_MIN) / 150.0f;

		// 表示時間
		mpEffect->particle[i].visibleTime = ((float)VISIBLE_TIME_RAND_MIN) / 1000.0f;
	
		
		
		// 色
		mpEffect->particle[i].r = GetRand(256);
		mpEffect->particle[i].g = GetRand(256);
		mpEffect->particle[i].b = GetRand(256);
	
	
	}

	// 画像の読み込み
	mnGraphHandle = LoadGraph(filename.c_str());
}

Effect3::~Effect3()
{

	delete mpEffect;

	DeleteGraph(mnGraphHandle);
}

void Effect3::Update()
{
	bool isEnd = true; // パーティクルが全消えたかどうか
	float stepTime = 1.0f / 60.0f; // １フレームにどれだけ進めるかの基本値

	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		// アルファ値が０以下になったらそのパーティクルは処理しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// ここまで来るということは少なくともひとつはパーティクルが存在しているので終了させない
		isEnd = false;

		// 速度が残っている場合
		if (mpEffect->particle[i].speed > 0.0f)
		{
			// 座標を移動させる
			mpEffect->particle[i].pos = VAdd(mpEffect->particle[i].pos,
				VScale(mpEffect->particle[i].dir,
					mpEffect->particle[i].speed));

			// 速度を徐々に落としていく
			mpEffect->particle[i].speed -= 2.0f * stepTime;

			// 速度が０以下になった場合
			if (mpEffect->particle[i].speed <= 0.0f)
			{
				// 表示時間を強制的に０にする
				mpEffect->particle[i].visibleTime = 0.0f;
			}
		}

		// 表示時間が残っている場合
		if (mpEffect->particle[i].visibleTime > 0.0f)
		{
			// 表示時間を減らしていく
			mpEffect->particle[i].visibleTime -= 0.75f * stepTime;
		}
		else
		{
			// アルファ値を落としていく(徐々に透明にしていく)
			mpEffect->particle[i].alpha -= 12.0f * stepTime;
		}

	}
	// パーティクルが１つも存在していないのであれば削除する
	if (isEnd)
	{
		SetDeleteFlag(true);
	}

	// 下にいくほど表示を小さくしていくと炎になる
}

void Effect3::Draw()
{
	// Zバッファを使用する設定にする
	SetUseZBufferFlag(TRUE);

	// Zバッファへの書き込みはしない
	SetWriteZBufferFlag(FALSE);

	
	// 描画していく
	// DX_BLENDMODE_INVSRC    描画元の色を反転する
	for (int i = 0; i < PARTICLE_NUM; i++)
	{

		// ランダムにするからiをいれている
		SetDrawBright(
			mpEffect->particle[i].r,
			mpEffect->particle[i].g,
			mpEffect->particle[i].b
		);



		// アルファ値が０以下なら設定しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// 反転ブレンドを行いつつパーティクルを描画する
		// 描画の色が反転する
		// ビルボード　　カメラの方向に常に向いてくれる３Dポリゴン ★重要
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, (int)mpEffect->particle[i].alpha * 255);
		DrawBillboard3D(
			mpEffect->particle[i].pos, 0.5f, 0.5f,
			mpEffect->particle[i].size * mpEffect->particle[i].alpha,
			0.0f,
			mnGraphHandle,
			true
		);
	}


	// DX_BLENDMODE_ADD_X4  になっている
	// 加算ブレンドの描画元の輝度を最大４倍にできるモード
	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		// アルファ値が０以下なら設定しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// 加算ブレンドを行いつつパーティクルを描画する
		// 加算合成をしている ポリゴンが重なっている所が白に近づく描画方法
		// 輝度は最大４倍になる
		// ビルボード　　カメラの方向に常に向いてくれる３Dポリゴン ★重要
		SetDrawBlendMode(DX_BLENDMODE_ADD_X4, (int)mpEffect->particle[i].alpha * 255);

		DrawBillboard3D(
			mpEffect->particle[i].pos, 0.5f, 0.5f,
			mpEffect->particle[i].size * mpEffect->particle[i].alpha,
			0.0f,
			mnGraphHandle,
			true
		);
	}


	// 変えたものを元に戻す処理をする
	// ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 描画輝度を元に戻す
	SetDrawBright(255, 255, 255);

	// Zバッファを使用する
	SetUseZBufferFlag(TRUE);

	// Zバッファへの書き込みをする
	SetWriteZBufferFlag(TRUE);

}


