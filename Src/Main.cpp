/**
*	@file Main.cpp
*
*/


#include <Windows.h>
#include "GLFWEW.h"
#include "Scene/TitleScene.h"
#include "Audio/Audio.h"
#include <iostream>
#include "SkeletalMesh.h"

int main() 
{

	auto& window = GLFWEW::Window::Instance();
	window.Init(800, 600, "Title");
	// スケルタルメッシュ・アニメーションを利用可能にする
	Mesh::SkeletalAnimation::Initialize();

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	

	for (;!window.ShouldClose();) {

	
		window.Update();
		window.UpdateGamePad();
		//window.UpdateDeltaTime();
		const float deltaTime = window.DeltaTime();
		window.UpdateTimer();
		// スケルタル・アニメーション用データの作成準備
		Mesh::SkeletalAnimation::ResetUniformData();

		sceneStack.Update(deltaTime);

		// スケルタル・アニメーション用データをGPUにメモリに転送
		Mesh::SkeletalAnimation::UploadUniformData();



		// バックバッファを削除する
		glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GLコンテクストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// ESCキーが押されたら終了ウィンドウを表示
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "ゲーム終了しますか？", "終了", MB_OKCANCEL) == IDOK) {
				break;
			}
		}


		sceneStack.Render();

		window.SwapBuffers();
	}

	Mesh::SkeletalAnimation::Finalize();

	return 0;
}

