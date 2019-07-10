/**
* @file Shader.cpp
*/
#include "Shader.h"
#include "Geometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>

namespace Shader {

/**
* �V�F�[�_�R�[�h���R���p�C������.
*
* @param type   �V�F�[�_�̎��.
* @param string �V�F�[�_�R�[�h�ւ̃|�C���^.
*
* @return �쐬�����V�F�[�_�I�u�W�F�N�g.
*/
GLuint Compile(GLenum type, const GLchar* string)
{
  if (!string) {
    return 0;
  }

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &string, nullptr);
  glCompileShader(shader);
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      std::vector<char> buf;
      buf.resize(infoLen);
      if (static_cast<int>(buf.size()) >= infoLen) {
        glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
        std::cerr << "ERROR: �V�F�[�_�̃R���p�C���Ɏ��s\n" << buf.data() << std::endl;
      }
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

/**
* �v���O�����I�u�W�F�N�g���쐬����.
*
* @param vsCode ���_�V�F�[�_�R�[�h�ւ̃|�C���^.
* @param fsCode �t���O�����g�V�F�[�_�R�[�h�ւ̃|�C���^.
*
* @return �쐬�����v���O�����I�u�W�F�N�g.
*/
GLuint Build(const GLchar* vsCode, const GLchar* fsCode)
{
  GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
  GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
  if (!vs || !fs) {
    return 0;
  }
  GLuint program = glCreateProgram();
  glAttachShader(program, fs);
  glDeleteShader(fs);
  glAttachShader(program, vs);
  glDeleteShader(vs);
  glLinkProgram(program);
  GLint linkStatus = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE) {
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      std::vector<char> buf;
      buf.resize(infoLen);
      if (static_cast<int>(buf.size()) >= infoLen) {
        glGetProgramInfoLog(program, infoLen, NULL, buf.data());
        std::cerr << "ERROR: �V�F�[�_�̃����N�Ɏ��s\n" << buf.data() << std::endl;
      }
    }
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

/**
* �t�@�C����ǂݍ���.
*
* @param path �ǂݍ��ރt�@�C����.
*
* @return �ǂݍ��񂾃f�[�^.
*/
std::vector<GLchar> ReadFile(const char* path)
{
  std::basic_ifstream<GLchar> ifs;
  ifs.open(path, std::ios_base::binary);
  if (!ifs.is_open()) {
    std::cerr << "ERROR: " << path << " ���J���܂���\n";
    return {};
  }
  ifs.seekg(0, std::ios_base::end);
  const std::streamoff length = ifs.tellg();
  ifs.seekg(0, std::ios_base::beg);
  std::vector<GLchar> buf((size_t)length);
  ifs.read(buf.data(), length);
  buf.push_back('\0');
  return buf;
}

/**
* �t�@�C������v���O�����E�I�u�W�F�N�g���쐬����.
*
* @param vsPath ���_�V�F�[�_�[�t�@�C����.
* @param fsPath �t���O�����g�V�F�[�_�[�t�@�C����.
*
* @return �쐬�����v���O�����E�I�u�W�F�N�g.
*/
GLuint BuildFromFile(const char* vsPath, const char* fsPath)
{
  const std::vector<GLchar> vsCode = ReadFile(vsPath);
  const std::vector<GLchar> fsCode = ReadFile(fsPath);
  return Build(vsCode.data(), fsCode.data());
}

/**
* ���C�g���X�g������������.
*
* �S�Ă̌����̖��邳��0�ɂ���.
*/
void LightList::Init()
{
  ambient.color = glm::vec3(0);
  directional.color = glm::vec3(0);
  for (int i = 0; i < 8; ++i) {
    point.color[i] = glm::vec3(0);
  }
  for (int i = 0; i < 4; ++i) {
    spot.color[i] = glm::vec3(0);
  }
}

/**
* �R���X�g���N�^.
*/
Program::Program()
{
  lights.Init();
}

/**
* �R���X�g���N�^.
*
* @param id �v���O�����E�I�u�W�F�N�g��ID.
*/
Program::Program(GLuint programId)
{
  lights.Init();
  Reset(programId);
}

/**
* �f�X�g���N�^.
*
* �v���O�����E�I�u�W�F�N�g���폜����.
*/
Program::~Program()
{
  glDeleteProgram(id);
}

/**
* �v���O�����E�I�u�W�F�N�g��ݒ肷��.
*
* @param id �v���O�����E�I�u�W�F�N�g��ID.
*/
void Program::Reset(GLuint programId)
{
  glDeleteProgram(id);
  id = programId;
  if (id == 0) {
    locMatMVP = -1;
	locMatModel = -1;
    locPointLightPos = -1;
    locPointLightCol = -1;
    locDirLightDir = -1;
    locDirLightCol = -1;
    locAmbLightCol = -1;
    locSpotLightPos = -1;
    locSpotLightDir = -1;
    locSpotLightCol = -1;
    return;
  }

  locMatMVP = glGetUniformLocation(id, "matMVP");
  locMatModel = glGetUniformLocation(id, "matModel");
  locPointLightPos = glGetUniformLocation(id, "pointLight.position");
  locPointLightCol = glGetUniformLocation(id, "pointLight.color");
  locDirLightDir = glGetUniformLocation(id, "directionalLight.direction");
  locDirLightCol = glGetUniformLocation(id, "directionalLight.color");
  locAmbLightCol = glGetUniformLocation(id, "ambientLight.color");
  locSpotLightPos = glGetUniformLocation(id, "spotLight.posAndInnerCutOff");
  locSpotLightDir = glGetUniformLocation(id, "spotLight.dirAndCutOff");
  locSpotLightCol = glGetUniformLocation(id, "spotLight.color");

  const GLint texColorLoc = glGetUniformLocation(id, "texColor");
  if (texColorLoc >= 0) {
    glUseProgram(id);
    glUniform1i(texColorLoc, 0);
    glUseProgram(0);
  }
}

/**
* �v���O�����E�I�u�W�F�N�g���ݒ肳��Ă��邩���ׂ�.
*
* @retval true  �ݒ肳��Ă���.
* @retval false �ݒ肳��Ă��Ȃ�.
*/
bool Program::IsNull() const
{
  return !id;
}

/**
* �v���O�����E�I�u�W�F�N�g���O���t�B�b�N�X�E�p�C�v���C���Ɋ��蓖�Ă�.
*
* �v���O�����E�I�u�W�F�N�g���g���I�������glUseProgram(0)�����s���ĉ������邱��.
*/
void Program::Use()
{
  glUseProgram(id);
}

/**
* �`��Ɏg�p����VAO��ݒ肷��.
*
* @param vao �ݒ肷��VAO��ID.
*/
void Program::BindVertexArray(GLuint vao)
{
  glBindVertexArray(vao);
}

/**
* �`��Ɏg�p����e�N�X�`����ݒ肷��.
*
* @param unitNo �ݒ肷��e�N�X�`���E�C���[�W�E���j�b�g�̔ԍ�(0�`).
* @param texId  �ݒ肷��e�N�X�`����ID.
*/
void Program::BindTexture(GLuint unitNo, GLuint texId)
{
  glActiveTexture(GL_TEXTURE0 + unitNo);
  glBindTexture(GL_TEXTURE_2D, texId);
}

/**
* �`��Ɏg���郉�C�g��ݒ肷��.
*
* @param lights �ݒ肷�郉�C�g.
*
* ���̊֐����g���O�ɁAUse()�����s���Ă�������.
*/
void Program::SetLightList(const LightList& lights)
{
  this->lights = lights;

  // ���C�g�̐F����GPU�������ɓ]������.
  if (locAmbLightCol >= 0) {
    glUniform3fv(locAmbLightCol, 1, &lights.ambient.color.x);
  }
  if (locDirLightCol >= 0) {
    glUniform3fv(locDirLightCol, 1, &lights.directional.color.x);
  }
  if (locPointLightCol >= 0) {
    glUniform3fv(locPointLightCol, 8, &lights.point.color[0].x);
  }
  if (locSpotLightCol >= 0) {
    glUniform3fv(locSpotLightCol, 4, &lights.spot.color[0].x);
  }
}

/**
* �`��Ɏg����r���[�E�v���W�F�N�V�����s���ݒ肷��.
*
* @param matVP �ݒ肷��r���[�E�v���W�F�N�V�����s��.
*/
void Program::SetViewProjectionMatrix(const glm::mat4& matVP)
{
  this->matVP = matVP;
  if (locMatMVP >= 0) {
	  glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matVP[0][0]);
  }

}

/**
*	�`��Ɏg���郂�f���s���ݒ肷��
*
*	@param m �ݒ肷�郂�f���s��
*/
void Program::SetModelMatrix(const glm::mat4& m) {
	if (locMatModel >= 0 ) {
		glUniformMatrix4fv(locMatModel, 1, GL_FALSE, &m[0][0]);
	}
}
	



/**
*	�v���O�����I�u�W�F�N�g���쐬����
*
*	@param	vsPath	���_�V�F�[�_�t�@�C����
*	@param	fsPath	�t���O�ʂƃV�F�[�_�[�t�@�C����
*
*	@return	�쐬�����v���O�����I�u�W�F�N�g
*/
ProgramPtr Program::Create(const char* vsPath, const char* fsPath) {
	return std::make_shared<Program>(BuildFromFile(vsPath, fsPath));
}

} // namespace Shader
