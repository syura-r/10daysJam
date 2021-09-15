#pragma once
#include"Particle.h"
#include<forward_list>
#include<unordered_map>
#include<string>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "Camera.h"

class ParticleManager
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://�\����
	//���_�f�[�^
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz���W
		float scale; // �X�P�[��
		XMFLOAT4 color;//�J���[
		XMFLOAT3 rotation;//��]
		unsigned int billboradActive;//�r���{�[�h���邩�ǂ���
	};
	//�萔�o�b�t�@
	struct ConstBufferData
	{
		XMMATRIX mat;	// �r���[�v���W�F�N�V�����s��
		XMMATRIX matBillboard;	// �r���{�[�h�s��
	};

public://�����o�֐�
	static ParticleManager* GetInstance();
	void Add(Particle* newParticle, const std::string& TexName);
	void Initialize();
	void Update();
	void Draw();
	inline void SetCamera(Camera* camera) { this->camera = camera; }
	void CreateConstBuff();
	void CreateModel();
private://�����o�֐�
	ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	~ParticleManager() = default;
	ParticleManager& operator=(const ParticleManager&) = delete;

private://�����o�ϐ�
	std::unordered_map<std::string, std::forward_list<Particle*>> particles;
	Camera* camera = nullptr;
	ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	BLENDTYPE nowBlendType = ADD;

	bool playerAddGauge = false;
private: // �ÓI�����o�萔
	static const int vertexCount = 65536;		// ���_��
private:
};

