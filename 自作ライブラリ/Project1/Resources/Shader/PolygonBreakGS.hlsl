
#include "PolygonBreak.hlsli"
#include"PerlinNoise.hlsli"


float3 rotate(in float3 vec, in float3 axis, in float val)
{
    float4 tmp = float4(vec.xyz, 0);

    float3 a = (axis.xyz != 0) ? axis.xyz : 0;
    float thita = val;
    float s = sin(thita);
    float c = cos(thita);
    float r = 1.0 - c;

    float4x4 tmpMatrix = float4x4(
					a.x * a.x * r + c, a.y * a.x * r + a.z * s, a.z * a.x * r - a.y * s, 0,
					a.x * a.y * r - a.z * s, a.y * a.y * r + c, a.z * a.y * r + a.x * s, 0,
					a.x * a.z * r + a.y * s, a.y * a.z * r - a.x * s, a.z * a.z * r + c, 0,
					0, 0, 0, 1
					);
    tmp = mul(tmpMatrix, tmp);
    return tmp;
}


float EaseInOutQuart(const float start, const float end, const int count, const float time)
{
    float t = time;
    t /= count;
    return lerp(start, end, 8 * t * t * t * t * step(0.5f, 1 - t) + (1 - pow(-2 * t + 2, 4) / 2) * step(0.5, t));

}



[maxvertexcount(8)]
void GSmain(
	triangle DS_OUTPUT input[3] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
	// ポリゴンの中心を計算。
    const float3 center = (input[0].svpos + input[1].svpos + input[2].svpos) / 3;

    // ポリゴンの辺ベクトルを計算し、ポリゴンの法線を計算する。
    const float3 vec1 = input[1].svpos - input[0].svpos;
    const float3 vec2 = input[2].svpos - input[0].svpos;

    //const float destruction = _Destruction;
	
    float r = 2.0 * (random(float2(random(center.xy), random(center.yx))) - 0.5);
    const float3 r3 = r.xxx;
    const float3 randNormal = float3(random(float2(random(center.xx), random(center.zx))) - 0.5, random(float2(random(center.xy), random(center.xz))) - 0.5, random(float2(random(center.yy), random(center.yz))) - 0.5);
    const float3 normal = normalize(cross(vec1, vec2) + randNormal);
    const float3 randNormal2 = float3(random(float2(random(randNormal.xx), random(randNormal.zx))) - 0.5, random(float2(random(randNormal.xy), random(randNormal.xz))) - 0.5, random(float2(random(randNormal.yy), random(randNormal.yz))) - 0.5);

    const float randPoint = abs(random(randNormal2.xy) / 2);
	
    const float destruction = EaseInOutQuart(0, 1.0f, 600, 600 * smoothstep(randPoint, randPoint + 0.5f, _Destruction));


	//if (destruction != 0)
    //{
    //    GSOutput o1[4];
    //    o1[0].svpos = float4((float3(1, 1, 1) + center) * normal, 1);
    //    o1[1].svpos = float4((float3(1, -1, 1) + center) * normal, 1);
    //    o1[2].svpos = float4((float3(-1, 1, 1) + center) * normal, 1);
    //    o1[3].svpos = float4((float3(-1, -1, 1) + center) * normal, 1);

    	
    //    //GSOutput o1[8];
    //    //o1[0].svpos = float4(float3(1, 1, 1) + center, 1);
    //    //o1[1].svpos = float4(float3(1, -1, 1) + center, 1);
    //    //o1[2].svpos = float4(float3(-1, 1, 1) + center, 1);
    //    //o1[3].svpos = float4(float3(-1, -1, 1) + center, 1);
    //    //o1[4].svpos = float4(float3(1, 1, -1) + center, 1);
    //    //o1[5].svpos = float4(float3(1, -1, -1) + center, 1);
    //    //o1[6].svpos = float4(float3(-1, 1, 1) + center, 1);
    //    //o1[7].svpos = float4(float3(-1, -1, -1) + center, 1);
	
    //    for (uint i = 0; i < 4; i++)
    //    {
    //        o1[i].svpos = mul(mul(viewproj, world), o1[i].svpos);
    //        o1[i].normal = input[0].normal;
    //        o1[i].uv = input[0].uv;
    //    //float4 wpos = mul(world, v.worldpos);
    //    //o.worldpos = wpos;
    //        o1[i].worldpos = input[0].worldpos;

    //        output.Append(o1[i]);
    //    }
    //}
    //else
    //{
    for (uint i = 0; i < 3; i++)
    {
        DS_OUTPUT v = input[i];
        GSOutput o;

       //  UNITY_SETUP_INSTANCE_ID(v);
       //UNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(o);
        // center位置を起点にスケールを変化させます。
        v.svpos.xyz = (v.svpos.xyz - center) * (1.0 - smoothstep(0.1f, 1.0f, destruction) * _ScaleFactor) + center;
        // center位置を起点に、乱数を用いて回転を変化させます。
        v.svpos.xyz = rotate(v.svpos.xyz - center, center, r3 * smoothstep(0.1f, 1.0f, destruction) * _RotationFactor).xyz + center;

        // 法線方向に位置を変化させます
        //v.svpos.xyz += normal * destruction * _PositionFactor * (r3 + 1.0);
        v.svpos.xyz += normal * smoothstep(0.2f, 0.5f, destruction) * _PositionFactor * (r3 + 1.0) + normalize(randNormal2) * smoothstep(0.5f, 1.0f, destruction) * _PositionFactor * (r3 + 1.0);

        // 最後に、修正した頂点位置を射影変換しレンダリング用に変換します。
        o.svpos = mul(mul(viewproj, world), v.svpos);

        const float4 wnormal = normalize(mul(world, float4(v.normal, 0)));

        o.normal = wnormal.xyz;
        o.uv = v.uv;
        //float4 wpos = mul(world, v.worldpos);
        //o.worldpos = wpos;
        o.worldpos = v.worldpos;
        //UNITY_TRANSFER_FOG(o, o.vertex);
        o.destruction = destruction;
        output.Append(o);
    }
    //}
    output.RestartStrip();
}
