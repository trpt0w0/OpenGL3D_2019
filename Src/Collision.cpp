/**
*	@file Collsion.cpp
*/

#include "Collision.h"

namespace Collision {

	/**
	*	���`����쐬����
	*
	*	@param	center	���̒��S���W
	*	@param	r		���̔��a
	*
	*	@return	����ێ�����ėp�Փˌ`��I�u�W�F�N�g
	*/
	Shape CreateSphere(const glm::vec3& center, float r) {
		
		Shape result;
		result.type = Shape::Type::sphere;
		result.s = Sphere{ center,r };
		return result;
	}

	/**
	*	�J�v�Z���`����쐬����
	*
	*	@param a	���S�̐����̎n�_���W
	*	@param b	���S�̐����̏I�_���W	
	*	@param r	�J�v�Z���̔��a
	*
	*	@return �J�v�Z����ێ�����ėp�Փˌ`��I�u�W�F�N�g
	*/
	Shape CreateCapsule(const glm::vec3& a, const glm::vec3& b, float r) {
		
		Shape result;
		result.type = Shape::Type::capsule;
		result.c = Capsule{ { a, b }, r };
		return result;
	}

	
	/**
	*	���Ƌ����Փ˂��Ă��邩���ׂ�
	*
	*	@param s0	����Ώۂ̋�����1
	*	@param s1	����Ώۂ̋�����2
	*
	*	@retval true	�Փ˂��Ă���
	*	@retval false	�Փ˂��ĂȂ�
	*/

	bool TestSphereSphere(const Sphere& s0, const Sphere& s1) {
		const glm::vec3 m = s0.center - s1.center;
		const float radiusSum = s0.r + s1.r;
		return glm::dot(m, m) <= radiusSum * radiusSum;
	}

	/**
	*	�����Ɠ_�̍ŋߐړ_�𒲂ׂ�
	*
	*	@param seg	����
	*	@param p	�_
	*
	*	@return seg��p�̍ŋߐړ_
	*/
	glm::vec3 ClosePointSegment(const Segment& seg, const glm::vec3& p) {
		const glm::vec3 ab = seg.b - seg.a;
		const glm::vec3 ap = p - seg.a;
		const float lenAQ = glm::dot(ab, ap);
		const float lenAB = glm::dot(ab, ab);
		if (lenAQ <= 0) {
			return seg.a;
		} else if(lenAQ >= lenAB) {
			return seg.b;
		}

		return seg.a + ab * (lenAQ / lenAB);
	}

	/**
	*	���ƃJ�v�Z�����Փ˂��Ă��邩���ׂ�
	*
	*	@param s	��
	*	@param c	�J�v�Z��
	*	@param p	�ŋߐړ_�̊i�[��
	*
	*	@retval	true	�Փ˂��Ă���
	*	@retval false	�Փ˂��Ă��Ȃ�
	*/
	bool TestSphereCapsule(const Sphere& s, const Capsule& c, glm::vec3* p) {
		
		*p = ClosePointSegment(c.seg, s.center);
		const glm::vec3 distance = *p - s.center;
		const float radiusSum = s.r + c.r;
		return glm::dot(distance, distance) <= radiusSum * radiusSum;	

	}
	
	/**
	*	�V�F�C�v���m���Փ˂��Ă��邩���ׂ�
	*	
	*	@param a	����Ώۂ̃V�F�C�v����1
	*	@param b	����Ώۂ̃V�F�C�v����2
	*	@param pa	�Փ˂������W
	*	@param pb	�Փ˂������W
	*
	*	@retval	true	�Փ˂���
	*	@retval false	�Փ˂��Ȃ�����
	*/
	bool TestShapeShape(const Shape& a, const Shape& b, glm::vec3* pa, glm::vec3* pb) {

		if (a.type == Shape::Type::sphere) {
			if (b.type == Shape::Type::sphere) {
				if (TestSphereSphere(a.s, b.s)) {
					*pa = a.s.center;
					*pb = b.s.center;
					return true;
				}
			} else if (b.type == Shape::Type::capsule) {
				if (TestSphereCapsule(a.s, b.c,pb)) {
					*pa = a.s.center;
					return true;
				}
			}
		}else if(a.type == Shape::Type::capsule) {
			if (b.type == Shape::Type::sphere) {
				if (TestSphereCapsule(b.s, a.c, pa)) {
					*pb = b.s.center;
					return true;
				}
			}
		}
		return false;
	}




}	// namespace Collision