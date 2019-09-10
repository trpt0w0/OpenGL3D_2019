/**
*	@file MainGameScene.h
*/

#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include "../Scene.h"
#include "../Sprite.h"
#include "../Font.h"
#include "../Mesh.h"
#include "../Terrain.h"
#include "../Actor.h"
#include "../PlayerActor.h"
#include "../Audio/Audio.h"
#include "../JizoActor.h"
#include <random>
#include <vector>


/**
*	メインゲーム画面
*/
class MainGameScene : public Scene {

public:
	MainGameScene():Scene("MainGameScene"){}
	virtual ~MainGameScene() = default;
	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	bool HandleJizoEffescts(int id, const glm::vec3& pos);


private:
	bool flag = false;
	int jizoId = -1;	// 現在戦闘中のお地蔵様のID
	bool achivemests[4] = { false, false, false, false };	// 敵討伐状態
	std::mt19937 rand;
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Mesh::Buffer meshBuffer;
	Terrain::HeightMap heightMap;
	PlayerActorPtr player;
	ActorList enemies;
	ActorList trees;
	ActorList objects;
	Audio::SoundPtr bgm;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);
	};
	Camera camera;

};

#endif // MAINGAMESCENE_H_INCLUDED

