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

		// ESC�L�[�������ꂽ��I���E�B���h�E��\��
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "�Q�[���I�����܂����H", "�I��", MB_OKCANCEL) == IDOK) {
				break;
			}
		}
		

		// �o�b�N�o�b�t�@���폜����
		glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GL�R���e�N�X�g�̃p�����[�^��ݒ�
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);


		sceneStack.Render();

		window.SwapBuffers();
	}

	return 0;
}

