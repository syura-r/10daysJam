#include"DrawShadowOBJ.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ

SamplerState smp0 : register(s0); // 0番スロットに設定されたサンプラー
SamplerComparisonState smp1 : register(s1); // 1番スロットに設定されたサンプラー


float4 PSmain(VSOutput input) : SV_TARGET
{
    	//テクスチャマッピング
    float4 texcolor = tex0.Sample(smp0, input.uv);


	const float zlnLVP = input.shadowPos.z / input.shadowPos.w;
	
    float2 shadowMapUV = input.shadowPos.xy / input.shadowPos.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
	
    float shadow = tex1.SampleCmpLevelZero(smp1,shadowMapUV,zlnLVP);
	
    float3 shadowColor = texcolor.xyz * 0.5f;

    texcolor.xyz = lerp(texcolor.xyz, shadowColor, shadow);
	
	//シェーディングによる色
    float4 shadecolor;
	//鏡面反射光
    float3 ambient = m_ambient;
	//拡散反射光
    float3 diffuse = m_diffuse;
	//全て加算する
    shadecolor.rgb = (ambient + diffuse);
    shadecolor.a = m_alpha;

	//シェーディングによる色で描画
    return shadecolor * texcolor * color ;
}