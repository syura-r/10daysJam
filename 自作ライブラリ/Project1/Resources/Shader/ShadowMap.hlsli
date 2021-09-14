cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // ビュープロジェクション行列
    matrix world; // ワールド行列
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
	
    float _Destruction; //分解度合い
    float _ScaleFactor; //スケールの変化量
    float _PositionFactor; //ポジションの変化量
    float _RotationFactor; //回転の変化量
    uint _Tessellation; //ポリゴン分割度

}


// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct VSGrassOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};


struct VSInput
{
    float4 svpos : POSITION; // システム用頂点座標	
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3


struct DS_OUTPUT
{
    float4 svpos : SV_POSITION; // システム用頂点座標
};

