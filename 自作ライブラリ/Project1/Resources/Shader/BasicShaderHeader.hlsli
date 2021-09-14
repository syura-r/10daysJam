cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // �r���[�v���W�F�N�V�����s��
	matrix world; // ���[���h�s��
	float3 cameraPos; // �J�������W�i���[���h���W�j
	float4 color;
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient  : packoffset(c0); // �A���r�G���g�W��
	float3 m_diffuse  : packoffset(c1); // �f�B�t���[�Y�W��
	float3 m_specular : packoffset(c2); // �X�y�L�����[�W��
	float m_alpha : packoffset(c2.w);	// �A���t�@
}

// ���s�����̐�
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
	float3 lightv;    // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor;    // ���C�g�̐F(RGB)
};

//�_����
static const int POINTLIGHT_NUM = 3;
struct PointLight
{
	float3 lightpos;//���C�g���W
	float3 lightcolor;//���C�g�̐F
	float3 lightatten;//���C�g�̋��������W��
	uint active;
};

//�X�|�b�g���C�g
static const int SPOTLIGHT_NUM = 3;
struct SpotLight
{
	float3 lightv;    // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightpos;//���C�g���W
	float3 lightcolor;//���C�g�̐F
	float3 lightatten;//���C�g�̋��������W��
	float2 lightfactoranglecos;//���C�g�����p�x�̃R�T�C��
	uint active;
};

//�ۉe�̐�
static const int CIRCLESHADOW_NUM = 1;

struct CircleShadow
{
	float3 dir;    // ���e�����̋t�x�N�g��(�P�ʃx�N�g��)
	float3 casterPos;//�L���X�^�[���W
	float distanceCasterLight;//�L���X�^�[�ƃ��C�g�̋���
	float3 atten;//���������W��
	float2 factorAngleCos;//�����p�x�̃R�T�C��
	uint active;

};
cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION; // ���[���h���W
	float3 normal :NORMAL; // �@��
	float2 uv  :TEXCOORD; // uv�l
};

struct PSOutPut
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
    float4 target2 : SV_TARGET2;
	
};