/**
*	@file framebufferObject.h
*/

#ifndef FRAMEBUFFEROBJECT_H_INCLUDED
#define FRAMEBUFFEROBJECT_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include <memory>

class FramebufferObject;
using FramebufferObjectPtr = std::shared_ptr<FramebufferObject>;

/**
*	�t���[���o�b�t�@�I�u�W�F�N�g
*/
class FramebufferObject {
public:
	static FramebufferObjectPtr Create(int w, int h);
	FramebufferObject() = default;
	~FramebufferObject();

	GLuint GetFramebuffer() const;
	const Texture::Image2DPtr& GetColorTexture() const;
	const Texture::Image2DPtr& GetDepthTexture() const;

private:
	Texture::Image2DPtr texColor;	// �J���[�o�b�t�@�p�e�N�X�`��
	Texture::Image2DPtr texDepth;	// �[�x�o�b�t�@�p�e�N�X�`��
	GLuint id = 0;					// FBO��ID

};

#endif // FRAMEBUFFEROBJECT_H_INCLUDED