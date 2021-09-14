#include"FBX.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[


PSOutPut PSmain(VSOutput input) 
{
    PSOutPut output;
	//�e�N�X�`���}�b�s���O
	float4 texcolor = tex0.Sample(smp, input.uv);
	//�V�F�[�f�B���O�ɂ��F
    float3 light = normalize(float3(1, -1, 1));
	//�g�U���ˌ�
	float diffuse =  saturate(dot(-light,input.normal));
    float brightness = diffuse + 0.3f;
    float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
	//�V�F�[�f�B���O�ɂ��F�ŕ`��
	float4 rColor = shadecolor * texcolor * color;
    output.target0 = rColor;
    output.target1 = float4(1 - rColor.rgb,1);
    float y = dot(float3(0.2299f, 0.587f, 0.114f), rColor);
    output.target2 = lerp(0.0f, rColor, step(0.80, y));
    return output;
}