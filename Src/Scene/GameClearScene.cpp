#include "TitleScene.h"
#include "MainGameScene.h"
#include "../GLFWEW.h"
#include "GameClearScene.h"

/**
*	シーンを初期化する
*
*	@retval	true	初期化成功
*	@retval	false	初期化失敗
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
*	プレイヤーの入力を処理する
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
*	シーンを更新する
*
*	@param deltatime	前回の更新からの経過時間(秒)
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
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"ゲームクリア画面");
	fontRenderer.AddString(glm::vec2(-128, 0), L"アクションゲーム");
	fontRenderer.EndUpdate();
}


/**
*	シーンを描画する
*/
void GameClearScene::Render() {
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);

	fontRenderer.Draw(screenSize);

}
