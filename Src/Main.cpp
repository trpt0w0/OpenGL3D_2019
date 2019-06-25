/**
*	@file Main.cpp
*
*/
#include "TitleScene.h"
#include "GLFWEW.h"

int main() 
{

	auto& window = GLFWEW::Window::Instance();
	window.Init(800, 600, "Title");

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());


	for (;!window.ShouldClose();) {
		
		const float deltaTime = 1.0f / 60.0f;

		window.UpdateGamePad();

		sceneStack.Update(deltaTime);

		glClearColor(0.1f, 0.2f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}

