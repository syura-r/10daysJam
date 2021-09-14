#pragma once
#include<wrl.h>
#include<d3dx12.h>
#include"DirectXLib.h"
#include<unordered_map>
using namespace Microsoft::WRL;

class DirectXLib;
enum BLENDTYPE
{
	NOBLEND,
	ALPHA,
	ADD,
	SUB,
	COLORFLIP,
};
enum ShaderType
{
	PolygonBreak,
	GRASS,
	FBX,
	NoShade,
	PARTICLE,
	SPRITE,
	BasePostEffect,
	BasicObj,
	Bloom,
	NormalMap,
	NormalShadowMap,
	PBShadowMap,
	FBXShadowMap,
	GrassShadowMap,
	DrawShadowOBJ,
	GPUPARTICLE,
};

class PipelineState
{
public:
	static void SetPipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA);
	static void SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA);

	static void CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType = ALPHA);
private:
	DirectXLib* device;
	
	static std::unordered_map<std::string,ComPtr<ID3D12RootSignature>>rootsignature;
	static std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> pipelinestate;
	

};

