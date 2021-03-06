/**
* @file SkeletalMesh.vert
*/
#version 410

layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vTexCoord;
layout(location=2) in vec3 vNormal;
layout(location=3) in vec4 vWeights;
layout(location=4) in vec4 vJoints;

layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexCoord;
layout(location=2) out vec3 outNormal;
layout(location=3) out vec3 outPosition;

// global
uniform mat4x4 matMVP;

// per mesh
layout(std140) uniform MeshMatrixUniformData
{
  vec4 color;
  mat3x4 matModel[8]; // it must transpose.
  mat3x4 matBones[256]; // it must transpose.
} vd;

// per primitive
uniform vec4 materialColor;
uniform int meshIndex;

/**
* Vertex shader for SkeletalMesh.
*/
void main()
{
  outColor = materialColor * vd.color;
  outTexCoord = vTexCoord;
  mat3x4 matSkinTmp =
    vd.matBones[int(vJoints.x)] * vWeights.x +
    vd.matBones[int(vJoints.y)] * vWeights.y +
    vd.matBones[int(vJoints.z)] * vWeights.z +
    vd.matBones[int(vJoints.w)] * vWeights.w;
  mat4 matSkin = mat4(transpose(matSkinTmp));
  matSkin[3][3] = dot(vWeights, vec4(1)); // ウェイトが正規化されていない場合の対策([3][3]が1.0になるとは限らない).
  mat4 matModel = mat4(transpose(vd.matModel[0/*meshIndex*/])) * matSkin;
  mat3 matNormal = transpose(inverse(mat3(matModel)));
  outNormal = matNormal * vNormal;
  outPosition = vec3(matModel * vec4(vPosition, 1.0));
  gl_Position = matMVP * matModel * vec4(vPosition, 1.0);
}