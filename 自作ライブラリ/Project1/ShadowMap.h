#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "window.h"

class ShadowMap
{
public:
	ShadowMap();
	static void SetWindow(Window* window) { ShadowMap::window = window; };
	void PreDraw();
	void PostDraw();
	void Draw();
private:
	void Initialize();
	static Window* window;

	//シェーダ―用
	struct ConstBufferDate {
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};
	struct VERTEX
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	VERTEX vertices[4];
	D3D12_VERTEX_BUFFER_VIEW vbView{};//頂点バッファビュー
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> shadowResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraRTVHeap;//レンダーターゲット用
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraSRVHeap;//テクスチャ用
	ID3D12GraphicsCommandList* cmdList;
	ID3D12Device* dev;
	

}; 

