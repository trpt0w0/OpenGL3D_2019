#include "TitleScene.h"
#include "MainGameScene.h"
#include "../GLFWEW.h"
#include "GameClearScene.h"

/**
*	�V�[��������������
*
*	@retval	true	����������
*	@retval	false	���������s
*/
bool GameClearScene::Initialize() {
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	bgm = Audio::Engine::Instance().Prepare("Res/Audio/GameOverBgm.mp3");
	bgm->Play(Audio::Flag_Loop);
	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	return true;
}


/**
*	�v���C���[�̓��͂���������
*/
void GameClearScene::ProcessInput() {

	GLFWEW::Window& window = GLFWEW::Window::Instance();

	if (!flag) {

		if (window.GetGamePad().buttonDown & GamePad::START) {
			flag = true;
			bgm->Stop();
			SceneStack::Instance().Push(std::make_shared<TitleScene>());
		}
	}
}

/**
*	�V�[�����X�V����
*
*	@param deltatime	�O��̍X�V����̌o�ߎ���(�b)
*/
void GameClearScene::Update(float deltaTime) {
	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const float w = window.Width();
	const float h = window.Height();
	const float lineHeight = fontRenderer.LineHeight();
	fontRenderer.BeginUpdate();
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"�Q�[���N���A���");
	fontRenderer.AddString(glm::vec2(-128, 0), L"�A�N�V�����Q�[��");
	fontRenderer.EndUpdate();
}


/**
*	�V�[����`�悷��
*/
void GameClearScene::Render() {
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);

	fontRenderer.Draw(screenSize);

}