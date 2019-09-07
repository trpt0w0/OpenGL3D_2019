/**
*	@file MainGameScene.cpp
*/

#include "../GLFWEW.h"
#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "../Mesh.h"
#include "../SkeletalMeshActor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <random>



/**
*	�Փ˂���������
*
*	@param a	�Փ˂����A�N�^�[����1
*	@param b	�Փ˂����A�N�^�[����2
*	@param p	�Փˈʒu
*/
void PlayerCollisionHandler(const ActorPtr& a, const ActorPtr& b, const glm::vec3& p){
	
	const glm::vec3 v = a->colWorld.s.center - p;
	// �Փˈʒu�Ƃ̋������߂����Ȃ������ׂ�
	if (dot(v,v) > FLT_EPSILON) {
		// a��b�ɏd�Ȃ�Ȃ��ʒu�܂ňړ�
		const glm::vec3 vn = normalize(v);
		float radiusSum = a->colWorld.s.r ;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere : radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;
		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		a->position += vn * distance;
		a->colWorld.s.center += vn * distance;
		if (a->velocity.y < 0 && vn.y >= glm::cos(glm::radians(60.0f))) {
			a->velocity.y = 0;
		}
	} else {
		// �ړ����������i�������߂�����ꍇ�̗�O�����j
		const float deltaTime = static_cast<float> (GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = a->velocity * deltaTime;
		a->position -= deltaVelocity;
		a->colWorld.s.center -= deltaVelocity;
	}

}

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
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/MainGameBgm.mp3");
	bgm->Play(Audio::Flag_Loop);

	
	Sprite spr(Texture::Image2D::Create("Res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);
	
	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));
	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadSkeletalMesh("Res/bikuni.gltf");
	meshBuffer.LoadMesh("Res/wall_stone.gltf");
	meshBuffer.LoadSkeletalMesh("Res/oni_small.gltf");
	

	// �n�C�}�b�v���쐬����
	if (!heightMap.LoadFromFile("Res/Terrain.tga", 20.0f, 0.5f)) {
		return false;
	}

	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}

	//
	glm::vec3 startPos(100, 0, 100);
	startPos.y = heightMap.Height(startPos);
	player = std::make_shared<PlayerActor>(&heightMap, meshBuffer, startPos);

	
	// �Εǂ�z�u
	{
		const Mesh::FilePtr meshStoneWall = meshBuffer.GetFile("Res/wall_stone.gltf");
		glm::vec3 position = startPos + glm::vec3(3, 0, 3);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
			meshStoneWall, "StoneWall", 100, position, glm::vec3(0, 0.5f, 0));
		p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(2, 2, 0.5f));
		objects.Add(p);
	
	}

	std::mt19937 rand;
	rand.seed(0);

	// �G��z�u
	{
		const size_t oniCount = 30;
		enemies.Reserve(oniCount);
		for (size_t i = 0; i < oniCount; ++i) {
			// �G�̈ʒu��(50,50)-(150,150)�͈̔͂��烉���_���ɑI��
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(50, 150)(rand);
			position.z = std::uniform_real_distribution<float>(50, 150)(rand);
			position.y = heightMap.Height(position);
			// �G�̌����������_���ɑI��
			glm::vec3 rotation(0);
			rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(rand);
			const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
			SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(
				mesh, "Kooni", 13, position,rotation);
			p->GetMesh()->Play("Run");
			p->colLocal = Collision::CreateCapsule(
				glm::vec3(0, 0.5f, 0), glm::vec3(0,1,0), 0.5f );
			enemies.Add(p);

		}	
	}
	// �؂�z�u
	{
		const size_t treeCount = 50;
		trees.Reserve(treeCount);
		const Mesh::FilePtr meshtree = meshBuffer.GetFile("Res/red_pine_tree.gltf");
		for (int i = 0; i <= treeCount; ++i) {
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(50, 150)(rand);
			position.z = std::uniform_real_distribution<float>(50, 150)(rand);
			position.y = heightMap.Height(position);
			StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(meshtree, "trees", 100, position);
			p->colLocal = Collision::CreateCapsule(glm::vec3(0), glm::vec3(0, 4, 0), 0.5f);
			trees.Add(p);

		}
	}
	return true;
}


/**
*	�v���C���[�̓��͂���������	
*/
void MainGameScene::ProcessInput() {
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	// �v���C���[����
	player->ProcessInput();

	if (!flag) {
		if (window.GetGamePad().buttonDown & GamePad::START) {
			flag = true;
			Audio::Engine::Instance().Prepare("Res/Audio/StatusSoundEffect.mp3")->Play();
			SceneStack::Instance().Push(std::make_shared<StatusScene>());
		}
	} else{
		bgm->Stop();
		SceneStack::Instance().Replace(std::make_shared<GameOverScene>());

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
	
	{
		camera.target = player->position;
		camera.position = camera.target + glm::vec3(0, 30, 30);

	}

	player->Update(deltaTime);
	enemies.Update(deltaTime);
	trees.Update(deltaTime);
	objects.Update(deltaTime);

	DetectCollision(player, enemies);
	DetectCollision(player, trees);
	DetectCollision(player, objects);
	
	//auto err = glGetError();

	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);
	trees.UpdateDrawData(deltaTime);
	objects.UpdateDrawData(deltaTime);

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
	
	player->Draw();
	enemies.Draw();
	trees.Draw();
	objects.Draw();
}