cbuffer cbuff0 : register(b0)
{
	//matrix mat; // �R�c�ϊ��s��
	matrix viewproj;//�r���[�v���W�F�N�V�����s��
	matrix world;//���[���h�s��
	float3 cameraPos;//�J�������W(���[���h���W)
	float4 color;
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient  : packoffset(c0); // �A���r�G���g�W��
	float3 m_diffuse  : packoffset(c1); // �f�B�t���[�Y�W��
	float3 m_specular : packoffset(c2); // �X�y�L�����[�W��
	float m_alpha : packoffset(c2.w);	// �A���t�@
}

static const int MAX_BONES = 32;
cbuffer skinning:register(b3)
{
    matrix matSkinning[MAX_BONES];
}
// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W	
	float3 normal:NORMAL;//�@��
	float2 uv  :TEXCOORD; // uv�l
};
struct VSInput
{
    float4 svpos : POSITION; // �V�X�e���p���_���W	
    float3 normal : NORMAL; //�@��
    float2 uv : TEXCOORD; // uv�l
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct PSOutPut
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
    float4 target2 : SV_TARGET2;
	
};