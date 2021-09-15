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
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://構造体
	//頂点データ
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz座標
		float scale; // スケール
		XMFLOAT4 color;//カラー
		XMFLOAT3 rotation;//回転
		unsigned int billboradActive;//ビルボードするかどうか
	};
	//定数バッファ
	struct ConstBufferData
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	// ビルボード行列
	};

public://メンバ関数
	static ParticleManager* GetInstance();
	void Add(Particle* newParticle, const std::string& TexName);
	void Initialize();
	void Update();
	void Draw();
	inline void SetCamera(Camera* camera) { this->camera = camera; }
	void CreateConstBuff();
	void CreateModel();
private://メンバ関数
	ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	~ParticleManager() = default;
	ParticleManager& operator=(const ParticleManager&) = delete;

private://メンバ変数
	std::unordered_map<std::string, std::forward_list<Particle*>> particles;
	Camera* camera = nullptr;
	ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	BLENDTYPE nowBlendType = ADD;

	bool playerAddGauge = false;
private: // 静的メンバ定数
	static const int vertexCount = 65536;		// 頂点数
private:
};

