/**
*	@file Light.cpp
*/

#include "Light.h"
#include <iostream>

namespace /* unnamed */ {

	const char UniformBlockName[] = "LightUniformBlock";

}	// unnamed namespace


/**
*	���C�g�p���j�t�H�[���o�b�t�@���쐬����
*
*	@param bindingPoint ���j�t�@�[���o�b�t�@�Ɋ��蓖�Ă�Ǘ��ԍ�
*/
bool LightBuffer::Init(GLuint bindingPoint) {
	
	ubo[0] = UniformBuffer::Create(sizeof(data), bindingPoint, UniformBlockName);
	ubo[1] = UniformBuffer::Create(sizeof(data), bindingPoint, UniformBlockName);
	currentUboIndex = 0;

	return ubo[0] && ubo[1];
}

/**
*	�V�F�[�_�[�v���O�����ɃX�P���^�����b�V���̐���p�f�[�^�̃o�C���f�B���O�|�C���g��ݒ肷��
*
*	@param program	�o�C���f�B���O�|�C���g�ݒ��̃V�F�[�_�[�v���O����
*
*	@retval true	�ݒ萬��
*	@retval false	�ݒ莸�s
*/
bool LightBuffer::BindToShader(const Shader::ProgramPtr& program) {

	const GLuint id = program->Get();
	const GLuint blockIndex = glGetUniformBlockIndex(id, UniformBlockName);
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "[�G���[]Unform�u���b�N'" << UniformBlockName << "'������܂���\n";
		return false;
	}

	glUniformBlockBinding(id, blockIndex, ubo[0]->BindingPoint());
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "[�G���[]Uniform�u���b�N'" << UniformBlockName << "'�̃o�C���h�Ɏ��s\n";
		return false;
	}

	return true;
}

/**
*	GPU�֓]�����郉�C�g�f�[�^���X�V����
*
*	@param al			���C�g�̃A�N�^�[���X�g
*	@param ambientColor	�����̖��邳
*/
void LightBuffer::Update(const ActorList& al, const glm::vec3& ambientColor) {

	data.ambientLight.color = glm::vec4(ambientColor, 0);

	int pointLightCount = 0;
	int spotLightCount = 0;
	for (auto i = al.begin(); i != al.end(); ++i) {
		if (DirectionalLightActorPtr p =
			std::dynamic_pointer_cast<DirectionalLightActor>(*i)) {
			data.directionalLight.color = glm::vec4(p->color, 0);
			data.directionalLight.direction = glm::vec4(p->direction, 0);
		} else if (PointLightActorPtr p = std::dynamic_pointer_cast<PointLightActor>(*i)) {
			if (pointLightCount < 100) {
				p->index = pointLightCount;
				PointLight& light = data.pointLight[pointLightCount];
				light.color = glm::vec4(p->color, 1);
				light.position = glm::vec4(p->position, 1);
				++pointLightCount;
			} else if (SpotLightActorPtr p = std::dynamic_pointer_cast<SpotLightActor>(*i)) {
				if (spotLightCount < 100) {
					p->index = spotLightCount;
					SpotLight& light = data.spotLight[spotLightCount];
					light.color = glm::vec4(p->color, 1);
					light.dirAndCutOff = glm::vec4(p->direction, p->cutOff);
					light.posAndInnerCutOff = glm::vec4(p->position, p->innerCutOff);
					++spotLightCount;
				}
			}
		}
	}
}
/**
*	���C�g�f�[�^��GPU�������ɓ]������
*/
void LightBuffer::Upload() {
	UniformBufferPtr pUbo = ubo[currentUboIndex];
	pUbo->BufferSubData(&data, 0, sizeof(data));
	currentUboIndex = !currentUboIndex;
}

/**
*	���C�g�pUBO��GL�R���e�L�X�g��UBO�p�o�C���f�B���O�|�C���g�Ɋ��蓖�Ă�
*/
void LightBuffer::Bind(){

	ubo[!currentUboIndex]->BindBufferRange(0, sizeof(data));
}

