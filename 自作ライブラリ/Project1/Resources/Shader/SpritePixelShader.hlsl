#include"SpriteShaderHeader.hlsli"

Texture2D<float4> tex0:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0);  //0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) :SV_TARGET
{
	return tex0.Sample(smp,input.uv)*color;
}