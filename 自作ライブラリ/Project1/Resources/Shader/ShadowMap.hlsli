cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // �r���[�v���W�F�N�V�����s��
    matrix world; // ���[���h�s��
};

static const int MAX_BONES = 32;
cbuffer skinning : register(b1)
{
    matrix matSkinning[MAX_BONES];
}

cbuffer fbx : register(b1)
{
    float3 playerPos;
    float4 GrassBottomColor;
    float4 GrassTopColor;
    float2 AirDir;
    float AirPower;
    uint Time;
    float playerRadius;
    float GrassWidth;
    float GrassHeight;
	
}

cbuffer polygonBreak : register(b1)
{
    //float4 Status;
	
    float _Destruction; //����x����
    float _ScaleFactor; //�X�P�[���̕ω���
    float _PositionFactor; //�|�W�V�����̕ω���
    float _RotationFactor; //��]�̕ω���
    uint _Tessellation; //�|���S�������x

}


// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
};

struct VSGrassOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float2 uv : TEXCOORD; // uv�l
};


struct VSInput
{
    float4 svpos : POSITION; // �V�X�e���p���_���W	
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct GSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3


struct DS_OUTPUT
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
};

