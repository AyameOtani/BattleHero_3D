#include "DeathEffect.h"
#include "Master.h"

DeathEffect::DeathEffect(VECTOR initPos, std::string filename)
	: Object3D(initPos)
{
	mpEffect = new EffectInfo();

	mpEffect->color = GetColorU8(128, 16, 16, 255); // 今は固定で赤色っぽい色

	for (int i = 0; i < PARTICLE_NUM; i++) // 回数を回してほしい
	{
		// 初期座標
		mpEffect->particle[i].pos = initPos;

		// 進行方向
		mpEffect->particle[i].dir.x = ((float)GetRand(200) - 100.0f) / 200.0f;
		mpEffect->particle[i].dir.y = ((float)GetRand(200) - 100.0f) / 200.0f;
		mpEffect->particle[i].dir.z = ((float)GetRand(200) - 100.0f) / 200.0f;

		// 速度
		mpEffect->particle[i].speed = ((float)GetRand(SPEED_RAND_MAX) + SPEED_RAND_MIN) / 50.0f;

		// 不透明度 
		mpEffect->particle[i].alpha = 1.0f;

		// 大きさ
		mpEffect->particle[i].size = ((float)GetRand(SIZE_RAND_MAX) + SIZE_RAND_MIN) / 20.0f;

		// 表示時間
		mpEffect->particle[i].visibleTime = ((float)GetRand(VISIBLE_TIME_RAND_MAX) + VISIBLE_TIME_RAND_MIN) / 40.0f;
	}

	// 画像の読み込み
	// mnGraphHandle = LoadGraph(filename.c_str()); // 変更
	mnGraphHandle = Master::mpResourceManager->LoadGraphics(filename);
}

DeathEffect::~DeathEffect()
{

	delete mpEffect;

	//DeleteGraph(mnGraphHandle);// リソースマネー入れたから消す
}

void DeathEffect::Update()
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
			mpEffect->particle[i].speed -= 15.0f * stepTime;

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
			mpEffect->particle[i].visibleTime -= 0.97f * stepTime;
			mpEffect->particle[i].size *= (float)1.01f;
		}
		else
		{
			// アルファ値を落としていく(徐々に透明にしていく)
			mpEffect->particle[i].alpha -= 10.0f * stepTime;
		}

		mpEffect->particle[i].dir.y -= 0.01f; // 重力

	}
	// パーティクルが１つも存在していないのであれば削除する
	if (isEnd)
	{
		SetDeleteFlag(true);
	}
}

void DeathEffect::Draw()
{
	// Zバッファを使用する設定にする
	SetUseZBufferFlag(TRUE);

	// Zバッファへの書き込みはしない
	SetWriteZBufferFlag(FALSE);

	
	// 描画輝度の設定
	SetDrawBright(mpEffect->color.r, mpEffect->color.g, mpEffect->color.b);

	// 描画していく
	// DX_BLENDMODE_INVSRC    描画元の色を反転する
	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		// アルファ値が０以下なら設定しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// 反転ブレンドを行いつつパーティクルを描画する
		// 描画の色が反転する
		// ビルボード　　カメラの方向に常に向いてくれる３Dポリゴン ★重要
		SetDrawBlendMode(
			DX_BLENDMODE_INVSRC,
			(int)(mpEffect->particle[i].alpha * 30.0f)
		);

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
		SetDrawBlendMode(
			DX_BLENDMODE_ADD_X4,
			(int)(mpEffect->particle[i].alpha * 120.0f)
		);


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