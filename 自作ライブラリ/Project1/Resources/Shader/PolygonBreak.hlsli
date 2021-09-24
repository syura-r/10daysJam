cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // ビュープロジェクション行列
    matrix world; // ワールド行列
    float3 cameraPos; // カメラ座標（ワールド座標）
    float4 color;
    uint drawShadow;
};

cbuffer cbuff1 : register(b1)
{
    float3 m_ambient : packoffset(c0); // アンビエント係数
    float3 m_diffuse : packoffset(c1); // ディフューズ係数
    float3 m_specular : packoffset(c2); // スペキュラー係数
    float m_alpha : packoffset(c2.w); // アルファ
}

cbuffer cbuff3 : register(b2)
{
    //float4 Status;
	
    float _Destruction; //分解度合い
    float _ScaleFactor; //スケールの変化量
    float _PositionFactor; //ポジションの変化量
    float _RotationFactor; //回転の変化量
    uint _Tessellation;//ポリゴン分割度
    uint _OnEasing;//イージングで分解するか

}

static const int MAX_BONES = 32;
cbuffer skinning : register(b3)
{
    matrix matSkinning[MAX_BONES];
}

struct VSInput
{
    float4 svpos : POSITION; // システム用頂点座標	
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; // uv値
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

// 平行光源の数
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightcolor; // ライトの色(RGB)
};

//点光源
static const int POINTLIGHT_NUM = 3;
struct PointLight
{
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライトの距離減衰係数
    uint active;
};

//スポットライト
static const int SPOTLIGHT_NUM = 3;
struct SpotLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightpos; //ライト座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライトの距離減衰係数
    float2 lightfactoranglecos; //ライト減衰角度のコサイン
    uint active;
};

//丸影の数
static const int CIRCLESHADOW_NUM = 1;

struct CircleShadow
{
    float3 dir; // 投影方向の逆ベクトル(単位ベクトル)
    float3 casterPos; //キャスター座標
    float distanceCasterLight; //キャスターとライトの距離
    float3 atten; //距離減衰係数
    float2 factorAngleCos; //減衰角度のコサイン
    uint active;

};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 worldpos : POSITION; // ワールド座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // uv値
};

struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; // uv値
    float destruction : TEXCOORD2;
};


struct HS_CONTROL_POINT_OUTPUT
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 worldpos : POSITION; // ワールド座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // uv値
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
    float4 worldpos : POSITION; // ワールド座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // uv値
};

