/**
*	@file MainGameScene.h
*/

#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include "../Scene.h"
#include "../Sprite.h"
#include "../Font.h"
#include "../Mesh.h"
#include "../Terrain.h"
#include "../Actor.h"
#include "../PlayerActor.h"
#include "../Audio/Audio.h"
#include "../JizoActor.h"
#include "../Light.h"
#include "../FramebufferObject.h"
#include <random>
#include <vector>


/**
*	���C���Q�[�����
*/
class MainGameScene : public Scene {

public:
	MainGameScene():Scene("MainGameScene"){}
	virtual ~MainGameScene() = default;
	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

	bool HandleJizoEffescts(int id, const glm::vec3& pos);


private:
	bool flag = false;
	int jizoId = -1;	// ���ݐ퓬���̂��n���l��ID
	bool achivements[4] = { false, false, false, false };	// �G�������
	std::mt19937 rand;
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Mesh::Buffer meshBuffer;
	Terrain::HeightMap heightMap;
	PlayerActorPtr player;
	ActorList enemies;
	ActorList trees;
	ActorList objects;
	Audio::SoundPtr bgm;
	float timer = 0;

	LightBuffer lightBuffer;
	ActorList lights;
	FramebufferObjectPtr fboMain;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);

		// ��ʃp�����[�^
		float width = 1280;		// ��ʂ̕�(�s�N�Z����)
		float height = 720;		// ��ʂ̍���(�s�N�Z����)
		float near = 1;			// �ŏ�Z�l(���[�g��)
		float far = 500;		// �ő�Z�l(���[�g��)

		// �J�����p�����[�^
		float fNumber = 1.4f;				// �G�t�E�i���o�[ = �J������F�l
		float fov = glm::radians(60.0f);	// �t�B�[���h�E�I�u�E�r���[ = �J�����̎���p(���W�A��)
		float sensorSize = 36.0f;			// �Z���T�[�E�T�C�Y = �J�����̃Z���T�[�̉���(�~��)

		// Update�֐��Ōv�Z����p�����[�^
		float focalLength = 50.0f;		// �t�H�[�J���E�����O�X = �œ_����(�~��)
		float aperture = 20.0f;			// �A�p�[�`�� = �J��(�~��)
		float focalPlane = 10000.0f;	// �t�H�[�J���E�v���[�� = �s���g�̍�������

		void Update(const glm::mat4& matView);

	};
	Camera camera;

};

#endif // MAINGAMESCENE_H_INCLUDED

