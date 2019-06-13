/**
* @file Shader.h
*/
#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

struct Mesh;

namespace Shader {

class Program;
using ProgramPtr = std::shared_ptr<Program>;


GLuint Build(const GLchar* vsCode, const GLchar* fsCode);
GLuint BuildFromFile(const char* vsPath, const char* fsPath);

/**
* ����.
*/
struct AmbientLight
{
  glm::vec3 color;
};

/**
* �w�������C�g.
*/
struct DirectionalLight
{
  glm::vec3 direction;
  glm::vec3 color;
};

/**
* �|�C���g���C�g.
*/
struct PointLight
{
  glm::vec3 position[8];
  glm::vec3 color[8];
};

// �X�|�b�g���C�g
struct SpotLight
{
  glm::vec4 dirAndCutOff[4];
  glm::vec4 posAndInnerCutOff[4];
  glm::vec3 color[4];
};

/**
* ���C�g���܂Ƃ߂��\����.
*/
struct LightList
{
  AmbientLight ambient;
  DirectionalLight directional;
  PointLight point;
  SpotLight spot;

  void Init();
};

/**
* �V�F�[�_�[�E�v���O����.
*/
class Program
{
public:
	static ProgramPtr Create(const char* vsPath, const char* fsPath);
  Program();
  explicit Program(GLuint programId);
  ~Program();

  void Reset(GLuint programId);
  bool IsNull() const;
  void Use();
  void BindVertexArray(GLuint);
  void BindTexture(GLuint, GLuint);
  void SetLightList(const LightList&);
  void SetViewProjectionMatrix(const glm::mat4&);
  void Draw(const Mesh&, const glm::vec3& t, const glm::vec3& r, const glm::vec3& s);

private:
  GLuint id = 0; // �v���O����ID.

  // uniform�ϐ��̈ʒu.
  GLint locMatMVP = -1;
  GLint locPointLightPos = -1;
  GLint locPointLightCol = -1;
  GLint locDirLightDir = -1;
  GLint locDirLightCol = -1;
  GLint locAmbLightCol = -1;
  GLint locSpotLightPos = -1;
  GLint locSpotLightDir = -1;
  GLint locSpotLightCol = -1;

  glm::mat4 matVP = glm::mat4(1);
  LightList lights;
};

} // namespace Shader

#endif // SHADER_H_INCLUDED