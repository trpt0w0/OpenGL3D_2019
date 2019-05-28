/**
*	@file TitleScene.cpp
*/

#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/**
*	�v���C���[�̓��͂���������
*/
void TitleScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (window.GetGamePad().buttonDown & GamePad::START) {
		SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
	}
}
