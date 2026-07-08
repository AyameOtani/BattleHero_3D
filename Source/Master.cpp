#include "Master.h"

// ------------------3D用------
//Status3D* Master::mpNextStatus = nullptr;//  定義と初期化  やらないとエラーでた1227

// 1231　所有者をつくっっている
// 開始時は空っぽにしている
 std::unique_ptr<Status3D> Master::mpNextStatus = nullptr;

 // playerの見た目を入れるための箱 ポインタじゃないからnullptrだと外部出る。
 std::string Master::msPlayerModelName = "";

