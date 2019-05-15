/**
*	@file TitleScene.cpp
*/

#include "TitleScene.h"
#include "MainGameScene.h"

/**
*	プレイヤーの入力を処理する
*/
void TitleScene::ProcessInput() {
	SceneStack::Instance().Replace(std::make_shared<MainGameScene>());

}
