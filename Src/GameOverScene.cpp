/**
*	@file GameOverScene.cpp
*/

#include "GameOverScene.h"
#include "TitleScene.h"

/**
*	プレイヤーの入力
*/

void GameOverScene::ProcessInput() {
	SceneStack::Instance().Replace(std::make_shared<TitleScene>());

}