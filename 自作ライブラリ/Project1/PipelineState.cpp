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
	ComPtr < ID3DBlob> vsBlob = nullptr; //���_�V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> psBlob = nullptr; //�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> gsBlob = nullptr; //�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> hsBlob = nullptr; //�n���V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> dsBlob = nullptr; //�h���C���V�F�[�_�I�u�W�F�N�g

	ComPtr < ID3DBlob> csBlob = nullptr; //�R���s���[�g�V�F�[�_�I�u�W�F�N�g


	ComPtr < ID3DBlob> errorBlob = nullptr; //�G���[�I�u�W�F�N�g

				// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	//�f�X�N���v�^�����W�̐ݒ�			
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0 ���W�X�^
		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//�[�x�e�X�g���s��
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA

	//���[�g�p�����[�^�\�̔z��
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	//���_���C�A�E�g
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};

	switch (shader)
	{
	case Bloom:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);

		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/BlurPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		//�e�N�X�`���p
			//1�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��ɏ㏑�����[��
		break;
	}
	case BasePostEffect:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);

		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/BasePostEffectPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//�f�X�N���v�^�����W2�ڂ̐ݒ�			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 ���W�X�^

		//�f�X�N���v�^�����W3�ڂ̐ݒ�			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV3;
		descRangeSRV3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);//t2 ���W�X�^

		//�f�X�N���v�^�����W4�ڂ̐ݒ�			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV4;
		descRangeSRV4.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);//t3 ���W�X�^

		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		//�e�N�X�`���p
			//1�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//3�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//4�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(),rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��ɏ㏑�����[��

		break;
	}
	case NormalShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//R�̂�32�r�b�g��FLOAT�`��
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);

		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��ɏ㏑�����[��

		break;
	}
	case FBXShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//R�̂�32�r�b�g��FLOAT�`��
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapFBXVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);

		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
			
		layout =
		{//�e�����󂯂�{�[���ԍ�(4��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
			
		layout =
		{//�{�[���X�L�j���O�E�F�C�g(4��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case GrassShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//R�̂�32�r�b�g��FLOAT�`��
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NoConvertSMVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//�W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapGrassGS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"GSmain", "gs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case PBShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//R�̂�32�r�b�g��FLOAT�`��
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NoConvertSMVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);

		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//�W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBGS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"GSmain", "gs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//�n���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBHS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"HSmain", "hs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &hsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//�h���C���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPBDS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"DSmain", "ds_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &dsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		break;
	}

	case SPRITE:
	{
		gpipeline.BlendState.AlphaToCoverageEnable = true;
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/SpritePixelShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);

		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		//�e�N�X�`���p
			//1�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PARTICLE:
	{
			
		// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleVS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "vs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&vsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticlePS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "ps_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&psBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		// �W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleGS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"main", "gs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//���_���C�A�E�g
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


		// �f�v�X�̏������݂��֎~
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
		//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		//�e�N�X�`���p
			//1�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case GRASS:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//�W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/GrassGS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"GSmain", "gs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
			
		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
			//4�Ԗ�
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


		break;
	}
	case PolygonBreak:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//�W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakGS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"GSmain", "gs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &gsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//�n���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakHS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"HSmain", "hs_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &hsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		//�h���C���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/PolygonBreakDS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"DSmain", "ds_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &dsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);
		layout =
		{//�e�����󂯂�{�[���ԍ�(4��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
		layout =
		{//�{�[���X�L�j���O�E�F�C�g(4��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//4�Ԗ�
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//5�Ԗ�
		param.InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

			
		break;
	}
	case BasicObj:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicVertexShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicPixelShader.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA


		break;
	}
	case NormalMap:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
		// �W�I���g���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapGS.hlsl",	// �V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
			"GSmain", "gs_5_0",	// �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
			0,
			&gsBlob, &errorBlob);
		if (FAILED(result)) {
			// errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// �G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//�f�X�N���v�^�����W2�ڂ̐ݒ�			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 ���W�X�^


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//4�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case FBX:
	{
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`255 �w��� RGBA

		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}

		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);
		layout =
		{//�e�����󂯂�{�[���ԍ�(4��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
		layout =
		{//�{�[���X�L�j���O�E�F�C�g(4��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//4�Ԗ�
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case NoShade:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NoShadeVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/NoShadePS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�萔�p

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case DrawShadowOBJ:
	{
		//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJVS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, //�C���N���[�h�\�ɂ���
			"VSmain", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f����
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
			0, &vsBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJPS.hlsl",  //�V�F�[�_�t�@�C����
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
			"PSmain", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_���f���w��
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
			0, &psBlob, &errorBlob);
		if (FAILED(result)) {
			//errorBlob����G���[���e��string�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}


		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};// xy���W(1 �s�ŏ������ق������₷��)
		inputLayout.push_back(layout);
		layout =
		{//�@���x�N�g��
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
		}; //uv���W
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		//�f�X�N���v�^�����W2�ڂ̐ݒ�			
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 ���W�X�^


		//���[�g�p�����[�^�̐ݒ�
		CD3DX12_ROOT_PARAMETER param;
		//�萔�p
			//0�Ԗ�
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//1�Ԗ�
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//2�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);
		//�e�N�X�`���p
			//3�Ԗ�
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL); //���
		rootparams.push_back(param);

		//�萔�p
			//3�Ԗ�
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //���
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
	
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); //�J�����O���Ȃ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�w�i���J�����O
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�(8���邪���͈�����g��Ȃ�)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//�W���ݒ�
	//�u�����h�X�e�[�g�ɐݒ肷��
	blenddesc.BlendEnable = true; // �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // �\�[�X�̒l�� 100% �g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // �f�X�g�̒l�� 0% �g��



	gpipeline.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//�[�x�l�̃t�H�[�}�b�g

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//static HRESULT r;
	//r = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//printf("%d\n", r);
	assert(SUCCEEDED(result));
	// ���[�g�V�O�l�`���̐���
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature[keyName]));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
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

