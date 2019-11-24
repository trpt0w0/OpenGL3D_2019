/**
* @file Texture.h
*/
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#include <GL/glew.h>
#include "BufferObject.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Texture {
	
	class Interface;
	using InterfacePtr = std::shared_ptr<Interface>;
	class Image2D;
	using Image2DPtr = std::shared_ptr<Image2D>;
	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	/**
	*	�摜�f�[�^
	*/
	class ImageData {
	public:
		glm::vec4 GetColor(int x, int y) const;
	public:
		GLint width = 0;		// ���̃s�N�Z��
		GLint height = 0;		// �c�̃s�N�Z��
		GLenum format = GL_NONE;// �L�^����Ă���F�̎��
		GLenum type = GL_NONE;	// ���ꂼ��̐F���̃r�b�g�z�u
		std::vector<uint8_t> data; // �o�C�g�f�[�^

	};

GLuint CreateImage2D(GLsizei width, GLsizei height, const GLvoid* data, GLenum format, GLenum type);
GLuint LoadImage2D(const char* path);
bool LoadImage2D(const char* path, ImageData* imageData);

/**
*	�e�N�X�`������C���^�[�t�F�C�X
*/
class Interface {

public:
	Interface() = default;
	virtual ~Interface() = default;
	virtual bool IsNull() const = 0;
	virtual GLuint Get() const = 0;
	virtual GLint Width() const = 0;
	virtual GLint Height() const = 0;
	virtual GLenum Target() const = 0;

};

/**
* �e�N�X�`���E�C���[�W.
*/
class Image2D : public Interface
{
public:
	static Image2DPtr Create(const char*);
	Image2D() = default;
	explicit Image2D(GLuint texId);
	virtual ~Image2D();


	void Reset(GLuint texId);
	virtual bool IsNull() const override;
	virtual GLuint Get() const override;
	virtual GLint Width() const override { return width; }
	virtual GLint Height() const override { return height; }
	virtual GLenum Target() const override { return GL_TEXTURE_2D; }
	
private:
	GLuint id = 0;
	GLint width = 0;
	GLint height = 0;

};

class Buffer : public Interface {
public:
	static BufferPtr Create(GLenum iternalFormat, GLsizeiptr size,
		const GLvoid* data = nullptr, GLenum usage = GL_STATIC_DRAW);
	Buffer() = default;
	virtual ~Buffer();

	virtual bool IsNull() const override { return !id; }
	virtual GLuint Get() const override { return id;}
	virtual GLint Width() const override { return bo.Size(); }
	virtual GLint Height() const override { return 1; }
	virtual GLenum Target() const override { return GL_TEXTURE_BUFFER; }

	bool BufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data);
	GLuint BufferId() const { return bo.Id(); }
	GLsizeiptr Size() const { return bo.Size(); }

private:
	GLuint id = 0;
	BufferObject bo;

};

} // namespace Texture

#endif // TEXTURE_H_INCLUDED