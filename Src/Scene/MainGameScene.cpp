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

/**
*	衝突を解決する
*
*	@param a	衝突したアクターその1
*	@param b	衝突したアクターその2
*	@param p	衝突位置
*/
void PlayerCollisionHandler(const ActorPtr& a, const ActorPtr& b, const glm::vec3& p){
	
	const glm::vec3 v = a->colWorld.s.center - p;
	// 衝突位置との距離が近すぎないか調べる
	if (dot(v,v) > FLT_EPSILON) {
		// aをbに重ならない位置まで移動
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
		// 移動を取り消す（距離が近すぎる場合の例外処理）
		const float deltaTime = static_cast<float> (GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = a->velocity * deltaTime;
		a->position -= deltaVelocity;
		a->colWorld.s.center -= deltaVelocity;
	}

}

/**
*	シーンを初期化する
*
*	@retval	true	初期化成功
*	@retval	false	初期化失敗
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
	meshBuffer.LoadMesh("Res/wall_stone.gltf");
	meshBuffer.LoadMesh("Res/jizo_statue.gltf");
	meshBuffer.LoadSkeletalMesh("Res/bikuni.gltf");
	meshBuffer.LoadSkeletalMesh("Res/oni_small.gltf");
	

	// ハイマップを作成する
	if (!heightMap.LoadFromFile("Res/Terrain.tga", 20.0f, 0.5f)) {
		return false;
	}

	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}

	
	glm::vec3 startPos(100, 0, 100);
	startPos.y = heightMap.Height(startPos);
	player = std::make_shared<PlayerActor>(&heightMap, meshBuffer, startPos);
	
	rand.seed(0);

	// お地蔵様を配置
	
	for (int i = 0; i < 4; ++i) {
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(50, 150)(rand));
		position.z = static_cast<float>(std::uniform_int_distribution<>(50, 150)(rand));
		position.y = heightMap.Height(position);
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0.0f, 3.14f * 2.0f)(rand);
		JizoActorPtr p = std::make_shared<JizoActor>(
			meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
		p->scale = glm::vec3(3);	// 見つけやすいように拡大
		objects.Add(p);


	}

	const size_t oniCount = 100;
	enemies.Reserve(oniCount);

#if 0
	for (size_t i = 0; i < oniCount; ++i) {
		// 敵の位置を(50,50)-(150,150)の範囲からランダムに選択
		glm::vec3 position(0);
		position.x = std::uniform_real_distribution<float>(50, 150)(rand);
		position.z = std::uniform_real_distribution<float>(50, 150)(rand);
		position.y = heightMap.Height(position);
		// 敵の向きをランダムに選択
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 6.3f)(rand);
		const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
		SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(
			mesh, "Kooni", 13, position, rotation);
		p->GetMesh()->Play("Run");
		p->colLocal = Collision::CreateCapsule(
			glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
		enemies.Add(p);
	}
#endif

	
	// 石壁を配置
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
	rand.seed(0);

		
	
	// 木を配置
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
*	プレイヤーの入力を処理する	
*/
void MainGameScene::ProcessInput() {
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	// プレイヤー操作
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
*	シーンを更新する
*
*	@param deltatime	前回の更新からの経過時間(秒)
*/
void MainGameScene::Update(float deltaTime) {

	const GLFWEW::Window& window = GLFWEW::Window::Instance();


	//カメラの状態の更新
	
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
	// プレイヤーの攻撃判定
	ActorPtr attackCollision = player->GetAttackCollision();
	if (attackCollision) {
		bool hit = false;
		DetectCollision(attackCollision, enemies,[this, &hit](const ActorPtr& a, const ActorPtr& b, const glm::vec3& p){
			SkeletalMeshActorPtr bb = std::static_pointer_cast<SkeletalMeshActor>(b);
			bb->health -= a->health;
			if (bb->health <= 0) {
				bb->colLocal = Collision::Shape{};
				bb->health = 1;
				bb->GetMesh()->Play("Down", false);
			} else {
				bb->GetMesh()->Play("Hit", false);
			}
			hit = true;
		}
		);	
		if (hit) {
			attackCollision->health = 0;

		}
	}

	// 死亡アニメーションの終わった敵を消す
	for (auto& e : enemies) {
		SkeletalMeshActorPtr enemy = std::static_pointer_cast<SkeletalMeshActor>(e);
		Mesh::SkeletalMeshPtr mesh = enemy->GetMesh();
		if (mesh->IsFinished()) {
			if (mesh->GetAnimation() == "Down") {
				enemy->health = 0;
			} else {
				mesh->Play("Wait");
			}
		}

	}

	// 敵を全滅させたら目的達成フラグをtrueにする
	if (jizoId >= 0) {
		if (enemies.Empty()) {
			achivements[jizoId] = true;
			jizoId = -1;
		}
	}


	//ゲームクリア画面の切り替え(クリア画面がないのでゲームオーバー画面)
	if (timer > 0) {
		timer -= deltaTime;
		if (timer == 0.0f) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
		}

	}

	if (timer == 0.0f && achivements) {
		timer = 1.0f;
	}

	
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
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"メイン画面");
	fontRenderer.AddString(glm::vec2(-128, 0), L"アクションゲーム");
	fontRenderer.EndUpdate();
}



/**
*	シーンを描画する
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

/**
*	お地蔵様に触れたときの処理
*
*	@param id	お地蔵様の番号
*	@param pos	お地蔵様の座標
*
*	@retval true	処理成功
*	@retval false	処理失敗
*/
bool MainGameScene::HandleJizoEffescts(int id, const glm::vec3& pos) {
	
	if (jizoId >= 0) {
		return false;
	}
	jizoId = id;

	const size_t oniCount = 8;
	for (size_t i = 0; i < oniCount; ++i) {
		glm::vec3 position(pos);
		position.x += std::uniform_real_distribution<float>(-15, 15)(rand);
		position.z += std::uniform_real_distribution<float>(-15, 15)(rand);
		position.y = heightMap.Height(position);
		// 敵の向きをランダムに選択
		
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 3.14f * 2.0f)(rand);
		const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
		SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(
			mesh, "Kooni", 13, position, rotation);
		p->GetMesh()->Play("Wait");
		p->colLocal = Collision::CreateCapsule(
			glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
		enemies.Add(p);
	}

	return true;
}