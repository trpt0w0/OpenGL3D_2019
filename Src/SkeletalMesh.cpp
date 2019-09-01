/**
* @file SleletalMesh.cpp
*/
#define NOMINMAX
#include "SkeletalMesh.h"
#include "UniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <algorithm>

#pragma runtime_checks("", off)
#pragma optimize("", on)
#pragma auto_inline(on)
#pragma inline_depth(16)
#pragma inline_recursion(on)
#pragma intrinsic(memset, memcpy, sin, cos, tan, sqrt, atan2)

namespace Mesh {

// Implemented in Mesh.cpp
std::vector<char> ReadFile(const char* path);
void GetBuffer(const json11::Json& accessor, const json11::Json& bufferViews, const std::vector<std::vector<char>>& binFiles, const void** pp, size_t* pLength, int* pStride = nullptr);

/**
* �X�P���^�����b�V���Ɋւ���O���[�o���f�[�^�y�т��̐���R�[�h���i�[���閼�O���.
*/
namespace SkeletalAnimation {

namespace /* unnamed */ {

/**
* �X�P���^���E���b�V���`��pUBO�f�[�^.
*/
struct UniformDataMeshMatrix
{
  glm::vec4 color;
  glm::mat3x4 matModel[8]; // it must transpose.
  glm::mat3x4 matBones[255]; // it must transpose.
};

const char UniformNameForBoneMatrix[] = "MeshMatrixUniformData";

bool isInitialized = false; ///< �O���[�o���f�[�^������������Ă�����true.
int currentUboIndex = 0; ///< UBO�_�u���o�b�t�@�̏������ݑ��C���f�b�N�X.
UniformBufferPtr ubo[2]; ///< �{�[���s�񓙂̓]����ƂȂ�UBO(�_�u���o�b�t�@).
std::vector<uint8_t> uboData; ///< UBO�ɓ]������f�[�^���ꎞ�I�ɕۑ����邽�߂̃o�b�t�@.
GLint uboOffsetAlignment = 0;

} // unnamed namespace

/**
* �X�P���^�����b�V������p�̃O���[�o���f�[�^������������.
*/
bool Initialize()
{
  if (!isInitialized) {
    const size_t maxMeshCount = 1024;
    const GLsizeiptr uboSize = static_cast<GLsizeiptr>(sizeof(UniformDataMeshMatrix) * maxMeshCount);
    ubo[0] = UniformBuffer::Create(uboSize, 0, UniformNameForBoneMatrix);
    ubo[1] = UniformBuffer::Create(uboSize, 0, UniformNameForBoneMatrix);
    uboData.reserve(uboSize);
    currentUboIndex = 0;

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uboOffsetAlignment);

    isInitialized = true;
  }

  return true;
}

/**
* �X�P���^�����b�V������p�̃O���[�o���f�[�^��j������.
*/
void Finalize()
{
  if (isInitialized) {
    uboData.clear();
    uboData.shrink_to_fit();
    ubo[1].reset();
    ubo[0].reset();
    isInitialized = false;
  }
}

/**
* �V�F�[�_�[�v���O�����ɃX�P���^�����b�V���̐���p�f�[�^�̃o�C���f�B���O�|�C���g��ݒ肷��.
*
* @param program �o�C���f�B���O�|�C���g�ݒ��̃V�F�[�_�[�v���O����.
*
* @retval true  �ݒ萬��.
* @retval false �ݒ莸�s.
*/
bool BindUniformBlock(const Shader::ProgramPtr& program)
{
  static const GLuint bindingPoint = 0;
  static const char* const blockName = UniformNameForBoneMatrix;

  const GLuint id = program->Get();
  const GLuint blockIndex = glGetUniformBlockIndex(id, blockName);
  if (blockIndex == GL_INVALID_INDEX) {
    std::cerr << "[�G���[] Uniform�u���b�N'" << blockName << "'��������܂���\n";
    return false;
  }
  glUniformBlockBinding(id, blockIndex, bindingPoint);
  const GLenum result = glGetError();
  if (result != GL_NO_ERROR) {
    std::cerr << "[�G���[] Uniform�u���b�N'" << blockName << "'�̃o�C���h�Ɏ��s\n";
    return false;
  }
  return true;
}

/**
* �X�P���^�����b�V���̐���p�f�[�^��ǉ��\�ɂ���.
*/
void ResetUniformData()
{
  if (!isInitialized) {
    return;
  }

  uboData.clear();
}

/**
* �X�P���^�����b�V���̐���p�f�[�^��ǉ�����.
*
* @param data UBO�ɒǉ�����f�[�^�̃A�h���X.
* @param size UBO�ɒǉ�����f�[�^�̃o�C�g��.
*
* @retval 0�ȏ� �f�[�^���ǉ����ꂽ�ʒu�������I�t�Z�b�g�l.
* @retval -1    �ǉ��Ɏ��s.
*/
GLintptr PushUniformData(const void* data, size_t size)
{
  if (!isInitialized) {
    return -1;
  }

  const size_t alignedSize = ((size + uboOffsetAlignment - 1) / uboOffsetAlignment) * uboOffsetAlignment;

  UniformBufferPtr pUbo = ubo[currentUboIndex];
  if (uboData.size() + alignedSize >= static_cast<size_t>(pUbo->Size())) {
    return -1;
  }
  const uint8_t* p = static_cast<const uint8_t*>(data);
  const GLintptr offset = static_cast<GLintptr>(uboData.size());
  uboData.insert(uboData.end(), p, p + size);
  if (size < alignedSize) {
    uboData.resize(uboData.size() + (alignedSize - size));
  }
  return offset;
}

/**
* �X�P���^�����b�V���̐���p�f�[�^��GPU�������ɓ]������.
*/
void UploadUniformData()
{
  if (!isInitialized) {
    return;
  }

  if (!uboData.empty()) {
    UniformBufferPtr pUbo = ubo[currentUboIndex];
    pUbo->BufferSubData(uboData.data(), 0, uboData.size());
    currentUboIndex = !currentUboIndex;
  }
}

/**
* �X�P���^�����b�V������p�f�[�^��UBO�o�C���f�B���O�|�C���g�Ɋ��蓖�Ă�.
*
* @param offset ���ꂩ��`�悷��X�P���^�����b�V���̂��߂́A�X�P���^�����b�V������p�f�[�^�̈ʒu.
* @param size   �X�P���^�����b�V������p�f�[�^�̃o�C�g��.
*/
void BindUniformData(GLintptr offset, GLsizeiptr size)
{
  if (!isInitialized) {
    return;
  }

  ubo[!currentUboIndex]->BindBufferRange(offset, size);
}

} // namespace SkeletalAnimation

/**
* �A�j���[�V�����p�̒��ԃf�[�^.
*/
struct AnimatedNodeTree {
  struct Transformation {
    glm::vec3 translation = glm::vec3(0);
    glm::quat rotation = glm::quat(0, 0, 0, 1);
    glm::vec3 scale = glm::vec3(1);
    glm::mat4 matLocal = glm::mat4(1);
    glm::mat4 matGlobal = glm::mat4(1);
    bool isCalculated = false;
    bool hasTransformation = false;
  };
  std::vector<Transformation> nodeTransformations;
};

/**
* �w�肳�ꂽ�t���[���ɂ�����A�j���[�V�����f�[�^���擾����.
*
* @param data  �A�j���[�V�����f�[�^.
* @param frame �f�[�^���擾����t���[��.
*
* @return frame�ɂ�����A�j���[�V�����f�[�^.
*/
template<typename T>
T Interporation(const Timeline<T>& data, float frame)
{
  const auto maxFrame = std::lower_bound(data.timeline.begin(), data.timeline.end(), frame,
    [](const KeyFrame<T>& keyFrame, float frame) { return keyFrame.frame < frame; });
  if (maxFrame == data.timeline.begin()) {
    return data.timeline.front().value;
  }
  if (maxFrame == data.timeline.end()) {
    return data.timeline.back().value;
  }
  const auto minFrame = maxFrame - 1;
  const float ratio = glm::clamp((frame - minFrame->frame) / (maxFrame->frame - minFrame->frame), 0.0f, 1.0f);
  return glm::mix(minFrame->value, maxFrame->value, ratio);
}

/**
* �w�肳�ꂽ�t���[���ɂ�����A�j���[�V�����f�[�^���擾����.
*
* @param data  �A�j���[�V�����f�[�^.
* @param frame �f�[�^���擾����t���[��.
*
* @return frame�ɂ�����A�j���[�V�����f�[�^.
*/
template<typename T, glm::qualifier Q>
glm::qua<T, Q> Interporation(const Timeline<glm::qua<T, Q> >& data, float frame)
{
  const auto maxFrame = std::lower_bound(data.timeline.begin(), data.timeline.end(), frame,
    [](const KeyFrame<glm::qua<T, Q>>& keyFrame, float frame) { return keyFrame.frame < frame; });
  if (maxFrame == data.timeline.begin()) {
    return data.timeline.front().value;
  }
  if (maxFrame == data.timeline.end()) {
    return data.timeline.back().value;
  }
  const auto minFrame = maxFrame - 1;
  const float ratio = glm::clamp((frame - minFrame->frame) / (maxFrame->frame - minFrame->frame), 0.0f, 1.0f);
  return glm::slerp(minFrame->value, maxFrame->value, ratio);
}

/**
* �O���[�o���ϊ��s����v�Z����.
*
* @param nodes    �v�Z�Ώۂ̃m�[�h�Q.
* @param node     �v�Z�Ώۂ̃m�[�h.
* @param animated �v�Z���ʂ��i�[���钆�ԃf�[�^�\��.
*/
void CalcGlobalTransform(const std::vector<Node>& nodes, const Node& node, AnimatedNodeTree& animated)
{
  const int currentNodeId = &node - &nodes[0];
  AnimatedNodeTree::Transformation& transformation = animated.nodeTransformations[currentNodeId];
  if (transformation.isCalculated) {
    return;
  }

  if (node.parent) {
    CalcGlobalTransform(nodes, *node.parent, animated);
    const int parentNodeId = node.parent - &nodes[0];
    transformation.matLocal = animated.nodeTransformations[parentNodeId].matLocal;
  } else {
    transformation.matLocal = glm::mat4(1);
  }
  if (transformation.hasTransformation) {
    const glm::mat4 T = glm::translate(glm::mat4(1), transformation.translation);
    const glm::mat4 R = glm::mat4_cast(transformation.rotation);
    const glm::mat4 S = glm::scale(glm::mat4(1), transformation.scale);
    transformation.matLocal *= T * R * S;
  } else {
    transformation.matLocal *= node.matLocal;
  }
  transformation.matGlobal = transformation.matLocal * node.matInverseBindPose;
  transformation.isCalculated = true;
}

/**
* �A�j���[�V������Ԃ��ꂽ���W�ϊ��s����v�Z����.
*
* @param file      �A�j���[�V�����Ώۂ̃m�[�h�Q��ێ�����t�@�C��.
* @param animation �A�j���[�V�����f�[�^.
* @param frame     �f�[�^���擾����t���[��.
*
* @return �v�Z���ʂ��i�[���钆�ԃf�[�^�\��.
*/
AnimatedNodeTree MakeAnimatedNodeTree(const ExtendedFile& file, const Animation& animation, float frame)
{
  AnimatedNodeTree tmp;
  tmp.nodeTransformations.resize(file.nodes.size());
  for (const auto& e : animation.scaleList) {
    tmp.nodeTransformations[e.targetNodeId].scale = Interporation(e, frame);
    tmp.nodeTransformations[e.targetNodeId].hasTransformation = true;
  }
  for (const auto& e : animation.rotationList) {
    tmp.nodeTransformations[e.targetNodeId].rotation = Interporation(e, frame);
    tmp.nodeTransformations[e.targetNodeId].hasTransformation = true;
  }
  for (const auto& e : animation.translationList) {
    tmp.nodeTransformations[e.targetNodeId].translation = Interporation(e, frame);
    tmp.nodeTransformations[e.targetNodeId].hasTransformation = true;
  }
  for (auto& e : file.nodes) {
    CalcGlobalTransform(file.nodes, e, tmp);
  }
  return tmp;
}

/**
* �A�j���[�V������K�p�������W�ϊ��s�񃊃X�g.
*/
struct MeshTransformation {
  std::vector<glm::mat4> transformations;
  std::vector<glm::mat4> matRoot;
};

/**
* ���b�V���v�f�����m�[�h�̃��X�g���擾����.
*
* @param node �T���̋N�_�ƂȂ�m�[�h.
* @param list ���b�V���v�f�����m�[�h���X�g�̍쐬��.
*/
void GetMeshNodeList(const Node* node, std::vector<const Node*>& list)
{
  if (node->mesh >= 0) {
    list.push_back(node);
  }
  for (const auto& child : node->children) {
    GetMeshNodeList(child, list);
  }
}

/**
* �A�j���[�V������K�p�������W�ϊ��s�񃊃X�g���v�Z����.
*
* @param file      �A�j���[�V�����ƃm�[�h�����L����t�@�C���I�u�W�F�N�g.
* @param node      �X�L�j���O�Ώۂ̃m�[�h.
* @param animation �v�Z�̌��ɂȂ�A�j���[�V����.
* @param frame     �A�j���[�V�����̍Đ��ʒu.
*
* @return �A�j���[�V������K�p�������W�ϊ��s�񃊃X�g.
*/
MeshTransformation CalculateTransform(const ExtendedFilePtr& file, const Node* node, const Animation* animation, float frame)
{
  MeshTransformation transformation;
  if (file && node) {
    std::vector<const Node*> meshNodes;
    meshNodes.reserve(32);
    GetMeshNodeList(node, meshNodes);

    if (node->skin >= 0) {
      if (animation) {
        const AnimatedNodeTree tmp = MakeAnimatedNodeTree(*file, *animation, frame);
        const std::vector<int>& joints = file->skins[node->skin].joints;
        transformation.transformations.resize(joints.size());
        for (size_t i = 0; i < joints.size(); ++i) {
          const int jointNodeId = joints[i];
          transformation.transformations[i] = tmp.nodeTransformations[jointNodeId].matGlobal;
        }
      } else {
        const std::vector<int>& joints = file->skins[node->skin].joints;
        transformation.transformations.resize(joints.size(), glm::mat4(1));
      }
      transformation.matRoot.resize(meshNodes.size(), glm::mat4(1));
    } else {
      transformation.matRoot.reserve(meshNodes.size());
      for (const auto& e : meshNodes) {
        transformation.matRoot.push_back(e->matGlobal);
      }
    }
  }
  return transformation;
}

/**
* �R���X�g���N�^.
*
* @param p �e�ƂȂ郁�b�V���o�b�t�@.
* @param f ���b�V���t�@�C��.
* @param n ���b�V���m�[�h.
*/
SkeletalMesh::SkeletalMesh(const ExtendedFilePtr& f, const Node* n)
  : file(f), node(n)
{
}

/**
* �A�j���[�V������Ԃ��X�V����.
*
* @param deltaTime �O��̍X�V����̌o�ߎ���.
* @param matModel  ���f���s��.
* @param color     ���b�V���̐F.
*/
void SkeletalMesh::Update(float deltaTime, const glm::mat4& matModel, const glm::vec4& color)
{
  // �Đ��t���[���X�V.
  if (animation && state == State::play) {
    frame += deltaTime * animationSpeed;
    if (loop) {
      if (frame >= animation->totalTime) {
        frame -= animation->totalTime;
      } else if (frame < 0) {
        const float n = std::ceil(-frame / animation->totalTime);
        frame += animation->totalTime * n;
      }
    } else {
      if (frame >= animation->totalTime) {
        frame = animation->totalTime;
      } else if (frame < 0) {
        frame = 0;
      }
    }
  }

  // �eBuffer��UBO�f�[�^��ǉ�.
  SkeletalAnimation::UniformDataMeshMatrix uboData;
  uboData.color = color;
  const MeshTransformation mt = CalculateTransform(file, node, animation, frame);
  for (size_t i = 0; i < mt.transformations.size(); ++i) {
    uboData.matBones[i] = glm::transpose(mt.transformations[i]);
  }
  if (mt.matRoot.empty()) {
    uboData.matModel[0] = glm::transpose(matModel);
  } else {
    const size_t size = std::min(sizeof(uboData.matModel)/sizeof(uboData.matModel[0]), mt.matRoot.size());
    for (size_t i = 0; i < size; ++i) {
      const glm::mat4 m = matModel * mt.matRoot[i];
      uboData.matModel[i] = glm::transpose(m);
    }
  }
  uboSize = sizeof(glm::vec4) + sizeof(glm::mat3x4) * 8 + sizeof(glm::mat3x4) * mt.transformations.size();
  uboSize = ((uboSize + 255) / 256) * 256;
  uboOffset = SkeletalAnimation::PushUniformData(&uboData, uboSize);

  // ��Ԃ��X�V.
  if (animation) {
    switch (state) {
    case State::stop:
      break;
    case State::play:
      if (!loop && (frame >= animation->totalTime)) {
        state = State::stop;
      }
      break;
    case State::pause:
      break;
    }
  }
}

/**
* �X�P���^�����b�V����`�悷��.
*/
void SkeletalMesh::Draw() const
{
  if (!file) {
    return;
  }

  // TODO: �V�[�����x���̕`��ɑΉ����邱��.
  //std::vector<const Node*> meshNodes;
  //meshNodes.reserve(32);
  //GetMeshNodeList(node, meshNodes);

  SkeletalAnimation::BindUniformData(uboOffset, uboSize);

  const Mesh& meshData = file->meshes[node->mesh];
  GLuint prevTexId = 0;
  for (const auto& prim : meshData.primitives) {
    prim.vao->Bind();

    if (prim.material >= 0 && prim.material < static_cast<int>(file->materials.size())) {
      const Material& m = file->materials[prim.material];
      if (!m.progSkeletalMesh) {
        continue;
      }
      m.progSkeletalMesh->Use();
      if (m.texture) {
        const GLuint texId = m.texture->Get();
        if (prevTexId != texId) {
          m.progSkeletalMesh->BindTexture(0, texId);
          prevTexId = texId;
        }
      }
      const GLint locMaterialColor = glGetUniformLocation(m.progSkeletalMesh->Get(), "materialColor");
      if (locMaterialColor >= 0) {
        glUniform4fv(locMaterialColor, 1, &m.baseColor.x);
      }
      glDrawElementsBaseVertex(prim.mode, prim.count, prim.type, prim.indices, prim.baseVertex);
    }
  }
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
* �A�j���[�V�����̍Đ���Ԃ��擾����.
*
* @return �Đ���Ԃ�����State�񋓌^�̒l.
*/
SkeletalMesh::State SkeletalMesh::GetState() const
{
  return state;
}

/**
* ���b�V���Ɋ֘A�t����ꂽ�A�j���[�V�����̃��X�g���擾����.
*
* @return �A�j���[�V�������X�g.
*/
const std::vector<Animation>& SkeletalMesh::GetAnimationList() const
{
  if (!file) {
    static const std::vector<Animation> dummy;
    return dummy;
  }
  return file->animations;
}

/**
* �A�j���[�V�������Ԃ��擾����.
*
* @return �A�j���[�V��������(�b).
*/
float SkeletalMesh::GetTotalAnimationTime() const
{
  if (!file || !animation) {
    return 0;
  }
  return animation->totalTime;
}

/**
* �A�j���[�V�������Đ�����.
*
* @param animationName �Đ�����A�j���[�V�����̖��O.
* @param loop      ���[�v�Đ��̎w��(true=���[�v���� false=���[�v���Ȃ�).
*
* @retval true  �Đ��J�n.
* @retval false �Đ����s.
*/
bool SkeletalMesh::Play(const std::string& animationName, bool loop)
{
  if (file) {
    for (const auto& e : file->animations) {
      if (e.name == animationName) {
        animation = &e;
        frame = 0;
        state = State::play;
        this->loop = loop;
        return true;
      }
    }
  }
  return false;
}

/**
* �A�j���[�V�����̍Đ����~����.
*
* @retval true  ����.
* @retval false ���s(�A�j���[�V�������ݒ肳��Ă��Ȃ�).
*/
bool SkeletalMesh::Stop()
{
  if (animation) {
    switch (state) {
    case State::play:
      state = State::stop;
      return true;
    case State::stop:
      return true;
    case State::pause:
      state = State::stop;
      return true;
    }
  }
  return false;
}

/**
* �A�j���[�V�����̍Đ����ꎞ��~����.
*
* @retval true  ����.
* @retval false ���s(�A�j���[�V��������~���Ă���A�܂��̓A�j���[�V�������ݒ肳��Ă��Ȃ�).
*/
bool SkeletalMesh::Pause()
{
  if (animation) {
    switch (state) {
    case State::play:
      state = State::pause;
      return true;
    case State::stop:
      return false;
    case State::pause:
      return true;
    }
  }
  return false;
}

/**
* �A�j���[�V�����̍Đ����ĊJ����.
*
* @retval true  ����.
* @retval false ���s(�A�j���[�V��������~���Ă���A�܂��̓A�j���[�V�������ݒ肳��Ă��Ȃ�).
*/
bool SkeletalMesh::Resume()
{
  if (animation) {
    switch (state) {
    case State::play:
      return true;
    case State::stop:
      return false;
    case State::pause:
      state = State::play;
      return true;
    }
  }
  return false;
}

/**
* �Đ����̃A�j���[�V���������擾����.
*
* @return �Đ����̃A�j���[�V������.
*         ��x��Play()�ɐ������Ă��Ȃ��ꍇ�A��̕����񂪕Ԃ����.
*/
const std::string& SkeletalMesh::GetAnimation() const
{
  if (!animation) {
    static const std::string dummy("");
    return dummy;
  }
  return animation->name;
}

/**
* �A�j���[�V�����̍Đ����x��ݒ肷��.
*
* @param speed �Đ����x(1.0f=����, 2.0f=2�{��, 0.5f=1/2�{��).
*/
void SkeletalMesh::SetAnimationSpeed(float speed)
{
  animationSpeed = speed;
}

/**
* �A�j���[�V�����̍Đ����x���擾����.
*
* @return �Đ����x.
*/
float SkeletalMesh::GetAnimationSpeed() const
{
  return animationSpeed;
}

/**
* �A�j���[�V�����̍Đ��ʒu��ݒ肷��.
*
* @param position �Đ��ʒu(�b).
*/
void SkeletalMesh::SetPosition(float position)
{
  frame = position;
  if (animation) {
    if (loop) {
      if (frame >= animation->totalTime) {
        frame -= animation->totalTime;
      } else if (frame < 0) {
        const float n = std::ceil(-frame / animation->totalTime);
        frame += animation->totalTime * n;
      }
    } else {
      if (frame >= animation->totalTime) {
        frame = animation->totalTime;
      } else if (frame < 0) {
        frame = 0;
      }
    }
  }
}

/**
* �A�j���[�V�����̍Đ��ʒu���擾����.
*
* @return �Đ��ʒu(�b).
*/
float SkeletalMesh::GetPosition() const
{
  return frame;
}

/**
* �A�j���[�V�����̍Đ����I�����Ă��邩���ׂ�.
*
* @retval true  �I�����Ă���.
* @retval false �I�����Ă��Ȃ�. �܂��́A��x���L���Ȗ��O��Play()�����s����Ă��Ȃ�.
*
* ���[�v�Đ����̏ꍇ�A���̊֐��͏��false��Ԃ����Ƃɒ���.
*/
bool SkeletalMesh::IsFinished() const
{
  if (!file || !animation) {
    return false;
  }
  return animation->totalTime <= frame;
}

/**
* ���[�v�Đ��̗L�����擾����.
*
* @retval true ���[�v�Đ������.
* @retval false ���[�v�Đ�����Ȃ�.
*/
bool SkeletalMesh::Loop() const
{
  return loop;
}

/**
* ���[�v�Đ��̗L�����擾����.
*
* @param loop ���[�v�Đ��̗L��.
*/
void SkeletalMesh::Loop(bool loop)
{
  this->loop = loop;
}

namespace /* unnamed */ {

/**
* JSON�̔z��f�[�^��glm::vec3�ɕϊ�����.
*
* @param json �ϊ����ƂȂ�z��f�[�^.
*
* @return json��ϊ����Ăł���vec3�̒l.
*/
glm::vec3 GetVec3(const json11::Json& json)
{
  const std::vector<json11::Json>& a = json.array_items();
  if (a.size() < 3) {
    return glm::vec3(0);
  }
  return glm::vec3(a[0].number_value(), a[1].number_value(), a[2].number_value());
}

/**
* JSON�̔z��f�[�^��glm::quat�ɕϊ�����.
*
* @param json �ϊ����ƂȂ�z��f�[�^.
*
* @return json��ϊ����Ăł���quat�̒l.
*/
glm::quat GetQuat(const json11::Json& json)
{
  const std::vector<json11::Json>& a = json.array_items();
  if (a.size() < 4) {
    return glm::quat(0, 0, 0, 1);
  }
  return glm::quat(
    static_cast<float>(a[3].number_value()),
    static_cast<float>(a[0].number_value()),
    static_cast<float>(a[1].number_value()),
    static_cast<float>(a[2].number_value())
  );
}

/**
* JSON�̔z��f�[�^��glm::mat4�ɕϊ�����.
*
* @param json �ϊ����ƂȂ�z��f�[�^.
*
* @return json��ϊ����Ăł���mat4�̒l.
*/
glm::mat4 GetMat4(const json11::Json& json)
{
  const std::vector<json11::Json>& a = json.array_items();
  if (a.size() < 16) {
    return glm::mat4(1);
  }
  return glm::mat4(
    a[0].number_value(), a[1].number_value(), a[2].number_value(), a[3].number_value(),
    a[4].number_value(), a[5].number_value(), a[6].number_value(), a[7].number_value(),
    a[8].number_value(), a[9].number_value(), a[10].number_value(), a[11].number_value(),
    a[12].number_value(), a[13].number_value(), a[14].number_value(), a[15].number_value()
  );
}

/**
* �m�[�h�̃��[�J���p���s����v�Z����.
*
* @param node gltf�m�[�h.
*
* @return node�̃��[�J���p���s��.
*/
glm::mat4 CalcLocalMatrix(const json11::Json& node)
{
  if (node["matrix"].is_array()) {
    return GetMat4(node["matrix"]);
  } else {
    glm::mat4 m(1);
    if (node["translation"].is_array()) {
      m *= glm::translate(glm::mat4(1), GetVec3(node["translation"]));
    }
    if (node["rotation"].is_array()) {
      m *= glm::mat4_cast(GetQuat(node["rotation"]));
    }
    if (node["scale"].is_array()) {
      m *= glm::scale(glm::mat4(1), GetVec3(node["scale"]));
    }
    return m;
  }
}

} // unnamed namespace

/**
* glTF�t�@�C����ǂݍ���.
*
* @param path glTF�t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool Buffer::LoadSkeletalMesh(const char* path)
{
  // gltf�t�@�C����ǂݍ���.
  std::vector<char> gltfFile = ReadFile(path);
  if (gltfFile.empty()) {
    return false;
  }
  gltfFile.push_back('\0');

  // json���.
  std::string err;
  const json11::Json json = json11::Json::parse(gltfFile.data(), err);
  if (!err.empty()) {
    std::cerr << "ERROR: " << path << "�̓ǂݍ��݂Ɏ��s���܂���.\n";
    std::cerr << err << "\n";
    return false;
  }

  // �o�C�i���t�@�C����ǂݍ���.
  std::vector<std::vector<char>> binFiles;
  for (const json11::Json& e : json["buffers"].array_items()) {
    const json11::Json& uri = e["uri"];
    if (!uri.is_string()) {
      std::cerr << "[�G���[]" << __func__ << ": " << path << "�ɕs����uri������܂�.\n";
      return false;
    }
    const std::string binPath = std::string("Res/") + uri.string_value();
    binFiles.push_back(ReadFile(binPath.c_str()));
    if (binFiles.back().empty()) {
      return false;
    }
  }

  ExtendedFilePtr pFile = std::make_shared<ExtendedFile>();
  ExtendedFile& file = *pFile;

  // �A�N�Z�b�T����f�[�^���擾����GPU�֓]��.
  const json11::Json& accessors = json["accessors"];
  const json11::Json& bufferViews = json["bufferViews"];

  // �C���f�b�N�X�f�[�^�ƒ��_�����f�[�^�̃A�N�Z�b�TID���擾.
  file.meshes.reserve(json["meshes"].array_items().size());
  for (const auto& currentMesh : json["meshes"].array_items()) {
    Mesh mesh;
    mesh.name = currentMesh["name"].string_value();
    const std::vector<json11::Json>& primitives = currentMesh["primitives"].array_items();
    mesh.primitives.resize(primitives.size());
    for (size_t primId = 0; primId < primitives.size(); ++primId) {
      const json11::Json& primitive = currentMesh["primitives"][primId];

      // ���_�C���f�b�N�X.
      {
        const int accessorId_index = primitive["indices"].int_value();
        const json11::Json& accessor = accessors[accessorId_index];
        if (accessor["type"].string_value() != "SCALAR") {
          std::cerr << "ERROR: �C���f�b�N�X�f�[�^�E�^�C�v��SCALAR�łȂ��Ă͂Ȃ�܂��� \n";
          std::cerr << "  type = " << accessor["type"].string_value() << "\n";
          return false;
        }

        mesh.primitives[primId].mode = primitive["mode"].is_null() ? GL_TRIANGLES : primitive["mode"].int_value();
        mesh.primitives[primId].count = accessor["count"].int_value();
        mesh.primitives[primId].type = accessor["componentType"].int_value();
        mesh.primitives[primId].indices = reinterpret_cast<const GLvoid*>(iboEnd);

        const void* p;
        size_t byteLength;
        GetBuffer(accessor, bufferViews, binFiles, &p, &byteLength);
        ibo.BufferSubData(iboEnd, byteLength, p);
        iboEnd += ((byteLength + 3) / 4) * 4; // ���ɗ���̂��ǂ̃f�[�^�^�ł����v�Ȃ悤��4�o�C�g���E�ɐ���.
      }

      // ���_����.
      const json11::Json& attributes = primitive["attributes"];
      const int accessorId_position = attributes["POSITION"].int_value();
      const int accessorId_normal = attributes["NORMAL"].is_null() ? -1 : attributes["NORMAL"].int_value();
      const int accessorId_texcoord = attributes["TEXCOORD_0"].is_null() ? -1 : attributes["TEXCOORD_0"].int_value();
      const int accessorId_weights = attributes["WEIGHTS_0"].is_null() ? -1 : attributes["WEIGHTS_0"].int_value();
      const int accessorId_joints = attributes["JOINTS_0"].is_null() ? -1 : attributes["JOINTS_0"].int_value();

      mesh.primitives[primId].vao = std::make_shared<VertexArrayObject>();
      mesh.primitives[primId].vao->Create(vbo.Id(), ibo.Id());
      SetAttribute(&mesh.primitives[primId], 0, accessors[accessorId_position], bufferViews, binFiles);
      SetAttribute(&mesh.primitives[primId], 1, accessors[accessorId_texcoord], bufferViews, binFiles);
      SetAttribute(&mesh.primitives[primId], 2, accessors[accessorId_normal], bufferViews, binFiles);
      SetAttribute(&mesh.primitives[primId], 3, accessors[accessorId_weights], bufferViews, binFiles);
      SetAttribute(&mesh.primitives[primId], 4, accessors[accessorId_joints], bufferViews, binFiles);

      mesh.primitives[primId].material = primitive["material"].int_value();
    }
    file.meshes.push_back(mesh);
  }

  // �}�e���A��.
  {
    const std::vector<json11::Json> materials = json["materials"].array_items();
    file.materials.reserve(materials.size());
    for (const json11::Json& material : materials) {
      std::string texturePath;
      const json11::Json& pbr = material["pbrMetallicRoughness"];
      const json11::Json& index = pbr["baseColorTexture"]["index"];
      if (index.is_number()) {
        const int textureId = index.int_value();
        const json11::Json& texture = json["textures"][textureId];
        const int imageSourceId = texture["source"].int_value();
        const json11::Json& imageName = json["images"][imageSourceId]["name"];
        if (imageName.is_string()) {
          texturePath = std::string("Res/") + imageName.string_value() + ".tga";
        }
      }
      glm::vec4 col(1);
      const std::vector<json11::Json>& baseColorFactor = pbr["baseColorFactor"].array_items();
      if (baseColorFactor.size() >= 4) {
        for (size_t i = 0; i < 4; ++i) {
          col[i] = static_cast<float>(baseColorFactor[i].number_value());
        }
      }
      Texture::Image2DPtr tex;
      if (!texturePath.empty()) {
        tex = Texture::Image2D::Create(texturePath.c_str());
      }
      file.materials.push_back(CreateMaterial(col, tex));
    }
  }

  // �m�[�h�c���[���\�z.
  {
    const json11::Json& nodes = json["nodes"];
    int i = 0;
    file.nodes.resize(nodes.array_items().size());
    for (const auto& node : nodes.array_items()) {
      // �e�q�֌W���\�z.
      // NOTE: �|�C���^���g�킸�Ƃ��C���f�b�N�X�ŏ\����������Ȃ�.
      const std::vector<json11::Json>& children = node["children"].array_items();
      file.nodes[i].children.reserve(children.size());
      for (const auto& e : children) {
        const int childJointId = e.int_value();
        file.nodes[i].children.push_back(&file.nodes[childJointId]);
        if (!file.nodes[childJointId].parent) {
          file.nodes[childJointId].parent = &file.nodes[i];
        }
      }

      // ���[�J�����W�ϊ��s����v�Z.
      file.nodes[i].matLocal = CalcLocalMatrix(nodes[i]);

      ++i;
    }

    // �V�[���̃��[�g�m�[�h���擾.
    file.scenes.reserve(json["scenes"].array_items().size());
    for (const auto& scene : json["scenes"].array_items()) {
      Scene tmp;
      tmp.rootNode = scene.int_value();
      GetMeshNodeList(&file.nodes[tmp.rootNode], tmp.meshNodes);
      file.scenes.push_back(tmp);
    }
  }

  {
    for (size_t i = 0; i < file.nodes.size(); ++i) {
      file.nodes[i].matGlobal = file.nodes[i].matLocal;
      Node* parent = file.nodes[i].parent;
      while (parent) {
        file.nodes[i].matGlobal = parent->matLocal * file.nodes[i].matGlobal;
        parent = parent->parent;
      }
    }
  }

  file.skins.reserve(json["skins"].array_items().size());
  for (const auto& skin : json["skins"].array_items()) {
    Skin tmpSkin;

    // �o�C���h�|�[�Y�s����擾.
    const json11::Json& accessor = accessors[skin["inverseBindMatrices"].int_value()];
    if (accessor["type"].string_value() != "MAT4") {
      std::cerr << "ERROR: �o�C���h�|�[�Y��type��MAT4�łȂ��Ă͂Ȃ�܂��� \n";
      std::cerr << "  type = " << accessor["type"].string_value() << "\n";
      return false;
    }
    if (accessor["componentType"].int_value() != GL_FLOAT) {
      std::cerr << "ERROR: �o�C���h�|�[�Y��componentType��GL_FLOAT�łȂ��Ă͂Ȃ�܂��� \n";
      std::cerr << "  type = 0x" << std::hex << accessor["componentType"].string_value() << "\n";
      return false;
    }

    const void* p;
    size_t byteLength;
    GetBuffer(accessor, bufferViews, binFiles, &p, &byteLength);

    // gltf�̃o�b�t�@�f�[�^�̓��g���G���f�B�A��. �d�l�ɏ����Ă���.
    const std::vector<json11::Json>& joints = skin["joints"].array_items();
    std::vector<glm::mat4> inverseBindPoseList;
    inverseBindPoseList.resize(accessor["count"].int_value());
    memcpy(inverseBindPoseList.data(), p, std::min(byteLength, inverseBindPoseList.size() * 64));
    tmpSkin.joints.resize(joints.size());
    for (size_t i = 0; i < joints.size(); ++i) {
      const int jointId = joints[i].int_value();
      tmpSkin.joints[i] = jointId;
      file.nodes[jointId].matInverseBindPose = inverseBindPoseList[i];
    }
    tmpSkin.name = skin["name"].string_value();
    file.skins.push_back(tmpSkin);
  }

  {
    const std::vector<json11::Json>& nodes = json["nodes"].array_items();
    for (size_t i = 0; i < nodes.size(); ++i) {
      const json11::Json& meshId = nodes[i]["mesh"];
      if (meshId.is_number()) {
        file.nodes[i].mesh = meshId.int_value();
      }
      const json11::Json& skinId = nodes[i]["skin"];
      if (skinId.is_number()) {
        file.nodes[i].skin = skinId.int_value();
      }
    }
  }

  // �A�j���[�V����.
  {
    for (const auto& animation : json["animations"].array_items()) {
      Animation anime;
      anime.translationList.reserve(32);
      anime.rotationList.reserve(32);
      anime.scaleList.reserve(32);
      anime.name = animation["name"].string_value();

      const std::vector<json11::Json>& channels = animation["channels"].array_items();
      const std::vector<json11::Json>& samplers = animation["samplers"].array_items();
      for (const json11::Json& e : channels) {
        const int samplerId = e["sampler"].int_value();
        const json11::Json& sampler = samplers[samplerId];
        const json11::Json& target = e["target"];
        const int targetNodeId = target["node"].int_value();
        if (targetNodeId < 0) {
          continue;
        }

        const int inputAccessorId = sampler["input"].int_value();
        const int inputCount = accessors[inputAccessorId]["count"].int_value();
        const void* pInput;
        size_t inputByteLength;
        GetBuffer(accessors[inputAccessorId], bufferViews, binFiles, &pInput, &inputByteLength);

        const int outputAccessorId = sampler["output"].int_value();
        const int outputCount = accessors[outputAccessorId]["count"].int_value();
        const void* pOutput;
        size_t outputByteLength;
        GetBuffer(accessors[outputAccessorId], bufferViews, binFiles, &pOutput, &outputByteLength);

        const std::string& path = target["path"].string_value();
        anime.totalTime = 0;
        if (path == "translation") {
          const GLfloat* pKeyFrame = static_cast<const GLfloat*>(pInput);
          const glm::vec3* pData = static_cast<const glm::vec3*>(pOutput);
          Timeline<glm::vec3> timeline;
          timeline.timeline.reserve(inputCount);
          for (int i = 0; i < inputCount; ++i) {
            anime.totalTime = std::max(anime.totalTime, pKeyFrame[i]);
            timeline.timeline.push_back({ pKeyFrame[i], pData[i] });
          }
          timeline.targetNodeId = targetNodeId;
          anime.translationList.push_back(timeline);
        } else if (path == "rotation") {
          const GLfloat* pKeyFrame = static_cast<const GLfloat*>(pInput);
          const glm::quat* pData = static_cast<const glm::quat*>(pOutput);
          Timeline<glm::quat> timeline;
          timeline.timeline.reserve(inputCount);
          for (int i = 0; i < inputCount; ++i) {
            anime.totalTime = std::max(anime.totalTime, pKeyFrame[i]);
            timeline.timeline.push_back({ pKeyFrame[i], pData[i] });
          }
          timeline.targetNodeId = targetNodeId;
          anime.rotationList.push_back(timeline);
        } else if (path == "scale") {
          const GLfloat* pKeyFrame = static_cast<const GLfloat*>(pInput);
          const glm::vec3* pData = static_cast<const glm::vec3*>(pOutput);
          Timeline<glm::vec3> timeline;
          timeline.timeline.reserve(inputCount);
          for (int i = 0; i < inputCount; ++i) {
            anime.totalTime = std::max(anime.totalTime, pKeyFrame[i]);
            timeline.timeline.push_back({ pKeyFrame[i], pData[i] });
          }
          timeline.targetNodeId = targetNodeId;
          anime.scaleList.push_back(timeline);
        }
      }
      file.animations.push_back(anime);
    }
  }

  file.name = path;
  extendedFiles.insert(std::make_pair(file.name, pFile));
  for (size_t i = 0; i < file.nodes.size(); ++i) {
    const int meshIndex = file.nodes[i].mesh;
    if (meshIndex < 0) {
      continue;
    }
    const Mesh& mesh = file.meshes[meshIndex];
    meshes.insert(std::make_pair(mesh.name, MeshIndex{ pFile, &pFile->nodes[i] }));
  }

  std::cout << "[INFO]" << __func__ << ": '" << path << "'��ǂݍ��݂܂���.\n";
  std::cout << "  total nodes = " << file.nodes.size() << "\n";
  for (size_t i = 0; i < file.meshes.size(); ++i) {
    std::cout << "  mesh[" << i << "] = " << file.meshes[i].name << "\n";
  }
  for (size_t i = 0; i < file.animations.size(); ++i) {
    std::cout << "  animation[" << i << "] = " << file.animations[i].name << "(" << file.animations[i].totalTime << "sec)\n";
  }
  for (size_t i = 0; i < file.skins.size(); ++i) {
    std::cout << "  skin[" << i << "] = " << file.skins[i].name << "(" << file.skins[i].joints.size() << ")\n";
  }

  return true;
}

/**
* �X�P���^�����b�V�����擾����.
*
* @param meshName �擾�������X�P���^�����b�V���̖��O.
*
* @return meshName�Ɠ������O�����X�P���^�����b�V��.
*/
SkeletalMeshPtr Buffer::GetSkeletalMesh(const char* meshName) const
{
  const auto itr = meshes.find(meshName);
  if (itr == meshes.end()) {
    static SkeletalMeshPtr dummy(std::make_shared<SkeletalMesh>());
    return dummy;
  }

  ExtendedFilePtr pFile = itr->second.file;
  const Node* pNode = itr->second.node;
  return std::make_shared<SkeletalMesh>(pFile, pNode);
}

} // namespace Mesh