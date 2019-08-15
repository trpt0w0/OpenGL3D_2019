/**
*	@file Collision.h
*/

#ifndef COLLISION_H_ICLUDED
#define COLLISION_H_ICLUDED
#include <GL/glew.h>
#include <glm/glm.hpp>


namespace Collision {

	/**
	*	ãÖ
	*/
	struct Sphere {
		glm::vec3 center = glm::vec3(0);	// íÜêSç¿ïW
		float r = 0;	// îºåa
	};

	bool TestSphereSphere(const Sphere&, const Sphere&);

}

#endif	// COLLISION_H_ICLUDED