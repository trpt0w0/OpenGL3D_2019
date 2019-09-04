#ifndef ENEMYACTOR_H_INCLUDED
#define ENEMYACTOR_H_INCLUDED

#include "Actor.h"

class EnemyActor : public StaticMeshActor {

public:
	EnemyActor(const Mesh::FilePtr& m, const std::string& name, int hp, const glm::vec3& pos,
		const glm::vec3& rot = glm::vec3(0), const glm::vec3& scale = glm::vec3(1))
		: StaticMeshActor(m, name, hp, pos, rot, scale)
	{
		colLocal = Collision::CreateCapsule(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0), 0.5f);

	}
	virtual ~EnemyActor() = default;

	virtual void OnHit(const ActorPtr& other, const glm::vec3& p)override {
		if (!isAchieved) {
			isAchieved = true;
		}
	}

	bool IsAchieved() const { return isAchieved; }

private:
	bool isAchieved = false;

};

using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED
