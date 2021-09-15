#include "ParticleManager.h"
#include"DirectXLib.h"
#include"PipelineState.h"
#include"Texture.h"
#include"PtrDelete.h"
#include"Easing.h"
#include"Player.h"
ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Add(Particle* newParticle, const std::string& TexName)
{
	particles[TexName].push_front(newParticle);
}

void ParticleManager::Initialize()
{
	CreateConstBuff();
	CreateModel();
}

void ParticleManager::Update()
{
	HRESULT result;

	for (auto itr = particles.begin();
		itr != particles.end();
		itr++) {
		if (itr->second.empty())
			continue;
		for (auto it = itr->second.begin(); it != itr->second.end(); it++)
		{
			// �o�߃t���[�������J�E���g
			(*it)->parameter.frame++;
			// �i�s�x��0�`1�͈̔͂Ɋ��Z
			float f = (float)(*it)->parameter.frame / (*it)->parameter.num_frame;

			if ((*it)->parameter.type != FEVER)
			{
				// ���x�ɉ����x�����Z
				(*it)->parameter.velocity += (*it)->parameter.accel;
				(*it)->parameter.velocity.Normalize();
			}
			// ���x�ɂ��ړ�

			switch ((*it)->parameter.type)
			{
			case DEFAULT:
				(*it)->parameter.position += (*it)->parameter.velocity * 0.1f;
				break;
			case MOVE:
				if ((*it)->parameter.frame < 20)
				{
					(*it)->parameter.position += (*it)->parameter.velocity * 0.1f;
				}
				if ((*it)->parameter.frame > 20 && (*it)->parameter.frame < 45)
				{
					(*it)->parameter.position += (*it)->parameter.velocity * 0.05f;
				}
				if ((*it)->parameter.frame > 35)
				{
					//(*it)->parameter.easeFrame++;
					Vector3 vel = ((*it)->parameter.player->GetPosition() - (*it)->parameter.position) / ((*it)->parameter.num_frame - (*it)->parameter.frame);
					(*it)->parameter.velocity = vel;
					(*it)->parameter.position += (*it)->parameter.velocity;

					//(*it)->parameter.position.x = Easing::EaseInOutQuart((*it)->parameter.position.x, (*it)->parameter.endPos.x, (*it)->parameter.maxFrame, (*it)->parameter.easeFrame);
					//(*it)->parameter.position.y = Easing::EaseInOutQuart((*it)->parameter.position.y, (*it)->parameter.endPos.y, (*it)->parameter.maxFrame, (*it)->parameter.easeFrame);
					//(*it)->parameter.position.z = Easing::EaseInOutQuart((*it)->parameter.position.z, (*it)->parameter.endPos.z, (*it)->parameter.maxFrame, (*it)->parameter.easeFrame);
				}
				break;
			case HOMING:
			{
				if ((*it)->parameter.targetObj->IsDead())
				{
					PtrDelete(*it);
					continue;
				}
				Vector3 vel = ((*it)->parameter.targetObj->GetPosition() + (*it)->parameter.gap - (*it)->parameter.position) / ((*it)->parameter.num_frame - (*it)->parameter.frame);
				(*it)->parameter.velocity = vel;
				(*it)->parameter.position += (*it)->parameter.velocity;
				break;
			}
			case TIMEPLUS:
			{
				if ((*it)->parameter.frame <= 30)
					(*it)->parameter.position.y = Easing::EaseInOutQuint((*it)->parameter.startPos.y, (*it)->parameter.startPos.y + 0.2f, 30, (*it)->parameter.frame);
				else
					(*it)->parameter.alpha -= 1.0f / 30;
				break;
			}
			case FEVER:
				if ((*it)->parameter.accel.y > 0)
				{
					if ((*it)->parameter.velocity.y < 0)
						(*it)->parameter.velocity += (*it)->parameter.accel;
					else
						(*it)->parameter.velocity = {};
				}
				else
				{
					if ((*it)->parameter.velocity.y > 0)
						(*it)->parameter.velocity += (*it)->parameter.accel;
					else
						(*it)->parameter.velocity = {};
				}
				(*it)->parameter.position += (*it)->parameter.velocity;

				break;
			default:
				(*it)->parameter.position += (*it)->parameter.velocity * 0.1f;
				break;
			}

			// �J���[�̐��`���
			(*it)->parameter.color = (*it)->parameter.s_color + ((*it)->parameter.e_color - (*it)->parameter.s_color) * f;

			//�A���t�@�l�̐��`���
			//(*it)->parameter.alpha = (*it)->parameter.s_alpha + (((*it)->parameter.e_alpha - (*it)->parameter.s_alpha) * f);

			//�X�P�[�����x�ɉ����x�����Z
			(*it)->parameter.scaleVel += (*it)->parameter.scaleAce;

			// �X�P�[���ɑ��x�����Z
			(*it)->parameter.scale += (*it)->parameter.scaleVel;

			// ��]�̐��`���
			(*it)->parameter.rotation = (*it)->parameter.s_rotation + ((*it)->parameter.e_rotation - (*it)->parameter.s_rotation) * f;

			if ((*it)->parameter.frame >= (*it)->parameter.num_frame)
			{
				PtrDelete(*it);
			}
		}
		itr->second.remove_if([](Particle* x) {return x == nullptr; });
	}

	// ���_�o�b�t�@�փf�[�^�]��
	int vertCount = 0;
	VertexPos* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		// �p�[�e�B�N���̏���1�����f
		for (auto itr = particles.begin();
			itr != particles.end();
			itr++) {
			if (itr->second.empty())
				continue;
			for (auto it = itr->second.begin(); it != itr->second.end(); it++)
			{
				// ���W
				vertMap->pos = (*it)->parameter.position;
				// �X�P�[��
				vertMap->scale = (*it)->parameter.scale;
				//�J���[
				vertMap->color = { (*it)->parameter.color.x,(*it)->parameter.color.y,(*it)->parameter.color.z,(*it)->parameter.alpha };
				//��]
				Vector3 rotation = (*it)->parameter.rotation * 3.1415f / 180;
				vertMap->rotation = rotation;
				//�r���{�[�h���邩�ǂ���
				vertMap->billboradActive = (*it)->parameter.billboradActive;
				// ���̒��_��
				vertMap++;
				if (++vertCount >= vertexCount) {
					break;
				}
			}
		}
		vertBuff->Unmap(0, nullptr);
	}

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = camera->GetMatViewProjection();
	constMap->matBillboard = camera->GetMatBillboard();
	constBuff->Unmap(0, nullptr);
	if (playerAddGauge)
		playerAddGauge = false;
}

void ParticleManager::Draw()
{
	auto cmdlist = DirectXLib::GetInstance()->GetCommandList();
	// �p�C�v���C���X�e�[�g�̐ݒ�
	PipelineState::SetPipeline("Particle", nowBlendType);
	// �v���~�e�B�u�`���ݒ�
	cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ���_�o�b�t�@�̐ݒ�
	cmdlist->IASetVertexBuffers(0, 1, &vbView);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdlist->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	int count = 0;
	for (auto itr = particles.begin(); itr != particles.end(); itr++)
	{
		// �p�[�e�B�N����1���Ȃ��ꍇ
		if (itr->second.empty())
			continue;
		UINT drawNum = (UINT)std::distance(itr->second.begin(), itr->second.end());
		if (drawNum > vertexCount) {
			drawNum = vertexCount;
		}
		//if (drawNum == 0) {
		//	continue;
		//}
		auto particleIt = itr->second.begin();
		if (nowBlendType != (*particleIt)->parameter.blendType)
		{
			nowBlendType = (*particleIt)->parameter.blendType;
			PipelineState::SetPipeline("Particle", nowBlendType);
			// �v���~�e�B�u�`���ݒ�
			cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

			// ���_�o�b�t�@�̐ݒ�
			cmdlist->IASetVertexBuffers(0, 1, &vbView);

			// �f�X�N���v�^�q�[�v�̔z��
			ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
			cmdlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// �萔�o�b�t�@�r���[���Z�b�g
			cmdlist->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

		}

		// �V�F�[�_���\�[�X�r���[���Z�b�g
		cmdlist->SetGraphicsRootDescriptorTable(1, Texture::GetGpuDescHandleSRV(itr->first));
		// �`��R�}���h
		cmdlist->DrawInstanced(drawNum, 1, count, 0);
		count += drawNum;
	}
}

void ParticleManager::CreateConstBuff()
{
	HRESULT result;
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
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

	// ���_�o�b�t�@����
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPos) * vertexCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPos) * vertexCount;
	vbView.StrideInBytes = sizeof(VertexPos);
}
