/**
*	@file Collision.h
*/

#ifndef COLLISION_H_ICLUDED
#define COLLISION_H_ICLUDED
#include <GL/glew.h>
#include <glm/glm.hpp>


namespace Collision {

	/**
	*	��
	*/
	struct Sphere {
		glm::vec3 center = glm::vec3(0);	// ���S���W
		float r = 0;	// ���a
	};

	/**
	*	����
	*/
	struct Segment {
		glm::vec3 a = glm::vec3(0);	// �����̎n�_
		glm::vec3 b = glm::vec3(0);	// �����̏I�_
	};

	/**
	*	�J�v�Z��
	*/
	struct Capsule {
		Segment seg;	// �~�����̒��S�̐���
		float r = 0;	// �J�v�Z���̔��a
	};

	/**
	*	�ėp�Փˌ`��
	*/
	struct Shape {
		enum class Type {
			none,		// �`��Ȃ�
			sphere,		// ��
			capsule,	// �J�v�Z��
		};
		Type type = Type::none;	// ���ۂ̌`��
		Sphere s;				// ���̌`��f�[�^
		Capsule c;				// �J�v�Z���̌`��f�[�^
	};

	// �`��쐬�֐�
	Shape CreateSphere(const glm::vec3&, float);
	Shape CreateCapsule(const glm::vec3&, const glm::vec3&, float);

	bool TestSphereSphere(const Sphere&, const Sphere&);
	bool TestSphereCapsule(const Sphere& s, const Capsule& c, glm::vec3* p);
	bool TestShapeShape(const Shape&, const Shape&, glm::vec3* pa, glm::vec3* pb);
}

#endif	// COLLISION_H_ICLUDED