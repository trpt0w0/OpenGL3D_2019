/**
*	@file FramebufferObject.cpp
*/
#include "FramebufferObject.h"

/**
*	�t���[���o�b�t�@�I�u�W�F�N�g���쐬����
*
*	@param w	�t���[���o�b�t�@�I�u�W�F�N�g�̕�(�s�N�Z���P��)
*	@param h	�t���[���o�b�t�@�I�u�W�F�N�g�̍���(�s�N�Z���P��)
*	
*	@return		�쐬�����t���[���o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^ 
*/
FramebufferObjectPtr FramebufferObject::Create(int w, int h) {

	FramebufferObjectPtr fbo = std::make_shared<FramebufferObject>();
	if (!fbo) {
		return nullptr;
	}

	// �e�N�X�`�����쐬����
	fbo->texColor = std::make_shared<Texture::Image2D>(Texture::CreateImage2D(
		w, h, nullptr, GL_RGBA, GL_UNSIGNED_BYTE));
	fbo->texDepth = std::make_shared<Texture::Image2D>(Texture::CreateImage2D(
		w, h, nullptr, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F));

	// �t���[���o�b�t�@���쐬����
	glGenFramebuffers(1, &fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		fbo->texColor->Target(), fbo->texColor->Get(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		fbo->texDepth->Target(), fbo->texDepth->Get(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return  fbo;
}

/**
*	�f�X�g���N�^
*/
FramebufferObject::~FramebufferObject() {
	if (id) {
		glDeleteFramebuffers(1, &id);
	}
}

/**
*	FBO��ID���擾����
*
*	@return FBO��ID
*/
GLuint FramebufferObject::GetFramebuffer() const {
	return id;
}

/**
*	�J���[�o�b�t�@�p�e�N�X�`�����擾����
*
*	@return �J���[�o�b�t�@�p�e�N�X�`��
*/
const Texture::Image2DPtr& FramebufferObject::GetColorTexture() const {
	return texColor;
}

/**
*	�[�x�o�b�t�@�p�e�N�X�`�����擾����
*
*	@return �[�x�o�b�t�@�p�e�N�X�`��
*/
const Texture::Image2DPtr& FramebufferObject::GetDepthTexture() const {
	return texDepth;
}
