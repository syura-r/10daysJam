#include "ParticleManager.h"

#include"DirectXLib.h"
#include"PipelineState.h"
#include"Texture.h"
#include"PtrDelete.h"
ParticleManager * ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Add(Particle * newParticle,const std::string& TexName)
{
	//particles[TexName].push_back(newParticle->vsParam);
	//particleParams[TexName].push_back(newParticle->parameter);

	paramData.push_back(newParticle->parameter);
	vertData.push_back(newParticle->vsParam);
	activParticleCount++;
}

void ParticleManager::Initialize()
{
	CreateConstBuff();
	CreateModel();
	computeShade = new ComputeShade(vertexCount);
	//ComputeWrapper::GetInstance()->AddShade(computeShade);
}

void ParticleManager::Update()
{
	HRESULT result;
	//size_t offset  = NULL;
	//for(auto it = particles.begin();it!= particles.end();++it )
	//{
	//	memcpy(&vertData + offset, it->second.data(), sizeof(OutputData) * (UINT)std::distance(it->second.begin(), it->second.end()));
	//	offset += sizeof(OutputData) * (UINT)std::distance(it->second.begin(), it->second.end());
	//}
	//size_t offset2 = NULL;
	//for (auto it = particleParams.begin(); it != particleParams.end(); ++it)
	//{
	//	memcpy(&paramData + offset2, it->second.data(), sizeof(ParticleParameter) * (UINT)std::distance(it->second.begin(), it->second.end()));
	//	offset2 = sizeof(ParticleParameter) * (UINT)std::distance(it->second.begin(), it->second.end());
	//}
	
	//�R���s���[�g�V�F�[�_�[�Ƀf�[�^��]��
	computeShade->Update(paramData.data(), vertData.data(),activParticleCount);
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->mat = camera->GetMatViewProjection();
	constMap->matBillboard = camera->GetMatBillboard();
	constBuff->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	//computeShade->Dispatch(activParticleCount);
	//size_t offset = 0;
	memcpy(vertData.data(), computeShade->GetOutputData(), sizeof(OutputData) * activParticleCount);

	for (auto it = vertData.begin(); it != vertData.end();)
	{
		if ((*it).isDead == 0)
		{
			++it;
			continue;
		}
		it = vertData.erase(it);
		//particleParams[it->first].erase(particleParams[it->first].begin() + (int)std::distance(it->second.begin(), itr));
		paramData.erase(paramData.begin() + (int)std::distance(vertData.begin(), it));
		if (activParticleCount > 0)
			activParticleCount--;
		if (activParticleCount <= 0)
		{
			vertData.clear();
			paramData.clear();
			break;
		}
	}
	//std::vector<OutputData> sendData3;
	//for (auto it = particles.begin(); it != particles.end(); ++it)
	//{
	//	memcpy(sendData3.data() + offset, it->second.data(), sizeof(OutputData) * (UINT)std::distance(it->second.begin(), it->second.end()));
	//	offset = sizeof(OutputData) * (UINT)std::distance(it->second.begin(), it->second.end());
	//}

	const auto outPutPtr = vertData.data();
	//if (outPutPtr != nullptr)
	//{
		// ���_�o�b�t�@�փf�[�^�]��
	OutputData* vertMap = nullptr;
	auto result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, outPutPtr, sizeof(OutputData) * activParticleCount);
		vertBuff->Unmap(0, nullptr);
	}
	//}
	PipelineState::SetPipeline("Particle", ADD);

	//// �p�C�v���C���X�e�[�g�̐ݒ�
	//DirectXLib::GetCommandList()->SetPipelineState(PipelineState::GetPipelinestateParticle(ADD).Get());
	//// ���[�g�V�O�l�`���̐ݒ�
	//DirectXLib::GetCommandList()->SetGraphicsRootSignature(PipelineState::GetRootsugnatureParticle().Get());
	
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//const D3D12_VERTEX_BUFFER_VIEW  buf[2] = { vbView,matView };
	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//cmdList->IASetIndexBuffer(&ibView);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	int count = 0;
	//for (auto itr = vertData.begin(); itr != vertData.end(); ++itr)
	//{
		//// �p�[�e�B�N����1���Ȃ��ꍇ
		//if (itr.empty())
		//	continue;
	UINT drawNum = (UINT)std::distance(vertData.begin(), vertData.end());
	if (drawNum > vertexCount) {
		drawNum = vertexCount;
	}
	//if (drawNum == 0) {
	//	continue;
	//}

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1, Texture::GetGpuDescHandleSRV("particle"));
	// �`��R�}���h
	cmdList->DrawInstanced(drawNum, 1, count, 0);
	count += drawNum;
	//}

}

void ParticleManager::CreateConstBuff()
{
	HRESULT result;
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
	}

}

void ParticleManager::CreateModel()
{
	HRESULT result = S_FALSE;
	auto dev = DirectXLib::GetInstance()->GetDevice();
	// ���_�o�b�t�@����
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData)*vertexCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(OutputData)*vertexCount;
	vbView.StrideInBytes = sizeof(OutputData);


	//result = dev->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(sizeof(XMMATRIX) * vertexCount),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&matBuff));
	//if (FAILED(result)) {
	//	assert(0);
	//	return;
	//}

	//// ���_�o�b�t�@�r���[�̍쐬
	//matView.BufferLocation = matBuff->GetGPUVirtualAddress();
	//matView.SizeInBytes = sizeof(XMMATRIX) * vertexCount;
	//matView.StrideInBytes = sizeof(XMMATRIX);

	
	//UINT sizeIB = static_cast<UINT>(sizeof(unsigned short));
	//// �C���f�b�N�X�o�b�t�@����
	//result = dev->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&indexBuff));
	//if (FAILED(result)) {
	//	assert(0);
	//	return;
	//}

	//// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	//unsigned short* indexMap = nullptr;
	//result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//if (SUCCEEDED(result)) {
	//	indexMap[0] = 0;
	//	indexBuff->Unmap(0, nullptr);
	//}

	//// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	//ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeIB;

}

void ParticleManager::End()
{
	PtrDelete(computeShade);
}
