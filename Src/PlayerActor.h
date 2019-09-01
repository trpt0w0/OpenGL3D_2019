/**
*	@file PlayerActor.h
*/
#ifndef PLAYERACTOR_H_INCLUDED
#define PLAYERACTOR_H_INCLUDED
#include "Actor.h"
#include "Terrain.h"
#include <memory>
/**
*	プレイヤーアクター
*/
class PlayerActor : public StaticMeshActor {
public:
	PlayerActor(const Mesh::FilePtr& mesh,
		const glm::vec3& pos, const glm::vec3& rot, const Terrain::HeightMap* hm);
	virtual ~PlayerActor() = default;

	virtual void Update(float) override;
	void Jump();

private:
	const Terrain::HeightMap* heightMap = nullptr;

};
using PlayerActorPtr = std::shared_ptr<PlayerActor>;


#endif	// PLAYERACTOR_H_INCLUDED