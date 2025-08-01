#pragma once
#include "RenderTarget.h"


class Component;
class RenderTarget;
class RootSignature;
class Shader;
class Mesh;
class Texture;
class GameObject;
class BufferManager;
class MeshRenderer;
class Material;

class Core
{
public:

	static unique_ptr<Core> main;

	Core();
	~Core();

	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	void Fence();
	void FenceCurrentFrame();
	//void FenceAll();

	void ExcuteCommandQueue();
	void ResizeWindowSize();
	HWND GetHandle() { return _hwnd; }

public:
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }
	ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return _cmdList[CURRENT_CONTEXT_INDEX]; }
	ComPtr<ID3D12GraphicsCommandList>& GetResCmdList() { return _resCmdList; }
	ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return _cmdQueue; }
	shared_ptr<RenderTarget>& GetRenderTarget() { return _renderTarget; }
	shared_ptr<GBuffer>& GetGBuffer() {return _gBuffer;}
	shared_ptr<ShadowBuffer>& GetShadowBuffer() { return _shadowBuffer; }
	shared_ptr<Texture>& GetDSReadTexture() { return _dsReadTexture; }
	shared_ptr<Texture>& GetRTReadTexture() { return _rtReadTexture; }
	shared_ptr<RootSignature>& GetRootSignature() { return _rootSignature; }
	shared_ptr<BufferManager>& GetBufferManager() { return _bufferManager; }
	ComPtr<ID3D12DescriptorHeap>& GetImguiHeap() { return _imguiHeap; }

	void CopyTexture(const std::shared_ptr<Texture>& destTexture, const std::shared_ptr<Texture>& sourceTexture);
	void ResizeTexture(std::shared_ptr<Texture>& texture, int w, int h);

private:
	void InitDirectX12();

private:
	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void CreateSwapChain();
	void CreateCmdQueue();
	void SetDebugLayerInfo();

private:
	shared_ptr<GBuffer> _gBuffer;
	shared_ptr<RenderTarget> _renderTarget;
	shared_ptr<ShadowBuffer> _shadowBuffer;

	shared_ptr<Texture> _dsReadTexture;
	shared_ptr<Texture> _rtReadTexture;

	shared_ptr<RootSignature> _rootSignature;
	shared_ptr<BufferManager> _bufferManager;

private:
	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory = nullptr;
	D3D_FEATURE_LEVEL _FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1 _adapterDesc = {};

	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;

	array<ComPtr<ID3D12CommandAllocator>,MAX_FRAME_COUNT> _cmdMemory = {};
	array<ComPtr<ID3D12GraphicsCommandList>,MAX_FRAME_COUNT> _cmdList = {};
	ComPtr<IDXGISwapChain3> _swapChain = nullptr;
	uint32 _swapChainFlags;

	ComPtr<ID3D12CommandAllocator> _resCmdMemory{};
	ComPtr<ID3D12GraphicsCommandList> _resCmdList{};

	ComPtr<ID3D12DescriptorHeap> _imguiHeap;

	ComPtr<ID3D12Fence> _fence = nullptr;
	HANDLE _fenceEvent = nullptr;
	uint64 _fenceValue = 0;
	uint64 _lastFenceValue[MAX_FRAME_COUNT] = { 0 };



	HWND _hwnd{0};


public:
	
public:
	
};

