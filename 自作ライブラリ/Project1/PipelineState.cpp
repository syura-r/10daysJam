#include "PipelineState.h"
#include <d3dcompiler.h>

std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> PipelineState::rootsignature = {};
std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> PipelineState::pipelinestate = {};

void PipelineState::SetPipeline(const std::string& keyName, const BLENDTYPE blendType)
{
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
		assert(0);
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	cmdList->SetGraphicsRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
}

void PipelineState::SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType)
{
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
		assert(0);
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	cmdList->SetComputeRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());

}

void PipelineState::CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType)
{
	ID3D12Device* dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	ComPtr < ID3DBlob> vsBlob = nullptr; //頂点シェーダオブジェクト
	ComPtr < ID3DBlob> psBlob = nullptr; //ピクセルシェーダオブジェクト
	ComPtr < ID3DBlob> gsBlob = nullptr; //ピクセルシェーダオブジェクト
	ComPtr < ID3DBlob> hsBlob = nullptr; //ハルシェーダオブジェクト
	ComPtr < ID3DBlob> dsBlob = nullptr; //ドメインシェーダオブジェクト

	ComPtr < ID3DBlob> csBlob = nullptr; //コンピュートシェーダオブジェクト


	ComPtr < ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

				// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	//デスクリプタレンジの設定			
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0 レジスタ
		//デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//深度テストを行う
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA

	//ルートパラメータ―の配列
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	//頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};

	switch (shader)
	{
	case Bloom:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/BlurPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		//テクスチャ用
			//1番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール
		break;
	}
	case BasePostEffect:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/BasePostEffectPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//デスクリプタレンジ2つ目の設定			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 レジスタ

		//デスクリプタレンジ3つ目の設定			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV3;
		descRangeSRV3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);//t2 レジスタ

		//デスクリプタレンジ4つ目の設定			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV4;
		descRangeSRV4.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);//t3 レジスタ

		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		//テクスチャ用
			//1番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//3番目
		param.InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//4番目
		param.InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(),rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール

		break;
	}
	case NormalShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール

		break;
	}
	case FBXShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapFBXVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
			
		layout =
		{//影響を受けるボーン番号(4つ)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
			
		layout =
		{//ボーンスキニングウェイト(4つ)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case GrassShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NoConvertSMVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapGrassGS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"GSmain", "gs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case PBShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NoConvertSMVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBGS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"GSmain", "gs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//ハルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBHS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"HSmain", "hs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &hsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//ドメインシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBDS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"DSmain", "ds_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &dsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		break;
	}

	case SPRITE:
	{
		gpipeline.BlendState.AlphaToCoverageEnable = true;
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/SpritePixelShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);

		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		//テクスチャ用
			//1番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PARTICLE:
	{
			
		// 頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleVS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticlePS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleGS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC
			layout =
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"SCALE",0,DXGI_FORMAT_R32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"ROTATION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"BILLBOARD",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"TEXCOORD0a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"TEXCOORD1a",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"TEXCOORD2a",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"TEXCOORD3a",0,DXGI_FORMAT_R32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);
			
		layout =
		{
			"TEXCOORD4a",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		// デプスの書き込みを禁止
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		CD3DX12_ROOT_PARAMETER param;
		//定数用
		//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		//テクスチャ用
			//1番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case GRASS:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassGS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"GSmain", "gs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
			
		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
			//4番目
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


		break;
	}
	case PolygonBreak:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakGS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"GSmain", "gs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//ハルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakHS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"HSmain", "hs_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &hsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//ドメインシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakDS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"DSmain", "ds_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &dsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);
		layout =
		{//影響を受けるボーン番号(4つ)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
		layout =
		{//ボーンスキニングウェイト(4つ)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//4番目
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//5番目
		param.InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

			
		break;
	}
	case BasicObj:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicVertexShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicPixelShader.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA


		break;
	}
	case NormalMap:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		// ジオメトリシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapGS.hlsl",	// シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"GSmain", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//デスクリプタレンジ2つ目の設定			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 レジスタ


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//4番目
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case FBX:
	{
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA

		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);
		layout =
		{//影響を受けるボーン番号(4つ)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
		layout =
		{//ボーンスキニングウェイト(4つ)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//4番目
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case NoShade:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NoShadeVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/NoShadePS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//定数用

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case DrawShadowOBJ:
	{
		//頂点シェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJVS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
			"VSmain", "vs_5_0",//エントリーポイント名、シェーダーモデル名
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJPS.hlsl",  //シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
			"PSmain", "ps_5_0",//エントリーポイント名、シェーダモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//頂点レイアウト
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy座標(1 行で書いたほうが見やすい)
		inputLayout.push_back(layout);
		layout =
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv座標
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		//デスクリプタレンジ2つ目の設定			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 レジスタ


		//ルートパラメータの設定
		CD3DX12_ROOT_PARAMETER param;
		//定数用
			//0番目
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//1番目
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//2番目
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);
		//テクスチャ用
			//3番目
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);

		//定数用
			//3番目
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		rootparams.push_back(param);


		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		break;
	}
	default:
		assert(0);
		break;
	}

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob != nullptr)
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	if (hsBlob != nullptr)
		gpipeline.HS = CD3DX12_SHADER_BYTECODE(hsBlob.Get());
	if (dsBlob != nullptr)
		gpipeline.DS = CD3DX12_SHADER_BYTECODE(dsBlob.Get());
	
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); //カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//背景をカリング
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	//レンダーターゲットのブレンド設定(8個あるが今は一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
	//ブレンドステートに設定する
	blenddesc.BlendEnable = true; // ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う



	gpipeline.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値のフォーマット

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//static HRESULT r;
	//r = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//printf("%d\n", r);
	assert(SUCCEEDED(result));
	// ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature[keyName]));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature[keyName].Get();

	if (pipelinestate[keyName][blendType] != nullptr)
		pipelinestate[keyName][blendType]->Release();
	switch (blendType)
	{
	case NOBLEND:
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][NOBLEND]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case ALPHA:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		if (shader == BasicObj)
		{
			gpipeline.BlendState.RenderTarget[1] = blenddesc;
			gpipeline.BlendState.RenderTarget[2] = blenddesc;
		}

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ALPHA]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	case ADD:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ADD]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case SUB:
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][SUB]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case COLORFLIP:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][COLORFLIP]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	default:
		break;
	}
}

