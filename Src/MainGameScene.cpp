/**
*	@file MainGameScene.cpp
*/

#include "GLFWEW.h"
#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <random>


/**
*	�V�[��������������
*
*	@retval	true	����������
*	@retval	false	���������s
*/
bool MainGameScene::Initialize() {
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	fontRenderer.Init(1000);
	sprites.reserve(100);
	fontRenderer.LoadFromFile("Res/font.fnt");

	Sprite spr(Texture::Image2D::Create("Res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);
	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));
	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadMesh("Res/bikuni.gltf");
	meshBuffer.LoadMesh("Res/oni_small.gltf");

	// �n�C�}�b�v���쐬����
	if (!heightMap.LoadFromFile("Res/Terrain.tga", 20.0f, 0.5f)) {
		return false;
	}

	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}

	glm::vec3 startPos(100, 0, 100);
	startPos.y = heightMap.Height(startPos);
	player = std::make_shared<StaticMeshActor>(
		meshBuffer.GetFile("Res/bikuni.gltf"), "Player", 20, startPos);

	std::mt19937 rand;
	rand.seed(0);

	// �G��z�u
	{
		const size_t oniCount = 100;
		enemies.Reserve(oniCount);
		const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/oni_small.gltf");
		for (size_t i = 0; i < oniCount; ++i) {
			// �G�̈ʒu��(50,50)-(150,150)�͈̔͂��烉���_���ɑI��
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(50, 150)(rand);
			position.z = std::uniform_real_distribution<float>(50, 150)(rand);
			position.y = heightMap.Height(position);
			// �G�̌����������_���ɑI��
			glm::vec3 rotation(0);
			rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(rand);
			StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
				mesh, "Kooni",13, position, rotation);
			enemies.Add(p);

		}
	}
	return true;
}


/**
*	�v���C���[�̓��͂���������	
*/
void MainGameScene::ProcessInput() {
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	// �J��������
	const GamePad gamepad = window.GetGamePad();

	glm::vec3 velocity(0);

	if (gamepad.buttons & GamePad::DPAD_LEFT) {
		velocity.x = -1;
	} else if (gamepad.buttons & GamePad::DPAD_RIGHT) {
		velocity.x = 1;
	}

	if (gamepad.buttons & GamePad::DPAD_DOWN) {
		velocity.z = 1;

	} else if (gamepad.buttons & GamePad::DPAD_UP) {
		velocity.z = -1;
	}

	if (velocity.x || velocity.z) {
		velocity = normalize(velocity) * 20.0f;
	}
	camera.velocity = velocity;


	if (!flag) {
		if (window.GetGamePad().buttonDown & GamePad::X) {
			flag = true;
			SceneStack::Instance().Push(std::make_shared<StatusScene>());
		}
	} 
}

/**
*	�V�[�����X�V����
*
*	@param deltatime	�O��̍X�V����̌o�ߎ���(�b)
*/
void MainGameScene::Update(float deltaTime) {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();


	//�J�����̏�Ԃ̍X�V
	if (dot(camera.velocity, camera.velocity)) {
		camera.target += camera.velocity * deltaTime;
		camera.target.y = heightMap.Height(camera.target);
		camera.position = camera.target + glm::vec3(0, 50, 50);

	}

	player->Update(deltaTime);
	enemies.Update(deltaTime);
	
	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);

	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();

	const float w = window.Width();
	const float h = window.Height();
	const float lineHeight = fontRenderer.LineHeight();


	fontRenderer.BeginUpdate();
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"���C�����");
	fontRenderer.AddString(glm::vec2(-128, 0), L"�A�N�V�����Q�[��");
	fontRenderer.EndUpdate();
}



/**
*	�V�[����`�悷��
*/
void MainGameScene::Render() {
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);

	fontRenderer.Draw(screenSize);


	const glm::mat4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	const float aspectRatio =
		static_cast<float>(window.Width()) / static_cast<float> (window.Height());
	const glm::mat4 matProj = 
		glm::perspective(glm::radians(30.0f), aspectRatio, 1.0f, 1000.0f);
	glm::vec3 cubePos(100, 0, 100);
	cubePos.y = heightMap.Height(cubePos);
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);

	Mesh::Draw(meshBuffer.GetFile("Cube"),matModel);
	meshBuffer.SetViewProjectionMatrix(matProj * matView);
	Mesh::Draw(meshBuffer.GetFile("Terrain"), glm::mat4(1));
	meshBuffer.SetViewProjectionMatrix(matProj * matView);
	glm::vec3 treePos(110, 0, 110);
	treePos.y = heightMap.Height(treePos);
	const glm::mat4 matTreeModel =
		glm::translate(glm::mat4(1), treePos) * glm::scale(glm::mat4(1), glm::vec3(3));
	Mesh::Draw(meshBuffer.GetFile("Res/red_pine_tree.gltf"), matTreeModel);
	meshBuffer.SetViewProjectionMatrix(matProj * matView);

	player->Draw();
	enemies.Draw();
}