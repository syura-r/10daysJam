#include "Particle.hlsli"
//VSOutput main(float4 pos : POSITION, float scale : TEXCOORD,float4 color:COLOR,float3 rotation:ROTATION,uint billboardActive:BILLBOARD)
//{
//	VSOutput output; // ピクセルシェーダーに渡す値
//	output.pos = pos;
//	output.scale = scale;
//	output.color = color;
//	output.rotation = rotation;
//	output.billboardActive = billboardActive;
//	return output;
//}
float random(in float2 _st)
{
    return frac(sin(dot(_st.xy,
                         float2(12.9898, 78.233))) *
        43758.5453123);
}

struct VSInput
{
    float4 position : POSITION;
    float scale : SCALE;
	
    float3 rotation : ROTATION;
	//ビルボードするかどうか
    uint billboardActive : BILLBOARD;
	
    float4 color : COLOR;
	
 //   float3 pad :TEXCOORD0a;
 //   uint pad1 : TEXCOORD1a;
	////死亡フラグ
 //   uint pad2 : TEXCOORD2a;
 //   float pad3 : TEXCOORD3a;

 //   float2 pad4 : TEXCOORD4a;
    //uint InstanceId : SV_InstanceID; // インスタンスＩＤ

    //float3 pad : POSITION;
    ////ビルボードするかどうか
    //uint billboardActive : BILLBOARD;

    //float4x4 mat : MATRIX;
    //uint InstanceId : SV_InstanceID; // インスタンスＩＤ
};

VSOutput main(VSInput input)
{
    VSOutput output; // ピクセルシェーダーに渡す値
    output.pos = float4(input.position);
    output.scale = input.scale;
    output.color = input.color;
    output.rotation = input.rotation;
    output.billboardActive = input.billboardActive;
    return output;
}