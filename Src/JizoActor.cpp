/**
*	@file jizoActor.cpp
*/

#include "JizoActor.h"
#include "Scene/MainGameScene.h"

/**
*	コンストラクタ
*/

JizoActor::JizoActor(const Mesh::FilePtr& m, const glm::vec3& pos, int id, 
	MainGameScene* p) : 
	StaticMeshActor(m, "Jizo", 100, pos, glm::vec3(0), glm::vec3(1)),
	id(id),
	parent(p) 
{
	colLocal = Collision::CreateCapsule(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0), 0.5f);
}

/**
*	衝突判定
*/
void JizoActor::OnHit(const ActorPtr& other, const glm::vec3& p) {
	
	// ギミック起動済みの場合を何もしない
	if (isActivated) {
		return;
	}

	// 起動に成功したら起動フラグをtrueにする
	if (parent->HandleJizoEffescts(id, position)) {
		isActivated = true;

	}


}