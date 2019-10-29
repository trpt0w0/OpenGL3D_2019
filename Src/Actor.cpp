/**
*	@file Actor.cpp
*/
#include "Actor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
/**
*	�R���X�g���N�^
*
*	@param name		�A�N�^�[�̖��O
*	@param health	�ϋv��
*	@param position	�ʒu
*	@param rotation	��]
*	@param scale	�g�嗦
*
*	�w�肳�ꂽ���O�A�ϋv�́A�ʒu�A��]�A�g�嗦�ɂ���ăA�N�^�[������������
*/
Actor::Actor(const std::string& name, int health,
	const glm::vec3& position,const glm::vec3& rotation,
	const glm::vec3& scale) 
	: name(name), health(health),position(position),rotation(rotation),scale(scale)
{
}

/**
*	�A�N�^�[�̏�Ԃ��X�V����
*	
*	@param deltaTime	�o�ߎ���
*
*	UpdateDrawData()�����O�Ɏ��s���邱��
*/
void Actor::Update(float deltaTime) {
	
	position += velocity * 0.01f;

	// �Փ˔���̍X�V
	const glm::mat4 matT = glm::translate(glm::mat4(1), position);
	const glm::mat4 matR_Y = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0));
	const glm::mat4 matR_ZY	= glm::rotate(matR_Y, rotation.z, glm::vec3(0, 0, -1));
	const glm::mat4 matR_XZY = glm::rotate(matR_ZY,rotation.x, glm::vec3(1, 0, 0));
	const glm::mat4 matS = glm::scale(glm::mat4(1),scale);
	const glm::mat4 matModel = matT * matR_XZY * matS;
	colWorld.type = colLocal.type;
	switch (colLocal.type) {
	case Collision::Shape::Type::sphere:
		colWorld.s.center = matModel * glm::vec4(colLocal.s.center, 1);
		colWorld.s.r = colLocal.s.r;
		break;

	case Collision::Shape::Type::capsule:
		colWorld.c.seg.a = matModel * glm::vec4(colLocal.c.seg.a, 1);
		colWorld.c.seg.b = matModel * glm::vec4(colLocal.c.seg.b, 1);
		colWorld.c.r = colLocal.c.r;
		break;

	case Collision::Shape::Type::obb :
		colWorld.obb.center = matModel * glm::vec4(colLocal.obb.center, 1);
		for (size_t i = 0; i < 3; ++i) {
			colWorld.obb.axis[i] = matR_XZY * glm::vec4(colLocal.obb.axis[i], 1);
		}
		colWorld.obb.e = colLocal.obb.e;
		break;
	}

}

/**
*	�`����̍X�V
*
*	@param deltaTime	�o�ߎ���
*
*	Update()�̌�Ŏ��s���邱��
*/
void Actor::UpdateDrawData(float deltaTime) {

}

/**
*	�A�N�^�[�̕`��
*/
void Actor::Draw() {

}


/**
*	�R���X�g���N�^
*
*	@param m		�\�����郁�b�V��
*	@param name		�A�N�^�[�̖��O	
*	@param health	�ϋv�l
*	@param position	�ʒu
*	@param rotation	��]	
*	@param sclae	�g�嗦
*
*	�w�肳�ꂽ���b�V���A���O�A�ϋv�́A�ʒu�A��]�A�g�嗦�ɂ���ăA�N�^�[������������.
*/
StaticMeshActor::StaticMeshActor(const Mesh::FilePtr& m, const std::string& name, int health,
	const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	:Actor(name, health, position, rotation, scale), mesh(m) 
{
}

/**
*	 �`��
*/
void StaticMeshActor::Draw() {

	if (mesh) {
		
		const glm::mat4 matT = glm::translate(glm::mat4(1), position);
		const glm::mat4 matR_Y = glm::rotate(glm::mat4(1), rotation.y, glm::vec3(0, 1, 0));
		const glm::mat4 matR_ZY = glm::rotate(matR_Y, rotation.z, glm::vec3(0, 0, -1));
		const glm::mat4 matR_XZY = glm::rotate(matR_ZY, rotation.x, glm::vec3(1, 0, 0));
		const glm::mat4 matS = glm::scale(glm::mat4(1), scale);
		const glm::mat4 matModel = matT * matR_XZY * matS;
		if (!mesh->materials.empty()) {
			const Shader::ProgramPtr p = mesh->materials[0].program;
			if (p) {
				p->Use();
				p->SetPointLightIndex(pointLightCount, pointLightIndex);
				p->SetSpotLightIndex(spotLightCount, spotLightIndex);
			}
			Mesh::Draw(mesh, matModel);
		}
	}
}
/**
*	�A�N�^�[�ɉe������|�C���g���C�g�̃C���f�b�N�X��ݒ肷��
*
*	@param v	�|�C���g���C�g�̃C���f�b�N�X�z��
*/
void StaticMeshActor::SetPointLightList(const std::vector<int>& v) {
	
	pointLightCount = v.size();
	for (int i = 0; i < 8 && i < static_cast<int>(v.size()); ++i) {
		pointLightIndex[i] = v[i];
	}
}

/**
*	�A�N�^�[�ɉe������X�|�b�g���C�g�̃C���f�b�N�X��ݒ肷��
*
*	@param v	�X�|�b�g���C�g�̃C���f�b�N�X�z��
*
*/
void StaticMeshActor::SetSpotLightList(const std::vector<int>& v) {
	
	spotLightCount = v.size();
	for (int i = 0; i < 8 && i < static_cast<int>(v.size()); ++i) {
		spotLightIndex[i] = v[i];
	}
}




/**
*	�i�[�\�ȃA�N�^�[�����m�ۂ���
*
*	@param reserveCount	�A�N�^�[�̔z��̊m�ې�
*/
void ActorList::Reserve(size_t reserveCcunt) {
	actors.reserve(reserveCcunt);
}

/**
*	�A�N�^�[��ǉ�����
*
*	@param	actor �ǉ�����A�N�^�[
*/
void ActorList::Add(const ActorPtr& actor) {
	actors.push_back(actor);
}

/**
*	�A�N�^�[���폜����
*
*	@param actor�@�폜����A�N�^�[
*/
bool ActorList::Remove(const ActorPtr& actor) {
	for (auto itr = actors.begin(); itr != actors.end(); ++itr) {
		if (*itr == actor) {
			actors.erase(itr);
			return true;
		}
	}
	return false;
}

/**
*	�w�肳�ꂽ���W�ɑΉ�����i�q�̃C���f�b�N�X���擾����
*	
*	@param pos	�C���f�b�N�X�̌��ɂȂ�ʒu
*
*	@return pos�ɑΉ�����i�q�̃C���f�b�N�X
*/
glm::ivec2 ActorList::CalcMapIndex(const glm::vec3& pos)const {
	
	const int x = std::max(0, std::min(sepalationSizeX -1, static_cast<int>(pos.x / mapGridSizeX)));
	const int y = std::max(0, std::min(sepalationSizeY -1, static_cast<int>(pos.x / mapGridSizeY)));
	
	return glm::ivec2(x, y);

}


/**
*	�A�N�^�[�̏�Ԃ��X�V����
*
*	@param deltaTime	�O��̍X�V����̌o�ߎ���
*/
void ActorList::Update(float deltaTime) {
	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->Update(deltaTime);
		}
	}

	// ���S�����A�N�^�[���폜����
	for (auto i = actors.begin(); i != actors.end();) {
		const ActorPtr& e = *i;
		if (!e || e->health <= 0) {
			i = actors.erase(i);
		} else {
			++i;
		}
	}

	// �i�q���ԂɃA�N�^�[���蓖�Ă�
	for (int y = 0; y < sepalationSizeY; ++y) {
		for (int x = 0; x < sepalationSizeX; ++x) {
			grid[y][x].clear();
		}
	}
	for (auto i = actors.begin(); i != actors.end(); ++i) {
		const glm::ivec2 mapIndex = CalcMapIndex((*i)->position);
		grid[mapIndex.y][mapIndex.x].push_back(*i);
	}

}
/**
*	�A�N�^�[�̕`��f�[�^���X�V����
*
*	@param deltaTime �O��̍X�V����̌o�ߎ���
*/
void ActorList::UpdateDrawData(float deltaTime) {
	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->UpdateDrawData(deltaTime);
		}
	}
}

/**
*	Actor��`�悷��
*/
void ActorList::Draw() {
	for (const ActorPtr& e : actors) {
		if (e && e->health > 0) {
			e->Draw();
		}
	}
}

/**
*	�w�肳�ꂽ���W�̋ߖT�ɂ���A�N�^�[�̃��X�g���擾����
*
*	@param pos			�����̊�_�ƂȂ���W
*	@param maxDistance	�ߖT�Ƃ݂Ȃ��ő勗��(m)
*
*	@return		Actor::position��pos���甼�amaxDistance�ȓ��ɂ���A�N�^�[�̔z��
*/
std::vector<ActorPtr> ActorList::FindNearbyActors(const glm::vec3& pos, float maxDistance) const {
	
	std::vector<std::pair<float, ActorPtr>> buffer;
	buffer.reserve(1000);

	const glm::ivec2 mapIndex = CalcMapIndex(pos);
	const glm::ivec2 min = glm::max(mapIndex - 1, 0);
	const glm::ivec2 max = glm::min(mapIndex + 1, glm::ivec2(sepalationSizeX -1, sepalationSizeY - 1));
	for (int y = min.y; y <= max.y; ++y) {
		for (int x = min.x; x <= max.x; ++x) {
			const std::vector<ActorPtr>&list = grid[y][x];
			for (auto actor : list) {
				const float distance = glm::distance(glm::vec3(actor->position), pos);
				buffer.push_back(std::make_pair(distance, actor));
			}

		}
	}

	std::vector<ActorPtr> result;
	result.reserve(1000);
	for (const auto& e : buffer) {
		if (e.first <= maxDistance) {
			result.push_back(e.second);
		}
	}

	return result;
}



/**
*	�Փ˔�����s��
*
*	@param a		����Ώۂ̃A�N�^�[����1
*	@param b		����Ώۂ̃A�N�^�[����2
*	@param handler	�Փ˂����ꍇ�Ɏ��s�����֐�
*/

void DetectCollision(const ActorPtr& a,const ActorPtr& b, CollisionHandlerType handler) {
	if (a->health <= 0 || b->health <= 0) {
		return;
	}
	
	glm::vec3 pa, pb;
	if (Collision::TestShapeShape(a->colLocal, b->colLocal, &pa, &pb)) {
		if (handler) {
			handler(a, b, pb);

		} else {
			//a->OnHit(b, pb);
			//b->OnHit(a, pa);
		}
	}

}

/**
*	�Փ˔�����s��
*
*	@param a		����Ώۂ̃A�N�^�[
*	@param b		����Ώۂ̃A�N�^�[���X�g
*	@param handler	�Փ˂����ꍇ�Ɏ��s�����֐�
*/
void DetectCollision(const ActorPtr& a, ActorList& b, CollisionHandlerType handler) {
	
	if (a->health <= 0) {
		return;
	}
	for (const ActorPtr& actorB : b) {
		if (actorB->health <= 0) {
			continue;
		}

		glm::vec3 pa, pb;
		if (Collision::TestShapeShape(a->colWorld, actorB->colWorld, &pa, &pb)) {
			if (handler) {
				handler(a, actorB, pb);
			}
			else
			{
				a->OnHit(actorB, pb);
				actorB->OnHit(a, pa);

			}

			if (a->health <= 0) {
				break;
			}
		}
			
		
	}
}

/**
*	�Փ˔�����s��
*
*	@param a		����ΏۃA�N�^�[���X�g����1
*	@param b		����ΏۃA�N�^�[���X�g����2
*	@param handler	�Փ˂����ꍇ�Ɏ��s�����֐�
*/
void DetectCollision(ActorList& a, ActorList& b, CollisionHandlerType handler) {
	for (const ActorPtr& actorA : a) {
		if (actorA->health <= 0) {
			continue;
		}
		for (const ActorPtr& actorB : b) {
			if (actorB->health <= 0) {
				continue;
			}

			glm::vec3 pa, pb;
			if (Collision::TestShapeShape(actorA->colWorld, actorB->colWorld, &pa, &pb)) {

				if (handler) {
					handler(actorA, actorB, pa);
				}
				else {
					actorA->OnHit(actorB, pb);
					actorB->OnHit(actorA, pa);
				}


				if (actorA->health <= 0) {
					break;
				}

			}
			
		}
	}

}