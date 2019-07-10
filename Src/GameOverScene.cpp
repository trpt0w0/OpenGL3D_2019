/**
*	@file GameOverScene.cpp
*/

#include "GameOverScene.h"
#include "TitleScene.h"



/**
*	ƒvƒŒƒCƒ„[‚Ì“ü—Í
*/

void GameOverScene::ProcessInput() {
	SceneStack::Instance().Replace(std::make_shared<TitleScene>());

}