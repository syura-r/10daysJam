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
#include "ComputeShade.h"

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
	////頂点データ
	//struct VertexPos
	//{
	//	float frame;
	//	//死亡フラグ
	//	float isDead;
	//	float pad;		
	//	//ビルボードするかどうか
	//	int billboardActive;

	//};
	//定数バッファ
	struct ConstBufferData
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	// ビルボード行列
	};

public://メンバ関数
	static ParticleManager* GetInstance();
	void Add(Particle* newParticle,const std::string& TexName);
	void Initialize();
	void Update();
	void Draw();
	inline void SetCamera(Camera* camera) { this->camera = camera; }
	void CreateConstBuff();
	void CreateModel();
	void End();
private://メンバ関数
	ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	~ParticleManager() = default;
	ParticleManager& operator=(const ParticleManager&) = delete;
	int activParticleCount = 0;
	//更新時に変更されないデータ
	std::vector<ParticleParameter> paramData;
	//更新時に変更され、VSに送られるデータ
	std::vector<OutputData> vertData;
private://メンバ変数
	//std::unordered_map<std::string,std::vector<OutputData>> particles;
	//std::unordered_map<std::string, std::vector<ParticleParameter>> particleParams;
	//void* vertData = nullptr;
	//void* paramData = nullptr;

	ComPtr<ID3D12Resource> constBuff;
	Camera* camera = nullptr;
	ComPtr<ID3D12Resource> vertBuff;
	//ComPtr<ID3D12Resource> matBuff;
	//ComPtr<ID3D12Resource> indexBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	//D3D12_INDEX_BUFFER_VIEW ibView = {};
	//D3D12_VERTEX_BUFFER_VIEW matView{};//頂点バッファビュー

	ComputeShade* computeShade = nullptr;
private: // 静的メンバ定数
	static const int vertexCount = 65536;		// 頂点数
private:
};

