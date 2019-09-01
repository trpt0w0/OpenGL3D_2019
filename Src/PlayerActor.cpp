/**
*	@file PlayerActor.cpp
*/
#include "PlayerActor.h"
#include "SkeletalMesh.h"
#include <glm/gtc/matrix_transform.hpp>
/**
*	�R���X�g���N�^
*
*	@param hm		�v���C���[�̒��n����Ɏg�p���鍂���}�b�v
*	@param buffer	�v���C���[�̃��b�V���f�[�^�������b�V���o�b�t�@	
*	@param pos		�v���C���[�̏������W
*	@param rot		�v���C���[�̏�������
*/
PlayerActor::PlayerActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
	const glm::vec3& pos, const glm::vec3& rot)
	: SkeletalMeshActor(buffer.GetSkeletalMesh("Bikuni"), "Player", 13, pos, rot), heightMap(hm)
{
	colLocal = Collision::CreateSphere(glm::vec3(0, 0.7f, 0), 0.7f);
	GetMesh()->Play("Idle");
	state = State::idle;
}

/**
*	�X�V
*
*	@param deltaTime �o�ߎ���
*/
void PlayerActor::Update(float deltaTime) {

	// ���W�̍X�V
	SkeletalMeshActor::Update(deltaTime);

	// �ڒn����
	static const float gravity = 9.8f;
	const float groundHeight = heightMap->Height(position);
	if (position.y <= groundHeight) {
		position.y = groundHeight;
		velocity.y = 0;
		isInAir = false;
	}else if(position.y > groundHeight) {

		// ����Ă��镨�̂��痣�ꂽ��󒆔���ɂ���
		if (boardingActor) {
			Collision::Shape col = colWorld;
			col.s.r += 0.1f;	// �Փ˔���������傫������
			glm::vec3 pa, pb;
			if (!Collision::TestShapeShape(col,boardingActor->colWorld, &pa, &pb)) {
				boardingActor.reset();
			}
		}

		// ��������
		const bool isFloating = position.y > groundHeight + 0.1f; // �n�ʂ��畂���Ă���
		if (!isInAir && isFloating && !boardingActor) {
			isInAir = true;
		}

		// �d�͂�������
		if (isInAir) {
			velocity.y -= gravity * deltaTime;
		}
	}

	// �A�j���[�V�����̍X�V
	switch (state) {
	case State::run:
		if (isInAir) {
			GetMesh()->Play("Jump");
			state = State::jump;
		} else {
			const float horizontalSpeed = velocity.x * velocity.x * velocity.z * velocity.z;
			if (horizontalSpeed == 0) {
				GetMesh()->Play("Idle");
				state = State::idle;
			}
		}
		break;

	case State::idle:
		if (isInAir) {
			GetMesh()->Play("Jump");
			state = State::jump;
		}
		else {
			const float horizontalSpeed = velocity.x * velocity.x * velocity.z * velocity.z;
			if (horizontalSpeed != 0) {
				GetMesh()->Play("Run");
				state = State::run;
			}
		}
		break;
	case State::jump:
		if (!isInAir) {
			GetMesh()->Play("Idle");
			state = State::idle;
		}
		break;
	}

}
/**
*	���͂���������
*/
void PlayerActor::ProcessInput() {
	const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
	CheckRun(gamepad);
	CheckJump(gamepad);
}
/**
*	�ړ��������������
*
*	@param gamepad	�Q�[���p�b�h����
*/
void PlayerActor::CheckRun(const GamePad& gamepad) {
	// �󒆂ɂ���Ƃ��͈ړ��ł��Ȃ�
	if (isInAir) {
		return;
	}

	// �����L�[����ړ��������v�Z
	const glm::vec3 front(0, 0, -1);
	const glm::vec3 left(-1, 0, 0);
	glm::vec3 move(0);
	if (gamepad.buttons & GamePad::DPAD_UP) {
		move += front;
	}else if(gamepad.buttonDown & GamePad::DPAD_DOWN) {
		move -= front;
	}

	if (gamepad.buttons & GamePad::DPAD_LEFT) {
		move += left;
	}else if(gamepad.buttons & GamePad::DPAD_RIGHT) {
		move -= left;
	}

	// �ړ����s���Ă�����A�ړ������ɉ����Č����Ƒ��x���X�V
	if (glm::dot(move,move)) {
		//�����X�V
		move = glm::normalize(move);
		rotation.y = std::atan2(-move.z, move.x) + glm::radians(90.0f);

		//���̂ɏ���Ă��Ȃ��Ƃ��͒n�`�̌��z���l�����Ĉړ������𒲐�����
		if (!boardingActor) {
			// �ړ������̒n�`�̌��z(gradient)���v�Z
			const float minGradient = glm::radians(-60.0f);	//�������Ƃ̂ł�����z�̍ŏ��l
			const float maxGradient = glm::radians(60.0f);	// �������Ƃ̂ł�����z�̍ő�l
			const float frontY =
				heightMap->Height(position * move * 0.05f) - position.y - 0.01f;
			const float gradient =
				glm::clamp(std::atan2(frontY, 0.05f), minGradient, maxGradient);

			//�n�`�ɉ����悤�Ɉړ����x��ݒ�
			const glm::vec3 axis = glm::normalize(glm::cross(move, glm::vec3(0, 1, 0)));
			move = glm::rotate(glm::mat4(1), gradient, axis) * glm::vec4(move, 1.0f);
		}
		velocity = move * moveSpeed;

	} else {
		// �ړ����Ă��Ȃ��̂ő��x��0�ɂ���
		velocity = glm::vec3(0);
	}
}

void PlayerActor::CheckJump(const GamePad& gamepad) {
	if (isInAir) {
		return;
	}

	if (gamepad.buttonDown & GamePad::B) {
		Jump();
	}
}


/**
*	�W�����v������
*/

void PlayerActor::Jump() {
	velocity.y = 5.0f;
	boardingActor.reset();
	isInAir = true;
}

/**
*	�v���C���[������Ă��镨�̂�ݒ肷��
*
*	@param p	����Ă��镨��
*/
void PlayerActor::SetBoardingActor(ActorPtr p) {
	boardingActor = p;
	if (p) {
		isInAir = false;
	}
}


/**
*	�Փ˃n���h���[
*/
void PlayerActor::OnHit(const ActorPtr& b, const glm::vec3& p) {

	const glm::vec3 v = colWorld.s.center - p;
	// �Փˈʒu�Ƃ̋������߂����Ȃ������ׂ�
	if (dot(v, v) > FLT_EPSILON) {
		// this��b�ɏd�Ȃ�Ȃ��ʒu�܂ňړ�
		const glm::vec3 vn = normalize(v);
		float radiusSum = colWorld.s.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;
		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		position += vn * distance;
		colWorld.s.center += vn * distance;
	} else {
		// �ړ����������i�������߂�����ꍇ�̗�O�����j
		const float deltaTime = static_cast<float> (GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = velocity * deltaTime;
		position -= deltaVelocity;
		colWorld.s.center -= deltaVelocity;
	}
	SetBoardingActor(b);
}