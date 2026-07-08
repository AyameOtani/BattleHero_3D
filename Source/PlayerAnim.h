#pragma once
#include <string>

class Player3D;
class Model;

class PlayerAnim // プレイヤーごとにアニメをいれるのを分けた
{
public:
    // モデル名を見てアニメを全部登録する
    // staticつけないとerrorでるからつけとく
    static void Load(Player3D* player, const std::string& modelPath);


    // DrawPlayerで待機モーションを呼び出す
    static void LoadSelectAnim(Model* model, const std::string& modelPath);
};
