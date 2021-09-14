#include"DrawShadowOBJ.hlsli"

Texture2D<float4> tex0 : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1); // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��

SamplerState smp0 : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerComparisonState smp1 : register(s1); // 1�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[


float4 PSmain(VSOutput input) : SV_TARGET
{
    	//�e�N�X�`���}�b�s���O
    float4 texcolor = tex0.Sample(smp0, input.uv);


	const float zlnLVP = input.shadowPos.z / input.shadowPos.w;
	
    float2 shadowMapUV = input.shadowPos.xy / input.shadowPos.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
	
    float shadow = tex1.SampleCmpLevelZero(smp1,shadowMapUV,zlnLVP);
	
    float3 shadowColor = texcolor.xyz * 0.5f;

    texcolor.xyz = lerp(texcolor.xyz, shadowColor, shadow);
	
	//�V�F�[�f�B���O�ɂ��F
    float4 shadecolor;
	//���ʔ��ˌ�
    float3 ambient = m_ambient;
	//�g�U���ˌ�
    float3 diffuse = m_diffuse;
	//�S�ĉ��Z����
    shadecolor.rgb = (ambient + diffuse);
    shadecolor.a = m_alpha;

	//�V�F�[�f�B���O�ɂ��F�ŕ`��
    return shadecolor * texcolor * color ;
}