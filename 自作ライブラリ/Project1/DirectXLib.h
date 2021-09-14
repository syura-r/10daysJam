#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include "d3dx12.h"
#include<imgui.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;
class Window;
class DirectXLib
{
private:
	//friend class PostEffect;
	DirectXLib() = default;
	DirectXLib(const DirectXLib&) = delete;
	~DirectXLib() = default;
	DirectXLib& operator=(const DirectXLib&) = delete;

	Window* window;

	ComPtr < ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12Device> dev;
	HRESULT CreateDXGI();
	HRESULT CreateCommands();
	HRESULT CreateSwapChain();
	HRESULT CreateDesc();
	HRESULT CreateFence();	
	HRESULT CreateDSV();
	HRESULT CreatePeraResurce();

	HRESULT ImguiInitialize();

	std::vector< ComPtr <IDXGIAdapter>> adapters;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4 > swapchain;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr < ID3D12CommandQueue> cmdQueue;
	ComPtr < ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr < ID3D12Resource> depthBuffer;
	//ComPtr<ID3D12Resource> peraResource;
	//ComPtr<ID3D12DescriptorHeap> peraRTVHeap;//レンダーターゲット用
	//ComPtr<ID3D12DescriptorHeap> peraSRVHeap;//テクスチャ用

	ComPtr<ID3D12DescriptorHeap> imguiHeap;

	std::vector< ComPtr<ID3D12Resource>> backBuffers;
	ComPtr < ID3D12Fence> fence;
	UINT64 fenceVal;

	UINT bbIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;

	//PostEffect* postEffect = nullptr;
public:
	static DirectXLib* GetInstance();
	
	void Initialize(Window* window);
	ID3D12Device* GetDevice() { return dev.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }
	ID3D12CommandQueue* GetCmdQueue() { return cmdQueue.Get(); }
	
	std::vector< ComPtr<ID3D12Resource>>& GetBackBuffers() { return backBuffers; }
	ComPtr<ID3D12DescriptorHeap>& GetDsvHeap() { return dsvHeap; }
	ComPtr < ID3D12DescriptorHeap>& GetRtvHeaps() { return  rtvHeaps; }

	void ImguiDraw();
	void BeginDraw();
	void Depth();
	void EndDraw();
	void End();
};

