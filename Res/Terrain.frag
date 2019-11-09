/**
*	@file Terrain.frag
*/

#version 430

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inNormal;

out vec4 fragColor;

uniform sampler2D texColorArray[4];

struct AmbientLight{
	vec4 color;
};

struct DirectionalLight{
	vec4 color;
	vec4 direction;
};

struct PointLight{
	vec4 color;
	vec4 position;
};

struct SpotLight{
	vec4 color;
	vec4 dirAndCutOff;
	vec4 posAndInnerCutOff;
};


layout(std140) uniform LightUniformBlock{
	AmbientLight ambientLight;
	DirectionalLight directionalLight;
	PointLight pointLight[100];
	SpotLight spotLight[100];
};

uniform int pointLightCount;		//�|�C���g���C�g�̐�
uniform int pointLightIndex[8];

uniform int spotLightCount;			// �X�|�b�g���C�g�̐�
uniform int spotLightIndex[8];

/**
*	�X�v���C�g�p�t���O�����g�V�F�[�_�[
*/

void main(){
	vec3 normal = normalize(inNormal);
	vec3 lightColor = ambientLight.color.rgb;
	float power = max(dot(normal, -directionalLight.direction.xyz),0.0);
	lightColor += directionalLight.color.rgb * power;

	for(int i = 0; i < pointLightCount; ++i){
		int id = pointLightIndex[i];
		vec3 lightVector = pointLight[id].position.xyz - inPosition;
		vec3 lightDir = normalize(lightVector);
		float cosTheta = clamp(dot(normal,lightDir), 0.0, 1.0);
		float intensity = 1.0 / (1.0 + dot (lightVector,lightVector));
		lightColor += pointLight[id].color.rgb * cosTheta * intensity;
	}

	for(int i = 0; i < spotLightCount; ++i){
		int id = spotLightIndex[i];
		vec3 lightVector = spotLight[id].posAndInnerCutOff.xyz - inPosition;
		vec3 lightDir = normalize(lightVector);
		float cosTheta = clamp(dot(normal,lightDir), 0.0, 1.0);
		float intensity = 1.0 / (1.0 + dot(lightVector,lightVector));
		float spotCosTheta = dot(lightDir, -spotLight[id].dirAndCutOff.xyz);
		float cutOff = smoothstep(spotLight[id].dirAndCutOff.w,
			spotLight[id].posAndInnerCutOff.w,spotCosTheta);
		lightColor += spotLight[id].color.rgb * cosTheta * intensity * cutOff;
		}

	
	// �n�`�e�N�X�`��������
	vec4 ratio = texture(texColorArray[0],inTexCoord);
	float baseRatio = max(0.0, 1.0 - ratio.r - ratio.g);
	vec2 uv = inTexCoord * 10.0;
	fragColor.rgb = texture(texColorArray[1], uv).rgb * baseRatio;
	fragColor.rgb += texture(texColorArray[2],uv).rgb * ratio.r;
	fragColor.rgb += texture(texColorArray[3],uv).rgb * ratio.g;
	fragColor.a = 1.0;

	fragColor.rgb *= lightColor;
	
 }