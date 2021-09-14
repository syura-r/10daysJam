cbuffer cbuff0 : register(b0)
{
	//matrix mat; // ３Ｄ変換行列
	matrix viewproj;//ビュープロジェクション行列
	matrix world;//ワールド行列
	float3 cameraPos;//カメラ座標(ワールド座標)
	float4 color;
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient  : packoffset(c0); // アンビエント係数
	float3 m_diffuse  : packoffset(c1); // ディフューズ係数
	float3 m_specular : packoffset(c2); // スペキュラー係数
	float m_alpha : packoffset(c2.w);	// アルファ
}

static const int MAX_BONES = 32;
cbuffer skinning:register(b3)
{
    matrix matSkinning[MAX_BONES];
}
// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標	
	float3 normal:NORMAL;//法線
	float2 uv  :TEXCOORD; // uv値
};
struct VSInput
{
    float4 svpos : POSITION; // システム用頂点座標	
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; // uv値
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct PSOutPut
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
    float4 target2 : SV_TARGET2;
	
};