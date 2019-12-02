/**
* @file Texture.cpp
*/

#define NOMIMAX
#include "Texture.h"
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

/// �e�N�X�`���֘A�̊֐���N���X���i�[���閼�O���.
namespace Texture {


/**
*	FOURCC���쐬����
*/
#define MAKE_FOURCC(a, b, c, d) \
	static_cast<uint32_t>(a + (b << 8) + (c << 16) + (d << 24))


/**
*	�o�C�g�񂩂琔�l�𕜌�����
*
*	@param p		�o�C�g��ւ̃|�C���^
*	@param offset	���l�̃I�t�Z�b�g	
*	@param size		���l�̃o�C�g��(1�`4)
*
*	@return	�����������l
*/
uint32_t Get(const uint8_t* p, size_t offset, size_t size) {
	uint32_t result = 0;
	p += offset;
	for (size_t i = 0; i < size; ++i) {
		result += p[i] << (i * 8);
	}
	return result;

}

/**
*	DDS�摜���
*/
struct DDSPixelFormat {
	uint32_t size;			// ���̍\���̂̃o�C�g��(32) 
	uint32_t flags;			// �摜�Ɋ܂܂��f�[�^�̎�ނ������t���O
	uint32_t fourCC;		// �摜�t�H�[�}�b�g������FOURCC
	uint32_t rgbBitCount;	// 1�s�N�Z���̃r�b�g��
	uint32_t redBitMask;	// �ԗv�f���g�������������r�b�g
	uint32_t greenBitMask;	// �Ηv�f���g�������������r�b�g
	uint32_t blueBitMask;	// �v�f���g�������������r�b�g
	uint32_t alphaBitMask;	// �����x�v�f���g�������������r�b�g
};

/**
*	�o�b�t�@����DDS�摜����ǂݏo��
*
*	@param buf	�ǂݏo�����o�b�t�@
*
*	@return		�ǂݏo����DDS�摜���
*/
DDSPixelFormat ReadDDSPixelFormat(const uint8_t* buf) {
	DDSPixelFormat tmp;
	tmp.size = Get(buf, 0, 4);
	tmp.flags = Get(buf, 4, 4);
	tmp.fourCC = Get(buf, 8, 4);
	tmp.rgbBitCount = Get(buf, 12, 4);
	tmp.redBitMask = Get(buf, 16, 4);
	tmp.greenBitMask = Get(buf, 20, 4);
	tmp.blueBitMask = Get(buf, 24, 4);
	tmp.alphaBitMask = Get(buf, 28, 4);
	return tmp;
}

/**
*	DDS�t�@�C���w�b�_
*/
struct DDSHeader {
	uint32_t size;				// ���̍\���̂̃o�C�g��(124)
	uint32_t flags;				// �ǂ̃p�����[�^���L�����������t���O
	uint32_t height;			// �摜�̍���(�s�N�Z����)
	uint32_t width;				// �摜�̕�(�s�N�Z����)
	uint32_t pitchOrLinearSize;	// ���̃o�C�g���܂��͉摜1���̃o�C�g��
	uint32_t depth;				// �摜�̉��s(����)(3�����e�N�X�`�����Ŏg�p)
	uint32_t mipMapCount;		// �܂܂�Ă���~�b�v�}�b�v���x����
	uint32_t reserved1[11];		// (�����̂��߂ɗ\�񂳂�Ă���)
	DDSPixelFormat ddspf;		// DDS�摜���
	uint32_t caps[4];			// �܂܂�Ă���摜�̎��
	uint32_t reserved2;			// (�����̂��߂ɗ\�񂳂�Ă���)

};

/**
*	�o�b�t�@����DDS�t�@�C���w�b�_��ǂݏo��
*
*	@param buf	�ǂݏo�����̃o�b�t�@
*
*	@return �ǂݏo����DDS�t�@�C���w�b�_
*/
DDSHeader ReadDDSHeader(const uint8_t* buf) {
	DDSHeader tmp = {};
	tmp.size = Get(buf, 0, 4);
	tmp.flags = Get(buf, 4, 4);
	tmp.height = Get(buf, 8, 4);
	tmp.width = Get(buf, 12, 4);
	tmp.pitchOrLinearSize = Get(buf, 16, 4);
	tmp.depth = Get(buf, 20, 4);
	tmp.mipMapCount = Get(buf, 24, 4);
	tmp.ddspf = ReadDDSPixelFormat(buf + 28 + 4 * 11);
	for (int i = 0; i < 4; ++i) {
		tmp.caps[i] = Get(buf, 28 + 4 * 11 + 32 + i * 4, 4);
	}

	return tmp;
}

/**
*	DDS�t�@�C������e�N�X�`�����쐬����
*
*	@param filename	DDS�t�@�C����
*
*	@retval 0�ȊO	�쐬�����e�N�X�`��ID
*	@retval	0		�쐬���s
*/
GLuint LoadDDS(const char* filename) {

	std::basic_ifstream<uint8_t> ifs(filename, std::ios_base::binary);
	
	if (!ifs) {
		std::cerr << "[�G���[] " << filename << "���J���܂���.\n";
		return 0;
	}
	// 1���K�o�C�g�̃����������蓖�ĂāA��x�ɂ�������ǂލ��߂�悤�ɂ���
	std::vector<uint8_t> readBuffer(1'000'000);
	ifs.rdbuf()->pubsetbuf(readBuffer.data(), readBuffer.size());

	// DDS�w�b�_�[��ǂݍ���
	std::vector<uint8_t> buf(128);
	ifs.read(buf.data(), 128);
	if (ifs.eof()) {
		return 0;
	}

	if (buf[0] != 'D' || buf[1] != 'D' || buf[2] != 'S' || buf[3] != ' ') {
		return 0;
	}

	const DDSHeader header = ReadDDSHeader(buf.data() + 4);
	if (header.size != 124) {
		std::cerr << "[�x��] " << filename << "�͖��Ή���DDS�t�@�C���ł�.\n";
		return 0;
	}

	// �t�@�C����DDS�`���ɑΉ�����OpenGL�̃t�H�[�}�b�g��I������
	GLenum iformat = GL_RGBA8;
	GLenum format = GL_RGBA;
	uint32_t blockSize = 16;
	bool isCompressed = false;
	if (header.ddspf.flags & 0x04) {
		// ���k�t�H�[�}�b�g
		switch (header.ddspf.fourCC) {
		case MAKE_FOURCC('D', 'X', 'T','1'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case MAKE_FOURCC('D', 'X', 'T', '2'):
		case MAKE_FOURCC('D', 'X', 'T', '3'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case MAKE_FOURCC('D', 'X', 'T', '4'):
		case MAKE_FOURCC('D', 'X', 'T', '5'):
			iformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		case MAKE_FOURCC('B', 'C', '4', 'U'):
			iformat = GL_COMPRESSED_RED_RGTC1;
			break;
		case MAKE_FOURCC('B', 'C', '4', 'S'):
			iformat = GL_COMPRESSED_SIGNED_RED_RGTC1;
			break;
		case MAKE_FOURCC('B', 'C', '5', 'U'):
			iformat = GL_COMPRESSED_RG_RGTC2;
			break;
		case MAKE_FOURCC('B', 'C', '5', 'S'):
			iformat = GL_COMPRESSED_SIGNED_RG_RGTC2;
			break;
		default:
			std::cerr << "[�x��] " << filename << "�͖��Ή���DDS�t�@�C���ł�.\n";
			return 0;
		}
		isCompressed = true;
	}else if(header.ddspf.flags & 0x40) {
		// �����k�t�H�[�}�b�g
		if (header.ddspf.redBitMask == 0xff) {
			iformat = header.ddspf.alphaBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_RGBA : GL_RGB;
		} else if (header.ddspf.blueBitMask == 0xff) {
			iformat = header.ddspf.alphaBitMask ? GL_RGBA8 : GL_RGB8;
			format = header.ddspf.alphaBitMask ? GL_RGBA : GL_RGB;
		} else {
			std::cerr << "[�x��] " << filename << "�͖��Ή���DDS�t�@�C���ł�.\n";
			return 0;
		}
	} else {
		std::cerr << "[�x��] " << filename << "�͖��Ή���DDS�t�@�C���ł�.\n";
		return 0;
	}

	// �摜�������擾����
	const bool isCubemap = header.caps[1] & 0x200;
	const GLenum target = isCubemap ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_2D;
	const int faceCount = isCubemap ? 6 : 1;
	
	// �摜��ǂݍ����GPU�������ɓ]��
	buf.resize(header.width * header.height * 4);
	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(isCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texId);
	for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
		GLsizei curWidth = header.width;
		GLsizei curHeight = header.height;
		for (int mipLevel = 0; mipLevel < static_cast<int>(header.mipMapCount); ++mipLevel) {
			if (isCompressed) {
				// ���k�`���̏ꍇ
				const uint32_t imageBytes =
					((curWidth + 3) / 4) * ((curHeight + 3) / 4) * blockSize;
				ifs.read(buf.data(), imageBytes);
				glCompressedTexImage2D(target + faceIndex, mipLevel, iformat,
					curWidth, curHeight, 0, imageBytes, buf.data());
			} else {
				// �����k�`���̏ꍇ
				const uint32_t imageBytes = curWidth * curHeight * 4;
				ifs.read(buf.data(), imageBytes);
				glTexImage2D(target + faceIndex, mipLevel, iformat,
					curWidth, curHeight, 0, format, GL_UNSIGNED_BYTE, buf.data());
			}
			const GLenum result = glGetError();
			if (result != GL_NO_ERROR) {
				std::cerr << "[�x��] " << filename << "�̓ǂݍ��݂Ɏ��s(" <<
					std::hex << result << ").\n";
			}
			curWidth = std::max(1, curWidth / 2);
			curHeight = std::max(1, curHeight / 2);
		}
	}
	// �e�N�X�`���E�p�����[�^�[��ݒ肷��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, header.mipMapCount - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		header.mipMapCount <= 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texId;

}

/*
*	�F�f�[�^���擾����
*
*	@param x	X���W
*	@param y	Y���W
*
*	@return ���W(x,y)�̐F��0.0�`1.0�ŕ\�����l
*			�F�v�f���f�[�^�ɑ��݂��Ȃ��ꍇ�ARGB��0.0 A��1.0�ɂȂ�
*/
	glm::vec4 ImageData::GetColor(int x, int y) const {
	
		// ���W���摜�͈̔͂ɐ���
		x = std::max(0, std::min(x, width - 1));
		y = std::max(0, std::min(y, height - 1));

		if (type == GL_UNSIGNED_BYTE) {
			// �e�F�P�o�C�g�̃f�[�^
			glm::vec4 color(0, 0, 0, 255);
			if (format == GL_BGRA) {
				// BGRA�̏��Ԃ�1�o�C�g���A���v4�o�C�g�i�[����Ă���
				const uint8_t* p = &data[x * 3 + y * (width * 4)];
				color.b = p[0];
				color.g = p[1];
				color.r = p[2];
				color.a = p[3];
			} else if (format == GL_BGR) {
				// BGR�̔ԍ���1�o�C�g���A���v�łR�o�C�g�i�[����Ă���
				const uint8_t* p = &data[x * 3 + y * (width * 3)];
				color.b = p[0];
				color.g = p[1];
				color.r = p[2];
			} else if (format == GL_RED) {
				// �ԐF�����A���v1�o�C�g�i�[����Ă���
				color.r = data[x + y * width];
			}
			return color / 255.0f;
		} else if(type == GL_UNSIGNED_SHORT_1_5_5_5_REV) {
			// �F��2�o�C�g�ɋl�ߍ��񂾃f�[�^
			glm::vec4 color(0, 0, 0, 1);
			const uint8_t* p = &data[x * 2 + y * (width * 2)];
			const uint16_t c = p[0] + p[1] * 0x100;		// 2�̃o�C�g������
			if (format == GL_BGRA) {
				// 16�r�b�g�̃f�[�^����e�F�����o��
				color.b = static_cast<float>((c & 0b0000'0000'0001'1111));
				color.g = static_cast<float>((c & 0b0000'0011'1110'0000) >> 5);
				color.r = static_cast<float>((c & 0b0111'1100'0000'0000) >> 10);
				color.a = static_cast<float>((c & 0b1000'0000'0000'0000) >> 15);
			}

			return color / glm::vec4(31.0f, 31.0f, 31.0f, 1.0f);
		}
		
		return glm::vec4(0, 0, 0, 1);
	}


/**
* 2D�e�N�X�`�����쐬����.
*
* @param width   �e�N�X�`���̕�(�s�N�Z����).
* @param height  �e�N�X�`���̍���(�s�N�Z����).
* @param data    �e�N�X�`���f�[�^�ւ̃|�C���^.
* @param format  �]�����摜�̃f�[�^�`��.
* @param type    �]�����摜�̃f�[�^�i�[�`��.
*
* @retval 0�ȊO  �쐬�����e�N�X�`���E�I�u�W�F�N�g��ID.
* @retval 0      �e�N�X�`���̍쐬�Ɏ��s.
*/
GLuint CreateImage2D(GLsizei width, GLsizei height, const GLvoid* data, GLenum format, GLenum type)
{
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, type, data);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  const GLenum result = glGetError();
  if (result != GL_NO_ERROR) {
    std::cerr << "ERROR: �e�N�X�`���̍쐬�Ɏ��s(0x" << std::hex << result << ").";
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &id);
    return 0;
  }

  // �e�N�X�`���̃p�����[�^��ݒ肷��.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (format == GL_RED) {
    const GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}

/**
* �t�@�C������2D�e�N�X�`����ǂݍ���.
*
* @param path 2D�e�N�X�`���Ƃ��ēǂݍ��ރt�@�C���̃p�X.
*
* @retval 0�ȊO  �쐬�����e�N�X�`���E�I�u�W�F�N�g��ID.
* @retval 0      �e�N�X�`���̍쐬�Ɏ��s.
*/
GLuint LoadImage2D(const char* path)
{
	const size_t len = strlen(path);
	if (_stricmp(path + (len - 4), ".dds") == 0) {
		const GLuint id = LoadDDS(path);
		if (id) {
			return id;
		}
	}

	ImageData imageData;
	if (!LoadImage2D(path, &imageData)) {
		return 0;
	}
	return CreateImage2D(imageData.width, imageData.height, imageData.data.data(),
		imageData.format, imageData.type);

}

/**
*	�t�@�C������摜�f�[�^��ǂݍ���
*
*	@param path			�摜�Ƃ��ēǂݍ��ރt�@�C���p�X
*	@param imageData	�摜�f�[�^���i�[����\����
*	
*	@retval true	�ǂݍ��ݐ���
*	@retval false	�ǂݍ��ގ��s
*
*/

bool LoadImage2D(const char* path, ImageData* imageData){

	// TGA�w�b�_��ǂݍ���.
	std::basic_ifstream<uint8_t> ifs;

	ifs.open(path, std::ios_base::binary);
	if (!ifs) {
		std::cerr << "WARNING: " << path << "���J���܂���.\n";
		return 0;
	}
	std::vector<uint8_t> tmp(1024 * 1024);
	ifs.rdbuf()->pubsetbuf(tmp.data(), tmp.size());

	std::cout << "INFO: " << path << "��ǂݍ��ݒ��c";
	uint8_t tgaHeader[18];
	ifs.read(tgaHeader, 18);

	// �C���[�WID���΂�.
	ifs.ignore(tgaHeader[0]);

	// �J���[�}�b�v���΂�.
	if (tgaHeader[1]) {
		const int colorMapLength = tgaHeader[5] | (tgaHeader[6] << 8);
		const int colorMapEntrySize = tgaHeader[7];
		const int colorMapSize = colorMapLength * colorMapEntrySize / 8;
		ifs.ignore(colorMapSize);
	}

	// �摜�f�[�^��ǂݍ���.
	const int width = tgaHeader[12] | (tgaHeader[13] << 8);
	const int height = tgaHeader[14] | (tgaHeader[15] << 8);
	const int pixelDepth = tgaHeader[16];
	const int imageSize = width * height * pixelDepth / 8;
	std::vector<uint8_t> buf(imageSize);
	ifs.read(buf.data(), imageSize);

	// �摜�f�[�^���u�ォ�牺�v�Ŋi�[����Ă���ꍇ�A�㉺�����ւ���.
	if (tgaHeader[17] & 0x20) {
		std::cout << "���]���c";
		const int lineSize = width * pixelDepth / 8;
		std::vector<uint8_t> tmp(imageSize);
		std::vector<uint8_t>::iterator source = buf.begin();
		std::vector<uint8_t>::iterator destination = tmp.end();
		for (int i = 0; i < height; ++i) {
			destination -= lineSize;
			std::copy(source, source + lineSize, destination);
			source += lineSize;
		}
		buf.swap(tmp);
	}
	std::cout << "����\n";

	GLenum type = GL_UNSIGNED_BYTE;
	GLenum format = GL_BGRA;
	if (tgaHeader[2] == 3) {
		format = GL_RED;
	}
	if (tgaHeader[16] == 24) {
		format = GL_BGR;
	}
	else if (tgaHeader[16] == 16) {
		type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	}

	imageData->width = width;
	imageData->height = height;
	imageData->format = format;
	imageData->type = type;
	imageData->data.swap(buf);
	return true;


}

/**
* �R���X�g���N�^.
*
* @param texId �e�N�X�`���E�I�u�W�F�N�g��ID.
*/
Image2D::Image2D(GLuint texId)
{
  Reset(texId);
}

/**
* �f�X�g���N�^.
*/
Image2D::~Image2D()
{
  glDeleteTextures(1, &id);
}

/**
* �e�N�X�`���E�I�u�W�F�N�g��ݒ肷��.
*
* @param texId �e�N�X�`���E�I�u�W�F�N�g��ID.
*/
void Image2D::Reset(GLuint texId)
{
  glDeleteTextures(1, &id);
  id = texId;
  if (id) {
	  // �e�N�X�`���̕��ƍ������擾����
	  glBindTexture(GL_TEXTURE_2D, id);
	  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	  glBindTexture(GL_TEXTURE_2D, 0);
  }
}

/**
* �e�N�X�`���E�I�u�W�F�N�g���ݒ肳��Ă��邩���ׂ�.
*
* @retval true  �ݒ肳��Ă���.
* @retval false �ݒ肳��Ă��Ȃ�.
*/
bool Image2D::IsNull() const
{
  return id;
}

/**
* �e�N�X�`���E�I�u�W�F�N�g���擾����.
*
* @return �e�N�X�`���E�I�u�W�F�N�g��ID.
*/
GLuint Image2D::Get() const
{
  return id;
}


/**
*	2D�e�N�X�`�����쐬����
*
*	@param path �e�N�X�`���t�@�C����
*
*	@return �쐬�����e�N�X�`���I�u�W�F�N�g
*/
Image2DPtr Image2D::Create(const char* path) {
	return  std::make_shared<Image2D>(LoadImage2D(path));

}
/**
*	�o�b�t�@�E�e�N�X�`�����쐬����
*
*	@param internalFormat	�o�b�t�@�̃f�[�^�`��
*	@param size				�o�b�t�@�̃T�C�Y
*	@param data				�o�b�t�@�ɓ]������f�[�^
*	@param usage			�o�b�t�@�̃A�N�Z�X�^�C�v
*
*	@return �쐬�����e�N�X�`���I�u�W�F�N�g
*/
BufferPtr Buffer::Create(GLenum internalFormat, GLsizeiptr size,
	const GLvoid* data, GLenum usage) 
{
	
	BufferPtr buffer = std::make_shared<Buffer>();
	if (!buffer->bo.Create(GL_TEXTURE_BUFFER, size, data, usage)) {
		return false;
	}
	glGenTextures(1, &buffer->id);
	glBindTexture(GL_TEXTURE_BUFFER, buffer->id);
	glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buffer->bo.Id());
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return buffer;

}

/**
*	�f�X�g���N�^
*/
Buffer::~Buffer() {
	glDeleteTextures(1, &id);
}

/**
*	�o�b�t�@�Ƀf�[�^�]������
*
*	@param offset	�]���J�n�ʒu(�o�C�g�P��)
*	@param size		�]������o�C�g��
*	@param data		�]������f�[�^�ւ̃|�C���^
*
*	@retval true	�]������
*	@retval false	�]�����s
*/
bool Buffer::BufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
	return bo.BufferSubData(offset, size, data);
}

/**
*	�L���[�u�}�b�v�E�e�N�X�`�����쐬����
*
*	@param pathList	�L���[�u�}�b�v�p�摜�t�@�C�����̃��X�g
*
*	@return �쐬�����e�N�X�`���I�u�W�F�N�g
*/
CubePtr Cube::Create(const std::vector<std::string>& pathList) {
	if (pathList.size() < 6) {
		std::cerr << "[] " << __func__ << "�L���[�u�}�b�v�ɂ�6���̉摜���K�v�ł���" <<
			pathList.size() << "�������w�肳��Ă��܂���.\n";
		for (size_t i = 0; i < pathList.size(); ++i) {
			std::cerr << "	pathList["<< i << "]=" << pathList[i] <<"\n";
		}
		return  nullptr;
	}

	std::vector<ImageData> imageDataList;
	imageDataList.resize(6);
	for (int i = 0; i < 6; ++i) {
		if (!LoadImage2D(pathList[i].c_str(), &imageDataList[i])) {
			return nullptr;
		}
	}

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (int i = 0; i < 6; ++i) {
		const ImageData& image = imageDataList[i];
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
			image.width, image.height, 0, image.format, image.type, image.data.data());
		const GLenum result = glGetError();
		if (result != GL_NO_ERROR) {
			std::cerr << "[�G���[] " << pathList[i] << "�̓ǂݍ��݂Ɏ��s("
				<< std::hex << result << ").\n";
			glDeleteTextures(1, &id);
			return nullptr;
		}
	}

	// �e�N�X�`���̃p�����[�^�[��ݒ肷��B
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	CubePtr p = std::make_shared<Cube>();
	p->id = id;
	p->width = imageDataList[0].width;
	p->height = imageDataList[0].height;

	return p;
}

/**
*	�f�X�g���N�^
*/
Cube::~Cube() {
	glDeleteTextures(1, &id);
}




} // namespace Texture