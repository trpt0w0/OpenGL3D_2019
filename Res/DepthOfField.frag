/**
*	 @file DepthOfField.frag
*/

#version 430

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D texColorArray[2];		// 0:�J���[ 1:�[�x

/**
*	��ʂ̏��
*
*	x: 1.0 / �E�B���h�E�̕�(�s�N�Z���P��)
*	y: 1.0 / �E�B���h�E�̍���(�s�N�Z���P��)
*	x: near(m�P��) 
*	y: far(m�P��)
*/
uniform vec4 viewInfo;

/**
*	�J�����̏��
*
*	x: �ŕ���(�s���g�̍����ʒu�̃����Y����̋���. �o�P��)
*	y: �œ_����(����1�_�ɏW�܂�ʒu�̃����Y����̋���. mm�P��)
*	z: �J��(���̎�����̃T�C�Y. mm�P��)
*	w: �Z���T�[�T�C�Y(�����󂯎��Z���T�[�̉���. mm�P��)
*/
uniform vec4 cameraInfo;

// �|�A�\���f�B�X�N�T���v�����O�̂��߂̍��W���X�g
const int poissonSampleCount = 12;

const vec2 poissonDisk[poissonSampleCount] = {
	{ -0.326, -0.406 }, { -0.840, -0.074 }, { -0.696,  0.457 },
	{ -0.203,  0.621 }, {  0.962, -0.195 }, {  0.473, -0.480 },
	{  0.519,  0.767 }, {  0.185, -0.893 }, {  0.507,  0.064 },
	{  0.896,  0.412 }, { -0.322, -0.933 }, { -0.792, -0.598 }
	};

/**
*	�[�x�o�b�t�@�̐[�x�l���r���[���W�n�̐[�x�l�ɕϊ�����
*
*	@param w �[�x�o�b�t�@�̐[�x�l
*
*	@return w���r���[���W�n�̐[�x�l�ɕϊ������l
*/
float ToRealZ(float w){
	float near = viewInfo.z;
	float far = viewInfo.w;
	float n = 2.0 * w -1.0;
	return -2.0 * near * far / (far + near - n * (far - near));
}

/**
*	�����~�̔��a���v�Z����
*
*	@param objectDistance �Ώۃs�N�Z���̃J�������W�n�ɂ�����Z���W
*
*	@return �����~�̔��a(�s�N�Z���P��).
*/
vec2 CalcCoC(float objectDistance){
	float focalPlane = cameraInfo.x;
	float focalLength = cameraInfo.y;
	float aperture = cameraInfo.z;
	float sensorSize = cameraInfo.w;
	float mmToPixel = 1.0 / (sensorSize * viewInfo.x);

	objectDistance *= -1000.0;	// �o�P�ʂɕϊ����A�l�𐳂ɂ���B
	return abs(aperture * (focalLength * (focalPlane - objectDistance)) / 
			(objectDistance * (focalPlane - focalLength))) * mmToPixel * viewInfo.xy;


}

/**
*	��ʊE�[�x�V�F�[�_�[.
*/

void main(){
	float centerZ = ToRealZ(texture(texColorArray[1], inTexCoord).r);
	vec2 coc = CalcCoC(centerZ);
	fragColor.rgb = texture(texColorArray[0],inTexCoord).rgb;
	for(int i = 0; i < poissonSampleCount; ++i ){
		vec2 uv = inTexCoord + coc * poissonDisk[i];
		fragColor.rgb += texture(texColorArray[0],uv).rgb;
	}

	fragColor.rgb = fragColor.rgb / float(poissonSampleCount + 1);
	fragColor.a = 1.0;

}

