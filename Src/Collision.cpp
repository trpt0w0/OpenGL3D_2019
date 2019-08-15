/**
*	@file Collsion.cpp
*/

#include "Collision.h"

namespace Collision {
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


}	// namespace Collision