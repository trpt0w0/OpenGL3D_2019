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


	//if (!window.Init(800, 600, "Title")) {
	if(!window.Init(1600, 1200,"Title")){
	return 1;
	}

	// ���y�Đ��v���O����������������
	Audio::Engine& audioEngine = Audio::Engine::Instance();
	if (!audioEngine.Initialize()) {
		return 1;
	}

	// �X�P���^�����b�V���E�A�j���[�V�����𗘗p�\�ɂ���
	if (!Mesh::SkeletalAnimation::Initialize()) {
		return 1;
	}

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());
	

	for (;!window.ShouldClose();) {

	
		window.Update();
		window.UpdateGamePad();
		//window.UpdateDeltaTime();
		const float deltaTime = window.DeltaTime();
		window.UpdateTimer();
		// �X�P���^���E�A�j���[�V�����p�f�[�^�̍쐬����
		Mesh::SkeletalAnimation::ResetUniformData();

		sceneStack.Update(deltaTime);

		// �X�P���^���E�A�j���[�V�����p�f�[�^��GPU�Ƀ������ɓ]��
		Mesh::SkeletalAnimation::UploadUniformData();

		// �����Đ��v���O�������X�V����
		audioEngine.Update();

		// �o�b�N�o�b�t�@���폜����
		glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GL�R���e�N�X�g�̃p�����[�^��ݒ�
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// ESC�L�[�������ꂽ��I���E�B���h�E��\��
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, "�Q�[���I�����܂����H", "�I��", MB_OKCANCEL) == IDOK) {
				break;
			}
		}


		sceneStack.Render();

		window.SwapBuffers();
	}

	// �X�P���^���E�A�j���[�V�����̗��p���I������
	Mesh::SkeletalAnimation::Finalize();

	// �����Đ��v���O�������I������
	audioEngine.Finalize();

	return 0;
}

