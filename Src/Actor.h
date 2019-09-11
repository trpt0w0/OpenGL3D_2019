/**
*	@file Actor.h
*/

#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED
#include <GL/glew.h>
#include "Mesh.h"
#include "Collision.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

class Actor;
using ActorPtr = std::shared_ptr<Actor>;

/**
*	�V�[����z�u����I�u�W�F�N�g
*/

class Actor {
public:
	Actor(const std::string& name, int hp, const glm::vec3& pos,
		const glm::vec3& rot = glm::vec3(0),const glm::vec3& scale = glm::vec3(1));
	virtual ~Actor() = default;
	virtual void Update(float);
	virtual void UpdateDrawData(float);
	virtual void Draw();
	virtual void OnHit(const ActorPtr&, const glm::vec3&) {}

public:
	std::string name;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 velocity = glm::vec3(0);		// ���x
	int health = 0;		// �̗�
	Collision::Shape colLocal;
	Collision::Shape colWorld;

};

using ActorPtr = std::shared_ptr<Actor>;

/**
*	���b�V���\���@�\�t���A�N�^�[
*/

class StaticMeshActor : public Actor {

public:
	StaticMeshActor(const Mesh::FilePtr& m, const std::string& name, int hp,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0), 
		const glm::vec3& scale = glm::vec3(1));
	virtual ~StaticMeshActor() = default;

	virtual void Draw() override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&) {}
	const Mesh::FilePtr& GetMesh() const { return mesh; }

private:
	Mesh::FilePtr mesh;
};
using StaticMeshActorPtr = std::shared_ptr<StaticMeshActor>;


/**
*	�A�N�^�[���܂Ƃ߂đ��삷��N���X
*/

class ActorList {
public:
	// �C�e���[�^�[���`����
	using iterator = std::vector<ActorPtr>::iterator;
	using const_iterator = std::vector<ActorPtr>::const_iterator;

	ActorList() = default;
	~ActorList() = default;

	void Reserve(size_t);
	void Add(const ActorPtr&);
	bool Remove(const ActorPtr&);
	void Update(float);
	void UpdateDrawData(float);
	void Draw();
	bool Empty() { return actors.empty(); }

	// �C�e���[�^�[���擾����֐�
	iterator begin() { return actors.begin(); }
	iterator end() { return actors.end(); }
	const_iterator begin() const { return actors.begin(); }
	const_iterator end() const { return actors.end(); }


private:
	std::vector<ActorPtr> actors;

};

using CollisionHandlerType =
std::function<void(const ActorPtr&, const ActorPtr&, const glm::vec3&)>;
void DetectCollision(const ActorPtr& a, const ActorPtr& b, CollisionHandlerType handler = nullptr);
void DetectCollision(const ActorPtr& a, ActorList& b, CollisionHandlerType handler = nullptr);
void DetectCollision(ActorList& a, ActorList& b, CollisionHandlerType handler = nullptr);

class Enemy : public StaticMeshActor {
public:
	Enemy(const Mesh::FilePtr& m, const std::string& name, int hp,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0),
		const glm::vec3& scale = glm::vec3(1));
	~Enemy() = default;

	virtual void OnHit();



};
using EnemyPtr = std::shared_ptr<Enemy>;



#endif	// ACTOR_H_INCLUDED