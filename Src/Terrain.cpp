/**
*	@file Terrain.cpp
*/
#include "Terrain.h"
#include "Texture.h"
#include <iostream>
#include <algorithm>

// �n�`�Ɋւ���N���X�����i�[���閼�O���
namespace Terrain {
	/**
	*	�摜�t�@�C������n�`�f�[�^��ǂݍ���
	*
	*	@param path			�摜�t�@�C����
	*	@param scale		�����Ɋ|����W��
	*	@param baseLevel	����0�Ƃ݂Ȃ������l(�F�f�[�^0.0�`1.0�̂ǂ�������0�Ƃ��邩)
	*
	*	@retval	true	�ǂݍ��ݐ���
	*	@retval	false	�ǂݍ��ݎ��s
	*
	*	�摜�̐ԗv�f�������f�[�^�Ƃ݂Ȃ��ēǂݍ���
	*/
	bool HeightMap::LoadFromFile(const char* path, float scale, float baseLevel) {
		// �摜�t�@�C����ǂݍ���
		Texture::ImageData imageData;
		if (!Texture::LoadImage2D(path, &imageData)) {
			std::cerr << "[�G���[]" << __func__ << ": �n�C�g�}�b�v��ǂݍ��߂܂���ł���.\n";
			return false;
		}

		name = path;

		// �摜�̑傫����ۑ�.
		size = glm::ivec2(imageData.width, imageData.height);

		// �摜�f�[�^�͉������Ɍ������Ċi�[����Ă���̂ŁA�㉺���]���Ȃ��獂���f�[�^�ɕϊ�
		heights.resize(imageData.data.size());
		for (int y = 0; y < size.y; ++y) {
			const int offsetY = (size.y - 1) - y;
			for (int x = 0; x < size.x; ++x) {
				const glm::vec4 color = imageData.GetColor(x, y);
				heights[offsetY * size.x + x] = (color.r - baseLevel) * scale;
			}
		}

		return true;

	}


	/**
	*	�������擾����
	*
	*	@param pos	�������擾������W
	*
	*	@return pos�̈ʒu�̍���
	*
	*	�n�C�g�}�b�v��XZ���ʂɑΉ����A���_(0,0)����v���X�����ɒ�`�����
	*/

	float HeightMap::Height(const glm::vec3& pos)const {
		// ���W�������}�b�v�͈͓̔��Ɏ��܂�悤�ɐ؂�グ�A�܂��͐؂�̂Ă�
		const glm::vec2 fpos = glm::clamp(
			glm::vec2(pos.x, pos.z), glm::vec2(0.0f), glm::vec2(size) - glm::vec2(1));

		//�u�����v��fpos���܂܂�鐳���`�̍��㒸�_����Ƃ��Čv�Z����̂ŁA����̍��W�����߂�
		// �}�b�v�͉E�����v���X�����ŁA�����`�̑傫��1.0x1.0�Ƃ���
		// �������邱�ƂŁA����̒��_���W�����߂�v�Z�́A�P��fpos�̏�������؂�̂Ă邾���ɂ���
		// �������A�E�[�Ɖ��[��1��́A���̐�ɐ����`���Ȃ����Ƃ��獶����W�Ƃ��Ďg���Ȃ����߁A
		// ���[���_���W��size-2�ɐ�������B���Wfpos��
		//	(index.x, index.y)	, (index.x+1, index.y),
		//	(index.x, index.y+1), (index.x+1, index.y+1)
		// ��4�_����Ȃ鐳���`�̓����ɑ��݂��邱�ƂɂȂ�
		const glm::ivec2 index = glm::min(glm::ivec2(fpos), size - glm::ivec2(2));

		// ���㒸�_���瑊�΍��W���v�Z
		const glm::vec2 offset = fpos - glm::vec2(index);

		// �n�`��4���_����Ȃ鐳���`�ł���A�����`�͉��L�̂悤�ɂQ�̎O�p�`�Ƃ��Ē�`�����B
		//		-Y
		//	   +--+	
		//  -X |�^| +X
		//	   +--+
		//		+Y
		// ����-Y�����A��O��+Y�����ŁA1x1�̐����`�ł��邱�Ƃ���A
		//�uoffset.x + offset.y < 0�v �Ȃ�΁Aoffset�͍���̎O�p�`�̈�ɑ��݂���
		// �����łȂ���ΉE���̎O�p�`�̈�ɑ��݂���
		if (offset.x + offset.y < 1) {
			const float h0 = heights[index.y * size.x + index.x];
			const float h1 = heights[index.y * size.x + (index.x + 1)];
			const float h2 = heights[(index.y + 1) * size.x + index.x];
			return  h0 + (h1 - h0) * offset.x + (h2 - h0) * offset.y;
		} else {
			const float h0 = heights[(index.y + 1) * size.x + (index.x + 1)];
			const float h1 = heights[(index.y + 1) * size.x + index.x];
			const float h2 = heights[index.y * size.x + (index.x + 1)];
			return h0 + (h1 - h0) * (1.0f, -offset.x) + (h2 - h0) * (1.0f - offset.y);
		}
	}

	/**
	*	�����}�b�v���烁�b�V�����쐬����
	*	
	*	@param	meshBuffer	���b�V���쐬��ƂȂ郁�b�V���o�b�t�@
	*	@param	meshName	�쐬���郁�b�V���̖��O
	*	@param	texName		���b�V���ɓ\��t����e�N�X�`���t�@�C����
	*	
	*	@retval true		���b�V���̍쐬�ɐ���
	*	@retval false		���b�V�����쐬�ł��Ȃ�����
	*
	*	�O�p�`�͈ȉ��̌`�ɂȂ�
	*	d--c
	*	|�^|
	*	a--b
	*/
	bool HeightMap::CreateMesh(
		Mesh::Buffer& meshBuffer, const char*  meshName, const char* texName)const {

		if (heights.empty()) {
			std::cerr << "[�G���[]" << __func__ << ": �n�C�g�}�b�v���ǂݍ��܂�Ă��܂���.\n";
			return false;
		}

		// ���_�f�[�^���쐬
		Mesh::Vertex v;
		std::vector<Mesh::Vertex> vertices;
		vertices.reserve(size.x * size.y);
		for (int z = 0; z < size.y; ++z) {
			for (int x = 0; x < size.x; ++x) {
				// �e�N�X�`�����W�͏オ�v���X�Ȃ̂ŁA�������t���ɂ���K�v������
				v.position = glm::vec3(x, heights[z * size.x + x], z);
				v.texCoord = glm::vec2(x, (size.y - 1) - z) / (glm::vec2(size) - 1.0f);
				v.normal = CalcNormal(x, z);
				vertices.push_back(v);
			}
		}
		const size_t vOffset =
			meshBuffer.AddVertexData(vertices.data(), vertices.size() * sizeof(Mesh::Vertex));

		// �C���f�b�N�X�f�[�^���쐬
		std::vector<GLuint> indices;
		indices.reserve(size.x * size.y);
		for (int z = 0; z < size.y - 1; ++z) {
			for (int x = 0; x < size.x - 1; ++x) {
				const GLuint a = (z + 1) * size.x + x;
				const GLuint b = (z + 1) * size.x + (x + 1);
				const GLuint c = z * size.x + (x + 1);
				const GLuint d = z * size.x + x;
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);

				indices.push_back(c);
				indices.push_back(d);
				indices.push_back(a);

			}
		}

		const size_t iOffset =
			meshBuffer.AddIndexData(indices.data(), indices.size() * sizeof(GLuint));

		// ���_�f�[�^�ƃC���f�b�N�X�f�[�^���烁�b�V�����쐬
		Mesh::Primitive p =
			meshBuffer.CreatePrimitive(indices.size(), GL_UNSIGNED_INT, iOffset, vOffset);
		Mesh::Material m = meshBuffer.CreateMaterial(glm::vec4(1), nullptr);
		m.texture[0] = Texture::Image2D::Create("Res/Terrain_Ratio.tga");
		m.texture[1] = Texture::Image2D::Create("Res/Terrain_Soil.tga");
		m.texture[2] = Texture::Image2D::Create("Res/Terrain_Rock.tga");
		m.texture[3] = Texture::Image2D::Create("Res/Terrain_Plant.tga");
		m.program = meshBuffer.GetTerrainShader();


		meshBuffer.AddMesh(meshName, p, m);

		return true;
		
	}

	/**
	*	������񂩂�@�����v�Z����
	*
	*	@param centerX	�v�Z�Ώۂ�X���W
	*	@param centerZ	�v�Z�Ώۂ�Z���W
	*
	*	@return (centerX, centerZ)�̈ʒu�̖@��
	*/

	glm::vec3 HeightMap::CalcNormal(int centerX, int centerZ) const {
		const glm::ivec2 offsetList[] = {
			{ 0, -1 }, { 1, -1 }, { 1, 0 },{ 0, 1 }, { -1, 1 }, { -1 , 0 }, { 0 , -1 } 
		};
		const glm::vec3 center(centerX, heights[centerZ * size.x + centerX], centerZ);
		glm::vec3 sum(0);
		for (size_t i = 0; i < 6; ++i) {
			glm::vec3 p0(centerX + offsetList[i].x, 0, centerZ + offsetList[i].y);
			if (p0.x < 0 || p0.x >= size.x || p0.z < 0 || p0.z >= size.y) {
				continue;
			}

			p0.y = heights[static_cast<size_t>(p0.z * size.x + p0.x)];

			glm::vec3 p1(centerX + offsetList[i + 1].x, 0, centerZ + offsetList[i + 1].y);
			
			if (p1.x < 0 || p1.x >= size.x || p1.z < 0 || p1.z >= size.y) {
				continue;
			}

			p1.y = heights[static_cast<size_t>(p1.z * size.x + p1.x)];

			sum += normalize(cross(p1 - center,p0 - center));


		}

		return normalize(sum);
	}

}	// namespace Terrain
