/**
*	@file TitleScene.cpp
*/

#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/**
*	�V�[��������������
*
*	@retval	true	����������	
*	@retval	false	���������s 
*/
bool TitleScene::Initialize() {
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	return true;
}


/**
*	�v���C���[�̓��͂���������
*/
void TitleScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (window.GetGamePad().buttonDown & GamePad::START) {
		SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
	}
}

/**
*	�V�[�����X�V����
*
*	@param deltatime	�O��̍X�V����̌o�ߎ���(�b)
*/
void TitleScene::Update(float deltaTime) {
	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();
}


/**
*	�V�[����`�悷��
*/
void TitleScene::Render() {
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
}