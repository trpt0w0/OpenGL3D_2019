/**
*	Mesh.h
*/

#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED
#include <GL/glew.h>
#include "BufferObject.h"
#include "Texture.h"
#include "Shader.h"
#include "Json/json11.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace Mesh{

	//��s�錾
	struct Mesh;
	using MeshPtr = std::shared_ptr<Mesh>;
	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	// �X�P���^�����b�V���p�̐�s�錾
	struct Node;
	struct ExtendedFile;
	using  ExtendedFilePtr = std::shared_ptr<ExtendedFile>;
	class  SkeletalMesh;
	using  SkeletalMeshPtr = std::shared_ptr<SkeletalMesh>;


	/**
	*	���_�f�[�^
	*/
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
		glm::vec3 normal;
	};

	/**
	*	�v���~�e�B�u�̍ގ�
	*/
	struct Material {
		glm::vec4 baseColor = glm::vec4(1);
		Texture::InterfacePtr texture[8];
		Shader::ProgramPtr program;
		// �X�P���^�����b�V���p�V�F�[�_�[
		Shader::ProgramPtr progSkeletalMesh;
	};

	/**
	*	���_�f�[�^�̕`��p�����[�^
	*/

	struct Primitive{
		GLenum mode;
		GLsizei count;
		GLenum type;
		const GLvoid* indices;
		GLint baseVertex = 0;
		std::shared_ptr<VertexArrayObject> vao;
		int material = 0;
	};

	/**
	*	���b�V��
	*/
	struct Mesh {
		std::string name;	// ���b�V����
		std::vector<Primitive> primitives;
	};

	/**
	*	�t�@�C��
	*/
	struct File{
		std::string name;	// �t�@�C����
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
	};

	using FilePtr = std::shared_ptr<File>;

	/**
	*	���b�V���Ǘ��N���X
	*/
	class Buffer {
	public:
		Buffer() = default;
		~Buffer() = default;

		bool Init(GLsizeiptr vboSize, GLsizeiptr iboSize);
		GLintptr AddVertexData(const void* data, size_t size);
		GLintptr AddIndexData(const void* data, size_t size);
		Primitive CreatePrimitive(
			size_t count, GLenum type, size_t iOffset, size_t vOffset) const;
		Material CreateMaterial(const glm::vec4& color, Texture::Image2DPtr texture) const;
		bool AddMesh(const char* name, const Primitive& primitive, const Material& aterial);
		bool SetAttribute(
			Primitive*, int, const json11::Json&, const json11::Json&, const std::vector<std::vector<char>>&);
		bool LoadMesh(const char* path);

		FilePtr GetFile(const char* name) const;
		void SetViewProjectionMatrix(const glm::mat4&) const;

		void AddCube(const char* name);

		// �X�P���^�����b�V���ɑΉ��������b�V���̓ǂݍ��݂Ǝ擾
		bool LoadSkeletalMesh(const char* path);
		SkeletalMeshPtr GetSkeletalMesh(const char* meshName) const;

		const Shader::ProgramPtr& GetStaticMeshShader() const { return progStaticMesh; }
		const Shader::ProgramPtr& GetTerrainShader() const { return progTerrain; }

	private:
		BufferObject vbo;
		BufferObject ibo;
		GLintptr vboEnd = 0;
		GLintptr iboEnd = 0;
		std::unordered_map<std::string, FilePtr> files;
		Shader::ProgramPtr progStaticMesh;
		Shader::ProgramPtr progTerrain;

		//�X�P���^���E�A�j���[�V�����ɑΉ��������b�V����ێ����郁���o�ϐ�
		Shader::ProgramPtr progSkeletalMesh;
		struct MeshIndex {
			ExtendedFilePtr file;
			const Node* node = nullptr;
		};

		std::unordered_map<std::string, MeshIndex> meshes;
		std::unordered_map<std::string, ExtendedFilePtr> extendedFiles;
	};

	void Draw(const FilePtr&, const glm::mat4& matM);

}//namespace Mesh

#endif // MESH_H_INCLUDED