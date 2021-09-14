#include "Particle.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
	return tex0.Sample(smp, input.uv) * float4(input.color.rgb*input.color.a,1);
	//return float4(1, 1, 1, 1);
}