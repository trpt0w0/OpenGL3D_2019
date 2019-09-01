/**
*	@file PlayerActor.cpp
*/
#include "PlayerActor.h"

/**
*	�R���X�g���N�^
*/
PlayerActor::PlayerActor(const Mesh::FilePtr& mesh,
	const glm::vec3& pos, const glm::vec3& rot, const Terrain::HeightMap* hm)
	: StaticMeshActor(mesh, "Player", 20, pos, rot), heightMap(hm) 
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);
}

/**
*	�X�V
*
*	@param deltaTime �o�ߎ���
*/
void PlayerActor::Update(float deltaTime) {

	// ���W�̍X�V
	StaticMeshActor::Update(deltaTime);

	// �ڒn����
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y <= groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
	}else if(position.y > groundHeight) {
		velocity.y -= gravity * deltaTime;
	}

}

/**
*	�W�����v������
*/

void PlayerActor::Jump() {
	velocity.y = 5.0f;
}