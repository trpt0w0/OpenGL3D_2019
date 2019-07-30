/**
*	@file Main.cpp
*
*/
#include "TitleScene.h"
#include "GLFWEW.h"
#include <Windows.h>
#include <iostream>


int main() 
{

	auto& window = GLFWEW::Window::Instance();
	window.Init(800, 600, "Title");

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	

	for (;!window.ShouldClose();) {
	
		window.UpdateGamePad();

		const float deltaTime = 1.0f / 60.0f;//window.DeltaTime();

		sceneStack.Update(deltaTime);

		// ESCキーが押されたら終了ウィンドウを表示
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "ゲーム終了しますか？", "終了", MB_OKCANCEL) == IDOK) {
				break;
			}
		}
		

		// バックバッファを削除する
		glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GLコンテクストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);


		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}

